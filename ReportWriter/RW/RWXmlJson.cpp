/*
 *  RWXmlJson.cpp
 *  ReportWriter
 *
 *  Report documents as JSON (see RWXmlJson.h).
 */

# include	"RWXmlJson.h"
# include	"RWJson.h"
# include	"rapidjson/error/en.h"

namespace
{
	const char16_t	kTag []			= u"tag";
	const char16_t	kAttributes []	= u"attributes";
	const char16_t	kChildren []	= u"children";


	// -----------------------------------------------------------------------
	// XML -> JSON
	// -----------------------------------------------------------------------

	RWJsonValue		ElementToJson (RWXmlNode inElement, RWJsonAllocator &inAllocator)
	{
		RWJsonValue	element (rapidjson::kObjectType);
		element.AddMember (RWJson::String (kTag, inAllocator), RWJson::String (inElement.Name(), inAllocator), inAllocator);

		const auto	attributes = inElement.Attributes();
		if (!attributes.empty())
		{
			RWJsonValue	object (rapidjson::kObjectType);
			for (const auto &attribute : attributes)
				object.AddMember (RWJson::String (attribute.first, inAllocator), RWJson::String (attribute.second, inAllocator), inAllocator);
			element.AddMember (RWJson::String (kAttributes, inAllocator), object, inAllocator);
		}

		RWJsonValue	children (rapidjson::kArrayType);
		for (RWXmlNode child = inElement.FirstChild(); child; child = child.NextSibling())
		{
			if (child.IsElement())
				children.PushBack (ElementToJson (child, inAllocator), inAllocator);
			else if (child.IsText())
				children.PushBack (RWJson::String (child.Value(), inAllocator), inAllocator);
			// comments, processing instructions: not part of a report
		}
		if (!children.Empty())
			element.AddMember (RWJson::String (kChildren, inAllocator), children, inAllocator);

		return element;
	}


	// -----------------------------------------------------------------------
	// JSON -> XML
	// -----------------------------------------------------------------------

	struct	ConversionError
	{
		RWString	message;
	};

	RWString		ValueName (const RWJsonValue &inValue)
	{
		switch (inValue.GetType())
		{
			case rapidjson::kNullType:		return u"null";
			case rapidjson::kFalseType:
			case rapidjson::kTrueType:		return u"a boolean";
			case rapidjson::kObjectType:	return u"an object";
			case rapidjson::kArrayType:		return u"an array";
			case rapidjson::kStringType:	return u"a string";
			case rapidjson::kNumberType:	return u"a number";
		}
		return u"?";
	}

	[[noreturn]] void	Fail (const RWString &inPath, const RWString &inMessage)
	{
		throw ConversionError { inPath + u": " + inMessage };
	}

	RWStringView	StringOf (const RWJsonValue &inValue)
	{
		return RWStringView (inValue.GetString(), inValue.GetStringLength());
	}

	// attribute value as text: strings as they are, numbers as written by the
	// report writers, booleans as 1 / 0
	bool			AttributeText (const RWJsonValue &inValue, RWString &outText)
	{
		if (inValue.IsString())
			outText = RWString (StringOf (inValue));
		else if (inValue.IsBool())
			outText = inValue.GetBool() ? u"1" : u"0";
		else if (inValue.IsInt64())
			outText = RWStr::FromInteger (inValue.GetInt64());
		else if (inValue.IsUint64())
			outText = RWStr::FromDouble (double (inValue.GetUint64()), "%.0f");
		else if (inValue.IsNumber())
			outText = RWStr::FromDouble (inValue.GetDouble(), "%.15g");
		else
			return false;
		return true;
	}

	bool			IsValidName (RWStringView inName)
	{
		if (inName.empty())
			return false;
		for (char16_t c : inName)
			if (c <= u' ' || c == u'<' || c == u'>' || c == u'&' || c == u'"' || c == u'\'' || c == u'=' || c == u'/')
				return false;
		return !(inName [0] >= u'0' && inName [0] <= u'9') && inName [0] != u'-' && inName [0] != u'.';
	}

	void			JsonToElement (const RWJsonValue &inValue, RWXmlNode inParent, const RWString &inPath)
	{
		if (!inValue.IsObject())
			Fail (inPath, u"an element must be an object, not " + ValueName (inValue));

		auto	tag = inValue.FindMember (kTag);
		if (tag == inValue.MemberEnd() || !tag->value.IsString())
			Fail (inPath, u"\"tag\" (the element name) is missing or not a string");
		if (!IsValidName (StringOf (tag->value)))
			Fail (inPath + u".tag", u"\"" + RWString (StringOf (tag->value)) + u"\" is not a valid element name");

		RWXmlNode	element = inParent.Append (StringOf (tag->value));

		for (auto member = inValue.MemberBegin(); member != inValue.MemberEnd(); ++member)
		{
			const RWStringView	key = StringOf (member->name);
			if (key == kTag)
				continue;

			if (key == kAttributes)
			{
				if (member->value.IsNull())
					continue;
				if (!member->value.IsObject())
					Fail (inPath + u".attributes", u"must be an object, not " + ValueName (member->value));
				for (auto attribute = member->value.MemberBegin(); attribute != member->value.MemberEnd(); ++attribute)
				{
					const RWStringView	name = StringOf (attribute->name);
					const RWString		path = inPath + u".attributes." + RWString (name);
					if (!IsValidName (name))
						Fail (path, u"not a valid attribute name");
					if (attribute->value.IsNull())
						continue;
					RWString	text;
					if (!AttributeText (attribute->value, text))
						Fail (path, u"an attribute value must be a string, number or boolean, not " + ValueName (attribute->value));
					element.SetAttr (name, text);
				}
			}
			else if (key == kChildren)
			{
				if (member->value.IsNull())
					continue;
				if (!member->value.IsArray())
					Fail (inPath + u".children", u"must be an array, not " + ValueName (member->value));
				rapidjson::SizeType	index = 0;
				for (const RWJsonValue &child : member->value.GetArray())
				{
					const RWString	path = inPath + u".children[" + RWStr::FromInteger (index++) + u"]";
					if (child.IsString())
						element.AppendText (StringOf (child));
					else if (child.IsObject())
						JsonToElement (child, element, path);
					else
						Fail (path, u"a child must be an element (object) or text (string), not " + ValueName (child));
				}
			}
			else
				Fail (inPath + u"." + RWString (key), u"unknown key, an element has only \"tag\", \"attributes\" and \"children\"");
		}
	}

	template <class Document>
	RWXmlResult		Convert (Document &inJson, RWXmlDocument &outDocument)
	{
		RWXmlResult	result;
		outDocument.Clear();

		if (inJson.HasParseError())
		{
			result.offset = std::ptrdiff_t (inJson.GetErrorOffset());
			result.description = RWStr::FromASCII (rapidjson::GetParseError_En (inJson.GetParseError()))
								 + u" (offset " + RWStr::FromInteger (result.offset) + u")";
			return result;
		}

		try
		{
			JsonToElement (inJson, outDocument.Node(), u"report");
			result.ok = true;
		}
		catch (const ConversionError &e)
		{
			outDocument.Clear();
			result.description = e.message;
		}
		return result;
	}
}


RWString
RWXmlJson::ToJson (RWXmlNode inElement, bool inPretty)
{
	RWJsonDocument	json;
	RWJsonValue		value = ElementToJson (inElement, json.GetAllocator());
	return RWJson::ToString (value, inPretty);
}

std::string
RWXmlJson::ToJsonUTF8 (RWXmlNode inElement, bool inPretty)
{
	RWJsonDocument	json;
	RWJsonValue		value = ElementToJson (inElement, json.GetAllocator());
	return RWJson::ToUTF8 (value, inPretty);
}

RWXmlResult
RWXmlJson::FromJson (RWStringView inJson, RWXmlDocument &outDocument)
{
	const RWString	text (inJson);		// RapidJSON reads UTF-16 only from null terminated strings
	RWJsonDocument	json;
	json.Parse (text.c_str());
	return Convert (json, outDocument);
}

RWXmlResult
RWXmlJson::FromJsonUTF8 (std::string_view inJson, RWXmlDocument &outDocument)
{
	if (inJson.size() >= 3 && inJson.substr (0, 3) == "\xEF\xBB\xBF")
		inJson.remove_prefix (3);

	const std::string	text (inJson);
	RWJsonDocument		json;
	json.Parse<rapidjson::kParseDefaultFlags, rapidjson::UTF8<>> (text.c_str());
	return Convert (json, outDocument);
}
