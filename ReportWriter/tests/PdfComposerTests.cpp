/*
 *  PdfComposerTests.cpp
 *  ReportWriter
 *
 *  PDF output (RWPoDoFoPageComposer, PoDoFo 1.0): a page with text in several
 *  styles, shapes and pictures is written, loaded again with PoDoFo and checked
 *  for subset fonts, image encodings, compression and the text. Mac only (the
 *  test pictures are made with ImageIO).
 *		tests/run_tests.sh
 */

# include	"RWPoDoFoPageComposer.h"
# include	"RWPdfFonts.h"
# include	"RWStyle.h"
# include	"RWXml.h"

#undef	CreateFont
#undef	DrawText
# include	<podofo/podofo.h>

# include	<CoreGraphics/CoreGraphics.h>
# include	<ImageIO/ImageIO.h>

# include	<cmath>
# include	<cstdio>
# include	<fstream>
# include	<sstream>
# include	<string>

using namespace PoDoFo;

extern "C" void	PluginMain (PA_long32, PA_PluginParameters)	{}

// the plugin converts other picture formats with 4D; the test only uses JPEG and PNG
void	RW_ConvertPictureForPrinting (RWValue &, bool)	{}
void	RW_RunInMainThread (void (*inFunction) (void*), void *inData)	{ inFunction (inData); }

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


// 64 x 48 RGBA picture: colour gradient, alpha 0 on the left to 255 on the right
static	std::string		MakePicture (CFStringRef inType)
{
	const size_t	width = 64, height = 48;
	CGColorSpaceRef	space = CGColorSpaceCreateDeviceRGB();
	CGContextRef	context = CGBitmapContextCreate (NULL, width, height, 8, 0, space, kCGImageAlphaPremultipliedLast);
	unsigned char	*pixels = static_cast <unsigned char*> (CGBitmapContextGetData (context));
	const size_t	rowBytes = CGBitmapContextGetBytesPerRow (context);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
		{
			unsigned char	*p = pixels + y * rowBytes + x * 4;
			const unsigned	alpha = unsigned (x * 255 / (width - 1));
			p [0] = (unsigned char) (x * 4 * alpha / 255);
			p [1] = (unsigned char) (y * 5 * alpha / 255);
			p [2] = (unsigned char) (128 * alpha / 255);
			p [3] = (unsigned char) alpha;
		}
	CGImageRef		image = CGBitmapContextCreateImage (context);

	CFMutableDataRef			data = CFDataCreateMutable (kCFAllocatorDefault, 0);
	CGImageDestinationRef		destination = CGImageDestinationCreateWithData (data, inType, 1, NULL);
	CGImageDestinationAddImage (destination, image, NULL);
	CGImageDestinationFinalize (destination);
	std::string	result (reinterpret_cast <const char*> (CFDataGetBytePtr (data)), size_t (CFDataGetLength (data)));

	CFRelease (destination);
	CFRelease (data);
	CGImageRelease (image);
	CGContextRelease (context);
	CGColorSpaceRelease (space);
	return result;
}


static	std::unique_ptr<RWStyle>	MakeStyle (const char16_t *inXML)
{
	RWXmlDocument	xml;
	xml.LoadString (inXML);
	return std::unique_ptr<RWStyle> (new RWStyle (NULL, xml.Root()));
}


static	void	TestFontPrograms (void)
{
	// a regular face; the rebuilt sfnt is a valid font for PoDoFo / FreeType
	RWPdfFontProgram	helvetica;
	CHECK (RWPdfFonts::LoadFont (u"Helvetica", 0, helvetica));
	CHECK (helvetica.data.size() > 1000);
	CHECK (!helvetica.syntheticBold && !helvetica.syntheticItalic);
	CHECK ((helvetica.data.size() % 4) == 0);
	CHECK (std::fabs (helvetica.ascent - 0.77) < 0.005);		// CTFontGetAscent / size
	CHECK (std::fabs (helvetica.descent - 0.23) < 0.005);

	// bold face from a collection (Helvetica.ttc)
	RWPdfFontProgram	bold;
	CHECK (RWPdfFonts::LoadFont (u"Helvetica", RWStyle::st_bold, bold));
	CHECK (!bold.syntheticBold);
	CHECK (bold.data != helvetica.data);

	// Lucida Grande has no italic face: CoreText returns the regular face, the PDF slants it
	RWPdfFontProgram	lucida;
	CHECK (RWPdfFonts::LoadFont (u"Lucida Grande", RWStyle::st_italic, lucida));
	CHECK (lucida.syntheticItalic);

	// unknown family: the platform falls back to a real font
	RWPdfFontProgram	unknown;
	CHECK (RWPdfFonts::LoadFont (u"No Such Font Family 123", 0, unknown));
	CHECK (!unknown.data.empty());

	// head checkSumAdjustment makes the whole file sum to 0xB1B0AFBA
	uint32_t	sum = 0;
	for (size_t i = 0; i < helvetica.data.size(); i += 4)
		sum += (uint32_t (uint8_t (helvetica.data [i])) << 24) | (uint32_t (uint8_t (helvetica.data [i + 1])) << 16)
			 | (uint32_t (uint8_t (helvetica.data [i + 2])) << 8) | uint8_t (helvetica.data [i + 3]);
	CHECK (sum == 0xB1B0AFBA);
}


static	void	TestComposer (const std::string &inDir)
{
	const RWString	path = RWStr::FromUTF8 (inDir + "/composer.pdf");
	RWString		dst = path, printer;
	const RWString	sample = u"Žltý kôň úpel ďábelské ódy";

	std::string	png = MakePicture (CFSTR ("public.png"));
	std::string	jpeg = MakePicture (CFSTR ("public.jpeg"));
	RWValue		pngPicture (RWValue::eValue_PicturePNG, &png [0], png.size());
	RWValue		jpegPicture (RWValue::eValue_PictureJPG, &jpeg [0], jpeg.size());

	{
		RWPoDoFoPageComposer	composer (RWPageComposer::eDestinationPDF, dst, printer);
		composer.SetJobName (u"Composer test");

		RWXmlDocument	report;
		report.LoadString (u"<Report Version=\"1.0\" pageWidth=\"595\" pageHeight=\"842\" Orientation=\"Portrait\"/>");
		composer.ParseReport (report.Root());

		SRect	page;
		composer.GetPageBounds (u"Portrait", u"A4", page);
		CHECK (page.Width() > 500 && page.Height() > 800);

		std::unique_ptr<RWStyle>	regular = MakeStyle (u"<Style name=\"r\" id=\"1\" font=\"Helvetica\" size=\"12\"/>");
		std::unique_ptr<RWStyle>	bold = MakeStyle (u"<Style name=\"b\" id=\"2\" font=\"Helvetica\" size=\"14\" bold=\"1\" underline=\"1\"/>");
		std::unique_ptr<RWStyle>	italic = MakeStyle (u"<Style name=\"i\" id=\"3\" font=\"Lucida Grande\" size=\"10\" italic=\"1\"/>");
		CHECK (regular->GetSize() == 12);
		CHECK ((bold->GetStyle() & RWStyle::st_bold) != 0);

		// measurement before the first page (the formatter does that)
		double	ascent, descent, leading;
		const double	width = composer.MeasureWord (sample, int (sample.size()), regular.get(), ascent, descent, leading);
		CHECK (width > 100 && width < 250);
		CHECK (std::fabs (ascent - 9.24) < 0.01);		// CoreText: 9.240 / 2.760 for Helvetica 12
		CHECK (std::fabs (descent - 2.76) < 0.01);
		const double	widthBold = composer.MeasureWord (sample, int (sample.size()), bold.get(), ascent, descent, leading);
		CHECK (widthBold > width);

		composer.OpenNewPage (page, 1, 1);
		composer.DrawTextBox (sample, regular.get(), SRect (40.0, 40.0, 60.0, 400.0), false, false, false, NULL);
		composer.DrawTextBox (u"Bold underlined " + sample, bold.get(), SRect (70.0, 40.0, 130.0, 300.0), true, false, false, NULL);
		composer.DrawTextBox (u"Synthetic italic", italic.get(), SRect (140.0, 40.0, 160.0, 400.0), false, false, false, NULL);
		composer.DrawLine (180, 40, 180, 400, 1, cBlackColor, 4, 2);
		composer.DrawRect (SRect (190.0, 40.0, 240.0, 200.0), 2, true, cRedColor, true, cWhiteColor);
		composer.DrawOval (SRect (190.0, 220.0, 240.0, 380.0), 1, true, cBlueColor, false, cWhiteColor);

		RWPictData	*pngData = NULL, *jpegData = NULL;
		SRect		r1 (260.0, 40.0, 308.0, 104.0), r2 (260.0, 140.0, 356.0, 268.0);
		composer.DrawPict (r1, pngPicture, ePictFormat_Normal, &pngData, 0, 1);
		composer.DrawPict (r2, jpegPicture, ePictFormat_ScaledToFit, &jpegData, 0, 1);
		CHECK (pngData != NULL && pngData->GetWidth() == 64 && pngData->GetHeight() == 48);
		CHECK (jpegData != NULL && jpegData->GetWidth() == 64);

		// second page with the same fonts
		composer.OpenNewPage (page, 2, 2);
		composer.DrawTextBox (u"Page 2", regular.get(), SRect (40.0, 40.0, 60.0, 400.0), false, false, false, NULL);

		size_t	size;
		composer.FinishReport (size);
		CHECK (!composer.GetPDFData().empty());
		delete pngData;
		delete jpegData;
	}

	// load the file and look at what was written
	PdfMemDocument	doc;
	doc.Load (RWStr::ToUTF8 (path));
	CHECK (doc.GetPages().GetCount() == 2);

	std::vector<PdfTextEntry>	entries;
	doc.GetPages().GetPageAt (0).ExtractTextTo (entries);
	std::string	text;
	for (const auto &entry : entries)
		text += entry.Text + "\n";
	CHECK (text.find (RWStr::ToUTF8 (sample)) != std::string::npos);
	CHECK (text.find ("Synthetic italic") != std::string::npos);

	int		subsetFonts = 0, embeddedPrograms = 0, jpegImages = 0, flateImages = 0, softMasks = 0, unfiltered = 0;
	for (PdfObject *obj : doc.GetObjects())
	{
		if (!obj->IsDictionary())
			continue;
		const PdfDictionary	&dict = obj->GetDictionary();
		auto	nameOf = [&dict] (const char *inKey) -> std::string
		{
			const PdfObject	*value = dict.FindKey (inKey);
			return value != nullptr && value->IsName() ? std::string (value->GetName().GetString()) : std::string();
		};
		const std::string	type = nameOf ("Type"), subtype = nameOf ("Subtype");

		if (type == "Font")
		{
			const std::string	base = nameOf ("BaseFont");
			if (base.size() > 7 && base [6] == '+')
				subsetFonts++;
		}
		if (type == "FontDescriptor")
			if (dict.HasKey ("FontFile2") || dict.HasKey ("FontFile3"))
				embeddedPrograms++;
		if (subtype == "Image")
		{
			const PdfObject	*filter = dict.GetKey ("Filter");
			std::string		filterText;
			if (filter)
				filter->ToString (filterText);
			if (filterText.find ("DCTDecode") != std::string::npos)
				jpegImages++;
			else if (filterText.find ("FlateDecode") != std::string::npos)
				flateImages++;
			if (dict.HasKey ("SMask"))
				softMasks++;
		}
		if (obj->HasStream() && !dict.HasKey ("Filter"))
			unfiltered++;
	}
	CHECK (subsetFonts >= 3);					// Helvetica, Helvetica Bold, Lucida Grande
	CHECK (embeddedPrograms >= 3);
	CHECK (jpegImages == 1);					// the JPEG is embedded as it is
	CHECK (flateImages >= 2);					// PNG colour + its soft mask
	CHECK (softMasks == 1);						// PNG alpha
	CHECK (unfiltered == 0);					// content streams, fonts, images all compressed

	std::ifstream		in (RWStr::ToUTF8 (path), std::ios::binary);
	std::stringstream	bytes;
	bytes << in.rdbuf();
	std::printf ("composer.pdf: %zu bytes\n", bytes.str().size());
	CHECK (bytes.str().size() < 120000);		// subsets: well below the size of the full fonts
}


static	void	TestCreatePrinterComposer (const std::string &inDir)
{
	RWString	dst = RWStr::FromUTF8 (inDir + "/factory.pdf"), printer;
	std::unique_ptr<RWPageComposer>	composer (RWPageComposer::CreatePrinterComposer (RWPageComposer::eDestinationPDF, dst, printer));
	CHECK (dynamic_cast <RWPoDoFoPageComposer*> (composer.get()) != nullptr);
}


int		main (int argc, char **argv)
{
	const std::string	dir = argc > 1 ? argv [1] : ".";

	TestFontPrograms();
	try
	{
		TestComposer (dir);
	}
	catch (const PdfError &e)
	{
		std::printf ("FAILED: PdfError %s\n", e.what());
		sFailures++;
	}
	TestCreatePrinterComposer (dir);

	std::printf ("%d checks, %d failed\n", sChecks, sFailures);
	return sFailures == 0 ? 0 : 1;
}
