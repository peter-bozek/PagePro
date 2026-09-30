# include	"RWPDFPageComposer.h"
# include	"RWStyle.h"
//# include	<string.h>
//# include	<ctype.h>
# include	<math.h>
#ifdef __MWERKS__
# include	"pdflib.h"
//# include	"p_intern.h"
//extern "C"	size_t	pdf_strlen(const char *text);
#else
# include	"lib/pdflib/pdflib/pdflib.h"
#endif


#define	ROUND_UP(x)	(x)
/*
static	int	ROUND_UP (float x)
{
	long	l = ceil (x);
	return (int) l;
}
*/


#if	WIN32
const char	cDefEncoding[]		= "cp1250";		// "iso8859-2"
const TextEncoding	sTargetEncoding = DEFAULT_ENCODING;
#else
const char	cDefEncoding[]		= "cp1250";		// "macCE";
const TextEncoding	sTargetEncoding = kTextEncodingWindowsLatin2;
#endif

# define	PDF_EPSILON			1e-5
# define	PDF_EMBED_FONT		0		// 0 = don't embed, 1 = embed if not PDF font
# define	PDF_COMPRESSION		0		// 0 = no compression, 9 == maximum compression
# define	PDF_PAGE_OFFSET		12		// 12 @ 72 dpi = 12/72*3.54 = 0.59cm


# if PDF_EMBED_FONT
const char	*	RWPDFPageComposer::PDFFonts[] =
{
	"Courier",
	"Courier-Bold",
	"Courier-BoldOblique",
	"Courier-Oblique",
	"Helvetica",
	"Helvetica-Bold",
	"Helvetica-BoldOblique",
	"Helvetica-Oblique",
	"Symbol",
	"Times-Bold",
	"Times-BoldItalic",
	"Times-Italic",
	"Times-Roman",
	"ZapfDingbats",
	0
};
# endif


const char *	RWPDFPageComposer::PDFErrorNames[] =
{
	NULL,
	"memory error",
	"I/O error",
	"runtime error",
	"index error",
	"type error",
	"division by zero error",
	"overflow error",
	"syntax error",
	"value error",
	"system error",
	"warning (ignored)",
	"unknown error"
};


// ---------------------------------------------------------------------------
// PDFErrorHandler										  [static] [protected]
// ---------------------------------------------------------------------------
// Error handler for PDFLib - we don't want to exit!

void
RWPDFPageComposer::PDFErrorHandler (PDF *inPDF, int inType, const char* inShortMsg)
{
	fflush (stdout);
    fprintf (stderr, "RWPDFPageComposer: %s: %s\n", PDFErrorNames[inType], inShortMsg);

	switch (inType)
	{
		case PDF_NonfatalError:
			return;

		case PDF_MemoryError:
		case PDF_IOError:
		case PDF_RuntimeError:
		case PDF_IndexError:
		case PDF_TypeError:
		case PDF_DivisionByZero:
		case PDF_OverflowError:
		case PDF_SyntaxError:
		case PDF_ValueError:
		case PDF_SystemError:
		case PDF_UnknownError:
		default:
			PDF_delete (inPDF);		// clean up PDFlib
//			exit (99);				// good-bye
			throw (long (32000 + inType));
	}
}


// ---------------------------------------------------------------------------
// RWPDFPageComposer						Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPDFPageComposer::RWPDFPageComposer (void)
	:	RWPageComposer (RWPageComposer::eDefault),
		mEncoding (cDefEncoding),
		mPDF (0),
		mAuxPDF (0),
		mPageIsOpen (false),
		mPageRect (0, 0, 0, 0)
{
    PDF_boot();

	// create auxiliary PDF for text measuring
	mAuxPDF = PDF_new2 (PDFErrorHandler, NULL, NULL, NULL, this);
	if (mAuxPDF != NULL)
	{
		PDF_open_mem (mAuxPDF, RWPDFDummyWriteProc);
		PDF_begin_page (mAuxPDF, a3_height, a3_width);
	}

	return;
}



// ---------------------------------------------------------------------------
// ~RWPDFPageComposer						Destructor				  [public]
// ---------------------------------------------------------------------------

RWPDFPageComposer::~RWPDFPageComposer (void)
{
	if (mPDF != NULL)
	{
		try
		{
			ClosePage();
		    PDF_close (mPDF);
		    PDF_delete (mPDF);
		}
		catch (...)
		{
		}
	    mPDF = NULL;
	}

	if (mAuxPDF != NULL)
	{
		try
		{
			PDF_end_page (mAuxPDF);
		    PDF_close (mAuxPDF);
		    PDF_delete (mAuxPDF);
		}
		catch (...)
		{
		}
	    mAuxPDF = NULL;
	}

    PDF_shutdown();

	return;
}


// ---------------------------------------------------------------------------
// FindFont														   [protected]
// ---------------------------------------------------------------------------
// Same as PDF_findfont, but don't embed internal PDF fonts

int
RWPDFPageComposer::FindFont (const char *inFontName)
{
	int	embed = 0;

# if PDF_EMBED_FONT
	const char	**p;
	for (p = PDFFonts; *p; p++)
		if (STR_EQUALS (*p, inFontName))
			break;
	embed = (*p == 0 ? 1 : 0);
# endif

	int font = PDF_findfont (mPDF, inFontName, mEncoding, embed);
	return font;
}


// ---------------------------------------------------------------------------
// RWPDFDummyWriteProc									  [static] [protected]
// ---------------------------------------------------------------------------
// Dummy write procedure for auxiliary PDF

size_t RWPDFPageComposer::RWPDFDummyWriteProc(PDF * /*p*/, void * /*data*/, size_t size)
{
	return size;
}


// ---------------------------------------------------------------------------
// FinishReport													   [protected]
// ---------------------------------------------------------------------------
// Close the page, close the PDF

void*
RWPDFPageComposer::FinishReport (size_t &outSize)
{
	if (mPDF != NULL)
	{
		ClosePage();
	    PDF_close (mPDF);
	    PDF_delete (mPDF);
	    mPDF = NULL;
	}

	outSize = 0;

	return NULL;
}


// ---------------------------------------------------------------------------
// ParseReport													   [protected]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding
// Initialize PDF (title, creator, ...)

void
RWPDFPageComposer::ParseReport (RWXmlNode inReport)
{
	RWPageComposer::ParseReport (inReport);

	mEncoding = inReport->Attribute ("Encoding");
	if (mEncoding == NULL)
		mEncoding = cDefEncoding;


	if (mPDF != NULL)
	{
		const CXMLText	name = inReport->Attribute ("Name");
		const CXMLText	id = inReport->Attribute ("Id");
		const CXMLText	repId = inReport->Attribute ("Report_Id");

		PDF_set_value (mPDF, "compress", PDF_COMPRESSION);
		PDF_set_info (mPDF, "Keywords", "report");
//		PDF_set_info (mPDF, "Subject", "Check many PDFlib function calls");
		PDF_set_info (mPDF, "Title", name ? name : "unknown");
		PDF_set_info (mPDF, "Creator", "Report Writer 1.0");
//		PDF_set_info (mPDF, "Author", "Thomas Merz");
		if (id != NULL)
			PDF_set_info (mPDF, "ID", id);
		if (repId != NULL)
			PDF_set_info (mPDF, "Report_ID", repId);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding

void
RWPDFPageComposer::GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect)
{
	RWPageComposer::GetPageBounds (inOrientation, inSize, outRect);

	outRect *= PDF_PAGE_OFFSET;

	return;
}


// ---------------------------------------------------------------------------
// OpenNewPage														  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages)
{
	ClosePage();

	if (mPDF != NULL)
	{
		PDF_begin_page (mPDF, inRect.Width() + (PDF_PAGE_OFFSET << 1), inRect.Height() + (PDF_PAGE_OFFSET << 1));
		mPageIsOpen = true;
		mPageRect = inRect;

#ifdef	__DEBUGMEM_H
		PDF_save (mPDF);
		PDF_setlinewidth (mPDF, 0.25);
		PDF_setcolor (mPDF, "stroke", "gray", 0.50, 0.0, 0.0, 0.0);
		PDF_rect (mPDF, inRect.left, inRect.top, inRect.Width(), inRect.Height());
		PDF_stroke (mPDF);
		PDF_restore (mPDF);
#endif
	}

	return;
}


// ---------------------------------------------------------------------------
// ClosePage														  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::ClosePage (void)
{
	if (mPageIsOpen)
	{
		PDF_end_page (mPDF);
		mPageIsOpen = false;
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen, float inSpaceLen)
{
	PDF_save (mPDF);
	PDF_setlinewidth (mPDF, inThickness > 0 ? inThickness : 0.25);
	PDF_setcolor (mPDF, "stroke", "rgb", inLineColor.red / 65535., inLineColor.green / 65535., inLineColor.blue / 65535., 1 - inLineColor.alpha / 65535.);
	PDF_moveto (mPDF, left, mPageRect.bottom - top);
	PDF_lineto (mPDF, right, mPageRect.bottom - bottom);
	PDF_stroke (mPDF);
	PDF_restore (mPDF);

	return;
}


// ---------------------------------------------------------------------------
// DrawRect															  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		PDF_save (mPDF);
		PDF_setlinewidth (mPDF, inThickness > 0 ? inThickness : 0.25);
		if (inFill)
			PDF_setcolor (mPDF, "fill", "rgb", inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535., 1 - inFillColor.alpha / 65535.);
		if (inFrame)
			PDF_setcolor (mPDF, "stroke", "rgb", inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535., 1 - inFrameColor.alpha / 65535.);
		PDF_rect (mPDF, inRect.left, mPageRect.bottom - inRect.bottom, inRect.Width(), inRect.Height());
		if (inFill)
			if (inFrame)
				PDF_fill_stroke (mPDF);
			else
				PDF_fill (mPDF);
		else
			PDF_stroke (mPDF);
		PDF_restore (mPDF);
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawOval															  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		PDF_save (mPDF);
		PDF_setlinewidth (mPDF, inThickness > 0 ? inThickness : 0.25);
		if (inFill)
			PDF_setcolor (mPDF, "fill", "rgb", inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535., 1 - inFillColor.alpha / 65535.);
		if (inFrame)
			PDF_setcolor (mPDF, "stroke", "rgb", inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535., 1 - inFrameColor.alpha / 65535.);
//		PDF_rect (mPDF, inRect.left, mPageRect.bottom - inRect.bottom, inRect.Width(), inRect.Height());
		float	scale = 1;
		if (fabs (inRect.Width() - inRect.Height()) > PDF_EPSILON)
			scale = inRect.Width() / inRect.Height();
		float	r = inRect.Height() / 2;
		float	x = scale * (mPageRect.left + r);
		float	y = mPageRect.bottom - inRect.bottom + r;
		PDF_scale (mPDF, scale, 1.0);
		PDF_circle (mPDF, x, y, r);
		if (inFill)
			if (inFrame)
				PDF_fill_stroke (mPDF);
			else
				PDF_fill (mPDF);
		else
			PDF_stroke (mPDF);
		PDF_restore (mPDF);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPictBounds													  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData *cd, bool inGrow)
{
	if (inGrow)	// can shrink/expand
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawPict															  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::DrawPict (const SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData *cd, double inRotation)
{
	if (mPageIsOpen)
	{
	}

	return;
}


/*
// ---------------------------------------------------------------------------
// FreePict															  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::FreePict (void **cd)
{
	return;
}
*/


// ---------------------------------------------------------------------------
// DrawTextBox														  [public]
// ---------------------------------------------------------------------------

void
RWPDFPageComposer::DrawTextBox (const CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, RWPrintText **ioPrintText)
{
	if (mPageIsOpen)
	{
		int	chars;

		if (inText == NULL)
			chars = 0;
		else
			#if	CChar_Size == 1
				chars = strlen (reinterpret_cast <const char*> (inText));
			#else
				chars = CText::StrLength (inText);
			#endif

		if (chars > 0)
		{
			char	*text = NULL;	// UCS2 in BigEndian
			{
			#if	CChar_Size == 1
				CText	us (reinterpret_cast <const UTF8Char*> (inText));
			#else
				CText	us (reinterpret_cast <const UTF16Char*> (inText));
			#endif
//				text = us.u16_str (true, false);
//				chars = us.StrLength();
				text = us.CopyCStr (sTargetEncoding);
				chars = text ? strlen (text) : 0;
			}
//			chars = pdf_strlen (text) - 2;

			if (chars > 0)
			{
				int	font = FindFont (inStyle->GetPSName());
				if (font < 0)
					font = FindFont (RWStyle::cDefFontPSName);

				PDF_save (mPDF);
				float	size = inStyle->GetSize();
				PDF_setfont (mPDF, font, size);

				SRGBColor	textColor = inStyle->GetTextColor();
				PDF_setcolor (mPDF, "stroke", "rgb", textColor.red / 65535., textColor.green / 65535., textColor.blue / 65535., 1 - textColor.alpha / 65535.);
				if (inStyle->GetStyle() & RWStyle::st_underline)
					PDF_set_parameter (mPDF, "underline", "true");

				const char	*alignment;
				switch (inStyle->GetJustification())
				{
					case RWStyle::st_right:			alignment = "right"; break;
					case RWStyle::st_center:		alignment = "center"; break;
					case RWStyle::st_justify:		alignment = "justify"; break;
					case RWStyle::st_fulljustify:	alignment = "fulljustify"; break;
					default:						alignment = "left"; break;
				}
	//			float	size = PDF_get_value (mPDF, "fontsize", 0);
	//			float	leading = PDF_get_value (mPDF, "leading", 0);
				float	desc = PDF_get_value (mPDF, "descender", font) * size;
				SRect	r (inRect);
				switch (inStyle->GetVerticalJustification())
				{
					case RWStyle::st_bottom:
					case RWStyle::st_center:
						MeasureText (inText, inStyle, r, inWrap, inAttributed, NULL);
						r.left = inRect.left;
						r.right = inRect.right;
						if (r.bottom >= inRect.bottom)
							r.bottom = inRect.bottom;
						else
						{
							if (inStyle->GetVerticalJustification() == RWStyle::st_bottom)
							{
								r.top += inRect.bottom - r.bottom;
								r.bottom = inRect.bottom;
							}
							else
							{
								r.top += (inRect.bottom - r.bottom) / 2;
								r.bottom += r.top - inRect.top;
							}
						}
						break;
				}

				if (inWrap)
					PDF_show_boxed (mPDF, text, r.left, mPageRect.bottom - r.bottom - desc - PDF_EPSILON,
									r.Width(), r.Height() + PDF_EPSILON, alignment, NULL);
				else
				{
					do
					{
						float	width = PDF_stringwidth2 (mPDF, text, chars, font, size);
						if (width <= r.Width())
							break;
						chars -= 2;
					} while (chars > 0);

					if (chars > 0)
						PDF_show_xy2 (mPDF, text, chars, r.left, mPageRect.bottom - r.top - size - desc);
				}

				if (inStyle->GetStyle() & RWStyle::st_underline)
					PDF_set_parameter (mPDF, "underline", "false");
				PDF_restore (mPDF);
			}

			free (text);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// MeasureText														  [public]
// ---------------------------------------------------------------------------

float
RWPDFPageComposer::MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, RWPrintText **ioPrintText)
{
	int	chars;

	if (inText == NULL)
		chars = 0;
	else
		#if	CChar_Size == 1
			chars = strlen (reinterpret_cast <const char*> (inText));
		#else
			chars = CText::StrLength (inText);
		#endif

	float	width = 0;
	if (chars > 0)
	{
		char	*text = NULL;	// USC2 in BigEndian with BOM
		{
		#if	CChar_Size == 1
			CText	us (reinterpret_cast <const UTF8Char*> (inText));
		#else
			CText	us (reinterpret_cast <const UTF16Char*> (inText));
		#endif
//				text = us.u16_str (true, false);
//				chars = us.StrLength();
			text = us.CopyCStr (sTargetEncoding);
			chars = text ? strlen (text) : 0;
		}
//		chars = pdf_strlen (text) - 2;

		if (chars > 0)
		{
			int	font = PDF_findfont (mAuxPDF, inStyle->GetPSName(), mEncoding, 0);
			if (font < 0)
				font = PDF_findfont (mAuxPDF, RWStyle::cDefFontPSName, mEncoding, 0);

			float	size = inStyle->GetSize();
			PDF_setfont (mAuxPDF, font, size);

			const char	*alignment;
			switch (inStyle->GetJustification())
			{
				case RWStyle::st_right:			alignment = "right"; break;
				case RWStyle::st_center:		alignment = "center"; break;
				case RWStyle::st_justify:		alignment = "justify"; break;
				case RWStyle::st_fulljustify:	alignment = "fulljustify"; break;
				default:						alignment = "left"; break;
			}
//			float	size = PDF_get_value (mAuxPDF, "fontsize", 0);
			float	leading = PDF_get_value (mAuxPDF, "leading", 0);
			float	desc = PDF_get_value (mAuxPDF, "descender", font) * size;

			if (inWrap)
			{
				PDF_show_boxed (mAuxPDF, text, ioRect.left, leading,
								ioRect.Width(), a3_width - leading - leading, alignment, NULL);
				float	currY = PDF_get_value (mAuxPDF, "texty", 0);
				width = ioRect.Width();
				ioRect.bottom = ROUND_UP (ioRect.top + (a3_width - currY - leading /* - desc */));
			}
			else
			{
				do
				{
					width = PDF_stringwidth2 (mAuxPDF, text, chars, font, size);
					if (ioRect.Width() == 0 || width <= ioRect.Width())
						break;
					chars -= 2;
				} while (chars > 0);
				ioRect.right = ROUND_UP (ioRect.left + width);
				ioRect.bottom = ROUND_UP (ioRect.top + leading);
			}
		}

		free (text);
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return width;
}




// ---------------------------------------------------------------------------
// RWPDFFilePageComposer					Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPDFFilePageComposer::RWPDFFilePageComposer (const char *inFileName)
{
	mPDF = PDF_new2 (PDFErrorHandler, NULL, NULL, NULL, this);
	if (mPDF != NULL)
		if (PDF_open_file (mPDF, inFileName) == -1)
			throw 1L;

	return;
}


// ---------------------------------------------------------------------------
// ~RWPDFFilePageComposer					Destructor				  [public]
// ---------------------------------------------------------------------------

RWPDFFilePageComposer::~RWPDFFilePageComposer (void)
{
	return;
}




// ---------------------------------------------------------------------------
// RWPDFBlobWriteProc									  [static] [protected]
// ---------------------------------------------------------------------------
// Error handler for PDFLib - we don't want to exit!

size_t RWPDFBlobPageComposer::RWPDFBlobWriteProc(PDF *p, void *data, size_t size)
{
	RWPDFBlobPageComposer	*c = reinterpret_cast<RWPDFBlobPageComposer*> (PDF_get_opaque (p));
	if (c != NULL)
		return c->ll.add_data_in_datablock (data, size);

	return -1;
}


// ---------------------------------------------------------------------------
// RWPDFBlobPageComposer					Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPDFBlobPageComposer::RWPDFBlobPageComposer (void)
{
	mPDF = PDF_new2 (PDFErrorHandler, NULL, NULL, NULL, this);
	if (mPDF != NULL)
		PDF_open_mem (mPDF, RWPDFBlobWriteProc);

	return;
}


// ---------------------------------------------------------------------------
// ~RWPDFBlobPageComposer					Destructor				  [public]
// ---------------------------------------------------------------------------

RWPDFBlobPageComposer::~RWPDFBlobPageComposer (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FinishReport													   [protected]
// ---------------------------------------------------------------------------

void*
RWPDFBlobPageComposer::FinishReport (size_t &outSize)
{
	RWPDFPageComposer::FinishReport (outSize);

	void	*buf = ll.concatenate (outSize);

	return buf;
}
