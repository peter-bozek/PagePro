/*
 *  RWFontsMac.h
 *  ReportWriter
 *
 *  Font selection on the Mac, shared by the CoreText composer and the PDF font
 *  lookup so both use the same face for a style.
 */

#ifndef	_RWFontsMac_h_
# define	_RWFontsMac_h_

# include	"RWString.h"
# include	<CoreText/CoreText.h>

// family name and RWStyle::st_bold / st_italic; falls back to RWStyle::cDefFontName.
// Caller releases the result.
CTFontRef	RWCreateCTFont (RWStringView inName, float inSize, int inStyle);

#endif
