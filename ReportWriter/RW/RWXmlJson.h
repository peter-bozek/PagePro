/*
 *  RWXmlJson.h
 *  ReportWriter
 *
 *  Report documents as JSON (RW_ParseReportJSON / RW_SaveReportJSON and the
 *  4D object variants). The mapping is lossless, every XML element becomes
 *
 *		{ "tag": "Report",
 *		  "attributes": { "Version": "1.0", "name": "Invoice" },
 *		  "children": [ { "tag": "StyleSet", ... }, "text", ... ] }
 *
 *  "attributes" and "children" are left out when empty; text between elements
 *  is a JSON string in "children", in document order (attributed text keeps
 *  its <SPAN> / <NL/> elements). Attribute values are written as strings; when
 *  reading, numbers and booleans are accepted too (true / false become 1 / 0,
 *  what the report readers expect), null means "attribute not set".
 */

#ifndef	_RWXmlJson_h_
# define	_RWXmlJson_h_

# include	"RWXml.h"
# include	<string>
# include	<string_view>

namespace	RWXmlJson
{
	// element (normally the document root) as JSON text
	RWString		ToJson (RWXmlNode inElement, bool inPretty = false);
	std::string		ToJsonUTF8 (RWXmlNode inElement, bool inPretty = true);		// files

	// replaces the content of outDocument with the element described by the JSON;
	// the result describes JSON syntax errors and structure errors (with the path
	// of the offending value, e.g. "children[2].attributes.size")
	RWXmlResult		FromJson (RWStringView inJson, RWXmlDocument &outDocument);
	RWXmlResult		FromJsonUTF8 (std::string_view inJson, RWXmlDocument &outDocument);	// files (BOM allowed)
}

#endif
