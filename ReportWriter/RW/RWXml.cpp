/*
 *  RWXml.cpp
 *  ReportWriter
 *
 *  Thin XML layer over pugixml (wchar_t mode).
 */

# include	"RWXml.h"

namespace
{
	inline	std::wstring	W (RWStringView inText)			{ return RWStr::ToWide (inText); }
	inline	RWString		U (const pugi::char_t *inText)	{ return RWStr::FromWide (inText); }


	// ---------------------------------------------------------------------------
	// Writers collecting pugixml output in memory
	// ---------------------------------------------------------------------------

	class	UTF16Writer	:	public	pugi::xml_writer
	{
	public:
		RWString	fText;

		void		write (const void *inData, size_t inSize) override
		{
			fText.append (static_cast <const char16_t*> (inData), inSize / sizeof (char16_t));
		}
	};

	class	UTF8Writer	:	public	pugi::xml_writer
	{
	public:
		std::string	fText;

		void		write (const void *inData, size_t inSize) override
		{
			fText.append (static_cast <const char*> (inData), inSize);
		}
	};


	unsigned
	SaveFlags (bool inPretty, bool inDeclaration)
	{
		unsigned	flags = inPretty ? pugi::format_indent : pugi::format_raw;
		if (!inDeclaration)
			flags |= pugi::format_no_declaration;
		return flags;
	}


	RWXmlResult
	MakeResult (const pugi::xml_parse_result &inResult)
	{
		RWXmlResult	result;
		result.ok = bool (inResult);
		result.offset = inResult.offset;
		if (!result.ok)
			result.description = RWStr::FromASCII (inResult.description());
		return result;
	}
}


// ---------------------------------------------------------------------------
// RWXmlElementIterator
// ---------------------------------------------------------------------------

RWXmlElementIterator::RWXmlElementIterator (pugi::xml_node inNode, const std::wstring *inName)
	:	mNode (inNode),
		mName (inName)
{
	SkipToMatch();
}

RWXmlElementIterator&
RWXmlElementIterator::operator ++ (void)
{
	mNode = mNode.next_sibling();
	SkipToMatch();
	return *this;
}

void
RWXmlElementIterator::SkipToMatch (void)
{
	while (mNode && (mNode.type() != pugi::node_element || (!mName->empty() && *mName != mNode.name())))
		mNode = mNode.next_sibling();
}


// ---------------------------------------------------------------------------
// Name / Value
// ---------------------------------------------------------------------------

RWString
RWXmlNode::Name (void) const
{
	return U (mNode.name());
}

bool
RWXmlNode::NameIs (std::string_view inASCIIName) const
{
	const pugi::char_t	*name = mNode.name();
	size_t				i = 0;
	for (; i < inASCIIName.size(); i++)
		if (name[i] != pugi::char_t ((unsigned char) inASCIIName[i]))
			return false;
	return name[i] == 0;
}

RWString
RWXmlNode::Value (void) const
{
	return U (mNode.value());
}


// ---------------------------------------------------------------------------
// Attributes
// ---------------------------------------------------------------------------

bool
RWXmlNode::HasAttr (RWStringView inName) const
{
	return !mNode.attribute (W (inName).c_str()).empty();
}

RWString
RWXmlNode::Attr (RWStringView inName, RWStringView inDefault) const
{
	pugi::xml_attribute	attr = mNode.attribute (W (inName).c_str());
	return attr ? U (attr.value()) : RWString (inDefault);
}

long long
RWXmlNode::AttrInt (RWStringView inName, long long inDefault) const
{
	pugi::xml_attribute	attr = mNode.attribute (W (inName).c_str());
	return attr ? RWStr::ToInteger (U (attr.value())).value_or (inDefault) : inDefault;
}

double
RWXmlNode::AttrDouble (RWStringView inName, double inDefault) const
{
	pugi::xml_attribute	attr = mNode.attribute (W (inName).c_str());
	return attr ? RWStr::ToDouble (U (attr.value())).value_or (inDefault) : inDefault;
}

bool
RWXmlNode::AttrBool (RWStringView inName, bool inDefault) const
{
	pugi::xml_attribute	attr = mNode.attribute (W (inName).c_str());
	if (!attr)
		return inDefault;

	RWString		value = U (attr.value());
	RWStringView	trimmed = RWStr::Trim (value);
	if (std::optional<long long> number = RWStr::ToInteger (trimmed))
		return *number != 0;
	if (RWStr::EqualsNoCase (trimmed, "true") || RWStr::EqualsNoCase (trimmed, "yes"))
		return true;
	if (RWStr::EqualsNoCase (trimmed, "false") || RWStr::EqualsNoCase (trimmed, "no"))
		return false;
	return inDefault;
}

std::vector<std::pair<RWString, RWString>>
RWXmlNode::Attributes (void) const
{
	std::vector<std::pair<RWString, RWString>>	result;
	for (pugi::xml_attribute attr : mNode.attributes())
		result.emplace_back (U (attr.name()), U (attr.value()));
	return result;
}

void
RWXmlNode::SetAttr (RWStringView inName, RWStringView inValue)
{
	std::wstring		name = W (inName);
	pugi::xml_attribute	attr = mNode.attribute (name.c_str());
	if (!attr)
		attr = mNode.append_attribute (name.c_str());
	attr.set_value (W (inValue).c_str());
}

void
RWXmlNode::SetAttrInt (RWStringView inName, long long inValue)
{
	SetAttr (inName, RWStr::FromInteger (inValue));
}

void
RWXmlNode::SetAttrDouble (RWStringView inName, double inValue, const char *inPrintfFormat)
{
	SetAttr (inName, RWStr::FromDouble (inValue, inPrintfFormat));
}

void
RWXmlNode::SetAttrBool (RWStringView inName, bool inValue)
{
	SetAttr (inName, inValue ? u"1" : u"0");
}

bool
RWXmlNode::RemoveAttr (RWStringView inName)
{
	return mNode.remove_attribute (W (inName).c_str());
}


// ---------------------------------------------------------------------------
// Text
// ---------------------------------------------------------------------------

RWString
RWXmlNode::Text (void) const
{
	return U (mNode.text().get());
}

void
RWXmlNode::SetText (RWStringView inText)
{
	mNode.text().set (W (inText).c_str());
}

RWXmlNode
RWXmlNode::AppendText (RWStringView inText)
{
	pugi::xml_node	text = mNode.append_child (pugi::node_pcdata);
	text.set_value (W (inText).c_str());
	return RWXmlNode (text);
}


// ---------------------------------------------------------------------------
// Element navigation
// ---------------------------------------------------------------------------

RWXmlNode
RWXmlNode::Child (RWStringView inName) const
{
	return RWXmlNode (mNode.child (W (inName).c_str()));
}

RWXmlNode
RWXmlNode::FirstElement (void) const
{
	pugi::xml_node	node = mNode.first_child();
	while (node && node.type() != pugi::node_element)
		node = node.next_sibling();
	return RWXmlNode (node);
}

RWXmlNode
RWXmlNode::NextElement (void) const
{
	pugi::xml_node	node = mNode.next_sibling();
	while (node && node.type() != pugi::node_element)
		node = node.next_sibling();
	return RWXmlNode (node);
}

RWXmlNode
RWXmlNode::NextElement (RWStringView inName) const
{
	return RWXmlNode (mNode.next_sibling (W (inName).c_str()));
}

RWXmlElementRange
RWXmlNode::Children (RWStringView inName) const
{
	return RWXmlElementRange (mNode, W (inName));
}


// ---------------------------------------------------------------------------
// Modification
// ---------------------------------------------------------------------------

RWXmlNode
RWXmlNode::Append (RWStringView inName)
{
	return RWXmlNode (mNode.append_child (W (inName).c_str()));
}

RWXmlNode
RWXmlNode::AppendCopy (RWXmlNode inNode)
{
	return RWXmlNode (mNode.append_copy (inNode.mNode));
}

bool
RWXmlNode::Remove (RWXmlNode inChild)
{
	return mNode.remove_child (inChild.mNode);
}


// ---------------------------------------------------------------------------
// RWXmlDocument - loading
// ---------------------------------------------------------------------------

RWXmlResult
RWXmlDocument::LoadString (RWStringView inXML, unsigned inOptions)
{
	return MakeResult (mDoc.load_buffer (inXML.data(), inXML.size() * sizeof (char16_t), inOptions, pugi::encoding_utf16));
}

RWXmlResult
RWXmlDocument::LoadBuffer (const void *inData, size_t inSize, unsigned inOptions)
{
	return MakeResult (mDoc.load_buffer (inData, inSize, inOptions, pugi::encoding_auto));
}

RWXmlResult
RWXmlDocument::LoadFile (const RWString &inPath, unsigned inOptions)
{
	return MakeResult (mDoc.load_file (W (inPath).c_str(), inOptions, pugi::encoding_auto));
}


// ---------------------------------------------------------------------------
// RWXmlDocument - saving
// ---------------------------------------------------------------------------

RWString
RWXmlDocument::SaveString (bool inPretty, bool inDeclaration) const
{
	UTF16Writer	writer;
	mDoc.save (writer, L"\t", SaveFlags (inPretty, inDeclaration), pugi::encoding_utf16);
	return std::move (writer.fText);
}

std::string
RWXmlDocument::SaveUTF8 (bool inPretty, bool inDeclaration) const
{
	UTF8Writer	writer;
	mDoc.save (writer, L"\t", SaveFlags (inPretty, inDeclaration), pugi::encoding_utf8);
	return std::move (writer.fText);
}

bool
RWXmlDocument::SaveFile (const RWString &inPath, bool inPretty) const
{
	return mDoc.save_file (W (inPath).c_str(), L"\t", SaveFlags (inPretty, true), pugi::encoding_utf8);
}
