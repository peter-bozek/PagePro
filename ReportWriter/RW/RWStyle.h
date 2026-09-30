#ifndef	_RWStyle_h_
# define	_RWStyle_h_

# include	"RWBaseTypes.h"
# include	"PSObject.h"


class	RWStyle;
class	RWStyleContainer
{
public:
		virtual		RWStyle*	FindStyle (long inID) const = 0;
		virtual		long		GetNewID (void) const = 0;
};


class	RWStyle
	:	public	PSObject
{
public:
		enum {
			st_normal			= 0,
			st_bold				= 1,
			st_italic			= 2,
			st_underline		= 4,
			st_strikethrough	= 8,	// not implemented

			st_default			= 0,
			st_left,
			st_right,
			st_center,
			st_justify,
			st_fulljustify,
			st_top				= st_left,
			st_bottom
		};

		// cloned style features - mFeatures
		enum {
			stf_Based				= 0x00000001,
			stf_Font				= 0x00000002,
			stf_Size				= 0x00000004,
			stf_Style				= 0x00000008,
			stf_TextColor			= 0x00000010,
			stf_BackColor			= 0x00000020,
			stf_Justification		= 0x00000040,
			stf_VertJustification	= 0x00000080,
			stf_Wrap				= 0x00000100,
			stf_FrameColor			= 0x00000200,
			stf_Rotation			= 0x00000400,
			stf_BaseLineShift		= 0x00000800,
			stf_HorizontalScale		= 0x00001000,
			stf_LineSpacing			= 0x00002000,
			stf_Direct				= 0x00004000,

			stf_MASK				= 0x00002FFF
		};

    RWStyle (RWStyleContainer *inContainer, XMLElement *inElem);
								RWStyle (const RWStyle& inOriginal);
		virtual					~RWStyle (void);
//				RWStyle	*		Clone (void) const;
				void			CloneFrom (const RWStyle *inStyle);


	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		XMLElement*	WriteXML (XMLNode *inParent);

protected:
//	virtual		DMBase		*	Clone (DMBase *inParent);

    virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

public:
	inline		long			GetID (void) const;
	inline		long			GetBaseID (void) const;
	inline		UInt32			GetFeatures (void) const;
	inline		CText		    GetName (void) const;
				CText		    GetFName (void) const;
//				const char	*	GetPSName (void) const;
				float			GetSize (void) const;
				int				GetStyle (void) const;
				SRGBColor		GetTextColor (void) const;
				SRGBColor		GetBackColor (void) const;
				int				GetJustification (void) const;
				int				GetVerticalJustification (void) const;
//				float			GetHorizontalOffset (void) const;
//				float			GetVerticalOffset (void) const;
				bool			ShouldWrap (void) const;
//				bool			IsFramed (void) const;
				SRGBColor		GetFrameColor (void) const;
				float			GetRotation (void) const;
				float			GetBaseLineShift (void) const;
				float			GetHorizontalScale (void) const;
				float			GetLineSpacing (void) const;
	inline		void			Clear (long newID) ;
					bool		operator == (const RWStyle& inStyle)const;
					RWStyle	&	operator = (const RWStyle& instyle);
				bool			less (const RWStyle& inStyle)const;

	static	const char			cDefFontName[];
//	static	const char			cDefFontPSName[];
	static	const float			cDefFontSize;

	static	const int			cDefFontStyle;
	static	const int			cDefJustification;
	static	const int			cDefVertJustification;
	static	const SRGBColor		cDefTextColor;
	static	const SRGBColor		cDefBackColor;
	static	const SRGBColor		cDefFrameColor;
	static	const double		cDefLineSpacing;
protected:
	inline		bool			HasFeature (UInt32 inFeature) const;
			const RWStyle	*	GetStyleForFeature (UInt32 inFeature) const;
private:
				void			Init (void);
			// defensive programming - not implemented
			//					RWStyle (const RWStyle &inOriginal);
			//	RWStyle	&		operator = (const RWStyle &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	RWStyleContainer *		mContainer;
	long					mId;
	long					mBaseId;
	UInt32				mFeatures;
	RWTextValue			mName;
	RWTextValue			mFontName;
	float				mFontSize;
	int					mFontStyle;		// bold, italic, underline, strikethrough
	SRGBColor			mTextColor;
	SRGBColor			mBackColor;
	int					mJustification;
	int					mVertJustification;
	bool			    mWrap;
	SRGBColor		    mFrameColor;
	float			    mRotation;
	float			    mBaseLineShift;
	float			    mHorizontalScale;
	float			    mLineSpacing;
// properties used in Style creation on Mac: mName, mFontSize, mFontStyle, mTextColor
};


inline	long			RWStyle::GetID (void) const						{ return mId; }
inline	long			RWStyle::GetBaseID (void) const					{ return mBaseId; }
inline	UInt32			RWStyle::GetFeatures (void) const				{ return mFeatures; }
inline	void			RWStyle::Clear (long newID)						{ mFeatures = stf_Based; mId = mContainer->GetNewID (); mBaseId = newID;}
inline	CText		    RWStyle::GetName (void) const					{ return mName; }
inline	bool			RWStyle::HasFeature (UInt32 inFeature) const	{ return (mFeatures & stf_Based)? (mFeatures & inFeature) != 0: true; }
	 
/*
inline	const CText		RWStyle::GetFName (void) const					{ return mFontName; }
//inline	const char	*	RWStyle::GetPSName (void) const					{ return mFontPSName; }
inline	float			RWStyle::GetSize (void) const					{ return mFontSize; }
inline	int				RWStyle::GetStyle (void) const					{ return mFontStyle; }
inline	SRGBColor		RWStyle::GetTextColor (void) const				{ return mTextColor; }
inline	SRGBColor		RWStyle::GetBackColor (void) const				{ return mBackColor; }
inline	int				RWStyle::GetJustification (void) const			{ return mJustification; }
inline	int				RWStyle::GetVerticalJustification (void) const	{ return mVertJustification; }
//inline	float			RWStyle::GetHorizontalOffset (void) const		{ return mHorizontalOffset; }
//inline	float			RWStyle::GetVerticalOffset (void) const			{ return mVerticalOffset; }
inline	bool			RWStyle::ShouldWrap (void) const				{ return mWrap; }
//inline	bool			RWStyle::IsFramed (void) const					{ return mFrameText; }
inline	SRGBColor		RWStyle::GetFrameColor (void) const				{ return mFrameColor; }
inline	float			RWStyle::GetRotation (void) const				{ return mRotation; }
*/


inline	const PSObject::PSObjProps *	RWStyle::GetProperties (void) const	{ return sProperties; }

# define RWStyleToIDMap std::map<long, RWStyle*>

// container for RWStyle pointers, destructor deletes the RWStyles first
class	RWStyleList
	:	public	RWStyleToIDMap,
		public	RWStyleContainer
{
public:
								RWStyleList (void)		{}
		virtual					~RWStyleList (void);

					RWStyle*	FindStyle (long inID) const;
					long		GetNewID (void) const;

private:
			// defensive programming - not implemented
								RWStyleList (const RWStyleList &inOriginal);
				RWStyleList	&	operator = (const RWStyleList &inOriginal);
};

#endif
