#ifndef	_SRDataFormatter_h_
# define	_SRDataFormatter_h_

# include	"RWBaseTypes.h"


namespace	SRDataFormatter
{
	// value as text, formatted by 4D (numbers, dates, times) or by the simple rules below
	RWString		FormatVariable (const RWValue &inVar, RWStringView inFormat);
}

#endif
