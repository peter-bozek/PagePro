#ifndef	_RWCalculator_h_
# define	_RWCalculator_h_

# include	"RWBaseTypes.h"


class	RWCalcDataProvider
{
public:
	virtual	bool		GetCalculatedValue (const CText inName, RWValue &outVar) const = 0;
inline	virtual			~RWCalcDataProvider()		{};
};

class	RWCalculatedValue
{
friend class	RWCalculator;
public:
	enum	RWCalculatedType
	{
		RWCalculatedType_None = 0,		// noop
		RWCalculatedType_Sum,			// c += v
		RWCalculatedType_Min,			// c = min (c, v)
		RWCalculatedType_Avg,			// _Sum / _Count
		RWCalculatedType_Max,			// c = max (c, v)
		RWCalculatedType_Count,			// c++
		RWCalculatedType_Var,			// c = (_Count - 1) * c / _Count + (_Avg - v) * (_Avg - v) / (_Count - 1)
		RWCalculatedType_Dev			// sqrt (_Var)
	};

    const CText					GetName (void) const;

protected:
	struct	SCalculatedValue
	{
		unsigned long		fCount;
		double			fSum;
		double			fMin;
		double			fMax;
		double			fVar;

						SCalculatedValue (void);
		void				Init (void);
	};

	explicit					RWCalculatedValue (const CText inName);
							~RWCalculatedValue (void);

	void						InitLevels (int inLevels);
	void						Increment (double newValue);
	double					GetValue (RWCalculatedType inWhich, int inLevel) const;
	double					GetOldValue (RWCalculatedType inWhich) const;
	void						Shunt (int inLevel);
	void						Push (void);

private:
			// defensive programming - not implemented
							RWCalculatedValue (const RWCalculatedValue &inOriginal);
	RWCalculatedValue	&		operator = (const RWCalculatedValue &inOriginal);

private:
	int					mLevels;
	SCalculatedValue		*mValues;
	RWTextValue			mName;
	SCalculatedValue		mLast;
};


class	RWCalculator
{
public:
	explicit					RWCalculator (void);
								~RWCalculator (void);

	void						Add (const CText inName);
	RWCalculatedValue	*		Get (const CText inName) const;
	void						InitLevels (int inLevels);

//	void						Increment (const CText inName, double newValue);
	bool						GetValue (const CText inName, RWCalculatedValue::RWCalculatedType inWhich, double &outValue) const;

	int							GetLevel (void) const;
	void						SetLevel (int inLevel);
	void						ShuntTotals (int inLevel);
	void						Increment (const RWCalcDataProvider *inData);
const RWList<RWCalculatedValue*>&	GetVariables (void) const	{ return mVariables; }

private:
			// defensive programming - not implemented
								RWCalculator (const RWCalculator &inOriginal);
	RWCalculator	&			operator = (const RWCalculator &inOriginal);

private:
	int							mLevels;
	int							mLevel;
	RWList<RWCalculatedValue*>	mVariables;
};

#endif
