#ifndef	_SRDataSourceProvider_h_
# define	_SRDataSourceProvider_h_

# include	"RWBaseTypes.h"
# include	"RWDataSourceProvider.h"


class	SRDataSourceProvider
	:	public	RWDataSourceProvider
{
public:
						SRDataSourceProvider (void);
						~SRDataSourceProvider (void);

virtual	RWString		FormatVariable (const RWValue &inVar, const RWString inFormat) const;

private:
			// defensive programming - not implemented
						SRDataSourceProvider (const SRDataSourceProvider &inOriginal);
SRDataSourceProvider&	operator = (const SRDataSourceProvider &inOriginal);
};

#endif
