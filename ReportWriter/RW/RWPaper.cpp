// see <wingdi.h>
# include    "RWBaseTypes.h"

/* Paper types */
enum	RWPaperSize
{
	rwPAPER_NONE,               // Use specific dimensions
	rwPAPER_LETTER,             // Letter, 8 1/2 by 11 inches
	rwPAPER_LEGAL,              // Legal, 8 1/2 by 14 inches
	rwPAPER_A4,                 // A4 Sheet, 210 by 297 millimeters
	rwPAPER_CSHEET,             // C Sheet, 17 by 22 inches
	rwPAPER_DSHEET,             // D Sheet, 22 by 34 inches
	rwPAPER_ESHEET,             // E Sheet, 34 by 44 inches
	rwPAPER_LETTERSMALL,        // Letter Small, 8 1/2 by 11 inches
	rwPAPER_TABLOID,            // Tabloid, 11 by 17 inches
	rwPAPER_LEDGER,             // Ledger, 17 by 11 inches
	rwPAPER_STATEMENT,          // Statement, 5 1/2 by 8 1/2 inches
	rwPAPER_EXECUTIVE,          // Executive, 7 1/4 by 10 1/2 inches
	rwPAPER_A3,                 // A3 sheet, 297 by 420 millimeters
	rwPAPER_A4SMALL,            // A4 small sheet, 210 by 297 millimeters
	rwPAPER_A5,                 // A5 sheet, 148 by 210 millimeters
	rwPAPER_B4,                 // B4 sheet, 250 by 354 millimeters
	rwPAPER_B5,                 // B5 sheet, 182-by-257-millimeter paper
	rwPAPER_FOLIO,              // Folio, 8-1/2-by-13-inch paper
	rwPAPER_QUARTO,             // Quarto, 215-by-275-millimeter paper
	rwPAPER_10X14,              // 10-by-14-inch sheet
	rwPAPER_11X17,              // 11-by-17-inch sheet
	rwPAPER_NOTE,               // Note, 8 1/2 by 11 inches
	rwPAPER_ENV_9,              // #9 Envelope, 3 7/8 by 8 7/8 inches
	rwPAPER_ENV_10,             // #10 Envelope, 4 1/8 by 9 1/2 inches
	rwPAPER_ENV_11,             // #11 Envelope, 4 1/2 by 10 3/8 inches
	rwPAPER_ENV_12,             // #12 Envelope, 4 3/4 by 11 inches
	rwPAPER_ENV_14,             // #14 Envelope, 5 by 11 1/2 inches
	rwPAPER_ENV_DL,             // DL Envelope, 110 by 220 millimeters
	rwPAPER_ENV_C5,             // C5 Envelope, 162 by 229 millimeters
	rwPAPER_ENV_C3,             // C3 Envelope, 324 by 458 millimeters
	rwPAPER_ENV_C4,             // C4 Envelope, 229 by 324 millimeters
	rwPAPER_ENV_C6,             // C6 Envelope, 114 by 162 millimeters
	rwPAPER_ENV_C65,            // C65 Envelope, 114 by 229 millimeters
	rwPAPER_ENV_B4,             // B4 Envelope, 250 by 353 millimeters
	rwPAPER_ENV_B5,             // B5 Envelope, 176 by 250 millimeters
	rwPAPER_ENV_B6,             // B6 Envelope, 176 by 125 millimeters
	rwPAPER_ENV_ITALY,          // Italy Envelope, 110 by 230 millimeters
	rwPAPER_ENV_MONARCH,        // Monarch Envelope, 3 7/8 by 7 1/2 inches
	rwPAPER_ENV_PERSONAL,       // 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
	rwPAPER_FANFOLD_US,         // US Std Fanfold, 14 7/8 by 11 inches
	rwPAPER_FANFOLD_STD_GERMAN, // German Std Fanfold, 8 1/2 by 12 inches
	rwPAPER_FANFOLD_LGL_GERMAN, // German Legal Fanfold, 8 1/2 by 13 inches

	rwPAPER_ISO_B4,             // B4 (ISO) 250 x 353 mm
	rwPAPER_JAPANESE_POSTCARD,  // Japanese Postcard 100 x 148 mm
	rwPAPER_9X11,               // 9 x 11 in
	rwPAPER_10X11,              // 10 x 11 in
	rwPAPER_15X11,              // 15 x 11 in
	rwPAPER_ENV_INVITE,         // Envelope Invite 220 x 220 mm
	rwPAPER_LETTER_EXTRA,       // Letter Extra 9 \275 x 12 in
	rwPAPER_LEGAL_EXTRA,        // Legal Extra 9 \275 x 15 in
	rwPAPER_TABLOID_EXTRA,      // Tabloid Extra 11.69 x 18 in
	rwPAPER_A4_EXTRA,           // A4 Extra 9.27 x 12.69 in
	rwPAPER_LETTER_TRANSVERSE,  // Letter Transverse 8 \275 x 11 in
	rwPAPER_A4_TRANSVERSE,      // A4 Transverse 210 x 297 mm
	rwPAPER_LETTER_EXTRA_TRANSVERSE, // Letter Extra Transverse 9\275 x 12 in
	rwPAPER_A_PLUS,             // SuperA/SuperA/A4 227 x 356 mm
	rwPAPER_B_PLUS,             // SuperB/SuperB/A3 305 x 487 mm
	rwPAPER_LETTER_PLUS,        // Letter Plus 8.5 x 12.69 in
	rwPAPER_A4_PLUS,            // A4 Plus 210 x 330 mm
	rwPAPER_A5_TRANSVERSE,      // A5 Transverse 148 x 210 mm
	rwPAPER_B5_TRANSVERSE,      // B5 (JIS) Transverse 182 x 257 mm
	rwPAPER_A3_EXTRA,           // A3 Extra 322 x 445 mm
	rwPAPER_A5_EXTRA,           // A5 Extra 174 x 235 mm
	rwPAPER_B5_EXTRA,           // B5 (ISO) Extra 201 x 276 mm
	rwPAPER_A2,                 // A2 420 x 594 mm
	rwPAPER_A3_TRANSVERSE,      // A3 Transverse 297 x 420 mm
	rwPAPER_A3_EXTRA_TRANSVERSE, // A3 Extra Transverse 322 x 445 mm

	rwPaperSize_Last
};


struct	RWPaper
{
	RWPaperSize		    mID;
	const char *		mPaperName;
	SPoint			    mPaperDimension;
	SRect			    mImageableArea;

	SRect				GetMargins (void) const;
static const RWPaper	*	GetPaper (RWPaperSize inPaper);
static const RWPaper	*	FindPaper (SPoint &inSize);
static const RWPaper	*	FindPaper (SPoint &inSize, SRect &inMargins);
static const RWPaper	sRWPaper[];
};


SRect
RWPaper::GetMargins (void)
const
{
	SRect	margins;
	margins.SetRect (mImageableArea.top, mImageableArea.left,
					mPaperDimension.v - mImageableArea.bottom,
					mPaperDimension.h - mImageableArea.right);
	return margins;
}


const RWPaper	RWPaper::sRWPaper[] = 
{
	{ rwPAPER_LETTER, "Letter, 8 1/2 x 11 in", { 612, 792 }, { 12.00, 12.00, 599.76, 779.76 } },
	{ rwPAPER_LETTERSMALL, "Letter Small, 8 1/2 x 11 in", { 612, 792 }, { 30.00, 31.00, 582.00, 761.00 } },
	{ rwPAPER_EXECUTIVE, "Executive, 7 1/4 by 10 1/2 inches", { 522, 756 }, { 12.00, 12.00, 509.76, 743.76 } },
	{ rwPAPER_LEGAL, "Legal, 8 1/2 by 14 inches", { 612, 1008 }, { 12.00, 12.00, 599.76, 995.76 } },
//	{ rwPAPER_LEGALSMALL, "Legal Small, 8 1/2 by 14 inches", { 612, 1008 }, { 64.00, 54.00, 548.00, 954.00 } },
	{ rwPAPER_A4, "A4 sheet, 210 x 297 mm", { 595, 842 }, { 12.00, 12.00, 582.96, 829.44 } },
	{ rwPAPER_A4SMALL, "A4 small sheet, 210 x 297 mm", { 595, 842 }, { 28.00, 30.00, 566.00, 811.00 } },
	{ rwPAPER_A5, "A5 sheet, 148 x 210 mm", { 420, 595 }, { 12.00, 12.00, 407.28, 582.96 } },
//	{ rwPAPER_ISOB5, "ISO B5", { 499, 709 }, { 12.00, 12.00, 486.48, 696.24 } },
	{ rwPAPER_B5, "JIS B5", { 516, 729 }, { 12.00, 12.00, 503.52, 716.16 } },
	{ rwPAPER_FANFOLD_LGL_GERMAN, "German Legal Fanfold, 8 1/2 x 13 in", { 612, 936 }, { 12.00, 12.00, 599.76, 923.76 } },
//	{ rwPAPER_Postcard, "Postcard (JIS)", { 284, 419 }, { 12.00, 12.00, 271.20, 407.28 } },
//	{ rwPAPER_DoublePostcard, "Double Postcard (JIS)", { 419.5, 567 }, { 12.00, 12.00, 407.28, 554.64 } },
//	{ rwPAPER_EXECUTIVE, "Executive (JIS)", { 612, 935 }, { 12.00, 12.00, 599.76, 922.76 } },
//	{ rwPAPER_16K, "16K", { 558, 774 }, { 12.00, 12.00, 545.76, 761.76 } },
	{ rwPAPER_ENV_10, "#10 Envelope, 4 1/8 x 9 1/2 in", { 297, 684 }, { 12.00, 12.00, 284.64, 671.76 } },
	{ rwPAPER_ENV_MONARCH, "Monarch Envelope, 3 7/8 x 7 1/2 in", { 279, 540 }, { 12.00, 12.00, 266.64, 527.76 } },
	{ rwPAPER_ENV_DL, "DL Envelope, 110 x 220 mm", { 312, 624 }, { 12.00, 12.00, 299.52, 611.28 } },
	{ rwPAPER_ENV_C5, "C5 Envelope, 162 x 229 mm", { 459, 649 }, { 12.00, 12.00, 446.88, 636.72 } },
	{ rwPAPER_ENV_B5, "B5 Envelope, 176 x 250 mm", { 499, 709 }, { 12.00, 12.00, 486.48, 696.24 } },
	{ rwPAPER_NONE, "", { 0, 0 }, { 0, 0, 0, 0 } }
};


RWPaper*
RWPaper::GetPaper (RWPaperSize inPaper)
{
	RWPaper	*p;
	for (p = sRWPaper; p->mID, p++)
	{
		if (p->mID == inPaper)
			return p;
	}
	return NULL;
}


RWPaper*
RWPaper::FindPaper (SPoint &inSize)
{
	RWPaper	*p;
	for (p = sRWPaper; p->mID, p++)
	{
		if (p->mPaperDimension == inSize)
			return p;
	}
	return NULL;
}


RWPaper*
RWPaper::FindPaper (SPoint &inSize, SRect &inMargins)
{
	RWPaper	*p;
	for (p = sRWPaper; p->mID, p++)
	{
		if (p->mPaperDimension == inSize)
		{
			SRect	margins = p->GetMargins();
			if (margins == inMargins)
				return p;
		}
	}
	return NULL;
}
