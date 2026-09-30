# include	"RWStyle.h"
# include	"PSObjProps.h"

#if	WIN32
	const	char		RWStyle::cDefFontName[]			= "Arial";
//	const	char		RWStyle::cDefFontPSName[]		= "Arial";
	const	float		RWStyle::cDefFontSize			= 8;
#else
	const	char		RWStyle::cDefFontName[]			= "Helvetica";
//	const	char		RWStyle::cDefFontPSName[]		= "Helvetica";
	const	float		RWStyle::cDefFontSize			= 9;
#endif

	const	int			RWStyle::cDefFontStyle			= RWStyle::st_normal;
	const	int			RWStyle::cDefJustification		= RWStyle::st_default;	// st_left
	const	int			RWStyle::cDefVertJustification	= RWStyle::st_default;	// st_top
	const	SRGBColor	RWStyle::cDefTextColor			( 0, 0, 0, 65535 );		// = cBlackColor;
	const	SRGBColor	RWStyle::cDefBackColor			( 65535, 65535, 65535, 0 );		// = cWhiteColor, transparent;
	const	SRGBColor	RWStyle::cDefFrameColor			( 0, 0, 0, 65535 );		// = cBlackColor;


const PSObject::PSObjProps	RWStyle::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_Integer,	"id",				{ NULL, -1, 0, LONG_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }		},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }		},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
//{ PSObjPropPSName,			true,	PSProps_Attribute,	PSProps_String,		"fontPS",			{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
//{ PSObjPropHorizontalOffset,true,	PSProps_Attribute,	PSProps_Real,		"hOffset",			{ NULL, 0, -10, 64 }			},
//{ PSObjPropVerticalOffset,	true,	PSProps_Attribute,	PSProps_Real,	"vOffset",			{ NULL, 0, -10, 64 }			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
//{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAlign,			true,	PSProps_Attribute,	PSProps_List,		"align",			{ sJustification, cDefJustification }	},
{ PSObjPropAlign,			true,	PSProps_Attribute,	PSProps_List,		"justification",	{ sJustification, cDefJustification }, true	},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, cDefVertJustification }	},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};


// ---------------------------------------------------------------------------
// RWStyle									Constructor				  [public]
// ---------------------------------------------------------------------------

RWStyle::RWStyle (RWStyleContainer *inContainer, XMLElement *inElem)
	:	PSObject (eObject_Style),
		mContainer (inContainer),
		mId (0),
		mBaseId (-1),
		mFeatures (0),
//		mName (0),
//		mFontName (0),
//		mFontPSName (0),
		mFontSize (cDefFontSize),
		mFontStyle (cDefFontStyle),
		mTextColor (cDefTextColor),
		mBackColor (cDefBackColor),
		mJustification (cDefJustification),
		mVertJustification (cDefVertJustification),
//		mHorizontalOffset (0),
//		mVerticalOffset (0),
		mWrap (false),
//		mFrameText (false),
		mFrameColor (cDefFrameColor),
		mRotation (0),
		mBaseLineShift (0),
		mHorizontalScale (1),
		mLineSpacing (1.2)
{
//	if (inElem)
		LoadXML (inElem);

#if 0
	if (inElem)
	{
//		bool			thisStylePSFontNotSet = true;
		XMLAttribute	*attrib;

		for ( attrib = inElem->FirstAttribute(); attrib; attrib = attrib->Next() )
		{
			const CXMLText	name = attrib->Name();
			const CXMLText	value = attrib->Value();
			if (STR_EQUALS (name, "id"))
				mId = atoi (value);
			else if (STR_EQUALS (name, "name"))
			{
				mName.FromXML (value);
			}
			else if (STR_EQUALS (name, "font"))
			{
				mFontName.FromXML (value);
			}
//			else if (STR_EQUALS (name, "fontPS"))
//			{
//				mFontPSName = strdup ((const char*) value);
//			}
			else if (STR_EQUALS (name, "size"))
			{
				mFontSize = atof (value);
				if (mFontSize < 4 || mFontSize > 127)
					mFontSize = cDefFontSize;
			}
//			else if (STR_EQUALS (name, "hOffset"))
//				mHorizontalOffset = atof (value);
//			else if (STR_EQUALS (name, "vOffset"))
//				mVerticalOffset = atof (value);
			else if (STR_EQUALS (name, "wrap"))
				mWrap = (atoi (value) != 0);
//			else if (STR_EQUALS (name, "frame"))
//				mFrameText = (atoi (value) != 0);
			else if (STR_EQUALS (name, "bold"))
			{
				if (atoi (value) != 0)
					mFontStyle |= st_bold;
				else
					mFontStyle &= ~st_bold;
			}
			else if (STR_EQUALS (name, "italic"))
			{
				if (atoi (value) != 0)
					mFontStyle |= st_italic;
				else
					mFontStyle &= ~st_italic;
			}
			else if (STR_EQUALS (name, "underline"))
			{
				if (atoi (value) != 0)
					mFontStyle |= st_underline;
				else
					mFontStyle &= ~st_underline;
			}
			else if (STR_EQUALS (name, "justification") || STR_EQUALS (name, "align"))
			{
				if (sscanf (value, "%li", &lVal) == 1)
				{
					if (lVal >= st_default && lVal <= st_fulljustify)
						mJustification = lVal;
				}
				else if (STR_EQUALS (value, "left"))
					mJustification = st_left;
				else if (STR_EQUALS (value, "center") || STR_EQUALS (value, "middle"))
					mJustification = st_center;
				else if (STR_EQUALS (value, "right"))
					mJustification = st_right;
				else if (STR_EQUALS (value, "default"))
					mJustification = cDefJustification;
				else if (STR_EQUALS (value, "justify") || STR_EQUALS (value, "justified"))
					mJustification = st_justify;
				else if (STR_EQUALS (value, "fulljustify") || STR_EQUALS (value, "fullyjustified"))
					mJustification = st_fulljustify;
			}
			else if (STR_EQUALS (name, "verticalJustification") || STR_EQUALS (name, "valign"))
			{
				if (sscanf (value, "%li", &lVal) == 1)
				{
					if (lVal >= st_default && lVal <= st_bottom)
						mVertJustification = lVal;
				}
				else if (STR_EQUALS (value, "top"))
					mVertJustification = st_top;
				else if (STR_EQUALS (value, "center") || STR_EQUALS (value, "middle"))
					mVertJustification = st_center;
				else if (STR_EQUALS (value, "bottom"))
					mVertJustification = st_bottom;
				else if (STR_EQUALS (value, "default"))
					mVertJustification = cDefVertJustification;
			}
			else if (STR_EQUALS (name, "textColor") || STR_EQUALS (name, "foreColor"))
				mTextColor = value;
			else if (STR_EQUALS (name, "backColor"))
				mBackColor = value;
			else if (STR_EQUALS (name, "frameColor"))
				mFrameColor = value;
		}
	}
#endif

	return;
}


// ---------------------------------------------------------------------------
// RWStyle									Constructor				  [public]
// ---------------------------------------------------------------------------

RWStyle::RWStyle (const RWStyle& inOriginal)
	:	PSObject (eObject_Style),
	mContainer (inOriginal.mContainer),
	mId (inOriginal.mId),
	mBaseId (inOriginal.mBaseId),
	mFeatures (inOriginal.mFeatures),
	mFontName (inOriginal.mFontName),
	mFontSize (inOriginal.mFontSize),
	mFontStyle (inOriginal.mFontStyle),
	mTextColor (inOriginal.mTextColor),
	mBackColor (inOriginal.mBackColor),
	mJustification (inOriginal.mJustification),
	mVertJustification (inOriginal.mVertJustification),
	mWrap (inOriginal.mWrap),
	mFrameColor (inOriginal.mFrameColor),
	mRotation (inOriginal.mRotation),
	mBaseLineShift (inOriginal.mBaseLineShift),
	mHorizontalScale (inOriginal.mHorizontalScale),
	mLineSpacing (inOriginal.mLineSpacing)
{
}


// ---------------------------------------------------------------------------
// ~RWStyle									Destructor				  [public]
// ---------------------------------------------------------------------------

RWStyle::~RWStyle (void)
{
//	mName.Free();
//	mFontName.Free();
//	free (mFontPSName);

	return;
}

// ---------------------------------------------------------------------------
// RWStyle::operator ==												  [public]
// ---------------------------------------------------------------------------

bool
RWStyle::operator == (const RWStyle& inStyle) const
{
	if ( (mBaseId == inStyle.mBaseId)
		&& (mFeatures == inStyle.mFeatures)
		&& (mFontSize == inStyle.mFontSize)
		&& (mFontStyle == inStyle.mFontStyle)
		&& (mTextColor == inStyle.mTextColor)
		&& (mBackColor == inStyle.mBackColor)
		&& (mJustification == inStyle.mJustification)
		&& (mVertJustification == inStyle.mVertJustification)
		&& (mWrap == inStyle.mWrap)
		&& (mFrameColor == inStyle.mFrameColor)
		&& (mRotation == inStyle.mRotation)
		&& (mBaseLineShift == inStyle.mBaseLineShift)
		&& (mHorizontalScale == inStyle.mHorizontalScale)
		&& (mLineSpacing == inStyle.mLineSpacing)
		)
		
	{
		if (inStyle.mFontName.equal( mFontName) == 0)
			return true;
		
	}
	return false;
}

// ---------------------------------------------------------------------------
// RWStyle::operator =												  [public]
// ---------------------------------------------------------------------------

RWStyle&
RWStyle::operator = (const RWStyle& inStyle) 
{
	mContainer = inStyle.mContainer;
	mId = inStyle.mId;
	mBaseId = inStyle.mBaseId;
	mFeatures = inStyle.mFeatures;
	mName = inStyle.mName;
	mFontName = inStyle.mFontName;
	mFontSize = inStyle.mFontSize;
	mFontStyle = inStyle.mFontStyle;
	mTextColor = inStyle.mTextColor;
	mBackColor = inStyle.mBackColor;
	mJustification = inStyle.mJustification;
	mVertJustification = inStyle.mVertJustification;
	mWrap = inStyle.mWrap;
	mFrameColor = inStyle.mFrameColor;
	mRotation = inStyle.mRotation;
	mBaseLineShift = inStyle.mBaseLineShift;
	mHorizontalScale = inStyle.mHorizontalScale;
	mLineSpacing = inStyle.mLineSpacing;
	return *this;
}

// ---------------------------------------------------------------------------
// RWStyle::operator <												  [public]
// ---------------------------------------------------------------------------

bool
RWStyle::less (const RWStyle& inStyle) const
{
	
	if (mBaseId != inStyle.mBaseId)
		return mBaseId < inStyle.mBaseId;
	
	if (mBaseId != inStyle.mBaseId)
		return mBaseId < inStyle.mBaseId;
	
	if (mFeatures != inStyle.mFeatures)
		return mFeatures < inStyle.mFeatures;
	
	if (mFontSize != inStyle.mFontSize)
		return mFontSize < inStyle.mFontSize;
	
	if (mFontStyle != inStyle.mFontStyle)
		return mFontStyle < inStyle.mFontStyle;
	
	if (mTextColor != inStyle.mTextColor)
		return (unsigned long) mTextColor < (unsigned long) inStyle.mTextColor;
		
	if (mBackColor != inStyle.mBackColor)
		return (unsigned long) mBackColor < (unsigned long) inStyle.mBackColor;
	
	if (mJustification != inStyle.mJustification)
		return mJustification < inStyle.mJustification;
	
	if (mVertJustification != inStyle.mVertJustification)
		return mVertJustification < inStyle.mVertJustification;
	
	if (mWrap != inStyle.mWrap)
		return mWrap < inStyle.mWrap;
	
	if (mFrameColor != inStyle.mFrameColor)
		return (unsigned long) mFrameColor < (unsigned long) inStyle.mFrameColor;
	
	if (mRotation != inStyle.mRotation)
		return mRotation < inStyle.mRotation;
	
	if (mBaseLineShift != inStyle.mBaseLineShift)
		return mBaseLineShift < inStyle.mBaseLineShift;
	
	if (mHorizontalScale != inStyle.mHorizontalScale)
		return mHorizontalScale < inStyle.mHorizontalScale;
	
	if (mLineSpacing != inStyle.mLineSpacing)
		return mLineSpacing < inStyle.mLineSpacing;
		
    return this->mFontName.compare(inStyle.mFontName) < 0;
		
}

#if 0
// ---------------------------------------------------------------------------
// Clone															  [public]
//
// pB Used for cloning styles during copy, do not allow multiple chained styles
// ---------------------------------------------------------------------------

RWStyle*
RWStyle::Clone (void)
const
{
	RWStyle	*st;
	if (mFeatures & stf_Based) // already chained
	{
		 st = new RWStyle (this);	}
	else 
	{
		st = new RWStyle (mContainer, NULL);
		st->mBaseId = mId;
		st->mFeatures = stf_Based;
	}

	return st;
}
#endif

// ---------------------------------------------------------------------------
// CloneFrom[public]
//
// pB Used for cloning text run styles, create copy of style
// ---------------------------------------------------------------------------

void
RWStyle::CloneFrom (const RWStyle* inStyle)
{
	if (inStyle->mFeatures & stf_Based)
	{
		*this = *inStyle;
	}
	else 
	{
		mContainer = inStyle->mContainer;
		mBaseId = inStyle->mId;
		mFeatures = stf_Based;
	}
	if (mContainer == NULL)
	{
		mContainer = reinterpret_cast <RWStyleContainer*> (const_cast <RWStyle*> (inStyle));
		mFeatures = stf_Based | stf_Direct;
	}

}


// ---------------------------------------------------------------------------
// Init																 [private]
// ---------------------------------------------------------------------------

void
RWStyle::Init (void)
{
	mId = 0;
	mBaseId = -1;
	mFeatures = 0;
	mName.Free();
	mFontName.Free();
//	if (mFontPSName != NULL)
//	{
//		free (mFontPSName);
//		mFontPSName = NULL;
//	}
	mFontSize = cDefFontSize;
	mFontStyle = cDefFontStyle;
	mTextColor = cDefTextColor;
	mBackColor = cDefBackColor;
	mJustification = cDefJustification;
	mVertJustification = cDefVertJustification;
//	mHorizontalOffset = 0;
//	mVerticalOffset = 0;
	mWrap = false;
//	mFrameText = false;
	mFrameColor = cDefFrameColor;
	mRotation = 0;
	mBaseLineShift = 0;
	mHorizontalScale = 1;
	mLineSpacing = 1.2;
	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
RWStyle::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	Init();

	if (inNode)
		PSObject::LoadXML (inNode, pes);

	if (!mFontName.empty())
	{
		mFontName = cDefFontName;
	}

	return;
}

const RWStyle*
RWStyle::GetStyleForFeature (UInt32 inFeature)
const
{
	const	RWStyle	*baseStyle = this;
	while (baseStyle && not baseStyle->HasFeature (inFeature))	//mbs 20052011	test baseStyle!
	{
		if (baseStyle->mFeatures & stf_Direct)
			baseStyle = reinterpret_cast <const RWStyle*> (baseStyle->mContainer);
		else
			baseStyle = baseStyle->mContainer->FindStyle (baseStyle->mBaseId);
	}
	return baseStyle;
}

CText
RWStyle::GetFName (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Font);
	if (style != NULL)
		return style->mFontName;
	return NULL;	// cDefFontName;
}

float
RWStyle::GetSize (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Size);
	if (style)
		return style->mFontSize;
	return cDefFontSize;
}

int
RWStyle::GetStyle (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Style);
	if (style)
		return style->mFontStyle;
	return cDefFontStyle;
}

SRGBColor
RWStyle::GetTextColor (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_TextColor);
	if (style)
		return style->mTextColor;
	return cDefTextColor;
}

SRGBColor
RWStyle::GetBackColor (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_BackColor);
	if (style)
		return style->mBackColor;
	return cDefBackColor;
}

int
RWStyle::GetJustification (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Justification);
	if (style)
		return style->mJustification;
	return cDefJustification;
}


int
RWStyle::GetVerticalJustification (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_VertJustification);
	if (style)
		return style->mVertJustification;
	return cDefVertJustification;
}

bool
RWStyle::ShouldWrap (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Wrap);
	if (style)
		return style->mWrap;
	return false;
}

SRGBColor
RWStyle::GetFrameColor (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_FrameColor);
	if (style)
		return style->mFrameColor;
	return cDefFrameColor;
}

float
RWStyle::GetRotation (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_Rotation);
	if (style)
		return style->mRotation;
	return 0;
}

float
RWStyle::GetBaseLineShift (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_BaseLineShift);
	if (style)
		return style->mBaseLineShift;
	return 0;
}

float
RWStyle::GetHorizontalScale (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_HorizontalScale);
	if (style)
		return style->mHorizontalScale;
	return 1;
}

float
RWStyle::GetLineSpacing (void)
const
{
	const	RWStyle	*style = GetStyleForFeature (stf_LineSpacing);
	if (style)
		return style->mLineSpacing;
	return 1;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
RWStyle::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropID:				outValue.SetInteger (mId); break;
		case PSObjPropBaseID:			outValue.SetInteger (mBaseId); break;
		case PSObjPropFlags:			outValue.SetInteger (mFeatures); break;
		case PSObjPropName:				outValue.SetText (mName); break;
		case PSObjPropFontName:			outValue.SetText (GetFName()); break;
//		case PSObjPropPSName:			outValue.SetXMLText (mFontPSName); break;
		case PSObjPropSize:				outValue.SetReal (GetSize()); break;
		case PSObjPropStyleF:			outValue.SetInteger (GetStyle()); break;
		case PSObjPropStyleB:			outValue.SetBoolean (GetStyle() & st_bold); break;
		case PSObjPropStyleI:			outValue.SetBoolean (GetStyle() & st_italic); break;
		case PSObjPropStyleU:			outValue.SetBoolean (GetStyle() & st_underline); break;
		case PSObjPropStyleS:			outValue.SetBoolean (GetStyle() & st_strikethrough); break;
//		case PSObjPropHorizontalOffset:	outValue.SetReal (mHorizontalOffset); break;
//		case PSObjPropVerticalOffset:	outValue.SetReal (mVerticalOffset); break;
		case PSObjPropWrap:				outValue.SetBoolean (ShouldWrap()); break;
//		case PSObjPropFrame:			outValue.SetBoolean (mFrameText); break;
		case PSObjPropAlign:			
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (GetJustification()); 
			else
				outValue.SetXMLText (sJustification [GetJustification()]);
			break;									
		case PSObjPropVertAlign:		
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (GetVerticalJustification()); 
			else
				outValue.SetXMLText (sVAlignment [GetVerticalJustification()]);
			break;									
		case PSObjPropTextColor:		outValue.SetXMLText ((const char*) GetTextColor()); break;
		case PSObjPropBackColor:		outValue.SetXMLText ((const char*) GetBackColor()); break;
		case PSObjPropFrameColor:		outValue.SetXMLText ((const char*) GetFrameColor()); break;
		case PSObjPropRotation:			outValue.SetReal (GetRotation()); break;
		case PSObjPropBaseLineShift:	outValue.SetReal (GetBaseLineShift()); break;
		case PSObjPropHorizontalScale:	outValue.SetReal (GetHorizontalScale()); break;
		case PSObjPropLineSpacing:		outValue.SetReal (GetLineSpacing()); break;

		default:						return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
RWStyle::SetProperty (OSType id, RWValue &inValue)
{
	long	lVal;
	switch (id)
	{
		case PSObjPropID:				return SetIntegerProperty (inValue, mId, 0);
		case PSObjPropBaseID:			return SetIntegerProperty (inValue, mBaseId, 0);
		case PSObjPropFlags:
		{
			long	lVal;
			if (SetIntegerProperty (inValue, lVal, 0, stf_MASK))
			{
				mFeatures = UInt32 (lVal);
				return true;
			}
			break;
		}
		case PSObjPropName:				return SetStringProperty (inValue, mName);
		case PSObjPropFontName:
			if (SetStringProperty (inValue, mFontName)) 
			{
				if (mFeatures & stf_Based)
					mFeatures |= stf_Font;
				return true;
			}
			break;
//		case PSObjPropPSName:			return SetXMLStringProperty (inValue, mFontPSName);
		case PSObjPropSize:
			{
				float	oldFontSize = GetSize ();
				if (SetRealProperty (inValue, mFontSize, 4, 128))
				{
					if (mFontSize != oldFontSize) 
					{
						if (mFeatures & stf_Based)
							mFeatures |= stf_Size;
					}
					return true;
				}
				break;
			}
		case PSObjPropStyleF:
		{
			int	oldFontStyle = GetStyle();
			if (SetIntegerProperty (inValue, mFontStyle, 0, 7))
			{
				if (mFontStyle != oldFontStyle) 
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_Style;
				}
				return true;
			}
			break;
		}
		case PSObjPropStyleB:
		case PSObjPropStyleI:
		case PSObjPropStyleU:
		case PSObjPropStyleS:
		{
			bool	bVal;
			if (SetBooleanProperty (inValue, bVal))
			{
				
				int	mOldFontStyle = GetStyle();
				if (id == PSObjPropStyleB)
					mFontStyle = (GetStyle() & ~st_bold) | (bVal ? st_bold : 0);
				else if (id == PSObjPropStyleI)
					mFontStyle = (GetStyle() & ~st_italic) | (bVal ? st_italic : 0);
				else if (id == PSObjPropStyleU)
					mFontStyle = (GetStyle() & ~st_underline) | (bVal ? st_underline : 0);
				else // if (id == PSObjPropStyleS)
					mFontStyle = (GetStyle() & ~st_strikethrough) | (bVal ? st_strikethrough : 0);
				if (mFontStyle != mOldFontStyle) 
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_Style;
				}
				return true;
			}
			break;
		}
//		case PSObjPropHorizontalOffset:	return SetRealProperty (inValue, mHorizontalOffset, -10, 64);
//		case PSObjPropVerticalOffset:	return SetRealProperty (inValue, mVerticalOffset, -10, 64);
		case PSObjPropWrap:
		{
			bool	oldWrap = ShouldWrap();
			if (SetBooleanProperty (inValue, mWrap))
			{
				if (mWrap != oldWrap)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_Wrap;
				}
				return true;
			}
			break;
		}
//		case PSObjPropFrame:			return SetBooleanProperty (inValue, mFrameText);
		case PSObjPropHorAlign:
		case PSObjPropAlign:
			if ((lVal = SetListProperty (inValue, sJustification)) >= 0)
			{
				if (mJustification != lVal)
				{
					mJustification = lVal;
					if (mFeatures & stf_Based)
						mFeatures |= stf_Justification;
				}
				return true;
			}
			break;
		case PSObjPropVertAlign:
			if ((lVal = SetListProperty (inValue, sVAlignment)) >= 0)
			{
				if (mVertJustification != lVal)
				{
					mVertJustification = lVal;
					if (mFeatures & stf_Based)
						mFeatures |= stf_VertJustification;
				}
				return true;
			}
			break;
		case PSObjPropTextColor:
		{
			SRGBColor		color = GetTextColor();
			if (SetColorProperty (inValue, mTextColor))
			{
				if (mTextColor != color)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_TextColor;
				}
				return true;
			}
			break;
		}
		case PSObjPropBackColor:
		{
			SRGBColor		color = GetBackColor();
			if (SetColorProperty (inValue, mBackColor))
			{
				if (mBackColor != color)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_BackColor;
				}
				return true;
			}
			break;
		}
		case PSObjPropFrameColor:
		{
			SRGBColor		color = GetFrameColor();
			if (SetColorProperty (inValue, mFrameColor))
			{
				if (mFrameColor != color)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_FrameColor;
				}
				return true;
			}
			break;
		}
		case PSObjPropRotation:
		{
			float		oldValue = GetRotation();
			if (SetRealProperty (inValue, mRotation, -360, 360))
			{
				if (oldValue != mRotation)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_Rotation;
				}
				return true;
			}
			break;
		}
		case PSObjPropBaseLineShift:
		{
			float		oldValue = GetBaseLineShift();
			if (SetRealProperty (inValue, mBaseLineShift, -100, 256))
			{
				if (oldValue != mBaseLineShift)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_BaseLineShift;
				}
				return true;
			}
			break;
		}
		case PSObjPropHorizontalScale:
		{
			float		oldValue = GetHorizontalScale();
			if (SetRealProperty (inValue, mHorizontalScale, 0.1, 100))
			{
				if (oldValue != mHorizontalScale)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_HorizontalScale;
				}
				return true;
			}
			break;
		}
		case PSObjPropLineSpacing:
		{
			float		oldValue = GetLineSpacing();
			if (SetRealProperty (inValue, mLineSpacing, 0.5, 16))
			{
				if (oldValue != mLineSpacing)
				{
					if (mFeatures & stf_Based)
						mFeatures |= stf_LineSpacing;
				}
				return true;
			}
			break;
		}

		default:						return PSObject::SetProperty (id, inValue);
	}

	return false;
}




// ---------------------------------------------------------------------------
// ~RWStyleList								Destructor				  [public]
// ---------------------------------------------------------------------------

RWStyleList::~RWStyleList (void)
{
	{
		RWStyleList::const_iterator	it;

		for (it = begin(); it != end(); it++)
		{
			RWStyle	*style = (*it).second;
			delete style;
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FindStyle														  [public]
// ---------------------------------------------------------------------------

RWStyle*
RWStyleList::FindStyle (long inID)
const
{
	RWStyle	*style = NULL;

	if (size() > 0)
	{
		RWStyleList::const_iterator	it = find (inID);

		if(it == end())
			style = begin()->second;
		else 
			style = (*it).second;

		/* for (it = begin(); it != end(); it++)
		{
			style = *it;
			if (style->GetID() == inID)
				return style;
		}

		style = *begin(); */
	}

	return style;
}


// ---------------------------------------------------------------------------
// GetNewID															  [public]
// ---------------------------------------------------------------------------

long
RWStyleList::GetNewID (void) const
{
	if (size() == 0)	//mbs 20052011	don't crash!
		return 1;
	return	(*rbegin()).first + 1;
}
