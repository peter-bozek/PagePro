# include	"SRDataSourceProvider.h"
# include	"SRDataFormatter.h"


// ---------------------------------------------------------------------------
// SRDataSourceProvider						Constructor				  [public]
// ---------------------------------------------------------------------------

SRDataSourceProvider::SRDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// ~SRDataSourceProvider					Destructor				  [public]
// ---------------------------------------------------------------------------

SRDataSourceProvider::~SRDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Convert specified variable to a text representation

RWTextValue
SRDataSourceProvider::FormatVariable (const RWValue &inVar, const CText inFormat)
const
{
	return SRDataFormatter::FormatVariable (inVar, inFormat);
}
