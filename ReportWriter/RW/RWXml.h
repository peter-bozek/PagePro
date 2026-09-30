/*
 *  RWXml.h
 *  ReportWriter
 *
 *  Thin XML layer over pugixml (built in wchar_t mode, see pugiconfig.hpp).
 *  Report code sees only RWString (UTF-16); the conversion to pugi::char_t
 *  happens here - a copy on Windows, UTF-16 <-> UTF-32 on Mac.
 *
 *  Documents are always built in memory and written out as a whole
 *  (SaveFile / SaveUTF8 / SaveString) - there are no streaming writers.
 *
 *  RWXmlNode is a small value type (a handle into its document); a null node
 *  is returned instead of NULL pointers and every method is safe to call on it.
 */

#ifndef	_RWXml_h_
# define	_RWXml_h_

# include	"RWString.h"
# include	"pugixml.hpp"

# include	<iterator>
# include	<type_traits>
# include	<utility>
# include	<vector>

class	RWXmlNode;


// Range over element children, optionally only those with a given name.
class	RWXmlElementIterator
{
public:
	typedef	std::forward_iterator_tag	iterator_category;
	typedef	RWXmlNode					value_type;
	typedef	std::ptrdiff_t				difference_type;
	typedef	void						pointer;
	typedef	RWXmlNode					reference;

						RWXmlElementIterator (pugi::xml_node inNode, const std::wstring *inName);

	inline	RWXmlNode	operator * (void) const;
	RWXmlElementIterator&	operator ++ (void);
	bool				operator == (const RWXmlElementIterator &inRhs) const	{ return mNode == inRhs.mNode; }
	bool				operator != (const RWXmlElementIterator &inRhs) const	{ return mNode != inRhs.mNode; }

private:
	void				SkipToMatch (void);

	pugi::xml_node		mNode;
	const std::wstring	*mName;
};

class	RWXmlElementRange
{
public:
						RWXmlElementRange (pugi::xml_node inParent, std::wstring inName)
							:	mParent (inParent), mName (std::move (inName)) {}

	RWXmlElementIterator	begin (void) const	{ return RWXmlElementIterator (mParent.first_child(), &mName); }
	RWXmlElementIterator	end (void) const	{ return RWXmlElementIterator (pugi::xml_node(), &mName); }

private:
	pugi::xml_node		mParent;
	std::wstring		mName;
};


class	RWXmlNode
{
public:
						RWXmlNode (void) {}
	explicit			RWXmlNode (pugi::xml_node inNode) : mNode (inNode) {}

	explicit			operator bool (void) const		{ return !mNode.empty(); }
	bool				operator == (const RWXmlNode &inRhs) const	{ return mNode == inRhs.mNode; }
	bool				operator != (const RWXmlNode &inRhs) const	{ return mNode != inRhs.mNode; }

	bool				IsElement (void) const		{ return mNode.type() == pugi::node_element; }
	bool				IsText (void) const			{ return mNode.type() == pugi::node_pcdata || mNode.type() == pugi::node_cdata; }

	// element name; Value is the content of a text node
	RWString			Name (void) const;
	bool				NameIs (std::string_view inASCIIName) const;
	RWString			Value (void) const;

	// attributes - typed getters return inDefault when the attribute is missing or not a number
	bool				HasAttr (RWStringView inName) const;
	RWString			Attr (RWStringView inName, RWStringView inDefault = RWStringView()) const;
	long long			AttrInt (RWStringView inName, long long inDefault = 0) const;
	double				AttrDouble (RWStringView inName, double inDefault = 0) const;
	bool				AttrBool (RWStringView inName, bool inDefault = false) const;	// numbers (non zero), true/false, yes/no
	std::vector<std::pair<RWString, RWString>>
						Attributes (void) const;

	void				SetAttr (RWStringView inName, RWStringView inValue);
	void				SetAttrInt (RWStringView inName, long long inValue);
	void				SetAttrDouble (RWStringView inName, double inValue, const char *inPrintfFormat = "%g");
	void				SetAttrBool (RWStringView inName, bool inValue);		// written as 1 / 0
	bool				RemoveAttr (RWStringView inName);

	// typed setter: bool as 1 / 0 (what the readers parse), integers in decimal,
	// float as "%.8g", double as "%.15g", text as is
	template <class T>
	void				SetAttribute (RWStringView inName, const T &inValue);

	// text content: first text child of an element (pugixml "text()")
	RWString			Text (void) const;
	void				SetText (RWStringView inText);
	RWXmlNode			AppendText (RWStringView inText);

	// navigation - all child nodes, including text
	RWXmlNode			Parent (void) const			{ return RWXmlNode (mNode.parent()); }
	RWXmlNode			FirstChild (void) const		{ return RWXmlNode (mNode.first_child()); }
	RWXmlNode			NextSibling (void) const	{ return RWXmlNode (mNode.next_sibling()); }

	// navigation - elements only
	RWXmlNode			Child (RWStringView inName) const;
	RWXmlNode			FirstElement (void) const;
	RWXmlNode			NextElement (void) const;
	RWXmlNode			NextElement (RWStringView inName) const;
	RWXmlElementRange	Children (RWStringView inName = RWStringView()) const;

	// modification
	RWXmlNode			Append (RWStringView inName);
	RWXmlNode			AppendCopy (RWXmlNode inNode);	// deep copy, may come from another document
	bool				Remove (RWXmlNode inChild);

	pugi::xml_node		Native (void) const			{ return mNode; }

private:
	pugi::xml_node		mNode;
};

inline	RWXmlNode	RWXmlElementIterator::operator * (void) const	{ return RWXmlNode (mNode); }

template <class T>
void
RWXmlNode::SetAttribute (RWStringView inName, const T &inValue)
{
	if constexpr (std::is_same<T, bool>::value)
		SetAttrBool (inName, inValue);
	else if constexpr (std::is_enum<T>::value || std::is_integral<T>::value)
		SetAttrInt (inName, (long long) inValue);
	else if constexpr (std::is_same<T, float>::value)
		SetAttrDouble (inName, (double) inValue, "%.8g");		// float precision, no "0.100000001"
	else if constexpr (std::is_floating_point<T>::value)
		SetAttrDouble (inName, (double) inValue, "%.15g");
	else
		SetAttr (inName, RWStringView (inValue));
}


struct	RWXmlResult
{
	bool				ok = false;
	RWString			description;
	std::ptrdiff_t		offset = 0;		// in characters of the parsed source

	explicit			operator bool (void) const	{ return ok; }
};


class	RWXmlDocument
{
public:
	// pugixml parse_default drops text nodes made only of white space (as TinyXML did);
	// parse_ws_pcdata_single keeps them when they are the only content, e.g. <Data> </Data>.
	static	const	unsigned	kDefaultParseOptions	=	pugi::parse_default | pugi::parse_ws_pcdata_single;

						RWXmlDocument (void) {}
						RWXmlDocument (const RWXmlDocument&) = delete;
	RWXmlDocument&		operator = (const RWXmlDocument&) = delete;

	// loading replaces the current content
	RWXmlResult			LoadString (RWStringView inXML, unsigned inOptions = kDefaultParseOptions);
	RWXmlResult			LoadBuffer (const void *inData, size_t inSize, unsigned inOptions = kDefaultParseOptions);	// encoding detected (BOM / declaration), UTF-8 assumed
	RWXmlResult			LoadFile (const RWString &inPath, unsigned inOptions = kDefaultParseOptions);
	void				Clear (void)				{ mDoc.reset(); }

	// saving - files and UTF-8 output carry an XML declaration; pretty printing indents with tabs
	RWString			SaveString (bool inPretty = true, bool inDeclaration = false) const;
	std::string			SaveUTF8 (bool inPretty = true, bool inDeclaration = true) const;
	bool				SaveFile (const RWString &inPath, bool inPretty = true) const;

	// the document node (append the root element here) and the root element
	RWXmlNode			Node (void)					{ return RWXmlNode (mDoc); }
	RWXmlNode			Root (void) const			{ return RWXmlNode (mDoc.document_element()); }

	pugi::xml_document&	Native (void)				{ return mDoc; }

private:
	pugi::xml_document	mDoc;
};

#endif
