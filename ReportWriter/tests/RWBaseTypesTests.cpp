/*
 *  RWBaseTypesTests.cpp
 *  ReportWriter
 *
 *  Tests for the RWBaseTypes core (SPoint / SRect / SRGBColor text form,
 *  RWValue, RWTools). Linked with the 4D plugin API stubs, nothing calls 4D.
 *		tests/run_tests.sh
 */

# include	"RWBaseTypes.h"
# include	"RWStyle.h"
# include	"RWDataProvider.h"
# include	"RW4DText.h"

# include	<cstdio>
# include	<cstring>

// entry point the 4D plugin API references; never called here
extern "C" void	PluginMain (PA_long32, PA_PluginParameters)	{}

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


static	void	TestGeometry (void)
{
	SPoint	pt;
	pt = u"1.5;-2";
	CHECK (pt.h == 1.5 && pt.v == -2);
	pt = u"3,4";									// older comma form
	CHECK (pt.h == 3 && pt.v == 4);
	pt = u"";
	CHECK (pt.h == 0 && pt.v == 0);
	CHECK (SPoint (1.25, 7.0).ToString() == u"1.25;7");

	SRect	r;
	r = u"10;20;110;220";							// left;top;right;bottom
	CHECK (r.left == 10 && r.top == 20 && r.right == 110 && r.bottom == 220);
	r = u"1,2,3,4";
	CHECK (r.left == 1 && r.top == 2 && r.right == 3 && r.bottom == 4);
	CHECK (SRect (20.0, 10.0, 220.0, 110.5).ToString() == u"10;20;110.5;220");
	SRect	r2;
	r2 = r.ToString();
	CHECK (r2 == r);
}

static	void	TestColor (void)
{
	CHECK (SRGBColor (u"#FF0000") == cRedColor);
	CHECK (SRGBColor (u"  #ff0000") == cRedColor);
	SRGBColor	c (u"#80FF0000");
	CHECK (c.alpha == 0x8080 && c.red == 0xFFFF && c.green == 0 && c.blue == 0);
	CHECK (SRGBColor (u"red") == cRedColor);
	CHECK (SRGBColor (u"lightgray") == cLightGrayColor);
	CHECK (SRGBColor (u"Red") == cBlackColor);		// names are case sensitive
	CHECK (SRGBColor (u"transparent") == cEmptyColor);
	c = SRGBColor (u"1,0,0.5");
	CHECK (c.red == 65535 && c.green == 0 && c.blue == 32767 && c.alpha == 65535);
	c = SRGBColor (u"65535,0,0");
	CHECK (c == cRedColor);
	c = SRGBColor (u"0xFF00FF00");
	CHECK (c == cGreenColor);
	CHECK (SRGBColor (u"") == SRGBColor (0, 0, 0, 0xFFFF));

	CHECK (cRedColor.ToString() == u"#ffff0000");
	CHECK (SRGBColor (cHighlightColor.ToString()) == SRGBColor (0, 0, 0x8080, 0x8080));	// 8 bit precision
	CHECK (SRGBColor (cBlueColor.ToString()) == cBlueColor);
}

static	void	TestValue (void)
{
	RWValue	undefined;
	CHECK (undefined.GetKind() == RWValue::eValue_Undefined);
	CHECK (undefined.IsEmpty());

	RWValue	text;
	text.SetText (u"Žltý kôň");
	CHECK (text.GetKind() == RWValue::eValue_Text);
	CHECK (text.GetText() == u"Žltý kôň");
	CHECK (!text.IsEmpty());

	RWValue	copy (text);
	CHECK (copy == text);
	copy.SetText (u"other");
	CHECK (copy != text);
	CHECK (text.GetText() == u"Žltý kôň");

	RWValue	moved;
	moved.Detach (text);
	CHECK (moved.GetText() == u"Žltý kôň");
	CHECK (text.GetText().empty());

	RWValue	number (42L);
	CHECK (!number.IsEmpty());
	number.SetText (u"x");							// switching kind frees nothing wrong
	number.SetInteger (7);
	CHECK (number.GetInteger() == 7 && number.GetText().empty());

	// BLOB: deep copy on clone, shared on attach
	char	bytes[] = "binary\0data";
	RWValue	blob (RWValue::eValue_BLOB, bytes, sizeof (bytes));
	CHECK (blob.GetBlobSize() == sizeof (bytes));
	RWValue	blobCopy (blob);
	CHECK (blobCopy == blob);
	CHECK (blobCopy.GetBlobData() != blob.GetBlobData());
	RWValue	blobShared;
	blobShared.Attach (blob);
	CHECK (blobShared.GetBlobData() == blob.GetBlobData());
	blobShared.Free();								// not owner, data must stay valid
	CHECK (std::memcmp (blob.GetBlobData(), bytes, sizeof (bytes)) == 0);
	RWValue	emptyBlob (RWValue::eValue_BLOB, NULL, 0);
	CHECK (emptyBlob.IsEmpty());
}

static	void	TestTextValue (void)
{
	RWString	out = u"unchanged";
	RWValue		v;

	v.GetTextValue (out, NULL);
	CHECK (out == u"<NULL>");

	v.SetInteger (42);
	v.GetTextValue (out, NULL);
	CHECK (out == u"42");
	v.GetTextValue (out, "%05ld");
	CHECK (out == u"00042");

	v.SetReal (2.5);
	v.GetTextValue (out, NULL);
	CHECK (out == u"2.5");
	v.GetTextValue (out, "%.2f");
	CHECK (out == u"2.50");

	v.SetBoolean (true);
	v.GetTextValue (out, NULL);
	CHECK (out == u"1");

	v.SetInteger (7 | (3 << 5) | (2024 << 9), RWValue::eValue_Date);
	v.GetTextValue (out, NULL);
	CHECK (out == u"2024-03-07");

	v.SetInteger (3600 + 2 * 60 + 5, RWValue::eValue_Time);
	v.GetTextValue (out, NULL);
	CHECK (out == u"01.02.05");

	v.SetText (u"text");
	v.GetTextValue (out, NULL);
	CHECK (out == u"text");

	char	data[4] = { 1, 2, 3, 4 };
	v.SetBlob (data, sizeof (data), false);
	v.GetTextValue (out, NULL);
	CHECK (out == u"<BLOB>");
	out = u"kept";
	v.GetTextValue (out, "");						// empty label leaves the value alone
	CHECK (out == u"kept");
	v.Free();
}

static	void	TestCoerce (void)
{
	RWValue	v;

	v.SetText (u" 42");
	CHECK (v.CoerceValue (RWValue::eValue_Integer) && v.GetKind() == RWValue::eValue_Integer && v.GetInteger() == 42);

	v.SetText (u"#FF0000");
	CHECK (v.CoerceValue (RWValue::eValue_Integer) && (unsigned long) v.GetInteger() == 0xFFFF0000UL);

	v.SetText (u"abc");
	CHECK (!v.CoerceValue (RWValue::eValue_Integer));

	v.SetText (u"");
	CHECK (v.CoerceValue (RWValue::eValue_Integer) && v.GetInteger() == 0);

	v.SetText (u"2.5");
	CHECK (v.CoerceValue (RWValue::eValue_Real) && v.GetReal() == 2.5);

	v.SetText (u"abc");
	CHECK (v.CoerceValue (RWValue::eValue_Real) && v.GetKind() == RWValue::eValue_Text);	// unchanged, as before

	v.SetText (u"0");
	CHECK (v.CoerceValue (RWValue::eValue_Boolean) && !v.GetBoolean());
	v.SetText (u"x");
	CHECK (v.CoerceValue (RWValue::eValue_Boolean) && v.GetBoolean());

	v.SetReal (3.9);
	CHECK (v.CoerceValue (RWValue::eValue_Integer) && v.GetInteger() == 3);
	CHECK (!v.CoerceValue (RWValue::eValue_BLOB));
}

static	void	TestXmlText (void)
{
	RWXmlDocument	doc;
	RWXmlNode		root = doc.Node().Append (u"Text");

	RWTools::WriteText (root, u"line 1\rline 2\r\n<3>\r");
	RWString	saved = doc.SaveString (false);
	CHECK (saved == u"<Text>line 1<NL/>line 2<NL/>\n&lt;3&gt;<NL/></Text>");

	RWXmlDocument	loaded;
	CHECK (loaded.LoadString (saved).ok);
	CHECK (RWTools::ParseIntoText (loaded.Root()) == u"line 1\rline 2\r\n<3>\r");

	// nested <Data> elements
	CHECK (loaded.LoadString (u"<Text>a<Data>b<NL/>c</Data>d<Other>x</Other></Text>").ok);
	CHECK (RWTools::ParseIntoText (loaded.Root()) == u"ab\rcd");

	RWXmlNode	empty = doc.Node().Append (u"Empty");
	RWTools::WriteText (empty, u"");
	CHECK (!empty.FirstChild());
}

static	void	TestXmlData (void)
{
	unsigned char	bytes[300];
	for (int i = 0; i < 300; i++)
		bytes[i] = (unsigned char) (i * 13);

	SBlob	in;
	in.fData = bytes;
	in.fSize = sizeof (bytes);

	RWXmlDocument	doc;
	RWXmlNode		node = doc.Node().Append (u"Data");
	RWTools::WriteData (node, in);
	CHECK (RWStr::Contains (node.Text(), u" "));		// grouped

	RWXmlDocument	loaded;
	CHECK (loaded.LoadString (doc.SaveString()).ok);
	SBlob	out;
	out.Init();
	RWTools::ReadData (loaded.Root(), out);
	CHECK (out.fSize == sizeof (bytes) && std::memcmp (out.fData, bytes, sizeof (bytes)) == 0);
	out.Free();

	// layout written by the old FILE* writer: tab + 128 chars + CRLF per line
	RWString	legacy = u"<Data>\r\n\t" + RWStr::Base64Encode (bytes, 96) + u"\r\n\t" + RWStr::Base64Encode (bytes + 96, 204) + u"\r\n</Data>";
	CHECK (loaded.LoadString (legacy).ok);
	RWTools::ReadData (loaded.Root(), out);
	CHECK (out.fSize == sizeof (bytes) && std::memcmp (out.fData, bytes, sizeof (bytes)) == 0);
	out.Free();

	CHECK (RWTools::FindInList (u"PDF", RWValue::GetPictFormats()) == 2);
	CHECK (RWTools::FindInList (u"pdf", RWValue::GetPictFormats()) == -1);
	CHECK (RWTools::FindInList (u"", RWValue::GetPictFormats()) == -1);
}

static	void	TestAttributed (void)
{
	CHECK (RWTools::EscapeAttributedString (u"a<b>&\"c'") == u"a&lt;b&gt;&amp;&quot;c'");

	std::vector<long>	attrs;
	CHECK (RWTools::SplitAttributedString (u"a<b>X</b>", &attrs) == u"aX");
	CHECK ((attrs == std::vector<long> { 6, 2, 2, 1, 6, 2 }));

	CHECK (RWTools::SplitAttributedString (u"x&lt;y&amp;z<BR/>w", NULL) == u"x<y&z\rw");
	CHECK (RWTools::SplitAttributedString (u"A&#x41;B", NULL) == u"AAB");
	CHECK (RWTools::SplitAttributedString (u"<%var%>", NULL) == u"<%var%>");
	CHECK (RWTools::SplitAttributedString (u"a<b", NULL) == u"a<b");
	CHECK (RWTools::SplitAttributedString (u"a&", NULL) == u"a&");			// entity at the very end
	CHECK (RWTools::SplitAttributedString (u"", &attrs) == u"");
	CHECK ((attrs == std::vector<long> { 2, 0 }));

	double		size = 0;
	int			sign = 0, style = 0;
	SRGBColor	color;
	RWString	font;

	CHECK (RWTools::ParseAttributedStringAttribute (u"b>text", size, sign, style, color, font) == 'B' && style == RWStyle::st_bold);
	CHECK (RWTools::ParseAttributedStringAttribute (u"/b>", size, sign, style, color, font) == 0);
	CHECK (RWTools::ParseAttributedStringAttribute (u"s +2.5>", size, sign, style, color, font) == 'S' && size == 2.5 && sign == '+');
	CHECK (RWTools::ParseAttributedStringAttribute (u"c #FF0000>", size, sign, style, color, font) == 'C' && color == cRedColor);
	CHECK (RWTools::ParseAttributedStringAttribute (u"f 'Times New Roman'>", size, sign, style, color, font) == 'F' && font == u"Times New Roman");
	CHECK (RWTools::ParseAttributedStringAttribute (u"f Arial>", size, sign, style, color, font) == 'F' && font == u"Arial");

	style = 0;
	int	span = RWTools::ParseAttributedStringAttribute (u"SPAN STYLE=\"font-family:'Courier New';font-size:12pt;font-weight:bold;"
														u"font-style:italic;text-decoration:underline;color:#00FF00\">x", size, sign, style, color, font);
	CHECK (span == (256 | 1 | 2 | 4 | 8 | 16 | 32));
	CHECK (font == u"Courier New");
	CHECK (size == 12 && sign == 0);
	CHECK (style == (RWStyle::st_bold | RWStyle::st_italic | RWStyle::st_underline));	// value was ignored before
	CHECK (color == cGreenColor);
	CHECK (RWTools::ParseAttributedStringAttribute (u"SPAN STYLE=\"font-weight:normal\">", size, sign, style, color, font) == (256 | 4));
	CHECK ((style & RWStyle::st_bold) == 0);
	CHECK (RWTools::ParseAttributedStringAttribute (u"SPAN CLASS=\"x\">", size, sign, style, color, font) == 0);
}

static	void	TestVariables (void)
{
	RWString	text = u"Total: <%sum;%.2f%> and <%count%>";
	long		start = 0, end = 0;
	RWString	name, format = u"stale";

	CHECK (RWTools::ParseTextForVar (false, text, long (text.size()), start, end, name, format));
	CHECK (name == u"sum" && format == u"%.2f" && start == 7 && end == 19);
	start = end;
	CHECK (RWTools::ParseTextForVar (false, text, long (text.size()), start, end, name, format));
	CHECK (name == u"count" && format.empty() && start == 24 && end == long (text.size()));
	start = end;
	CHECK (!RWTools::ParseTextForVar (false, text, long (text.size()), start, end, name, format));

	RWString	attributed = u"x &lt;%na&amp;me;fmt%&gt; y";
	start = 0;
	CHECK (RWTools::ParseTextForVar (true, attributed, long (attributed.size()), start, end, name, format));
	CHECK (name == u"na&me" && format == u"fmt" && start == 2 && end == long (attributed.size()) - 2);

	RWString	unterminated = u"<%abc";
	start = 0;
	CHECK (!RWTools::ParseTextForVar (false, unterminated, long (unterminated.size()), start, end, name, format));
	RWString	unterminatedAttr = u"&lt;%abc";
	start = 0;
	CHECK (!RWTools::ParseTextForVar (true, unterminatedAttr, long (unterminatedAttr.size()), start, end, name, format));

	RWString	emptyFirst = u"<%%>x<%v%>";
	start = 0;
	CHECK (RWTools::ParseTextForVar (false, emptyFirst, long (emptyFirst.size()), start, end, name, format));
	CHECK (name == u"v" && start == 5);

	RWString	beyond = u"abc <%v%>";
	start = 0;
	CHECK (!RWTools::ParseTextForVar (false, beyond, 3, start, end, name, format));	// starts after inTextLen
}

static	void	TestReadNumber (void)
{
	long	l = 7;
	CHECK (RWStr::ReadNumber (u" 42", l) && l == 42);
	CHECK (!RWStr::ReadNumber (u"x", l) && l == 42);			// unchanged on failure
	float	f = 1;
	CHECK (RWStr::ReadNumber (u"2.5pt", f) && f == 2.5f);
	bool	b = false;
	CHECK (RWStr::ReadNumber (u"3", b) && b);
	CHECK (RWStr::ReadNumber (u"0", b) && !b);
	int		i = 0;
	CHECK (RWStr::ReadNumber (u"0x10", i) && i == 16);
}

static	void	TestDataProvider (void)
{
	RWDataProvider	source;
	RWValue			v;

	v.SetText (u"Žltý\rkôň");
	RWDataID	textID = source.AddObject (v);
	v.SetInteger (-42);
	RWDataID	intID = source.AddObject (v);
	v.SetReal (3.25);
	RWDataID	realID = source.AddObject (v);
	v.SetInteger (15 | (6 << 5) | (2025 << 9), RWValue::eValue_Date);
	RWDataID	dateID = source.AddObject (v);
	v.SetInteger (10 * 3600 + 20 * 60 + 30, RWValue::eValue_Time);
	RWDataID	timeID = source.AddObject (v);
	char	bytes[] = { 1, 2, 3, 0, 5 };
	RWValue	blob (RWValue::eValue_PicturePNG, bytes, sizeof (bytes));
	RWDataID	blobID = source.AddObject (blob);

	RWDataID	tableID = source.AddTableObject (2, false);
	v.SetText (u"a1");
	source.PutTableCellData (tableID, 1, 1, v);
	v.SetInteger (7);
	source.PutTableCellData (tableID, 2, 2, v);

	RWXmlDocument	doc;
	RWXmlNode		data = doc.Node().Append (u"ReportData");
	source.Write (data);

	RWXmlDocument	loaded;
	CHECK (loaded.LoadString (doc.SaveString()).ok);
	RWDataProvider	parsed;
	parsed.Parse (loaded.Root());

	RWValue	out;
	CHECK (parsed.GetObject (textID, out) && out.GetKind() == RWValue::eValue_Text && out.GetText() == u"Žltý\rkôň");
	CHECK (parsed.GetObject (intID, out) && out.GetKind() == RWValue::eValue_Integer && out.GetInteger() == -42);
	CHECK (parsed.GetObject (realID, out) && out.GetKind() == RWValue::eValue_Real && out.GetReal() == 3.25);
	CHECK (parsed.GetObject (dateID, out) && out.GetKind() == RWValue::eValue_Date && out.GetInteger() == (15 | (6 << 5) | (2025 << 9)));
	CHECK (parsed.GetObject (timeID, out) && out.GetKind() == RWValue::eValue_Time && out.GetInteger() == 10 * 3600 + 20 * 60 + 30);
	CHECK (parsed.GetObject (blobID, out) && out.GetKind() == RWValue::eValue_PicturePNG && out.GetBlobSize() == sizeof (bytes)
		&& memcmp (out.GetBlobData(), bytes, sizeof (bytes)) == 0);

	CHECK (parsed.GetTableColumnCount (tableID) == 2);
	CHECK (parsed.GetTableCellData (tableID, 1, 1, out) && out.GetText() == u"a1");
	CHECK (parsed.GetTableCellData (tableID, 2, 2, out) && out.GetInteger() == 7);

	// kind 4 was UTF-8 text (eValue_XMLText) in older data
	RWXmlDocument	legacy;
	CHECK (legacy.LoadString (u"<ReportData><Objects><v id=\"3\" k=\"4\">old text</v></Objects></ReportData>").ok);
	RWDataProvider	old;
	old.Parse (legacy.Root());
	CHECK (old.GetObject (3, out) && out.GetKind() == RWValue::eValue_Text && out.GetText() == u"old text");
}


static	void	TestStyledText (void)
{
	CHECK (RW4DStyledText (u"Hello").toXMLString() == u"Hello");
	CHECK (RW4DStyledText (u"").toXMLString() == u"");

	RWString	bold = u"a<SPAN STYLE=\"font-weight:bold\">bc</SPAN>d";
	RW4DStyledText	t1 (bold);
	CHECK (t1.mPlainText == u"abcd");
	CHECK (t1.toXMLString() == bold);

	RWString	full = u"<SPAN STYLE=\"font-family:'Arial';font-size:12.00pt;color:#FF0000\">x</SPAN>";
	RW4DStyledText	t2 (full);
	CHECK (t2.mSpanList.size() == 1 && t2.mSpanList[0]->mFont == u"Arial" && t2.mSpanList[0]->mSize == 12);
	CHECK (t2.mSpanList.size() == 1 && t2.mSpanList[0]->mHasColor && t2.mSpanList[0]->mColor == 0xFFFF0000UL);
	CHECK (t2.toXMLString() == full);

	RW4DStyledText	t3 (u"a<BR/>b");
	CHECK (t3.mPlainText == u"a\rb");
	CHECK (t3.toXMLString() == u"a<BR/>b");

	RW4DStyledText	t4 (u"abcd");
	t4.AddSpan (RWSpan (0, 4, RWStyle::st_italic, 0, u"", 0, false), RWSpan::mode_add);
	CHECK (t4.toXMLString() == u"<SPAN STYLE=\"font-style:italic\">abcd</SPAN>");

	// removing part of a span: the old code kept the full length and repeated "ef"
	RW4DStyledText	t5 (u"<SPAN STYLE=\"font-weight:bold\">abcd</SPAN>ef");
	t5.RemoveSpan (RWSpan (2, 2, RWStyle::st_bold, 0, u"", 0, false));
	CHECK (t5.toXMLString() == u"<SPAN STYLE=\"font-weight:bold\">ab</SPAN>cdef");
}


int		main (void)
{
	TestGeometry();
	TestColor();
	TestValue();
	TestTextValue();
	TestCoerce();
	TestXmlText();
	TestXmlData();
	TestAttributed();
	TestVariables();
	TestReadNumber();
	TestDataProvider();
	TestStyledText();

	std::printf ("%d checks, %d failed\n", sChecks, sFailures);
	return sFailures == 0 ? 0 : 1;
}
