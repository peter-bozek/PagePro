/*
 *  ETExportTests.cpp
 *  ReportWriter
 *
 *  Export of a processed report (RWXML) to text, HTML, XML and JSON.
 *		tests/run_tests.sh
 */

# include	"ETReport.h"
# include	"ETReportData.h"
# include	"RWDataSourceProvider.h"

# include	<cstdio>
# include	<fstream>
# include	<sstream>

extern "C" void	PluginMain (PA_long32, PA_PluginParameters)	{}

// data source without 4D: values formatted as plain text
class	TestDataSource	:	public	RWDataSourceProvider
{
public:
	RWTextValue		FormatVariable (const RWValue &inVar, const CText) const override
	{
		RWString	text;
		inVar.GetTextValue (text, NULL);
		return text;
	}
};

static	int		sFailures = 0;
static	int		sChecks = 0;

# define	CHECK(cond)																\
	do {																			\
		sChecks++;																	\
		if (!(cond)) {																\
			sFailures++;															\
			std::printf ("FAILED %s:%d: %s\n", __FILE__, __LINE__, #cond);		\
		}																			\
	} while (0)

static	const char16_t	*kReport =
	u"<Report Version=\"1.0\" name=\"Sales &amp; more\">"
	u"<StyleSet><Style id=\"0\"/></StyleSet>"
	u"<Page>"
	u"<Text r=\"0;0;100;20\" id=\"t1\" name=\"first\">Žltý &lt;kôň&gt;</Text>"
	u"<Text r=\"0;30;100;50\" id=\"t2\">line 1<NL/>line 2</Text>"
	u"</Page>"
	u"</Report>";

static	std::string	Export (long inFlags, const std::string &inDir, const char *inName)
{
	RWXmlDocument	xml;
	if (!xml.LoadString (kReport).ok)
		return "<load failed>";

	TestDataSource			source;
	ETReportData			data (&xml);
	ETReport				report (source, data, inFlags);
	RWString				path = RWStr::FromUTF8 (inDir + "/" + inName);
	if (!report.ReportToFile (path))
		return "<write failed>";

	std::ifstream		file (inDir + "/" + inName, std::ios::binary);
	std::stringstream	content;
	content << file.rdbuf();
	return content.str();
}

int		main (int argc, char **argv)
{
	std::string	dir = argc > 1 ? argv[1] : ".";

	std::string	text = Export (eo_static | eo_text, dir, "export.txt");
	CHECK (text.find ("\xC5\xBD" "lt\xC3\xBD <k\xC3\xB4\xC5\x88>") != std::string::npos);		// "Žltý <kôň>" as UTF-8
	CHECK (text.find ("line 1\rline 2") != std::string::npos);

	std::string	html = Export (eo_static | eo_html, dir, "export.html");
	CHECK (html.find ("<title>Sales &amp; more</title>") != std::string::npos);
	CHECK (html.find ("<DIV name=\"first\" id=\"t1\">\xC5\xBD" "lt\xC3\xBD &lt;k\xC3\xB4\xC5\x88&gt;</DIV>") != std::string::npos);
	CHECK (html.find ("</html>") != std::string::npos);

	std::string	xmlOut = Export (eo_static | eo_xml, dir, "export.xml");
	CHECK (xmlOut.find ("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>") == 0);
	CHECK (xmlOut.find ("<Report Version=\"1.0\" Name=\"Sales &amp; more\">") != std::string::npos);
	CHECK (xmlOut.find ("name=\"first\" id=\"t1\"") != std::string::npos);
	CHECK (xmlOut.find (">\xC5\xBD" "lt\xC3\xBD &lt;k\xC3\xB4\xC5\x88&gt;</text>") != std::string::npos);
	RWXmlDocument	reread;
	CHECK (reread.LoadBuffer (xmlOut.data(), xmlOut.size()).ok);

	std::string	json = Export (eo_static | eo_json, dir, "export.json");
	RWJsonDocument	parsed;
	parsed.Parse (RWStr::FromUTF8 (json).c_str());
	CHECK (!parsed.HasParseError());
	CHECK (parsed.IsObject() && parsed.HasMember (u"sections") && parsed[u"sections"].IsArray());
	CHECK (parsed.IsObject() && parsed.HasMember (u"name") && RWString (parsed[u"name"].GetString()) == u"Sales & more");
	if (parsed.IsObject() && parsed[u"sections"].IsArray() && parsed[u"sections"].Size() > 0)
	{
		const RWJsonValue	&items = parsed[u"sections"][0][u"items"];
		CHECK (items.IsArray() && items.Size() == 2);
		CHECK (items.IsArray() && items.Size() == 2 && RWString (items[0][u"value"].GetString()) == u"Žltý <kôň>");
		CHECK (items.IsArray() && items.Size() == 2 && RWString (items[0][u"id"].GetString()) == u"t1");
		CHECK (items.IsArray() && items.Size() == 2 && RWString (items[1][u"value"].GetString()) == u"line 1\rline 2");
	}
	else
		CHECK (false);

	std::printf ("%d checks, %d failed\n", sChecks, sFailures);
	if (sFailures)
		std::printf ("--- text:\n%s\n--- html:\n%s\n--- xml:\n%s\n--- json:\n%s\n", text.c_str(), html.c_str(), xmlOut.c_str(), json.c_str());
	return sFailures == 0 ? 0 : 1;
}
