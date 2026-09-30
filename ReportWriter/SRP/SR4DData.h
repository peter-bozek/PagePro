#ifndef	_SR4DData_h_
# define	_SR4DData_h_

# include	"RWBaseTypes.h"


class	SR4DData
{
public:
							SR4DData (void);
virtual						~SR4DData (void);

		void				GetOldValue (RWValue &outValue) const;
		void				GetValue (RWValue &outValue) const;
		void				DetachValue (RWValue &outValue);	//mbs 05082010	support stack-based objects
		bool				IsChanged (void) const;
		void				Invalidate (void);
		void				Shunt (void);
virtual	void				Fetch (long inIteration, bool inSeek) = 0;
		long				GetSize (void) const;

static	void				GetPictureAsBlobFrom4D (void *ph, RWValue &outValue);
static	void				GetPictureFrom4D (void *ph, RWValue &outValue, bool inForPDF);
static	void		*		Get4DPicture (const RWValue &inValue);

protected:
		void				GetCurrentPicture (void *ph);

private:
	// defensive programming - not implemented
							SR4DData (const SR4DData &inOriginal);
		SR4DData	&		operator = (const SR4DData &inOriginal);

protected:
	RWValue					mOld;
	RWValue					mCurrent;
	bool					mNeedsFetch;
	bool					mChanged;
	long					mSize;
};


class	SR4DVariable	:	public	SR4DData
{
public:
	enum	SR4DVariable_ArrayElement
	{
		SR4DVariable_Expression = -3,	//mbs 17062010
		SR4DVariable_Variable = -2,
		SR4DVariable_ArrayAuto = -1
	};

							SR4DVariable (const CText inName, long inIndex = SR4DVariable_Variable);
virtual						~SR4DVariable (void);

virtual	void				Fetch (long inIteration, bool inSeek);

private:
			// defensive programming - not implemented
							SR4DVariable (const SR4DVariable &inOriginal);
		SR4DVariable	&	operator = (const SR4DVariable &inOriginal);

protected:
	RWTextValue				mName;
	long					mIndex;
};


class	SR4DField	:	public	SR4DData
{
public:
							SR4DField (long inTable, long inField);
//							~SR4DField (void);

virtual	void				Fetch (long inIteration, bool inSeek);

private:
			// defensive programming - not implemented
							SR4DField (const SR4DField &inOriginal);
		SR4DField	&		operator = (const SR4DField &inOriginal);

protected:
	long					mTable;
	long					mField;
	long					mFieldType;
};


#endif
