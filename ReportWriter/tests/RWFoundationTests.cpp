/*
 *  RWFoundationTests.cpp
 *  ReportWriter
 *
 *  Tests for RWString / RWXml / RWJson. Standalone, no 4D runtime needed:
 *		tests/run_tests.sh
 */

# include	"RWString.h"
# include	"RWString4D.h"
# include	"RWXml.h"
# include	"RWJson.h"

# include	<clocale>
# include	<cmath>
# include	<cstdio>
# include	<string>

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

static	std::string	Hex (const std::string &inBytes)
{
	std::string	result;
	char		buf[4];
	for (unsigned char ch : inBytes)
	{
		std::snprintf (buf, sizeof (buf), "%02X", ch);
		result += buf;
	}
	return result;
}


// ---------------------------------------------------------------------------
// Encoding conversions
// ---------------------------------------------------------------------------

static	void	TestUTF8 (void)
{
	const RWString	mixed = u"Až€\U0001F600";	// 1, 2, 3 and 4 byte UTF-8 sequences

	CHECK (Hex (RWStr::ToUTF8 (mixed)) == "41C5BEE282ACF09F9880");
	CHECK (RWStr::FromUTF8 (RWStr::ToUTF8 (mixed)) == mixed);
	CHECK (RWStr::FromUTF8 ("") == u"");
	CHECK (mixed.size() == 5);	// emoji is a surrogate pair

	// malformed input becomes U+FFFD, valid neighbours survive
	CHECK (RWStr::FromUTF8 ("a\x80" "b") == u"a�b");			// lone continuation byte
	CHECK (RWStr::FromUTF8 ("\xC0\x80") == u"��");			// overlong NUL (C0 is never valid)
	CHECK (RWStr::FromUTF8 ("\xE0\x80\x80") == u"�");			// overlong 3 byte form
	CHECK (RWStr::FromUTF8 ("\xED\xA0\x80") == u"�");			// encoded surrogate
	CHECK (RWStr::FromUTF8 ("\xE2\x82") == u"�");				// truncated at end
	CHECK (RWStr::FromUTF8 ("\xE2\x82" "x") == u"�x");			// truncated before ASCII
	CHECK (RWStr::FromUTF8 ("\xF5\x80\x80\x80") == u"����");	// above U+10FFFF

	// lone surrogates in UTF-16 are written as U+FFFD
	RWString	lone;
	lone.push_back (u'a');
	lone.push_back (char16_t (0xD800));
	lone.push_back (u'b');
	lone.push_back (char16_t (0xDC00));
	CHECK (Hex (RWStr::ToUTF8 (lone)) == "61EFBFBD62EFBFBD");
}

static	void	TestWide (void)
{
	const RWString	mixed = u"Až€\U0001F600";
	std::wstring	wide = RWStr::ToWide (mixed);

	if (sizeof (wchar_t) == 4)
	{
		CHECK (wide.size() == 4);
		CHECK (wide[3] == wchar_t (0x1F600));
		CHECK (RWStr::FromWide (std::wstring (1, wchar_t (0x110000))) == u"�");
		CHECK (RWStr::FromWide (std::wstring (1, wchar_t (0xD800))) == u"�");
	}
	else
	{
		CHECK (wide.size() == 5);
	}
	CHECK (RWStr::FromWide (wide) == mixed);
	CHECK (RWStr::FromWide (L"") == u"");
}

static	void	TestASCII (void)
{
	CHECK (RWStr::FromASCII ("abc") == u"abc");
	CHECK (RWStr::FromASCII ("a\xC5") == u"a�");
	CHECK (RWStr::ToASCII (u"ažb") == "a?b");
	CHECK (RWStr::ToASCII (u"ažb", '_') == "a_b");
}

static	void	Test4D (void)
{
	const PA_Unichar	text[] = { 'x', 0x017E, 0 };
	RWString			s = RWStr::FromPA (text);
	CHECK (s == u"xž");
	CHECK (RWStr::FromPA (text, 1) == u"x");
	CHECK (RWStr::FromPA ((const PA_Unichar *) NULL) == u"");
	CHECK (RWStr::ToPA (s)[1] == 0x017E);
	CHECK (RWStr::ToPA (s)[2] == 0);
}


// ---------------------------------------------------------------------------
// Comparison and transformations
// ---------------------------------------------------------------------------

static	void	TestCompare (void)
{
	CHECK (RWStr::Equals (u"Data", "Data"));
	CHECK (!RWStr::Equals (u"Data", "data"));
	CHECK (!RWStr::Equals (u"Dat", "Data"));
	CHECK (RWStr::EqualsNoCase (u"DATA", "data"));
	CHECK (RWStr::EqualsNoCase (u"DaTa", u"dAtA"));
	CHECK (!RWStr::EqualsNoCase (u"Ža", u"ža"));			// only ASCII is folded
	CHECK (RWStr::CompareNoCase (u"abc", u"ABD") < 0);
	CHECK (RWStr::CompareNoCase (u"abc", u"AB") > 0);
	CHECK (RWStr::CompareNoCase (u"", u"") == 0);
	CHECK (RWStr::StartsWith (u"r.left", u"r."));
	CHECK (!RWStr::StartsWith (u"r", u"r."));
	CHECK (RWStr::StartsWithNoCase (u"R.Left", u"r.l"));
	CHECK (RWStr::EndsWith (u"file.rwxml", u".rwxml"));
	CHECK (!RWStr::EndsWith (u"xml", u".rwxml"));
	CHECK (RWStr::Contains (u"a\U0001F600b", u"\U0001F600"));
}

static	void	TestTransform (void)
{
	CHECK (RWStr::ToLowerASCII (u"AbŽ") == u"abŽ");
	CHECK (RWStr::ToUpperASCII (u"abž") == u"ABž");
	CHECK (RWStr::Trim (u" \t x y \r\n") == u"x y");
	CHECK (RWStr::Trim (u"   ") == u"");

	std::vector<RWString>	parts = RWStr::Split (u"1;2;;4", u';');
	CHECK (parts.size() == 4);
	CHECK (parts.size() == 4 && parts[2].empty() && parts[3] == u"4");
	CHECK (RWStr::Split (u"", u';').size() == 1);

	CHECK (RWStr::ReplaceAll (u"a&b&c", u"&", u"&amp;") == u"a&amp;b&amp;c");
	CHECK (RWStr::ReplaceAll (u"aaa", u"aa", u"b") == u"ba");
	CHECK (RWStr::ReplaceAll (u"abc", u"", u"x") == u"abc");
}


// ---------------------------------------------------------------------------
// Numbers
// ---------------------------------------------------------------------------

static	void	TestNumbers (void)
{
	CHECK (RWStr::ToInteger (u" 42") == 42);
	CHECK (RWStr::ToInteger (u"-17") == -17);
	CHECK (RWStr::ToInteger (u"+5") == 5);
	CHECK (RWStr::ToInteger (u"0x1F") == 31);
	CHECK (RWStr::ToInteger (u"-0x10") == -16);
	CHECK (RWStr::ToInteger (u"0xZ") == 0);				// "0" then garbage, like strtol
	CHECK (RWStr::ToInteger (u"12abc") == 12);			// prefix, like sscanf
	CHECK (RWStr::ToInteger (u"010") == 10);				// decimal, no octal
	CHECK (!RWStr::ToInteger (u"abc"));
	CHECK (!RWStr::ToInteger (u""));
	CHECK (!RWStr::ToInteger (u"-"));
	CHECK (RWStr::ToInteger (u"9223372036854775807") == 9223372036854775807LL);
	CHECK (!RWStr::ToInteger (u"9223372036854775808"));
	CHECK (RWStr::ToInteger (u"-9223372036854775808") == (-9223372036854775807LL - 1));

	CHECK (RWStr::ToDouble (u"3.5") == 3.5);
	CHECK (RWStr::ToDouble (u" -2.5e3x") == -2500.0);
	CHECK (RWStr::ToDouble (u"1,5") == 1.0);				// always "." - never the user's locale
	CHECK (!RWStr::ToDouble (u"."));
	CHECK (!RWStr::ToDouble (u"ž"));

	CHECK (RWStr::FromInteger (0) == u"0");
	CHECK (RWStr::FromInteger (-9223372036854775807LL - 1) == u"-9223372036854775808");
	CHECK (RWStr::FromDouble (1.5) == u"1.5");
	CHECK (RWStr::FromDouble (0.1, "%.2f") == u"0.10");
	CHECK (RWStr::FromDouble (1e300, "%f").size() == 308);

	// a decimal comma locale must not change anything
	const char	*saved = std::setlocale (LC_ALL, NULL);
	std::string	savedLocale = saved ? saved : "C";
	if (std::setlocale (LC_ALL, "de_DE.UTF-8") || std::setlocale (LC_ALL, "sk_SK.UTF-8"))
	{
		CHECK (RWStr::FromDouble (1.5) == u"1.5");
		CHECK (RWStr::ToDouble (u"1.5") == 1.5);
	}
	else
		std::printf ("note: no decimal comma locale installed, locale test skipped\n");
	std::setlocale (LC_ALL, savedLocale.c_str());
}


// ---------------------------------------------------------------------------
// XML
// ---------------------------------------------------------------------------

static	const char16_t	*kSample =
	u"<?xml version=\"1.0\"?>\n"
	u"<Report name=\"Mesačný prehľad \U0001F600\" rows=\"12\" scale=\"1.25\" visible=\"1\" locked=\"no\">\n"
	u"\t<Section type=\"Header\" />\n"
	u"\t<!-- comment -->\n"
	u"\t<Section type=\"Body\"><Text>a&amp;b<NL/>c</Text></Section>\n"
	u"\t<Data> </Data>\n"
	u"\t<Section type=\"Footer\" />\n"
	u"</Report>\n";

static	void	TestXmlRead (void)
{
	RWXmlDocument	doc;
	RWXmlResult		result = doc.LoadString (kSample);
	CHECK (result.ok);

	RWXmlNode	root = doc.Root();
	CHECK (root.NameIs ("Report"));
	CHECK (!root.NameIs ("Repor"));
	CHECK (root.Name() == u"Report");
	CHECK (root.Attr (u"name") == u"Mesačný prehľad \U0001F600");
	CHECK (root.Attr (u"missing", u"def") == u"def");
	CHECK (root.AttrInt (u"rows") == 12);
	CHECK (root.AttrInt (u"missing", -1) == -1);
	CHECK (root.AttrInt (u"name", 7) == 7);			// not a number
	CHECK (root.AttrDouble (u"scale") == 1.25);
	CHECK (root.AttrBool (u"visible") == true);
	CHECK (root.AttrBool (u"locked", true) == false);
	CHECK (root.HasAttr (u"rows"));
	CHECK (!root.HasAttr (u"cols"));

	std::vector<std::pair<RWString, RWString>>	attrs = root.Attributes();
	CHECK (attrs.size() == 5);
	CHECK (attrs.size() == 5 && attrs[1].first == u"rows" && attrs[1].second == u"12");

	RWString	types;
	for (RWXmlNode section : root.Children (u"Section"))
		types += section.Attr (u"type") + u",";
	CHECK (types == u"Header,Body,Footer,");

	int	elements = 0;
	for (RWXmlNode child : root.Children())
	{
		(void) child;
		elements++;
	}
	CHECK (elements == 4);		// comment skipped

	RWXmlNode	text = root.Child (u"Section").NextElement (u"Section").Child (u"Text");
	CHECK (text.Text() == u"a&b");
	RWString	walked;
	for (RWXmlNode n = text.FirstChild(); n; n = n.NextSibling())
		walked += n.IsText() ? n.Value() : u"[" + n.Name() + u"]";
	CHECK (walked == u"a&b[NL]c");

	CHECK (root.Child (u"Data").Text() == u" ");		// single white space content is kept
	CHECK (root.FirstElement().Attr (u"type") == u"Header");
	CHECK (root.FirstElement().NextElement().Attr (u"type") == u"Body");
	CHECK (text.Parent().Attr (u"type") == u"Body");

	// null nodes are safe
	RWXmlNode	missing = root.Child (u"Nothing");
	CHECK (!missing);
	CHECK (missing.Attr (u"x", u"d") == u"d");
	CHECK (missing.AttrInt (u"x", 3) == 3);
	CHECK (missing.Text().empty());
	CHECK (!missing.Child (u"y"));
	CHECK (missing.Children().begin() == missing.Children().end());

	RWXmlDocument	bad;
	RWXmlResult		badResult = bad.LoadString (u"<a><b></a>");
	CHECK (!badResult.ok);
	CHECK (!badResult.description.empty());
}

static	void	TestXmlWrite (void)
{
	RWXmlDocument	doc;
	RWXmlNode		root = doc.Node().Append (u"Report");
	root.SetAttr (u"name", u"Žltý <kôň> & \"x\" \U0001F600");
	root.SetAttrInt (u"rows", -3);
	root.SetAttrDouble (u"scale", 0.5);
	root.SetAttrBool (u"visible", true);
	root.SetAttr (u"rows", u"4");					// replaces, does not duplicate
	CHECK (root.Attributes().size() == 4);
	CHECK (root.RemoveAttr (u"scale"));
	CHECK (!root.RemoveAttr (u"scale"));

	RWXmlNode	text = root.Append (u"Text");
	text.AppendText (u"line 1");
	text.Append (u"NL");
	text.AppendText (u"line 2 < 3");
	root.Append (u"Empty");
	root.Append (u"Value").SetText (u"42");

	// UTF-16 round trip
	RWString		saved = doc.SaveString();
	RWXmlDocument	copy;
	CHECK (copy.LoadString (saved).ok);
	CHECK (copy.Root().Attr (u"name") == u"Žltý <kôň> & \"x\" \U0001F600");
	CHECK (copy.Root().AttrInt (u"rows") == 4);
	CHECK (copy.Root().AttrBool (u"visible"));
	CHECK (copy.Root().Child (u"Value").Text() == u"42");
	CHECK (copy.SaveString() == saved);
	CHECK (!RWStr::StartsWith (saved, u"<?xml"));		// no declaration by default for 4D text
	CHECK (RWStr::StartsWith (doc.SaveString (true, true), u"<?xml"));

	// mixed content is not re-indented
	CHECK (RWStr::Contains (saved, u"<Text>line 1<NL />line 2 &lt; 3</Text>"));

	// UTF-8 output
	std::string	utf8 = doc.SaveUTF8 (false);
	CHECK (utf8.compare (0, 5, "<?xml") == 0);
	CHECK (utf8.find ("\xC5\xBD" "lt\xC3\xBD") != std::string::npos);		// "Žltý" as UTF-8
	CHECK (utf8.find ('\n') == std::string::npos || utf8.find ('\n') == utf8.find ("?>") + 2);	// raw: only after declaration

	RWXmlDocument	fromBytes;
	CHECK (fromBytes.LoadBuffer (utf8.data(), utf8.size()).ok);
	CHECK (fromBytes.Root().Attr (u"name") == copy.Root().Attr (u"name"));

	// UTF-16LE with BOM is detected too
	std::string	utf16bytes ("\xFF\xFE", 2);
	RWString	small = u"<a v=\"ž\"/>";
	for (char16_t ch : small)
	{
		utf16bytes.push_back (char (ch & 0xFF));
		utf16bytes.push_back (char (ch >> 8));
	}
	RWXmlDocument	fromUTF16;
	CHECK (fromUTF16.LoadBuffer (utf16bytes.data(), utf16bytes.size()).ok);
	CHECK (fromUTF16.Root().Attr (u"v") == u"ž");

	// copy between documents, remove
	RWXmlDocument	other;
	RWXmlNode		otherRoot = other.Node().Append (u"Other");
	otherRoot.AppendCopy (copy.Root().Child (u"Text"));
	CHECK (otherRoot.Child (u"Text").Text() == u"line 1");
	CHECK (otherRoot.Remove (otherRoot.Child (u"Text")));
	CHECK (!otherRoot.Child (u"Text"));
}

static	void	TestXmlFile (const std::string &inTempDir)
{
	RWString		path = RWStr::FromUTF8 (inTempDir) + u"/súbor \U0001F600.rwxml";
	RWXmlDocument	doc;
	doc.Node().Append (u"Report").SetAttr (u"name", u"Ďakujem");
	CHECK (doc.SaveFile (path));

	RWXmlDocument	loaded;
	CHECK (loaded.LoadFile (path).ok);
	CHECK (loaded.Root().Attr (u"name") == u"Ďakujem");

	FILE	*fd = std::fopen (RWStr::ToUTF8 (path).c_str(), "rb");
	CHECK (fd != NULL);
	if (fd)
	{
		char	head[64] = {};
		size_t	n = std::fread (head, 1, sizeof (head) - 1, fd);
		std::fclose (fd);
		CHECK (n > 5 && std::string (head, 5) == "<?xml");			// UTF-8, no BOM
	}
	std::remove (RWStr::ToUTF8 (path).c_str());

	RWXmlDocument	none;
	CHECK (!none.LoadFile (RWStr::FromUTF8 (inTempDir) + u"/does-not-exist.xml").ok);
}


// ---------------------------------------------------------------------------
// JSON
// ---------------------------------------------------------------------------

static	void	TestJson (void)
{
	RWJsonDocument	doc;
	doc.SetObject();
	RWJsonAllocator	&alloc = doc.GetAllocator();

	doc.AddMember (RWJson::String (u"názov", alloc), RWJson::String (u"Prehľad \"Q1\" \U0001F600", alloc), alloc);
	doc.AddMember (RWJson::String (u"rows", alloc), RWJsonValue (12), alloc);
	RWJsonValue	list (rapidjson::kArrayType);
	list.PushBack (RWJsonValue (1.5), alloc);
	list.PushBack (RWJsonValue (true), alloc);
	doc.AddMember (RWJson::String (u"list", alloc), list, alloc);

	std::string	utf8 = RWJson::ToUTF8 (doc);
	CHECK (utf8 == "{\"n\xC3\xA1zov\":\"Preh\xC4\xBE" "ad \\\"Q1\\\" \xF0\x9F\x98\x80\",\"rows\":12,\"list\":[1.5,true]}");

	RWString	utf16 = RWJson::ToString (doc);
	CHECK (RWStr::ToUTF8 (utf16) == utf8);

	std::string	pretty = RWJson::ToUTF8 (doc, true);
	CHECK (pretty.find ("\n\t\"rows\": 12") != std::string::npos);

	// parse UTF-16 text (e.g. from 4D) back
	RWJsonDocument	parsed;
	parsed.Parse (utf16.c_str());
	CHECK (!parsed.HasParseError());
	CHECK (parsed.IsObject() && parsed[u"rows"].GetInt() == 12);
	CHECK (RWString (parsed[u"názov"].GetString(), parsed[u"názov"].GetStringLength()) == u"Prehľad \"Q1\" \U0001F600");
}


int		main (int argc, char **argv)
{
	std::string	tempDir = argc > 1 ? argv[1] : ".";

	TestUTF8();
	TestWide();
	TestASCII();
	Test4D();
	TestCompare();
	TestTransform();
	TestNumbers();
	TestXmlRead();
	TestXmlWrite();
	TestXmlFile (tempDir);
	TestJson();

	std::printf ("%d checks, %d failed (wchar_t is %zu bytes)\n", sChecks, sFailures, sizeof (wchar_t));
	return sFailures == 0 ? 0 : 1;
}
