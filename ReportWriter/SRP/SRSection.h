#ifndef	_SRSection_h_
# define	_SRSection_h_

# include	"SRObject.h"
# include	"RWCalculator.h"
# include	"ExtendedExecute.h"

class	SRSection
	:	public	PSObject
{
public:
	enum	EPageThrow
	{
		ePageThrow_None = 0,
		ePageThrow_Before,
		ePageThrow_After
	};

    virtual		void				Parse (SRReportData *inReport, RWXmlNode inNode);
    virtual		void				Write (RWXmlNode inParent, bool inUseCalculator) const = 0;

				const RWString		GetType (void) const;
				const RWString			GetName (void) const;
				SRObjListD		*	GetObjects (void);
				EPageThrow			GetPageThrow (void) const;
				bool				IsEmpty (void) const;			// no objects, no script, ...
				bool				NeedsProcessing (void) const;	// contains objects?
				void				FetchValues (bool inUseOld);
				void				FetchCalcValues (void);
				SRDataSource	&	GetDataSource (void) const;

	virtual	const PSObjProps *		GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);

	virtual							~SRSection (void);

protected:
									SRSection (RWStringView inType);

                RWXmlNode			WriteSection (RWXmlNode inParent, RWStringView inSectionName) const;
//				void				WriteSection (FILE *fd, const RWString inSectionName) const; // REMOVED
//				void				WriteSpecial (FILE *fd, bool inUseCalculator) const;

private:
			// defensive programming - not implemented
									SRSection (const SRSection &inOriginal);
				SRSection	&		operator = (const SRSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	SRReportData		*	mReportData;
	RWString				mType;
	RWString				mName;
	RWString				mID;
	SRObjListD				mObjects;
	float					mHeight;
	float					mMinSpace;
	bool					mDraw;
	bool					mKeepTogether;
	bool					mFromBottom;
	EPageThrow				mPageThrow;
	ExtendedExecute			mScript;
	bool					mFixedHeight;
};


class	SRHeaderFooterSection
	:	public	SRSection
{
public:
									SRHeaderFooterSection (RWStringView inType);
//	virtual							~SRHeaderFooterSection (void);

    virtual		void				Parse (SRReportData *inReport, RWXmlNode inNode) override;
    virtual		void				Write (RWXmlNode inParent, bool inUseCalculator) const override;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);

private:
			// defensive programming - not implemented
									SRHeaderFooterSection (const SRHeaderFooterSection &inOriginal);
			SRHeaderFooterSection&	operator = (const SRHeaderFooterSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	float					mFixed;
	bool					mFirstPage;
	int						mEvenPage;
	int						mOddPage;
	bool					mLastPage;
	bool					mFillPage;
};


// Break Header, Break Footer
class	SRBreakSection
	:	public	SRSection
{
public:
	enum	EBreakOn
	{
		eBreakOn_None = 0,
		eBreakOn_Field,
		eBreakOn_Variable,
		eBreakOn_Array
	};
									SRBreakSection (RWStringView inType);
									SRBreakSection (SRReportData *inReport, RWStringView inType, int inLevel);
	virtual							~SRBreakSection (void);

    virtual		void				Parse (SRReportData *inReport, RWXmlNode inNode) override;
    virtual		void				Write (RWXmlNode inParent, bool inUseCalculator) const override;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);

				void				CreateBreak (SRReportData *inReport);
				int					GetLevel (void) const;
				bool				IsBreak (long inIteration) const;

private:
			// defensive programming - not implemented
									SRBreakSection (const SRBreakSection &inOriginal);
				SRBreakSection	&	operator = (const SRBreakSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	int						mLevel;
	bool					mPrintAlways;
	RWString				mBreakOn;
	EBreakOn				mBreakType;
	SR4DData			*	mBreakObject;
};


// Page
class	SRPageSection
	:	public	SRSection
{
public:
									SRPageSection (RWStringView inType);
									SRPageSection (bool inEmpty);
	virtual							~SRPageSection (void);

//	virtual		void				Parse (SRReportData *inReport,  *inNode);
//	virtual		void				Write (FILE *fd, bool inUseCalculator) const;
    virtual		void				Write (RWXmlNode inParent, bool inUseCalculator) const override;
//	virtual		const PSObjProps *	GetProperties (void) const;
//	virtual		bool				GetProperty (OSType id, RWValue &outValue);
//	virtual		bool				SetProperty (OSType id, RWValue &inValue);
				RWXmlNode			WritePage (RWXmlNode inParent, bool inSimple, bool inStart) const;
				const RWString			GetPageOrientation (void) const;
				const RWString			GetPageSize (void) const;

				void				CreateCalculatedObjects (const RWList<RWCalculatedValue*>& inCalc);

private:
			// defensive programming - not implemented
									SRPageSection (const SRPageSection &inOriginal);
				SRPageSection	&	operator = (const SRPageSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	RWString				mPageOrientation;
	RWString				mPageSize;
};


// Watermark
class	SRWatermarkSection
	:	public	SRHeaderFooterSection
{
public:
									SRWatermarkSection (RWStringView inType);
	virtual							~SRWatermarkSection (void);

	virtual		void				Parse (SRReportData *inReport, RWXmlNode inNode) override;
	virtual		void				Write (RWXmlNode inParent, bool inUseCalculator) const override;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);
                RWXmlNode			WritePage (RWXmlNode inParent, bool inSimple, bool inStart) const;

private:
			// defensive programming - not implemented
									SRWatermarkSection (const SRWatermarkSection &inOriginal);
				SRWatermarkSection&	operator = (const SRWatermarkSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	bool					mOnTop;
};

inline	const PSObject::PSObjProps *	SRSection::GetProperties (void) const				{ return sProperties; }
inline	const PSObject::PSObjProps *	SRHeaderFooterSection::GetProperties (void) const	{ return sProperties; }
inline	const PSObject::PSObjProps *	SRBreakSection::GetProperties (void) const			{ return sProperties; }
//inline	const PSObject::PSObjProps *	SRPageSection::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	SRWatermarkSection::GetProperties (void) const		{ return sProperties; }


// container for SRSection pointers, destructor deletes the SRSections first
typedef	RWArray<SRSection*>		SRSectionList;

#endif

