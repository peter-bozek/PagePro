/*
 *  PSObjProps.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 20.10.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */
#ifndef	_PSObjProps_h_
#define	_PSObjProps_h_

enum	PSObjectPropertyIDs
{
	// Base / Object
	PSObjPropOID			= ' oid',		// this - object ID
//	PSObjPropUOID			= 'uoid',		// UUID - object ID
	PSObjPropKind			= 'kind',		// similar to GetKind() - object kind
	PSObjPropID				= '  id',		// object ID (user's text, except for Style (numeric ID))
	PSObjPropXML			= ' xml',		// XML of the object
	PSObjPropOrder			= 'objZ',		// mSeqID - object order
	PSObjPropRect			= 'rect',		// mPosition - position
	PSObjPropSelected		= 'selc',		// mSelected
	PSObjPropDrawingRect	= 'edre',		// mDrawRect
	PSObjPropPosTop			= 'post',		// for change by user interaction - absolute
	PSObjPropPosLeft		= 'posl',
	PSObjPropPosBottom		= 'posb',
	PSObjPropPosRight		= 'posr',
	PSObjPropPosWidth		= 'posw',
	PSObjPropPosHeight		= 'posh',
	PSObjPropRelPosTop		= 'rpot',
	PSObjPropRelPosLeft		= 'rpol',
	PSObjPropRelPosBottom	= 'rpob',
	PSObjPropRelPosRight	= 'rpor',
	PSObjPropRelMoveH		= 'movH',		// for change by user interaction - relative
	PSObjPropRelMoveV		= 'movV',

	PSObjPropData			= 'data',		// Text/Picture/...
	PSObjPropEncoding		= 'code',		// encoding, e.g. "base64"
//	PSObjPropUserText		= 'user',		// text for user's free use
	PSObjPropName			= 'name',		// mName

//	PSObjPropFixH			= 'fixH',		// mFixedH
	PSObjPropFixV			= 'fixV',		// mFixedV
//	PSObjPropBindH			= 'bndH',		// mBindH
	PSObjPropBindV			= 'bndV',		// mBindV
	PSObjPropAlign			= 'algn',		// mAlignment - { "none", "left", "center", "right" }
	PSObjPropDraw			= 'draw',		// mDraw - { "no", "yes", "on overflow", "always" }
	
	// PSObjPropRotate			= 'rota',		// mRotation: rotation in degrees ( 0  - 360)
	PSObjPropObjectRotation	= 'rotr',		// mReportRotation: rotation by 180 degrees, mObjectRotation: in degrees
	PSObjPropMirror			= 'mirr',		// mMirror
	PSObjPropSkew			= 'skew',		// mSkew

	// Group
	PSObjPropOGroup			= 'GRP#',
//	PSObjPropExpandH		= 'expH',		// mExpandH
	PSObjPropExpandV		= 'expV',		// mExpandV
	PSObjPropLocked			= 'lock',		// mLocked
	PSObjPropObjects		= 'objs',		// objects

	// Line
	PSObjPropOLine			= 'LINE',
	PSObjPropThickness		= 'thic',		// mThickness
	PSObjPropLineColor		= 'lclr',		// mLineColor
	PSObjPropFlags			= 'flgs',		// mFlags

	// Oval
	PSObjPropOOval			= 'OVAL',
//	PSObjPropLineColor		= 'lclr',		// mFrameColor
	PSObjPropFill			= 'fill',		// mFill
	PSObjPropFillColor		= 'fclr',		// mFillColor

	// Rect
	PSObjPropORect			= 'RECT',
	PSObjPropRows			= 'rows',		// mRows
	PSObjPropCols			= 'cols',		// mCols
//	PSObjPropFlags			= 'flgs',		// mFlags

	// Pict
	PSObjPropOPict			= 'PICT',
//	PSObjPropExpandH		= 'expH',		// mExpandH
//	PSObjPropExpandV		= 'expV',		// mExpandV
	PSObjPropFormat			= ' fmt',		// mFormat
	PSObjPropFrame			= 'fram',		// mFrame
//	PSObjPropData			= 'data',		// mPicture
	PSObjPropWidth			= 'widt',		// mWidth
	PSObjPropHeight			= 'high',		// mHeight

    // Text
	PSObjPropOText			= 'TEXT',
	PSObjPropStyle			= 'styl',		// mStyleID
//	PSObjPropExpandH		= 'expH',		// mExpandH
//	PSObjPropExpandV		= 'expV',		// mExpandV
	PSObjPropDynamic		= 'dyna',		// mIsDynamic
	PSObjPropAttributed		= 'attr',		// mIsAttributed
	PSObjPropKeepTogether	= 'keep',		// mKeepTogether
	PSObjPropDrawEmpty		= 'drem',		// mDrawIfEmpty
//	PSObjPropFrame			= 'fram',		// mFrame
	PSObjPropFrameOffset	= 'foff',		// mFrameOffset
	PSObjPropFrameThickness	= 'fthi',		// mFrameThickness
	PSObjPropFrameColor		= 'fclr',		// mFrameColor
//	PSObjPropData			= 'data',		// mText
	PSObjPropText			= 'text',		// mParsedText
// v1.4
	PSObjPropTabStops		= 'tabs',		// mTabStops

	// Variable
	PSObjPropOVar			= 'VARI',
	PSObjPropOFld			= ' FLD',
	PSObjPropSource			= ' src',		// mSource
	PSObjPropAlias			= 'alis',		// mAlias
//	PSObjPropFormat			= ' fmt',		// mFormat
	PSObjPropElement		= 'elem',		// mIndex
	PSObjPropCalcType		= 'calt',		// mCalcType
	PSObjPropRepeat			= 'repe',		// mRepeat
	PSObjPropRepeatOffset	= 'repo',		// mRepeatOffset
	PSObjPropScript			= 'scrp',		// mScript

	// Table
	PSObjPropOTable			= 'TABL',
//	PSObjPropStyle			= 'styl',		// mStyleID
//	PSObjPropFrame			= 'fram',		// mFrame
//	PSObjPropFrameOffset	= 'foff',		// mFrameOffset
//	PSObjPropFrameThickness	= 'fthi',		// mFrameThickness
//	PSObjPropFrameColor		= 'fclr',		// mFrameColor
	PSObjPropHGridThickness	= 'hgrt',		// mHGridThickness
//	PSObjPropHeight			= 'high',		// mRowHeight
	PSObjPropNumCols		= 'coln',		// mNumColumns
	PSObjPropNumHeadings	= 'hdrn',		// mNumTopHeadings
//	PSObjPropScript			= 'scrp',		// mScript
	PSObjPropHeader			= 'head',		// mHeaders
	PSObjPropColumn			= 'colu',		// mColumns
	PSObjPropOHdr			= 'H000',		// n-th row of mHeaders
	PSObjPropDrawHeaders	= 'hedr',		// mDrawHeaders
	PSObjPropDrawColumns	= 'codr',		// mDrawColumns

	// Table Header
	PSObjPropOTblHdr		= 'TbHd',
//	PSObjPropStyle			= 'styl',		// mStyleID
//	PSObjPropWidth			= 'widt',		// mWidth
//	PSObjPropHeight			= 'high',		// mHeight
	PSObjPropColSpan		= 'cspn',		// mColSpan
	PSObjPropRowSpan		= 'rspn',		// mRowSpan
//	PSObjPropData			= 'data',		// mText

	// Table Column
	PSObjPropOTblCol		= 'TbCo',
//	PSObjPropID				= '  id',		// mId
//	PSObjPropStyle			= 'styl',		// mStyleID
//	PSObjPropWidth			= 'widt',		// mWidth
	PSObjPropGrid			= 'grid',		// mGrid
//	PSObjPropSource			= ' src',		// mSource
//	PSObjPropFormat			= ' fmt',		// mFormat
//	PSObjPropData			= 'data',		// mTitle
	PSObjPropDuplicates		= 'dups',		// mPrintRepeatingValues
	PSObjPropRowNum			= 'rown',		// mPrintRowNum
//	PSObjPropScript			= 'scrp',		// mScript
	PSObjPropLevel			= 'exel',		// mLevel - execution order


	// Section
	PSObjPropHeaderSection	= 'HDrs',
	PSObjPropBrkHdrSection	= 'HDBR',
	PSObjPropScrapSection	= 'scrs',
	PSObjPropPageSection	= 'page',
	PSObjPropBodySection	= 'body',
	PSObjPropBrkFtrSection	= 'FOBR',
	PSObjPropFooterSection	= 'FOOs',
	PSObjPropWatermarkSection	= 'wate',

	PSObjPropType			= 'type',		// mType
//	PSObjPropName			= 'name',		// mName
//	PSObjPropHeight			= 'high',		// mHeight
	PSObjPropMinSpace		= 'mins',		// mMinSpace
	PSObjPropVisible		= 'visi',		// mVisible
//	PSObjPropDraw			= 'draw',		// mDraw
//	PSObjPropKeepTogether	= 'keep',		// mKeepTogether
	PSObjPropPageThrow		= 'pthr',		// mPageThrow
//	PSObjPropScript			= 'scrp',		// mScript
//	PSObjPropObjects		= 'objs',		// objects
//	PSObjPropExpandV		= 'expV',		// r.height vs. mHeight

	// Header / Footer Section
	PSObjPropFixed			= 'fixd',		// mFixed
	PSObjPropFirstPage		= 'pagF',		// mFirstPage
	PSObjPropEvenPage		= 'pagE',		// mEvenPage
	PSObjPropOddPage		= 'pagO',		// mOddPage
	PSObjPropLastPage		= 'pagL',		// mLastPage
	PSObjPropBind			= 'bind',		// mFromBottom - footer only
//	PSObjPropFill			= 'fill',		// mFillPage - footer only

	// Break Section
	PSObjPropBreakOnField	= 'brkF',		// mBreakType == EBreakOn_Field
	PSObjPropBreakOnVariable= 'brkV',		// mBreakType == EBreakOn_Variable
	PSObjPropBreakOnArray	= 'brkA',		// mBreakType == EBreakOn_Array
	PSObjPropBreakLevel		= 'brkL',		// mLevel
	PSObjPropPrintAlways	= 'alwa',		// mPrintAlways
	PSObjPropBreakOn		= 'brkO',		// mBreakOn
	PSObjPropBreakType		= 'brkT',		// mBreakType

	// Watermark Section
	PSObjPropOnTop			= 'onto',		// mOnTop

	// Document
	PSObjPropVersion		= 'vers',
//	PSObjPropName			= 'name',		// mName
//	PSObjPropWidth			= 'widt',		// mPageWidth
//	PSObjPropHeight			= 'high',		// mPageHeight
	PSObjPropPaper			= 'phys',		// mUsePhysical
	PSObjPropMargins		= 'marg',		// mPageMargins
//	PSObjPropStyle			= 'styl',		// mStyles
//	PSObjPropBodySection	= 'body',		// mBody
	PSObjPropPageSections	= 'pags',		// mPageSections - headers/footers
	PSObjPropBreakHeaders	= 'brkh',		// mBreakHeaders
	PSObjPropBreakFooters	= 'brkf',		// mBreakFooters
//	PSObjPropWatermarkSection	= 'wate',	// mWatermark
//	PSObjPropPageSetup		= 'pgsc',		// mPageSetup - Classic (obsolete/unused)
	PSObjPropPageFormat		= 'pgfm',		// mPageFormat
	PSObjPropPrintSettings	= 'pgst',		// mPrintSettings
	PSObjPropDevMode		= 'wdmh',		// mDevMode
	PSObjPropDeviceNames	= 'wdnh',		// mDeviceNames
	PSObjPropPrintDlg		= 'wpdh',		// mPrintDialog
	PSObjPropPageSetupDlg	= 'wpsh',		// mPageSetupDialog

// v1.4
	PSObjPropLabel			= 'labl',		// mLabelReport
	PSObjPropLabelH			= 'labH',		// mLabelH
	PSObjPropLabelV			= 'labV',		// mLabelV
	
	PSObjPropLabelMTop		= 'laMt',		// mLabelMarginTop
	PSObjPropLabelMLeft		= 'laMl',		// mLabelMarginLeft
	PSObjPropLabelMBottom	= 'laMb',		// mLabelMarginBottom
	PSObjPropLabelMRight	= 'laMr',		// mLabelMarginRight
	
	// Editor
	PSObjPropEditor			= 'EDIT',
	PSObjPropShowMargins	= 'mars',		// mShowMargins
	PSObjPropShowRuler		= 'rush',		// mShowRuler
	PSObjPropRulerUnits		= 'rulu',		// mRulerUnits
	PSObjPropGridSize		= 'grsi',		// mGridSize
	PSObjPropShowGrid		= 'grsh',		// mShowGrid
	PSObjPropSnapToGrid		= 'grsn',		// mSnapToGrid
	PSObjPropShowGuides		= 'gush',		// mShowGuides
	PSObjPropLockGuides		= 'gulo',		// mLockGuides
	PSObjPropSnapToGuides	= 'gusn',		// mSnapToGide
//	PSObjPropShowSections	= 'sesh',		// mShowSections
	PSObjPropLockSections	= 'selo',		// mLockSections
	PSObjPropShowObjBorders	= 'obor',		// mShowObjBorders
	PSObjPropScale			= 'scal',		// mScale
	PSObjPropGridColor		= 'grco',		// mGridColor
	PSObjPropGridRadius		= 'grra',		// mGridRadius
	PSObjPropGuideColor		= 'guco',		// mGuideColor
	PSObjPropGuideWidth		= 'guwi',		// mGuideWidth

	// Guides
	PSObjPropOGuides		= 'GUID',
	PSObjPropOGuideH		= 'GUHo',
	PSObjPropOGuideV		= 'GUVe',

	// Data Source
	PSObjPropDataSource		= 'DATA',

	// 4D Data Source
//	PSObjPropDataSource		= 'DATA',
//	PSObjPropType			= 'type',		// "4D"
//	PSObjPropSource			= ' src',		// mSource
	PSObjPropIterations		= 'iter',		// mNumIterations
	PSObjPropTableID		= 'tbid',		// mMainTable
//	PSObjPropName			= 'name',		// mName
	PSObjPropRelateOne		= 'rel1',		// mRelateOne
	PSObjPropRelateMany		= 'relM',		// mRelateMany
	PSObjPropCallback		= 'call',		// mCallBackName
	PSObjPropStartScript	= 'scrS',		// StartScript
	PSObjPropBodyScript		= 'scrB',		// BodyScript
	PSObjPropEndScript		= 'scrE',		// EndScript
	PSObjPropSRPCompatibility= 'SRPc',		// mSRPCompatibility - set SRDate & co

	// Style
	PSObjPropStyleSet		= 'STL#',
	PSObjPropOStyle			= 'STYL',
//	PSObjPropID				= '  id',		// mId
	PSObjPropBaseID			= 'baid',		// mBaseId
//	PSObjPropFlags			= 'flgs',		// mFeatures
//	PSObjPropName			= 'name',		// mName
	PSObjPropFontName		= 'fnam',		// mFontName
//	PSObjPropPSName			= 'psnm',		// mFontPSName
	PSObjPropSize			= 'size',		// mFontSize
	PSObjPropStyleF			= 'styF',		// mFontStyle
	PSObjPropStyleB			= 'styB',
	PSObjPropStyleI			= 'styI',
	PSObjPropStyleU			= 'styU',
	PSObjPropStyleS			= 'styS',
	PSObjPropTextColor		= 'tclr',		// mTextColor
	PSObjPropBackColor		= 'bclr',		// mBackColor
	PSObjPropHorAlign		= 'halg',		// mJustification - { "default", "left", "right", "center", "justify", "fulljustify" }
	PSObjPropVertAlign		= 'valg',		// mVerticalJustification - { "default", "top", "bottom", "center" }
//	PSObjPropHorizontalOffset= 'hoff',		// mHorizontalOffset
//	PSObjPropVerticalOffset	= 'voff',		// mVerticalOffset
	PSObjPropWrap			= 'wrap',		// mWrap
//	PSObjPropFrame			= 'fram',		// mFrameText
//	PSObjPropFrameColor		= 'fclr',		// mFrameColor
	PSObjPropRotation		= 'rotd',		// mRotation - in degrees (0-360)
	PSObjPropBaseLineShift	= 'basl',		// mBaseLineShift
	PSObjPropHorizontalScale= 'hors',		// mHorizontalScale
	PSObjPropLineSpacing	= 'lisp',		// mLineSpacing


	PSObjPropWild			= '****'		// last, unused
};

#endif
