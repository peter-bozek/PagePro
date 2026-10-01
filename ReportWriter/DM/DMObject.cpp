/*
 *  DMObject.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 29.09.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

# include	"DMObject.h"
# include	"DMReport.h"
# include	"DMArea.h"
# include	"PSObjProps.h"
# include	<algorithm>
# include	"SR4DData.h"	// Get4DPicture
// using namespace    FourDAPIEx;


// # define	kSelectionSquareSize	3.0f
// # define	kSelectionSquareOffset	2.0f

# define	ALL_BaseProps	\
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true		},	\
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true					},	\
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }							},	\
{ PSObjPropOrder,			true,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 0, LONG_MAX }, true		},	\
{ PSObjPropVisible,			true,	PSProps_Attribute,	PSProps_Boolean,	"visible",			{ NULL, 1, 0, 1 }					},	\
{ PSObjPropLocked,			true,	PSProps_Attribute,	PSProps_Integer,	"locked",			{ NULL, 0, 0, 2 }					},	\
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }					},	\
{ PSObjPropDrawingRect,		false,	PSProps_Attribute,	PSProps_Rect,		"drawRect",			{ NULL }, true						},	\
{ PSObjPropXML,				false,	PSProps_Attribute,	PSProps_Rect,		"xml",				{ NULL }, true						},

# define	ALL_PositionProps	\
{ PSObjPropRect,			true,	PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }							},	\
{ PSObjPropPosLeft,			true,	PSProps_Attribute,	PSProps_Real,		"r.left",			{ NULL, 0, INT_MIN, INT_MAX }, true	},	\
{ PSObjPropPosTop,			true,	PSProps_Attribute,	PSProps_Real,		"r.top",			{ NULL, 0, INT_MIN, INT_MAX }, true	},	\
{ PSObjPropPosRight,		true,	PSProps_Attribute,	PSProps_Real,		"r.right",			{ NULL, 0, INT_MIN, INT_MAX }, true	},	\
{ PSObjPropPosBottom,		true,	PSProps_Attribute,	PSProps_Real,		"r.bottom",			{ NULL, 0, INT_MIN, INT_MAX }, true	},	\
{ PSObjPropPosWidth,		true,	PSProps_Attribute,	PSProps_Real,		"r.width",			{ NULL, 0, 1, INT_MAX }, true		},	\
{ PSObjPropPosHeight,		true,	PSProps_Attribute,	PSProps_Real,		"r.height",			{ NULL, 0, 1, INT_MAX }, true		},	\
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }				},	\
{ PSObjPropMirror,			true,	PSProps_Attribute,	PSProps_Boolean,	"mirror",			{ NULL, 0, 0, 1 }					},

// pB matrix manipulation is added to position properties, 

//{ PSObjPropFixH,			true,	PSProps_Attribute,	PSProps_Boolean,	"fixH",				{ NULL, 0, 0, 1 } 					},
//{ PSObjPropBindH,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindH",			{ NULL, 0, 0, 1 } 					},

# define	ALL_ObjProps	\
ALL_BaseProps	\
ALL_PositionProps	\
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }							},	\
{ PSObjPropFixV,			true,	PSProps_Attribute,	PSProps_Boolean,	"fixV",				{ NULL, 0, 0, 1 } 					},	\
{ PSObjPropBindV,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindV",			{ NULL, 0, 0, 1 } 					},	\
{ PSObjPropAlign,			true,	PSProps_Attribute,	PSProps_List,		"align",			{ sAlignment, 0 }					},	\
{ PSObjPropAlign,			true,	PSProps_Attribute,	PSProps_List,		"align",			{ sAlignment2, 0 }, true			},	\
{ PSObjPropAlign,			true,	PSProps_Attribute,	PSProps_Integer,	"align",			{ NULL, 0 }, true					},	\
{ PSObjPropDraw,			true,	PSProps_Attribute,	PSProps_List,		"draw",				{ sDraw, eDraw_Yes }				},	\

const PSObject::PSObjProps	DMBase::sProperties[] = {
ALL_BaseProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }							}
};

const PSObject::PSObjProps	DMObject::sProperties[] = {
ALL_ObjProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }							}
};

const PSObject::PSObjProps	DMGroup::sProperties[] = {
ALL_ObjProps
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",				{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",				{ NULL, 0, 0, 1 }				},
{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }, true	},
{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }, true	},
{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }, true	},
{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }, true	},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }, true	},
{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }, true	},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }, true	},
{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }, true	},
{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }, true	},
{ PSObjPropObjects,			false,	PSProps_Childs,		PSProps_Objects,	"Objects",				{ NULL }		},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",				{ NULL }, true	},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",				{ NULL }, true	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,					{ NULL }						}
};

const PSObject::PSObjProps	DMLine::sProperties[] = {
ALL_ObjProps
{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"flags",			{ NULL, RWLine_Horizontal, 0, RWLine_Full }	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMOval::sProperties[] = {
ALL_ObjProps
{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},

{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMRect::sProperties[] = {
ALL_ObjProps
{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},

{ PSObjPropRows,			true,	PSProps_Attribute,	PSProps_Integer,	"rows",				{ NULL, 1, 1, 512 }				},
{ PSObjPropCols,			true,	PSProps_Attribute,	PSProps_Integer,	"cols",				{ NULL, 1, 1, 512 }				},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"flags",			{ NULL, RWRect_Full, 0, 15 }	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMPict::sProperties[] = {
ALL_ObjProps
//{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
//{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
//{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
//{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},

//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_List,		"format",			{ sPictFormat, ePictFormat_Normal, ePictFormat_First, ePictFormat_Last-1 }, true },
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_Integer,	"format",			{ NULL, ePictFormat_Normal, ePictFormat_First, ePictFormat_Last-1 }				},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_OneChild,	PSProps_BLOB,		"ImageData",		{ NULL }						},
{ PSObjPropWidth,			false,	PSProps_Attribute,	PSProps_Real,		"img.width",		{ NULL, 0, INT_MIN, INT_MAX }, true	},
{ PSObjPropHeight,			false,	PSProps_Attribute,	PSProps_Real,		"img.height",		{ NULL, 0, INT_MIN, INT_MAX }, true	},
{ 'imgs',					false,	PSProps_Attribute,	PSProps_Integer,	"img.size",			{ NULL, 0, INT_MIN, INT_MAX }, true	},
{ PSObjPropObjectRotation,	true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

static const char*	sPictDataProperties[] = { "ImageData", "format", "encoding", "base64" };


const PSObject::PSObjProps	DMText::sProperties[] = {
ALL_ObjProps
//{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
//{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
//{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
//{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }				},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }			},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }				},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }			},

//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},
{ PSObjPropText,			true,	PSProps_Value,		PSProps_String,		"resource",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMVariable::sProperties[] = {
ALL_ObjProps
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }				},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }			},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }				},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }			},

//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
//{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},

{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_String,		"source",			{ NULL }						},
{ PSObjPropAlias,			true,	PSProps_Attribute,	PSProps_String,		"alias",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
{ PSObjPropElement,			true,	PSProps_Attribute,	PSProps_Integer,	"elem",				{ NULL, SR4DVariable_Variable, SR4DVariable_Variable, INT_MAX }		},
{ PSObjPropCalcType,		true,	PSProps_Attribute,	PSProps_Integer,	"calc",				{ NULL, ECalcType_None, ECalcType_None, ECalcType_Last-1 }		},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat, 0 }					},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat2, 0 }, true			},
{ PSObjPropRepeatOffset,	true,	PSProps_Attribute,	PSProps_Real,		"repeatOffset",		{ NULL, 0, 0, 128 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	// child element(s)
{ PSObjPropText,			true,	PSProps_Value,		PSProps_String,		"resource",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMField::sProperties[] = {
ALL_ObjProps
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }				},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }			},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }				},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }			},
	
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
//{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},

{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_String,		"source",			{ NULL }						},
{ PSObjPropAlias,			true,	PSProps_Attribute,	PSProps_String,		"alias",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
//{ PSObjPropElement,		true,	PSProps_Attribute,	PSProps_Integer,	"elem",				{ NULL, SR4DVariable_Variable, SR4DVariable_Variable, INT_MAX }		},
{ PSObjPropCalcType,		true,	PSProps_Attribute,	PSProps_Integer,	"calc",				{ NULL, ECalcType_None, ECalcType_None, ECalcType_Last-1 }		},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat, 0 }					},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat2, 0 }, true			},
{ PSObjPropRepeatOffset,	true,	PSProps_Attribute,	PSProps_Real,		"repeatOffset",		{ NULL, 0, 0, 128 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	// child element(s)
{ PSObjPropText,			true,	PSProps_Value,		PSProps_String,		"resource",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMHeader::sProperties[] = {
// ALL_BaseProps, but Visible is not stored
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true				},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropOrder,			true,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 0, LONG_MAX }, true	},
{ PSObjPropVisible,			true,	PSProps_Attribute,	PSProps_Boolean,	"visible",			{ NULL, 1, 0, 1 }, true			},
{ PSObjPropLocked,			true,	PSProps_Attribute,	PSProps_Integer,	"locked",			{ NULL, 0, 0, 2 }				},
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawingRect,		false,	PSProps_Attribute,	PSProps_Rect,		"drawRect",			{ NULL }, true					},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }				},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }			},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }				},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }			},
	
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},

{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"width",			{ NULL, 0, 0, 512 }				},
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height",			{ NULL, 0, 0, 512 }				},
{ PSObjPropColSpan,			true,	PSProps_Attribute,	PSProps_Integer,	"colspan",			{ NULL, 1, 0, 100 }				},
{ PSObjPropRowSpan,			true,	PSProps_Attribute,	PSProps_Integer,	"rowspan",			{ NULL, 1, 0, 100 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},
{ PSObjPropPosWidth,		true,	PSProps_Attribute,	PSProps_Real,		"r.width",			{ NULL, 0, 1, INT_MAX }, true	},
{ PSObjPropPosHeight,		true,	PSProps_Attribute,	PSProps_Real,		"r.height",			{ NULL, 0, 1, INT_MAX }, true	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMColumn::sProperties[] = {
// ALL_BaseProps, but Id is Order - for report processing
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true				},
//{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
//{ PSObjPropOrder,			true,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 0, LONG_MAX }, true		},
{ PSObjPropOrder,			true,	PSProps_Attribute,	PSProps_Integer,	"id",				{ NULL, 0, 1, LONG_MAX }		},
//{ PSObjPropVisible,			true,	PSProps_Attribute,	PSProps_Boolean,	"visible",			{ NULL, 1, 0, 1 }				},
{ PSObjPropLocked,			true,	PSProps_Attribute,	PSProps_Integer,	"locked",			{ NULL, 0, 0, 2 }				},
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawingRect,		false,	PSProps_Attribute,	PSProps_Rect,		"drawRect",			{ NULL }, true					},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }						},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }				},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }				},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }				},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }				},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }				},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }			},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }				},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }						},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }						},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }			},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }			},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }			},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }			},
	
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},

{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"width",			{ NULL, 0, 0, 512 }				},
{ PSObjPropGrid,			true,	PSProps_Attribute,	PSProps_Boolean,	"grid",				{ NULL, 1, 0, 1 }				},
{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_XMLString,	"source",			{ NULL }						},
{ PSObjPropAlias,			true,	PSProps_Attribute,	PSProps_String,		"alias",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
//{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},
{ PSObjPropRowNum,			true,	PSProps_Attribute,	PSProps_Boolean,	"rownum",			{ NULL, 0, 0, 1 }				},
{ PSObjPropDuplicates,		true,	PSProps_Attribute,	PSProps_Boolean,	"duplicates",		{ NULL, 1, 0, 1 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropLevel,			true,	PSProps_Attribute,	PSProps_Integer,	"level",			{ NULL, 0, 0, 10 }				},
{ PSObjPropPosWidth,		true,	PSProps_Attribute,	PSProps_Real,		"r.width",			{ NULL, 0, 1, INT_MAX }, true	},
{ PSObjPropPosHeight,		true,	PSProps_Attribute,	PSProps_Real,		"r.height",			{ NULL, 0, 1, INT_MAX }, true	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMTable::sProperties[] = {
ALL_ObjProps
{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, 0, 0, INT_MAX }			},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Integer,	"frame",			{ NULL, 1, 0, 2 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
{ PSObjPropHGridThickness,	true,	PSProps_Attribute,	PSProps_Real,		"hGridThickness",	{ NULL, 0.5, 0, 10 }			},

{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height", 			{ NULL, 0, 0, INT_MAX }			},
{ PSObjPropNumCols,			false,	PSProps_Attribute,	PSProps_Integer,	"cols",				{ NULL }						},
{ PSObjPropNumHeadings,		false,	PSProps_Attribute,	PSProps_Integer,	"hdrs",				{ NULL }, true					},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropHeader,			true,	PSProps_OneContainer, PSProps_Objects,	"Head",				{ NULL }						},
{ PSObjPropColumn,			true,	PSProps_OneContainer, PSProps_Objects,	"Columns",			{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMTable::sPropertiesHead[] = {
{ PSObjPropDrawHeaders,		true,	PSProps_Attribute,	PSProps_Boolean,	"draw",				{ NULL, 1, 0, 1 }				},
{ PSObjPropNumHeadings,		true,	PSProps_Attribute,	PSProps_Integer,	"rows",				{ NULL }						},
{ PSObjPropHeaderSection,	true,	PSProps_Container,	PSProps_Objects,	"tr", 				{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMTable::sPropertiesHeader[] = {
{ PSObjPropOTblHdr,			true,	PSProps_Childs,		PSProps_Objects,	"td", 				{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMTable::sPropertiesColumns[] = {
{ PSObjPropDrawColumns,		true,	PSProps_Attribute,	PSProps_Boolean,	"draw",				{ NULL, 1, 0, 1 }				},
{ PSObjPropOTblCol,			true,	PSProps_Childs,		PSProps_Objects,	"Col", 				{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

#pragma mark	-

const DMBase::UserProps	DMGroup::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMLine::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
//{ PSObjPropExpandV, 		true	},
{ PSObjPropThickness,		true	},
{ PSObjPropLineColor,		true	},
{ PSObjPropFlags,			true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMOval::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
//{ PSObjPropExpandV, 		true	},
{ PSObjPropThickness,		true	},
{ PSObjPropLineColor,		true	},

{ PSObjPropFill,			true	},
{ PSObjPropFillColor,		true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMRect::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
//{ PSObjPropExpandV, 		true	},
{ PSObjPropThickness,		true	},
{ PSObjPropLineColor,		true	},

{ PSObjPropFill,			true	},
{ PSObjPropFillColor,		true	},

{ PSObjPropRows,			true	},
{ PSObjPropCols,			true	},
{ PSObjPropFlags,			true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMPict::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},

//{ PSObjPropFill,			true	},
{ PSObjPropBackColor,		true	},

{ PSObjPropFormat,			true	},
{ PSObjPropFrame,			true	},
{ PSObjPropFrameOffset,		true	},
{ PSObjPropFrameThickness,	true	},
{ PSObjPropFrameColor,		true	},
{ PSObjPropData,			true	},
{ PSObjPropWidth,			false	},
{ PSObjPropHeight,			false	},
{ PSObjPropObjectRotation,  true    },
{ 'imgs',					false	},
{ 0, 						false	}
};


const DMBase::UserProps	DMText::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
//{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},

{ PSObjPropStyle,			true	},
{ PSObjPropFontName,		true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
// { PSObjPropRotation,		true	},
{ PSObjPropWrap,			true	},
{ PSObjPropHorAlign,		true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},

{ PSObjPropDynamic,			true	},
{ PSObjPropAttributed,		true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropDrawEmpty,		true	},
{ PSObjPropFrame,			true	},
{ PSObjPropFrameOffset,		true	},
{ PSObjPropFrameThickness,	true	},
{ PSObjPropFrameColor,		true	},
{ PSObjPropData,			true	},
{ PSObjPropText,			false	},
{ 0, 						false	}
};

const DMBase::UserProps	DMVariable::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
//{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},

{ PSObjPropStyle,			true	},
{ PSObjPropFontName,		true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
// { PSObjPropRotation,		true	},
{ PSObjPropWrap,			true	},
{ PSObjPropHorAlign,		true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},

{ PSObjPropAttributed,		true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropDrawEmpty,		true	},
{ PSObjPropFrame,			true	},
{ PSObjPropFrameOffset,		true	},
{ PSObjPropFrameThickness,	true	},
{ PSObjPropFrameColor,		true	},

{ PSObjPropSource,			true	},
{ PSObjPropElement,			true	},
{ PSObjPropAlias,			true	},
{ PSObjPropFormat,			true	},
{ PSObjPropCalcType,		true	},
{ PSObjPropRepeat,			true	},
{ PSObjPropRepeatOffset,	true	},
{ PSObjPropScript,			true	},
{ PSObjPropText,			false	},
{ 0, 						false	}
};

const DMBase::UserProps	DMField::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
//{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},

{ PSObjPropStyle,			true	},
{ PSObjPropFontName,		true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
// { PSObjPropRotation,		true	},
{ PSObjPropWrap,			true	},
{ PSObjPropHorAlign,		true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},

{ PSObjPropAttributed,		true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropDrawEmpty,		true	},
{ PSObjPropFrame,			true	},
{ PSObjPropFrameOffset,		true	},
{ PSObjPropFrameThickness,	true	},
{ PSObjPropFrameColor,		true	},

{ PSObjPropSource,			true	},
{ PSObjPropAlias,			true	},
{ PSObjPropFormat,			true	},
{ PSObjPropCalcType,		true	},
{ PSObjPropRepeat,			true	},
{ PSObjPropRepeatOffset,	true	},
{ PSObjPropScript,			true	},
{ PSObjPropText,			false	},
{ 0, 						false	}
};

const DMBase::UserProps	DMHeader::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			false	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},

{ PSObjPropStyle,			true	},
{ PSObjPropFontName,		true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
// { PSObjPropRotation,		true	},
{ PSObjPropWrap,			true	},
{ PSObjPropHorAlign,		true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},
{ PSObjPropDynamic,			true	},
{ PSObjPropAttributed,		true	},

{ PSObjPropWidth,			true	},
{ PSObjPropHeight,			true	},
{ PSObjPropColSpan,			true	},
{ PSObjPropRowSpan,			true	},
{ PSObjPropData,			true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMColumn::sUserProperties[] = {
{ PSObjPropKind,			false	},
//{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
//{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},

{ PSObjPropStyle,			true	},
{ PSObjPropFontName,		true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
// { PSObjPropRotation,		true	},
{ PSObjPropWrap,			true	},
{ PSObjPropHorAlign,		true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},
{ PSObjPropAttributed,		true	},

{ PSObjPropWidth,			true	},
{ PSObjPropGrid,			true	},
{ PSObjPropSource,			true	},
{ PSObjPropAlias,			true	},
{ PSObjPropFormat,			true	},
//{ PSObjPropData,			true	},
{ PSObjPropRowNum,			true	},
{ PSObjPropDuplicates,		true	},
{ PSObjPropScript,			true	},
{ PSObjPropLevel,			true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMTable::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
{ PSObjPropRect,			true	},
{ PSObjPropPosLeft,			true	},
{ PSObjPropPosTop,			true	},
{ PSObjPropPosRight,		true	},
{ PSObjPropPosBottom,		true	},
{ PSObjPropPosWidth,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropName,			true	},
{ PSObjPropFixV,			true	},
//{ PSObjPropBindH,			true	},
//{ PSObjPropBindV,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropDraw,			true	},

//{ PSObjPropExpandH, 		true	},
{ PSObjPropExpandV, 		true	},

{ PSObjPropStyle,			true	},
{ PSObjPropFrame,			true	},
{ PSObjPropFrameOffset,		true	},
{ PSObjPropFrameThickness,	true	},
{ PSObjPropFrameColor,		true	},
{ PSObjPropHGridThickness,	true	},

{ PSObjPropHeight,			true	},
{ PSObjPropDrawHeaders,		true	},
{ PSObjPropDrawColumns,		true	},
{ PSObjPropScript,			true	},
{ PSObjPropNumCols,			true	},
{ PSObjPropNumHeadings,		true	},
{ 0, 						false	}
};

long		DMGroup::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMLine::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMOval::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMRect::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMPict::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMText::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMVariable::GetUserProperties (const UserProps* &outProps) const{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMField::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMHeader::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMColumn::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long		DMTable::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }

#pragma	mark	-

// ---------------------------------------------------------------------------
// DMBase									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMBase::DMBase (DMBase *inParent, EObject_Kind inKind)
	:	PSObject (inKind),
		mParent (inParent),
		mSeqID (0),
		mPosition (0, 0, 0, 0),
		mVisible (true),
		mLocked (0),
		mSelected (false),
		mDrawRect (0, 0, 0, 0)
{
	if (mParent)
	{
		DMReport* report = GetReport();
		if (report && report != this)
			report->AddObject (this);
	}
}


// ---------------------------------------------------------------------------
// ~DMBase									Destructor				  [public]
// ---------------------------------------------------------------------------

DMBase::~DMBase (void)
{
	if (mParent)
	{
		DMReport* report = GetReport();
		if (report && report != this)
			report->DeleteObject (this);
	}
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMBase::Init (void)
{
	mVisible = true;
	mLocked = 0;
	if (mSelected)
		SetSelected (false);
	mID.Free();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMBase::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	PSObject::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// SetSelected														  [public]
// ---------------------------------------------------------------------------
void
DMBase::SetSelected (bool inSelect)
{
	if (mSelected != inSelect)
	{
		mSelected = inSelect;
		DMReport* report = GetReport();
        if (report) {
			if (mSelected)
				report->AddSelectedObject (this);
			else
				report->RemoveSelectedObject (this);
        }
	}
}


// ---------------------------------------------------------------------------
// CompareOrder														  [public]
// ---------------------------------------------------------------------------

int
DMBase::CompareOrder (const DMBase *other)
const
{
	if (mSeqID < other->mSeqID)
		return -1;
	else if (mSeqID > other->mSeqID)
		return 1;

	return 0;	// should not happen - seqID should be unique within a section...
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBase::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropID:			outValue.SetText (mID); break;
		case PSObjPropOrder:		outValue.SetInteger (mSeqID); break;
		case PSObjPropRect:			outValue.SetText (mPosition.ToString()); break;
		case PSObjPropVisible:		outValue.SetBoolean (mVisible); break;
		case PSObjPropLocked:		outValue.SetInteger (mLocked); break;
		case PSObjPropSelected:		outValue.SetBoolean (mSelected); break;
		case PSObjPropDrawingRect:	outValue.SetText (mDrawRect.ToString()); break;

		case PSObjPropPosTop:		outValue.SetReal (mPosition.top); break;
		case PSObjPropPosLeft:		outValue.SetReal (mPosition.left); break;
		case PSObjPropPosBottom:	outValue.SetReal (mPosition.bottom); break;
		case PSObjPropPosRight:		outValue.SetReal (mPosition.right); break;
		case PSObjPropPosWidth:		outValue.SetReal (mPosition.Width()); break;
		case PSObjPropPosHeight:	outValue.SetReal (mPosition.Height()); break;

		default:
			return PSObject::GetProperty (id, outValue);
			break;
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBase::SetProperty (OSType id, RWValue &inValue)
{
	float	fVal;

	switch (id)
	{
		case PSObjPropID:			return SetStringProperty (inValue, mID);
		case PSObjPropOrder:
		{
			long	lVal;
			if (SetIntegerProperty (inValue, lVal, 1))
			{
				AdjustOrder (eOrder_Set, lVal);
				return true;
			}
			break;
		}
		case PSObjPropRect:			return SetRectProperty (inValue, mPosition);

		case PSObjPropVisible:		return SetBooleanProperty (inValue, mVisible);
		case PSObjPropLocked:		return SetIntegerProperty (inValue, mLocked, 0, 2);
		case PSObjPropSelected:
			if (inValue.CoerceValue (RWValue::eValue_Boolean))
			{
				SetSelected (inValue.GetBoolean());
				return true;
			}
			break;

		case PSObjPropPosTop:
		case PSObjPropPosLeft:
		case PSObjPropPosBottom:
		case PSObjPropPosRight:
			if (SetRealProperty (inValue, fVal))
			{
				AdjustPosition (id, mPosition, fVal);
//				AdjustPosition (id, mDrawRect, fVal);
				return true;
			}
			break;
		case PSObjPropPosWidth:
		case PSObjPropPosHeight:
			if (SetRealProperty (inValue, fVal) && fVal > 0)
			{
				AdjustPosition (id, mPosition, fVal);
//				AdjustPosition (id, mDrawRect, fVal);
				return true;
			}
			break;
		case PSObjPropRelPosTop:
		case PSObjPropRelPosLeft:
		case PSObjPropRelPosBottom:
		case PSObjPropRelPosRight:
		case PSObjPropRelMoveH:
		case PSObjPropRelMoveV:
			if (SetRealProperty (inValue, fVal) && fVal != 0)
			{
				AdjustPosition (id, mPosition, fVal);
//				AdjustPosition (id, mDrawRect, fVal);
				return true;
			}
			break;

		default:
			return PSObject::SetProperty (id, inValue);
			break;
	}

	return false;
}


// ---------------------------------------------------------------------------
// AdjustPosition													  [public]
// ---------------------------------------------------------------------------

void
DMBase::AdjustPosition (OSType id, SRect &rect, float fVal)
{
	switch (id)
	{
		case PSObjPropPosTop:
			rect.top = fVal;
			if (rect.top > rect.bottom)
				rect.bottom = rect.top;
			break;

		case PSObjPropPosLeft:
			rect.left = fVal;
			if (rect.left > rect.right)
				rect.right = rect.left;
			break;

		case PSObjPropPosBottom:
			rect.bottom = fVal;
			if (rect.top > rect.bottom)
				rect.bottom = rect.top;
			break;

		case PSObjPropPosRight:
			rect.right = fVal;
			if (rect.left > rect.right)
				rect.right = rect.left;
			break;

		case PSObjPropPosWidth:
			rect.right = rect.left + fVal;
			break;

		case PSObjPropPosHeight:
			rect.bottom = rect.top + fVal;
			break;

		case PSObjPropRelPosTop:
			rect.top += fVal;
			if (rect.top > rect.bottom)
				rect.bottom = rect.top;
			break;

		case PSObjPropRelPosLeft:
			rect.left += fVal;
			if (rect.left > rect.right)
				rect.right = rect.left;
			break;

		case PSObjPropRelPosBottom:
			rect.bottom += fVal;
			if (rect.top > rect.bottom)
				rect.bottom = rect.top;
			break;

		case PSObjPropRelPosRight:
			rect.right += fVal;
			if (rect.left > rect.right)
				rect.right = rect.left;
			break;

		case PSObjPropRelMoveH:
			rect.left += fVal;
			rect.right += fVal;
			break;

		case PSObjPropRelMoveV:
			rect.top += fVal;
			rect.bottom += fVal;
			break;
	}

	return;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

PSObjList *
DMBase::GetObjects (OSType id)
{
	PSObjList	*l = NULL;
	PSObjListD	*ld = NULL;
	if (this->GetObjects (id, ld) && ld != NULL)
		l = new PSObjList (*ld);
	return l;
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
DMBase::GetObjects (OSType id, PSObjListD* &outList)
{
	return false;
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

DMReport*
DMBase::GetReport (void)
const
{
	const	DMBase	*parent;
	if (mObjectKind == eObject_Document)
		parent = this;
	else
	{
		for (parent = mParent; parent != NULL && parent->mObjectKind != eObject_Document; parent = parent->mParent)
			;
	}
	return static_cast <DMReport*> (const_cast <DMBase*> (parent));
}


// ---------------------------------------------------------------------------
// AdjustDrawingPosition											  [public]
// ---------------------------------------------------------------------------

void
DMBase::AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent)
{
	mDrawRect = mPosition + inParent;
}


// ---------------------------------------------------------------------------
// AdjustOrder														  [public]
// ---------------------------------------------------------------------------

void
DMBase::AdjustOrder (EOrder inOrder, long inSeq)
{
	// sections, groups; table needs to override to handle columns...
	PSObjListD			*list = NULL;
	PSObjList::iterator	iter;
	DMBase				*obj;
	long				seq;
	OSType				selector = PSObjPropObjects;

	switch (inOrder)
	{
		case eOrder_Reset:
			SetOrder (inSeq);
			if (GetObjects (selector, list))
			{
				for (seq = 1, iter = list->begin(); iter != list->end(); seq++, iter++)
				{
					obj = static_cast <DMBase*> (*iter);
					obj->AdjustOrder (eOrder_Reset, seq);
				}
			}
			break;
			
		case eOrder_Deleted:
			if (GetObjects (selector, list))
			{
				for (seq = 1, iter = list->begin(); iter != list->end(); seq++, iter++)
				{
					obj = static_cast <DMBase*> (*iter);
					obj->SetOrder (seq);
				}
			}
			break;

		case eOrder_Sequentially:
			if (mObjectKind == eObject_Section)
				selector = PSObjPropBodySection;
			else if (mObjectKind == eObject_Guide)
				selector = PSObjPropOGuides;
			else if (mObjectKind == eObject_Style)
				selector = PSObjPropStyleSet;
			if (mParent && mParent->GetObjects (selector, list))
			{
				for (seq = 1, iter = list->begin(); iter != list->end(); seq++, iter++)
				{
					obj = static_cast <DMBase*> (*iter);
					obj->SetOrder (seq);
				}
			}
			break;

		case eOrder_Set:
			seq = inSeq;
			if (mObjectKind == eObject_Section)
				selector = PSObjPropBodySection;
			break;

		case eOrder_Up:
			seq = GetOrder() + 1;
			break;
			
		case eOrder_Down:
			seq = GetOrder() - 1;
			break;
	}

	if (inOrder == eOrder_Set || inOrder == eOrder_Up || inOrder == eOrder_Down)
	{
		if (seq > 0 && seq != GetOrder() && mParent && mParent->GetObjects (selector, list))
		{
			if (seq > (long) list->size())
				seq = (long) list->size();
			if (seq != GetOrder())
			{
				long		newSeq = seq;
				bool		proceed = true;
				DMSection *	section;
				if (mObjectKind == eObject_Section) // pB 2011 added test for reordering sections
				{
					for (seq = 1, iter = list->begin(); iter != list->end(); iter++)
					{
						if (newSeq == seq) // new position of object
						{
							section = static_cast <DMSection*> (*iter);
							proceed = section->GetSectionKind() == (static_cast <DMSection*> (this))->GetSectionKind();
							if (proceed && (section->GetSectionKind() == DMSection::eSectionKind_Footer))
							{
								proceed = (static_cast <DMHeaderFooterSection*> (section))->GetFill () == (static_cast <DMHeaderFooterSection*> (this))->GetFill();
							}
						}
					}
				}
				
				if (proceed)
				{
					for (seq = 1, iter = list->begin(); iter != list->end(); iter++)
					{
						obj = static_cast <DMBase*> (*iter);
						if (newSeq == seq) // 
							seq++;
						if (obj == this)
							obj->SetOrder (newSeq);
						else
							obj->SetOrder (seq++);
					}
					std::sort<PSObjListD::iterator, DMObjectCompareOrder> (list->begin(), list->end(), DMObjectCompareOrder());
			
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// GetProxySize			return size in window coordinated for proxy click
// ---------------------------------------------------------------------------
double
DMBase::GetProxySize(EOutsetSize flag)
{
    double scale = GetReport()->GetScale();
    if (!scale) scale = 1;
    double aHandleSize = 3.;
    double aProxiSize = 2 / scale;
    
    if (flag == eOut_Proximity) {
        return aHandleSize;
    } else {
        return (aHandleSize > aProxiSize) ? aHandleSize : aProxiSize;
    }
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// DMObject									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMObject::DMObject (DMBase *inParent, EObject_Kind inKind)
	:	DMBase (inParent, inKind)
{
	Init();
}


// ---------------------------------------------------------------------------
// ~DMObject								Destructor			   [protected]
// ---------------------------------------------------------------------------

DMObject::~DMObject (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMObject::Init (void)
{
//	mFixedH = false;
	mFixedV = false;
//	mBindH = false;
	mBindV = false;
	mAlignment = eAlign_None;
	mDraw = eDraw_Yes;
//	mExpandH = false;
	mExpandV = false;
	mName.Free();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMObject::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMBase::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMObject::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropName:			outValue.SetText (mName); break;
//		case PSObjPropFixH:			outValue.SetBoolean (mFixedH); break;
		case PSObjPropFixV:			outValue.SetBoolean (mFixedV); break;
//		case PSObjPropBindH:		outValue.SetBoolean (mBindH); break;
		case PSObjPropBindV:		outValue.SetBoolean (mBindV); break;
		case PSObjPropAlign:		
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (mAlignment); 
			else
				outValue.SetText (RWStr::FromASCII (sAlignment [mAlignment]));
			break;						
		case PSObjPropDraw:			
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (mDraw); 
			else
				outValue.SetText (RWStr::FromASCII (sDraw [mDraw]));
			break;						
//		case PSObjPropExpandH:
//			if (not PSObject::FindPropertyByID (id, GetProperties()))
//				return false;
//			outValue.SetBoolean (mExpandH);
//			break;
		case PSObjPropExpandV:
			if (not PSObject::FindPropertyByID (id, GetProperties()))
				return false;
			outValue.SetBoolean (mExpandV);
			break;

		default:					return DMBase::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMObject::SetProperty (OSType id, RWValue &inValue)
{
	long	lVal;
	switch (id)
	{
		case PSObjPropName:		return SetStringProperty (inValue, mName);
//		case PSObjPropFixH:		return SetBooleanProperty (inValue, mFixedH);
		case PSObjPropFixV:		return SetBooleanProperty (inValue, mFixedV);
//		case PSObjPropBindH:	return SetBooleanProperty (inValue, mBindH);
		case PSObjPropBindV:	return SetBooleanProperty (inValue, mBindV);
		case PSObjPropAlign:
			if ((lVal = SetListProperty (inValue, sAlignment)) >= 0 || (lVal = SetListProperty (inValue, sAlignment2)) >= 0)
			{
				mAlignment = EAlignment (lVal);
				return true;
			}
			break;
			//return SetListProperty (inValue, sAlignment, mAlignmentI)? true: SetListProperty (inValue, sAlignment2, mAlignmentI);
		case PSObjPropDraw:	//	return SetListProperty (inValue, sDraw, mDraw);
			if ((lVal = SetListProperty (inValue, sDraw)) >= 0 )
			{
				mDraw = EDraw (lVal);
				return true;
			}
			break;
		case PSObjPropExpandV:
			if (not PSObject::FindPropertyByID (id, GetProperties()))
				break;
			return SetBooleanProperty (inValue, mExpandV);

		default:				return DMBase::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMGroup*
DMGroup::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMGroup	*group = new DMGroup (inParent);
	if (inNode)
		group->LoadXML (inNode);

	return group;
}


// ---------------------------------------------------------------------------
// DMGroup									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMGroup::DMGroup (DMBase *inParent)
	:	DMObject (inParent, eObject_Group)
{
	Init();
}


// ---------------------------------------------------------------------------
// ~DMGroup									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMGroup::~DMGroup (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMGroup::Init (void)
{
	mObjects.clear();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMGroup::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMObject::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
DMGroup::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
	//	DMObject::LoadXMLObjects (pes, inNode);
	
	GetReport()->ParseObjects (pes, inNode, this, mObjects);
	
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMGroup::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropObjects:		outValue.SetInteger (mObjects.size()); break;

		default:					return DMObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMGroup::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		default:					return DMObject::SetProperty (id, inValue);
	}
	
	return false;
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
DMGroup::GetObjects (OSType id, PSObjListD* &outList)
{
	if (id == PSObjPropObjects)
	{
		outList = &mObjects;
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// AdjustDrawingPosition											  [public]
// ---------------------------------------------------------------------------

void
DMGroup::AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent)
{
	DMObject::AdjustDrawingPosition (inComposer, inParent);
//	std::sort<PSObjListD::iterator, DMObjectCompareOrder> (mObjects.begin(), mObjects.end(), DMObjectCompareOrder());

	PSObjList::iterator	iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
	{
		DMBase	*obj = static_cast <DMBase*> (*iter);
		if (obj != NULL)
			obj->AdjustDrawingPosition (inComposer, mDrawRect.TopLeft());
	}
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMLine*
DMLine::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMLine	*line = new DMLine (inParent);
	if (inNode)
		line->LoadXML (inNode);

	return line;
}


// ---------------------------------------------------------------------------
// DMLine									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMLine::DMLine (DMBase *inParent)
	:	DMObject (inParent, eObject_Line)
{
	Init();
}

// ---------------------------------------------------------------------------
// ~DMLine									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMLine::~DMLine (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMLine::Init (void)
{
	mThickness = 1;
	mLineColor = cBlackColor;
	if (mObjectKind != eObject_Rect)	//mbs 18062010
		mFlags = RWLine_Horizontal;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMLine::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMObject::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMLine::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropThickness:	outValue.SetReal (mThickness); break;
		case PSObjPropLineColor:	outValue.SetText (mLineColor.ToString()); break;
		case PSObjPropFlags:		outValue.SetInteger (mFlags); break;

		default:					return DMObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMLine::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropThickness:		return SetRealProperty (inValue, mThickness, 0, 256);
		case PSObjPropLineColor:		return SetColorProperty (inValue, mLineColor);
		case PSObjPropFlags:
		{
			long	lVal;
			if (SetIntegerProperty (inValue, lVal, 0, RWLine_Full))
			{
				mFlags = UInt8 (lVal);
				return true;
			}
			break;
		}
			
		default:						return DMObject::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMOval*
DMOval::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMOval	*rect = new DMOval (inParent);
	if (inNode)
		rect->LoadXML (inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// DMOval									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMOval::DMOval (DMBase *inParent)
	:	DMLine (inParent)
{
	mObjectKind = eObject_Oval;
	Init();
}

// ---------------------------------------------------------------------------
// ~DMOval									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMOval::~DMOval (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMOval::Init (void)
{
	mFill = false;
	mFillColor = cBlackColor;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMOval::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMLine::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMOval::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFlags:		return false;

		case PSObjPropFill:			outValue.SetInteger (mFill); break;
		case PSObjPropFillColor:	outValue.SetText (mFillColor.ToString()); break;

		default:					return DMLine::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMOval::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFlags:		break;

		case PSObjPropFill:			return SetBooleanProperty (inValue, mFill);
		case PSObjPropFillColor:	return SetColorProperty (inValue, mFillColor);

		default:					return DMLine::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMRect*
DMRect::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMRect	*rect = new DMRect (inParent);
	if (inNode)
		rect->LoadXML (inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// DMRect									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMRect::DMRect (DMBase *inParent)
	:	DMOval (inParent)
{
	mObjectKind = eObject_Rect;
	Init();
}

// ---------------------------------------------------------------------------
// ~DMRect									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMRect::~DMRect (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMRect::Init (void)
{
	mRows = 1;
	mCols = 1;
	mFlags = RWRect_Full;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMRect::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMOval::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMRect::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropRows:			outValue.SetInteger (mRows); break;
		case PSObjPropCols:			outValue.SetInteger (mCols); break;
		case PSObjPropFlags:		outValue.SetInteger (mFlags); break;

		default:					return DMOval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMRect::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropRows:			return SetIntegerProperty (inValue, mRows, 1, 512); break;
		case PSObjPropCols:			return SetIntegerProperty (inValue, mCols, 1, 512); break;
		case PSObjPropFlags:
		{
			long	lVal;
			if (SetIntegerProperty (inValue, lVal, 0, RWRect_Full))
			{
				mFlags = UInt8 (lVal);
				return true;
			}
			break;
		}

		default:					return DMOval::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMPict*
DMPict::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMPict	*rect = new DMPict (inParent);
	if (inNode)
		rect->LoadXML (inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// DMPict									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMPict::DMPict (DMBase *inParent)
	:	DMOval (inParent),
		mComposerPictureData (0)
{
	mObjectKind = eObject_Pict;
	Init();
}

// ---------------------------------------------------------------------------
// ~DMPict									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMPict::~DMPict (void)
{
	if (mComposerPictureData)
		delete mComposerPictureData;
//	mPicture.Free();
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMPict::Init (void)
{
	mFormatI = ePictFormat_Normal;
	mFrame = false;
	mFrameOffset = 2;
	mPicture.Free();
	if (mComposerPictureData)
	{
		delete mComposerPictureData;
		mComposerPictureData = 0;
	}
	mPicture4D.Free();
	mObjectRotation = 0;
    mFillColor = cEmptyColor;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMPict::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMOval::LoadXML (inNode, pes);
	return;
}



// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
DMPict::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
//	DMOval::LoadXMLObjects (pes, inNode);

assert (pes->id == PSObjPropData);

	if (inNode)
	{
		int				kind = RWValue::eValue_BLOB;
		const RWString	fmt = inNode.Attr (RWStr::FromASCII (sPictDataProperties [1]));
		if (!fmt.empty())
		{
			long	lVal = RWTools::FindInList (fmt, RWValue::GetPictFormats());
			if (lVal >= 0)
				kind = RWValue::EValue_Kind (RWValue::eValue_BLOB + lVal);
		}
		SBlob	pictData;
		pictData.Init();
		RWTools::ReadData (inNode, pictData);
		mPicture.SetPicture (RWValue::EValue_Kind (kind), pictData, true);
	}

	return;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// WriteXMLObjects													  [public]
// ---------------------------------------------------------------------------

bool
DMPict::WriteXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
#if	0
	if (mPicture.GetKind() == RWValue::eValue_PictRefScreen || mPicture.GetKind() == RWValue::eValue_PictRefPrint)
	{
		// ••• TODO •••	format/conversion of the picture
	}
#endif

	if (mPicture.GetKind() >= RWValue::eValue_BLOB && mPicture.GetBlobSize() > 0)
	{
		// ••• TODO •••	format/conversion of the picture
		inNode.SetAttr (RWStr::FromASCII (sPictDataProperties [1]), RWStr::FromASCII (RWValue::GetPictFormats() [mPicture.GetKind() - RWValue::eValue_BLOB]));
		inNode.SetAttr (RWStr::FromASCII (sPictDataProperties [2]), RWStr::FromASCII (sPictDataProperties [3]));
		RWTools::WriteData (inNode, mPicture.GetBlob());
		return true;
	}

	return false;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMPict::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	outValue.SetReal (mThickness); break;	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		outValue.SetText (mLineColor.ToString()); break;	// same as PSObjPropLineColor in DMOval

//		case PSObjPropExpandH:			outValue.SetBoolean (mExpandH); break;
//		case PSObjPropExpandV:			outValue.SetBoolean (mExpandV); break;
		case PSObjPropFormat:			outValue.SetInteger (mFormatI); break;
		case PSObjPropFrame:			outValue.SetBoolean (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropData:				outValue.Attach (mPicture); break;

		//mbs 25052010
		case PSObjPropWidth:			outValue.SetReal (mComposerPictureData? mComposerPictureData->GetWidth(): 0); break;
		case PSObjPropHeight:			outValue.SetReal (mComposerPictureData? mComposerPictureData->GetHeight(): 0); break;
		case 'imgs':					outValue.SetInteger (mPicture.GetBlobSize()); break;

		// pB 2011-12
        case PSObjPropObjectRotation:       outValue.SetReal (mObjectRotation); break; // ??
        case PSObjPropBackColor:        outValue.SetText (mFillColor.ToString()); break;

		default:						return DMOval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMPict::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	return DMOval::SetProperty (PSObjPropThickness, inValue);	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		return DMOval::SetProperty (PSObjPropLineColor, inValue);	// same as PSObjPropLineColor in DMOval
//		case PSObjPropExpandH:			return SetBooleanProperty (inValue, mExpandH);
//		case PSObjPropExpandV:			return SetBooleanProperty (inValue, mExpandV);
		case PSObjPropFormat:			return SetListProperty (inValue, sPictFormat, mFormatI);
		case PSObjPropFrame:			return SetBooleanProperty (inValue, mFrame);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 1024);
		case PSObjPropObjectRotation:   return SetRealProperty (inValue, mObjectRotation, -360, 360);
        case PSObjPropBackColor:        return SetColorProperty (inValue, mFillColor);

		case PSObjPropData:
			if (inValue.GetKind() >= RWValue::eValue_PictRefScreen)
			{
//				if (mComposerPictureData && GetReport()->GetPageComposer())
//					GetReport()->GetPageComposer()->FreePict (&mComposerPictureData);
				if (mComposerPictureData)
				{
					delete mComposerPictureData;
					mComposerPictureData = 0;
				}
				mPicture.Clone (inValue);
				mPicture4D.Free();
				return true;
			}
			break;

		default:						return DMOval::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-


// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMText*
DMText::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMText	*text = new DMText (inParent);
	if (inNode)
		text->LoadXML (inNode);

	return text;
}


// ---------------------------------------------------------------------------
// DMText									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMText::DMText (DMBase *inParent)
	:	DMOval (inParent), mStyle (inParent->GetReport()->GetStyleContainer(), RWXmlNode())
{
	mObjectKind = eObject_Text;
	mStyle.Clear (0);
	Init();
}

// ---------------------------------------------------------------------------
// ~DMText									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMText::~DMText (void)
{
	return;
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMText::Init (void)
{
	mStyleID = 0;
	mIsDynamic = false;
	mIsAttributed = false;
	mKeepTogether = false;
	mDrawIfEmptyI = eEmpty_Draw;
	mFrame = false;
	mFrameOffset = 2;
	mText.Free();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMText::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMOval::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMText::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	outValue.SetReal (mThickness); break;	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		outValue.SetText (mLineColor.ToString()); break;	// same as PSObjPropLineDM in DMOval

		case PSObjPropStyle:			outValue.SetInteger (mStyleID); break;
//		case PSObjPropExpandH:			outValue.SetBoolean (mExpandH); break;
//		case PSObjPropExpandV:			outValue.SetBoolean (mExpandV); break;
		case PSObjPropDynamic:			outValue.SetBoolean (mIsDynamic); break;
		case PSObjPropAttributed:		outValue.SetBoolean (mIsAttributed); break;
		case PSObjPropKeepTogether:		outValue.SetBoolean (mKeepTogether); break;
		case PSObjPropDrawEmpty:		
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (mDrawIfEmptyI); 
			else
				outValue.SetText (RWStr::FromASCII (sEmpty [mDrawIfEmptyI]));
			break;			
		case PSObjPropFrame:			outValue.SetBoolean (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropData:				outValue.SetText (mText); break;
		case PSObjPropText:				outValue.SetText (mParsedText); break;

		//case PSObjPropBaseID;	
		//case PSObjPropFlags;
		case PSObjPropHorAlign:
			id = PSObjPropAlign;
		case PSObjPropFontName:
		case PSObjPropSize:
		case PSObjPropStyleF:
		case PSObjPropStyleB:
		case PSObjPropStyleI:
		case PSObjPropStyleU:
		case PSObjPropStyleS:
		case PSObjPropWrap:
		case PSObjPropVertAlign:
		case PSObjPropTextColor:
		case PSObjPropBackColor:
		case PSObjPropRotation:
		case PSObjPropBaseLineShift:
		case PSObjPropHorizontalScale:
		case PSObjPropLineSpacing:
		{
			return mStyle.GetProperty (id, outValue);
		}

		default:						return DMOval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMText::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	return DMOval::SetProperty (PSObjPropThickness, inValue);	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		return DMOval::SetProperty (PSObjPropLineColor, inValue);	// same as PSObjPropLineColor in DMOval
//		case PSObjPropStyle:			return SetIntegerProperty (inValue, mStyleID, 0);
//		case PSObjPropExpandH:			return SetBooleanProperty (inValue, mExpandH);
//		case PSObjPropExpandV:			return SetBooleanProperty (inValue, mExpandV);
		case PSObjPropDynamic:			return SetBooleanProperty (inValue, mIsDynamic);
		case PSObjPropAttributed:		
		{
			bool oldAttributed = mIsAttributed;
			if (SetBooleanProperty (inValue, mIsAttributed))
			{
				if (oldAttributed != mIsAttributed)
				{
					if (mIsAttributed)  // attributed was canceled pB 2010-12
					{
						RWTextValue inText;
						inText.Attach (RWTools::EscapeAttributedString (mText));
						mText = inText;
					}
					else 
					{
						RWTextValue inText;
						inText.Attach (RWTools::SplitAttributedString (mText, NULL));
						mText = inText;
					}
				}
				return true;
			}
			else 
				return false;
		}
		case PSObjPropKeepTogether:		return SetBooleanProperty (inValue, mKeepTogether);
		case PSObjPropDrawEmpty:		return SetListProperty (inValue, sEmpty, mDrawIfEmptyI);
		case PSObjPropFrame:			return SetBooleanProperty (inValue, mFrame);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 256);
		case PSObjPropData:				mParsedText.Free(); return SetStringProperty (inValue, mText);

		case PSObjPropID:				return SetStringProperty (inValue, mID);
	
		case PSObjPropStyle:
		{
				if( SetIntegerProperty (inValue, mStyleID, 0))
				{
					mStyle.Clear (mStyleID);
					return true;
				};
				return false;
		}
		case PSObjPropHorAlign:
			id = PSObjPropAlign;
		case PSObjPropFontName:
		case PSObjPropSize:
		case PSObjPropStyleF:
		case PSObjPropStyleB:
		case PSObjPropStyleI:
		case PSObjPropStyleU:
		case PSObjPropStyleS:
		case PSObjPropWrap:
		case PSObjPropVertAlign:
		case PSObjPropTextColor:
		case PSObjPropBackColor:
		case PSObjPropRotation:
		case PSObjPropBaseLineShift:
		case PSObjPropHorizontalScale:
		case PSObjPropLineSpacing:
		{
			DMReport	*rep = GetReport();
			DMArea		*area = NULL;
			if (rep->GetPageComposer() != NULL)
				area = static_cast <DMArea*> (rep);
			
			if (mStyle.SetProperty (id, inValue))
			{
				if (area)
					rep->GetPageComposer()->StyleChanged (&mStyle);
				return true;
			}
			return false;
		}

		default:						return DMOval::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMVariable*
DMVariable::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMVariable	*var = new DMVariable (inParent);
	if (inNode)
		var->LoadXML (inNode);

	return var;
}


// ---------------------------------------------------------------------------
// DMVariable								Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMVariable::DMVariable (DMBase *inParent)
	:	DMText (inParent),
		mRepeatI (eRepeat_None),
		mRepeatOffset (0),
		mComposerPictureData (0)	//mbs 29072011	pict support
{
	mObjectKind = eObject_Var;
	Init();
}

// ---------------------------------------------------------------------------
// ~DMVariable								Destructor			   [protected]
// ---------------------------------------------------------------------------

DMVariable::~DMVariable (void)
{
	if (mComposerPictureData)	//mbs 29072011	pict support
		delete mComposerPictureData;
	return;
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMVariable::Init (void)
{
	mIndex = SR4DVariable_Variable;
	mCalcTypeI = ECalcType_None;
	mIsDynamic = false;
	mKeepTogether = false;
	mAlias.Free();
	mFormat.Free();
	mRepeatI = eRepeat_None;
	mRepeatOffset = 0;
	mScript.Free();
	mObjectRotation = 0;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMVariable::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMText::LoadXML (inNode, pes);
	mIsDynamic = false;
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMVariable::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropDynamic:			return false;
		case PSObjPropData:				return false;

		case PSObjPropSource:			outValue.SetText (mText); break;
		case PSObjPropAlias:			outValue.SetText (mAlias); break;
		case PSObjPropFormat:			outValue.SetText (mFormat); break;
		case PSObjPropElement:			outValue.SetInteger (mIndex); break;
		case PSObjPropCalcType:			outValue.SetInteger (mCalcTypeI); break;
		case PSObjPropRepeat:			
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (mRepeatI); 
			else
				outValue.SetText (RWStr::FromASCII (sRepeat [mRepeatI]));
			break;
		case PSObjPropRepeatOffset:		outValue.SetReal (mRepeatOffset); break;
		case PSObjPropScript:			outValue.SetText (mScript); break;

		default:						return DMText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMVariable::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropDynamic:		break;
		case PSObjPropData:			break;

		case PSObjPropSource:
			mParsedText.Free();
			if (mComposerPictureData)	//mbs 29072011	pict support
			{
				delete mComposerPictureData;
				mComposerPictureData = NULL;
			}
			mPicture4D.Free();	//mbs 29072011	pict support
			return SetStringProperty (inValue, mText);
		case PSObjPropAlias:		return SetStringProperty (inValue, mAlias);
		case PSObjPropFormat:		return SetStringProperty (inValue, mFormat);
		case PSObjPropElement:		return SetIntegerProperty (inValue, mIndex, SR4DVariable_Variable);
		case PSObjPropCalcType:		return SetListProperty (inValue, sCalcType, mCalcTypeI);
		case PSObjPropRepeat:		return SetListProperty (inValue, sRepeat, mRepeatI)? true: SetListProperty (inValue, sRepeat2, mRepeatI);
		case PSObjPropRepeatOffset:	return SetRealProperty (inValue, mRepeatOffset, 0, 256);
		case PSObjPropScript:
			mParsedText.Free();
			if (mComposerPictureData)	//mbs 29072011	pict support
			{
				delete mComposerPictureData;
				mComposerPictureData = NULL;
			}
			mPicture4D.Free();	//mbs 29072011	pict support
			return SetStringProperty (inValue, mScript);

		default:					return DMText::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMField*
DMField::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMField	*fld = new DMField (inParent);
	if (inNode)
		fld->LoadXML (inNode);

	return fld;
}


// ---------------------------------------------------------------------------
// DMField									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMField::DMField (DMBase *inParent)
	:	DMVariable (inParent)
{
	mObjectKind = eObject_Fld;
}

// ---------------------------------------------------------------------------
// ~DMField									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMField::~DMField (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMField::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropElement:			return false;

		default:						return DMVariable::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMField::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropElement:			break;

		default:						return DMVariable::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-


// ---------------------------------------------------------------------------
// DMHeader									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMHeader::DMHeader (DMBase *inParent)
	:	DMText (inParent),
		mWidth (0),
		mHeight (0),
		mColSpan (1),
		mRowSpan (1)
{
	mObjectKind = eObject_TblHdr;
	mStyleID = static_cast <DMTable*> (mParent)->GetStyleID();
}


// ---------------------------------------------------------------------------
// ~DMHeader									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMHeader::~DMHeader (void)
{
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMHeader::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	DMBase::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMHeader::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
    RWXmlNode	me;
	if (mVisible)
	{
		me = DMBase::WriteXML (inParent, pes);
		me.SetName (u"td");
	}
	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMHeader::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropWidth:		outValue.SetReal (mWidth); break;
		case PSObjPropHeight:		outValue.SetReal (mHeight); break;
		case PSObjPropColSpan:		outValue.SetInteger (mColSpan); break;
		case PSObjPropRowSpan:		outValue.SetInteger (mRowSpan); break;

		// ignored properties of Text
		case PSObjPropFrameThickness:
		case PSObjPropFrameColor:
		case PSObjPropExpandV:
		case PSObjPropKeepTogether:
		case PSObjPropDrawEmpty:
		case PSObjPropFrame:
		case PSObjPropFrameOffset:
		case PSObjPropText:			return false;


		default:					return DMText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMHeader::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropStyle:
		case PSObjPropFontName:
		case PSObjPropSize:
		case PSObjPropStyleF:
		case PSObjPropStyleB:
		case PSObjPropStyleI:
		case PSObjPropStyleU:
		case PSObjPropStyleS:
		case PSObjPropWrap:
		case PSObjPropHorAlign:
		case PSObjPropVertAlign:
		case PSObjPropTextColor:
		case PSObjPropBackColor:
		case PSObjPropRotation:
		case PSObjPropBaseLineShift:
		case PSObjPropHorizontalScale:
		case PSObjPropLineSpacing:

		case PSObjPropWidth:
		case PSObjPropHeight:
		case PSObjPropColSpan:
		case PSObjPropRowSpan:
		case PSObjPropData:
			static_cast <DMTable*> (mParent)->Recalculate();
			break;
	}

	switch (id)
	{
		case PSObjPropXML:
		case PSObjPropVisible:	break;

		// ignored properties of Text
		case PSObjPropFrameThickness:
		case PSObjPropFrameColor:
		case PSObjPropExpandV:
		case PSObjPropKeepTogether:
		case PSObjPropDrawEmpty:
		case PSObjPropFrame:
		case PSObjPropFrameOffset:
		case PSObjPropText:		break;

		case PSObjPropWidth:		return SetRealProperty (inValue, mWidth, 0, 4096);
		case PSObjPropHeight:		return SetRealProperty (inValue, mHeight, 0, 1024);
		case PSObjPropColSpan:		return SetIntegerProperty (inValue, mColSpan, 1, 100);
		case PSObjPropRowSpan:		return SetIntegerProperty (inValue, mRowSpan, 1, 100);

		default:
		{
			bool	ok = DMText::SetProperty (id, inValue);
			if (ok)
			{
				switch (id)
				{
					case PSObjPropRect:
						mHeight = mPosition.Height();
						mWidth = mPosition.Width();
						mHeight += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						mWidth += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
					case PSObjPropPosBottom:
					case PSObjPropPosHeight:
					case PSObjPropRelPosBottom:
						mHeight = mPosition.Height();
						mHeight += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
					case PSObjPropPosRight:
					case PSObjPropPosWidth:
					case PSObjPropRelPosRight:
						mWidth = mPosition.Width();
						mWidth += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
				}
			}
			return ok;
			break;
		}
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// DMColumn									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMColumn::DMColumn (DMBase *inParent)
	:	DMText (inParent),
		mWidth (0),
		mGrid (true),
		mPrintRowNum (false),
		mPrintRepeatingValues (true),
		mLevel (0)
{
	mObjectKind = eObject_TblCol;
	mStyleID = static_cast <DMTable*> (mParent)->GetStyleID();
	mScript.Free();
}


// ---------------------------------------------------------------------------
// ~DMColumn									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMColumn::~DMColumn (void)
{
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMColumn::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	DMBase::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMColumn::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
    RWXmlNode	me = DMBase::WriteXML (inParent, pes);
	me.SetName (u"Col");
	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMColumn::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		// ignored properties of Text
		case PSObjPropID:
		case PSObjPropVisible:
		case PSObjPropFrameThickness:
		case PSObjPropFrameColor:
		case PSObjPropExpandV:
		case PSObjPropDynamic:
		case PSObjPropKeepTogether:
		case PSObjPropDrawEmpty:
		case PSObjPropFrame:
		case PSObjPropFrameOffset:
		case PSObjPropData:
		case PSObjPropText:			return false;


		case PSObjPropWidth:		outValue.SetReal (mWidth); break;
		case PSObjPropGrid:			outValue.SetBoolean (mGrid); break;
		case PSObjPropSource:		outValue.SetText (mSource); break;
		case PSObjPropAlias:		outValue.SetText (mAlias); break;
		case PSObjPropFormat:		outValue.SetText (mFormat); break;
//		case PSObjPropData:			outValue.SetText (mText); break;
		case PSObjPropRowNum:		outValue.SetBoolean (mPrintRowNum); break;
		case PSObjPropDuplicates:	outValue.SetBoolean (mPrintRepeatingValues); break;
		case PSObjPropScript:		outValue.SetText (mScript); break;
		case PSObjPropLevel:		outValue.SetInteger (mLevel); break;

		default:					return DMText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMColumn::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropStyle:
		case PSObjPropFontName:
		case PSObjPropSize:
		case PSObjPropStyleF:
		case PSObjPropStyleB:
		case PSObjPropStyleI:
		case PSObjPropStyleU:
		case PSObjPropStyleS:
		case PSObjPropWrap:
		case PSObjPropHorAlign:
		case PSObjPropVertAlign:
		case PSObjPropTextColor:
		case PSObjPropBackColor:
		case PSObjPropRotation:
		case PSObjPropBaseLineShift:
		case PSObjPropHorizontalScale:
		case PSObjPropLineSpacing:

		case PSObjPropWidth:
		case PSObjPropGrid:
		case PSObjPropSource:
		case PSObjPropFormat:
		case PSObjPropOrder:
			static_cast <DMTable*> (mParent)->Recalculate();
			break;
	}

	switch (id)
	{
		case PSObjPropXML:
		case PSObjPropID:
		case PSObjPropVisible:		break;

		// ignored properties of Text
		case PSObjPropFrameThickness:
		case PSObjPropFrameColor:
		case PSObjPropExpandV:
		case PSObjPropDynamic:
		case PSObjPropKeepTogether:
		case PSObjPropDrawEmpty:
		case PSObjPropFrame:
		case PSObjPropFrameOffset:
		case PSObjPropData:
		case PSObjPropText:		break;

		case PSObjPropWidth:		return SetRealProperty (inValue, mWidth, 0, 4096);
		case PSObjPropGrid:			return SetBooleanProperty (inValue, mGrid);
		case PSObjPropSource:
			if (SetStringProperty (inValue, mSource))
			{
				mPrintRowNum = TEXT_EQUALS (mSource, "%ROWNUM%");
				return true;
			}
			break;
		case PSObjPropAlias:		return SetStringProperty (inValue, mAlias);
		case PSObjPropFormat:		return SetStringProperty (inValue, mFormat);
//		case PSObjPropData:			return SetStringProperty (inValue, mText);
		case PSObjPropRowNum:		return SetBooleanProperty (inValue, mPrintRowNum);
		case PSObjPropDuplicates:	return SetBooleanProperty (inValue, mPrintRepeatingValues);
		case PSObjPropScript:		return SetStringProperty (inValue, mScript);
		case PSObjPropLevel:		return SetIntegerProperty (inValue, mLevel, 0, 10);

		default:
		{
			bool	ok = DMText::SetProperty (id, inValue);
			if (ok)
			{
				float	height = -1;
				switch (id)
				{
					case PSObjPropRect:
						height = mPosition.Height();
						mWidth = mPosition.Width();
						height += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						mWidth += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
					case PSObjPropPosBottom:
					case PSObjPropPosHeight:
					case PSObjPropRelPosBottom:
						height = mPosition.Height();
						height += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
					case PSObjPropPosRight:
					case PSObjPropPosWidth:
					case PSObjPropRelPosRight:
						mWidth = mPosition.Width();
						mWidth += static_cast <DMTable*> (mParent)->GetDMFrameAdjustment();	//mbs 02082010	space for frame
						static_cast <DMTable*> (mParent)->Recalculate();
						break;
				}
				if (height != -1)
				{
					RWValue	rvHeight (height);
					static_cast <DMTable*> (mParent)->SetProperty (PSObjPropHeight, rvHeight);	// set mRowHeight for table data
				}
			}
			return ok;
		}
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMTable*
DMTable::Create (DMBase *inParent, RWXmlNode inNode)
{
	DMTable	*group = new DMTable (inParent);
	if (inNode)
		group->LoadXML (inNode);
	else
		group->ResizeGrid (1, 2);

	return group;
}


// ---------------------------------------------------------------------------
// DMTable									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMTable::DMTable (DMBase *inParent)
	:	DMOval (inParent)
{
	mObjectKind = eObject_Table;
	Init();
}


// ---------------------------------------------------------------------------
// ~DMTable									Destructor			   [protected]
// ---------------------------------------------------------------------------

DMTable::~DMTable (void)
{
}


// ---------------------------------------------------------------------------
// Init															   [protected]
// ---------------------------------------------------------------------------

void
DMTable::Init (void)
{
	mStyleID = 0;
	mFrame = 1;
	mFrameOffset = 2;
	mHGridThickness = 0.5;
	mRowHeight = 0;
	mDrawHeaders = true;
	mDrawColumns = true;
	mNumTopHeadings = 0;
	mNumColumns = 0;
	mCurHdrRow = 0;
	mTopHeadingsHeight = 0;
	mColWidthsCalculated = false;
	
	mHeaders.clear();
	mColumns.clear();
	mScript.Free();
	mTopRowHeights.clear();
	mColWidths.clear();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMTable::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	Init();
	DMOval::LoadXML (inNode, pes);
	
	mNumTopHeadings = mHeaders.size();
	mNumColumns = mColumns.size();
//	if (mNumTopHeadings == 0)
//		mDrawHeaders = false;
//	if (mNumColumns == 0)
//		mDrawColumns = false;

	FixUpGrid();
	return;
}


// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
DMTable::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
//	DMOval::LoadXMLObjects (pes, inNode);

	switch (pes->id)
	{
		case PSObjPropHeader:	// just a container
			mCurHdrRow = 0;
			PSObject::LoadXML (inNode, sPropertiesHead);
			break;

		case PSObjPropHeaderSection:	// next header row
			PSObject::LoadXML (inNode, sPropertiesHeader);
			mCurHdrRow++;
			break;

		case PSObjPropColumn:	// just a container
			PSObject::LoadXML (inNode, sPropertiesColumns);
			break;

		case PSObjPropOTblHdr:	// next header
		{
			PSObjListD	*hdrLine;
			while (mCurHdrRow >= (int) mHeaders.size())
			{
				hdrLine = new PSObjListD;
				mHeaders.push_back (hdrLine);
			}
			if (mCurHdrRow >= mNumTopHeadings)
				mNumTopHeadings = mCurHdrRow + 1;
			hdrLine = mHeaders [mCurHdrRow];
			PrepareForHeader();
			DMHeader	*hdr = new DMHeader (this);
			hdrLine->push_back (hdr);
			hdr->SetOrder (hdrLine->size());
			if (inNode)
				hdr->LoadXML (inNode);
			for (int i = hdr->GetColSpan(); i > 1; i--)
			{
				hdr = new DMHeader (this);
				hdrLine->push_back (hdr);
				hdr->SetOrder (hdrLine->size());
			}
			break;
		}

		case PSObjPropOTblCol:
		{
			DMColumn	*col = new DMColumn (this);
			mColumns.push_back (col);
			col->SetOrder (mColumns.size());
			if (inNode)
				col->LoadXML (inNode);
			break;
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// WriteXMLObjects												   [protected]
// ---------------------------------------------------------------------------

bool
DMTable::WriteXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
	switch (pes->id)
	{
		case PSObjPropHeader:	// just a container
			mCurHdrRow = 0;
			PSObject::WriteXML (inNode, sPropertiesHead);
			for (mCurHdrRow = 0; mCurHdrRow < (long) mHeaders.size(); mCurHdrRow++)
			{
                RWXmlNode	container = inNode.Append (u"tr");
				PSObject::WriteXML (container, sPropertiesHeader);
			}
			break;

		case PSObjPropHeaderSection:
			return false;

		case PSObjPropColumn:	// just a container
			PSObject::WriteXML (inNode, sPropertiesColumns);
			break;

		case PSObjPropOTblHdr:	// just a container
			break;

		case PSObjPropOTblCol:	// just a container
			break;
	}
	return true;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMTable::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
	return DMBase::WriteXML (inParent, pes);
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMTable::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropStyle:			outValue.SetInteger (mStyleID); break;
		case PSObjPropFrame:			outValue.SetInteger (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropFrameThickness:	outValue.SetReal (mThickness); break;	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		outValue.SetText (mLineColor.ToString()); break;	// same as PSObjPropLineColor in DMOval

		case PSObjPropHGridThickness:	outValue.SetReal (mHGridThickness); break;
		case PSObjPropHeight:			outValue.SetReal (mRowHeight); break;
		case PSObjPropDrawHeaders:		outValue.SetBoolean (mDrawHeaders); break;
		case PSObjPropDrawColumns:		outValue.SetBoolean (mDrawColumns); break;
		case PSObjPropScript:			outValue.SetText (mScript); break;
		case PSObjPropNumCols:			outValue.SetInteger (mNumColumns); break;
		case PSObjPropNumHeadings:		outValue.SetInteger (mNumTopHeadings); break;

		default:						return DMObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMTable::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropStyle:
		case PSObjPropFrame:
		case PSObjPropFrameOffset:
		case PSObjPropFrameThickness:
		case PSObjPropHeight:
		case PSObjPropDrawHeaders:
		case PSObjPropDrawColumns:
		case PSObjPropNumCols:
		case PSObjPropNumHeadings:
			Recalculate();
			break;
	}

	int	iVal;
	switch (id)
	{
		case PSObjPropStyle:			return SetIntegerProperty (inValue, mStyleID, 0);
		case PSObjPropFrame:			return SetIntegerProperty (inValue, mFrame, 0, 2);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 256);
		case PSObjPropFrameThickness:	return DMOval::SetProperty (PSObjPropThickness, inValue);	// same as PSObjPropThickness in DMOval
		case PSObjPropFrameColor:		return DMOval::SetProperty (PSObjPropLineColor, inValue);	// same as PSObjPropLineColor in DMOval

		case PSObjPropHGridThickness:	return SetRealProperty (inValue, mHGridThickness, 0, 64);
		case PSObjPropHeight:			return SetRealProperty (inValue, mRowHeight, 0, 1024);
		case PSObjPropDrawHeaders:		return SetBooleanProperty (inValue, mDrawHeaders);
		case PSObjPropDrawColumns:		return SetBooleanProperty (inValue, mDrawColumns);
		case PSObjPropScript:			return SetStringProperty (inValue, mScript);
		case PSObjPropNumCols:
			if (SetIntegerProperty (inValue, iVal, 1, 512))
				return ResizeGrid (mNumTopHeadings, iVal);
			break;
		case PSObjPropNumHeadings:
			if (SetIntegerProperty (inValue, iVal, 0, 64))
				return ResizeGrid (iVal, mNumColumns);
			break;

		default:						return DMObject::SetProperty (id, inValue);
	}
	
	return false;
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
DMTable::GetObjects (OSType id, PSObjListD* &outList)
{
	if ((id & PSObjPropOHdr) == PSObjPropOHdr)
	{
		int	line = id & ~PSObjPropOHdr;
		if (line >= 0 && line < mNumTopHeadings)
		{
			outList = mHeaders[line];
			return true;
		}
	}
	else if (id == PSObjPropColumn)
	{
		outList = &mColumns;
		return true;
	}
//	else if (id == PSObjPropOTblCol)
//	{
//		outList = &mColumns;
//		return true;
//	}
	else if (id == PSObjPropOTblHdr)
	{
		if (mCurHdrRow >= 0 && mCurHdrRow < (long) mHeaders.size())
		{
			outList = mHeaders [mCurHdrRow];
			return true;
		}
	}
	return false;
}


// ---------------------------------------------------------------------------
// AdjustDrawingPosition											  [public]
// ---------------------------------------------------------------------------
// all cells are offset/shrinked for mFrame/mThickness + mFrameOffset

void
DMTable::AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent)
{
	if (not mColWidthsCalculated)
		CalculateAll (inComposer);

	DMObject::AdjustDrawingPosition (inComposer, inParent);

	PSObjList::iterator	iter;
	int					col;

	if (mDrawHeaders)
	{
		for (mCurHdrRow = 0; mCurHdrRow < (long) mHeaders.size(); mCurHdrRow++)
		{
			for (col = 0, iter = mHeaders [mCurHdrRow]->begin(); iter != mHeaders [mCurHdrRow]->end(); col++, iter++)
			{
				DMHeader	*hdr = static_cast <DMHeader*> (*iter);
				if (hdr->IsVisible())
					hdr->AdjustDrawingPosition (inComposer, mDrawRect.TopLeft());
			}
		}
	}

	if (mDrawColumns)
	{
		for (col = 0, iter = mColumns.begin(); iter != mColumns.end(); col++, iter++)
		{
			DMColumn	*column = static_cast <DMColumn*> (*iter);
			column->AdjustDrawingPosition (inComposer, mDrawRect.TopLeft());
		}
	}
	return;
}


// ---------------------------------------------------------------------------
// AdjustHeaders												   [protected]
// ---------------------------------------------------------------------------
// set visibility of cells covered by colspan/rowspan to false

void
DMTable::AdjustHeaders (void)
{
	int			line, col, last;
	PSObjListD	*hdrLine;
	DMHeader	*header;

	for (line = 0; line < mNumTopHeadings; line++)
	{
		hdrLine = mHeaders [line];
		for (col = 0; col < mNumColumns; col++)
		{
			header = static_cast <DMHeader*> ((*hdrLine)[col]);
			header->SetVisible (true);
		}
	}

	for (line = 0; line < mNumTopHeadings; line++)
	{
		hdrLine = mHeaders [line];
		last = hdrLine->size();
		for (col = 0; col < mNumColumns && col < last; col++)
		{
			header = static_cast <DMHeader*> ((*hdrLine)[col]);
			int	subline, colend;

			colend = col + header->GetColSpan();
			for (subline = line + header->GetRowSpan() - 1; subline >= line; subline--)
			{
				int	subcol, sublast = mHeaders [subline]->size();
				for (subcol = col + (subline == line); subcol < colend && subcol < sublast; subcol++)
				{
					header = static_cast <DMHeader*> ((*mHeaders [subline]) [subcol]);
					header->SetVisible (false);
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
// PrepareForHeader												   [protected]
// ---------------------------------------------------------------------------
// create "missing" headers (due to rowspan) for XML import

void
DMTable::PrepareForHeader (void)
{
	int			line, col, maxCol, last;
	PSObjListD	*hdrLine;
	DMHeader	*header;
	
	maxCol = 1 + mHeaders [mCurHdrRow]->size();
	for (line = 0; line < mCurHdrRow; line++)
	{
		hdrLine = mHeaders [line];
		last = hdrLine->size();
		if (last > maxCol)
			last = maxCol;
		for (col = 0; col < last; col++)
		{
			header = static_cast <DMHeader*> ((*hdrLine)[col]);
			int	colend = col + header->GetColSpan();
			int	rowend = line + header->GetRowSpan() - 1;
			if (rowend > mCurHdrRow)
				rowend = mCurHdrRow;
			for (int subline = line + 1; subline <= rowend; subline++)
			{
				int	sublast = (int) mHeaders [subline]->size();
				while (sublast++ < colend)
				{
					header = new DMHeader (this);
					(*mHeaders [subline]).push_back (header);
					header->SetOrder (sublast);
				}
			}
		}
	}
}


// ---------------------------------------------------------------------------
// FixUpGrid													   [protected]
// ---------------------------------------------------------------------------

void
DMTable::FixUpGrid (void)
{
	int			line, col;
	PSObjListD	*hdrLine;
	mNumTopHeadings = mHeaders.size();
	mNumColumns = mColumns.size();
	for (line = 0; line < mNumTopHeadings; line++)
	{
		hdrLine = mHeaders [line];
		col = hdrLine->size();
		if (col > mNumColumns)
			mNumColumns = col;
	}
	for (line = 0; line < mNumTopHeadings; line++)
	{
		hdrLine = mHeaders [line];
		while ((int) hdrLine->size() < mNumColumns)
		{
			DMHeader	*hdr = new DMHeader (this);
			hdrLine->push_back (hdr);
			hdr->SetOrder (hdrLine->size());

			char	buf [32];
			snprintf (buf, sizeof (buf), "%s_%d,%ld", sKind [hdr->GetKind()], line + 1, hdrLine->size());
			RWValue	v;
			v.SetText (RWStr::FromASCII (buf));
			hdr->SetProperty (PSObjPropData, v);
		}
	}
	while ((int) mColumns.size() < mNumColumns)
	{
		DMColumn	*col = new DMColumn (this);
		mColumns.push_back (col);
		col->SetOrder (mColumns.size());

		char	buf [32];
		snprintf (buf, sizeof (buf), "%s_%d", sKind [col->GetKind()], mNumColumns);
		RWValue	v;
		v.SetText (RWStr::FromASCII (buf));
		col->SetProperty (PSObjPropSource, v);
	}

	AdjustHeaders();
	Recalculate();
	return;
}


// ---------------------------------------------------------------------------
// ResizeGrid													   [protected]
// ---------------------------------------------------------------------------

bool
DMTable::ResizeGrid (int inNumHeaders, int inNumColumns)
{
	if (inNumHeaders == mNumTopHeadings && inNumColumns == mNumColumns)
		return false;
	if (inNumHeaders < 0 || inNumColumns < 1)
		return false;

	PSObjListD	*hdrLine;
	DMHeader	*header;
	DMColumn	*column;
	int			line, col;

	while (inNumHeaders > mNumTopHeadings)
	{
		hdrLine = new PSObjListD;
		mHeaders.push_back (hdrLine);
		mNumTopHeadings++;
	}

	if (inNumHeaders < mNumTopHeadings)
	{
		do
		{
			hdrLine = mHeaders.back();
			delete hdrLine;
			mHeaders.pop_back();
			mNumTopHeadings--;
		}
		while (inNumHeaders < mNumTopHeadings);

		for (line = 0; line < mNumTopHeadings; line++)
		{
			hdrLine = mHeaders [line];
			for (col = 0; col < inNumColumns; col++)
			{
				header = static_cast <DMHeader*> ((*hdrLine)[col]);
				if (header->GetRowSpan() + line > inNumHeaders)
					header->SetRowSpan (inNumHeaders - line);
			}
		}
	}

	if (inNumColumns > mNumColumns)
	{
/*
		for (line = 0; line < mNumTopHeadings; line++)
		{
			hdrLine = mHeaders [line];
			for (col = mNumColumns; col < inNumColumns; col++)
			{
				header = new DMHeader (this);
				hdrLine->push_back (header);
				header->SetOrder (hdrLine->size());
			}
		}
*/
		
		do
		{
			column = new DMColumn (this);
			mColumns.push_back (column);
			column->SetOrder (mColumns.size());
			mNumColumns++;

			char	buf [32];
			snprintf (buf, sizeof (buf), "%s_%d", sKind [column->GetKind()], mNumColumns);
			RWValue	v;
			v.SetText (RWStr::FromASCII (buf));
			column->SetProperty (PSObjPropSource, v);
		}
		while (inNumColumns > mNumColumns);
	}

	if (inNumColumns < mNumColumns)
	{
		for (line = 0; line < mNumTopHeadings; line++)
		{
			hdrLine = mHeaders [line];
			for (col = inNumColumns; col < mNumColumns; col++)
			{
				header = static_cast <DMHeader*> (hdrLine->back());
				delete header;
				hdrLine->pop_back();
			}

			for (col = 0; col < inNumColumns; col++)
			{
				header = static_cast <DMHeader*> ((*hdrLine)[col]);
				if (header->GetColSpan() + col > inNumColumns)
					header->SetColSpan (inNumColumns - col);
			}
		}

		do
		{
			column = static_cast <DMColumn*> (mColumns.back());
			delete column;
			mColumns.pop_back();
			mNumColumns--;
		}
		while (inNumColumns < mNumColumns);
	}

	FixUpGrid();
	return true;
}


// ---------------------------------------------------------------------------
// GetHeaderRow													   [protected]
// ---------------------------------------------------------------------------

PSObjListD*
DMTable::GetHeaderRow (int inRow)
const
{
	assert (inRow >= 0);
	assert (inRow < mNumTopHeadings);
	
	return static_cast <PSObjListD*> (mHeaders [inRow]);
}


// ---------------------------------------------------------------------------
// GetHeader													   [protected]
// ---------------------------------------------------------------------------

DMHeader *
DMTable::GetHeader (PSObjListD *inRow, int inCol)
const
{
	assert (inCol >= 0);
	assert (inCol < mNumColumns);

	return static_cast <DMHeader*> ((*inRow) [inCol]);
}


// ---------------------------------------------------------------------------
// GetHeader													   [protected]
// ---------------------------------------------------------------------------

DMHeader *
DMTable::GetHeader (int inRow, int inCol)
const
{
	assert (inRow >= 0);
	assert (inRow < mNumTopHeadings);
	assert (inCol >= 0);
	assert (inCol < mNumColumns);
	
	return static_cast <DMHeader*> ((*mHeaders [inRow]) [inCol]);
}


// ---------------------------------------------------------------------------
// GetColumn													   [protected]
// ---------------------------------------------------------------------------

DMColumn *
DMTable::GetColumn (int inCol)
const
{
	assert (inCol >= 0);
	assert (inCol < mNumColumns);
	
	return static_cast <DMColumn*> (mColumns [inCol]);
}


// ---------------------------------------------------------------------------
// GetColsWidth													   [protected]
// ---------------------------------------------------------------------------

float
DMTable::GetColsWidth (int inFrom, int inTo)
const
{
	assert (inFrom >= 0);
	assert (inTo >= inFrom);
	assert (inTo < mNumColumns);
	assert (mColWidthsCalculated);
	assert ((int) mColWidths.size() == mNumColumns);
	
	int		col;
	float	colsWidth = 0;
	for (col = inFrom; col <= inTo; col++)
	{
//		DMColumn	*column = GetColumn (col);
//		colsWidth += column->GetDMWidth();
		colsWidth += mColWidths [col];
	}
	
	return colsWidth;
}


// ---------------------------------------------------------------------------
// CalculateAll													   [protected]
// ---------------------------------------------------------------------------

void
DMTable::CalculateAll (RWPageComposer *inComposer)
{
	if (mDrawHeaders || mDrawColumns)
	{
		AdjustHeaders();

		DMReport	*report = GetReport();
		PSObjListD	*hdrLine;
		DMHeader	*header;
		DMColumn	*column;
		float		width;
		float		height;
		int			line, col, last, span;
		float		lineWidth, lineHeight;
//		bool		fixed;
		float		frame = 0, dframe = 0;
		
		if (mFrame)		// space for frame
		{
			frame = mThickness + mFrameOffset;
			dframe = 2 * frame;
		}
			
		mColWidths.assign (mNumColumns, 0.0f);
		SRect	r (0, 0, 0, 0);
		// reset top headers
		for (line = 0; line < mNumTopHeadings; line++)
		{
			hdrLine = GetHeaderRow (line);
			lineWidth = 0;
			last = hdrLine->size();
			for (col = 0; col < last; col++)
			{
				header = GetHeader (hdrLine, col);
				header->SetPosition (r);
				if (mDrawHeaders && header->IsVisible())
					header->SetDMWidth (header->GetWidth());
			}
		}
		// reset columns
		for (col = 0; col < mNumColumns; col++)
		{
			column = GetColumn (col);
			column->SetPosition (r);
//			if (mDrawColumns)
				column->SetDMWidth (column->GetWidth());
		}
	
		mTopHeadingsHeight = 0;
		mTopHeadingsWidth = 0;

		if (mDrawHeaders)
		{
			// calculate top headers
			for (line = 0; line < mNumTopHeadings; line++)
			{
				hdrLine = GetHeaderRow (line);
				lineWidth = 0;
				last = hdrLine->size();
				for (col = 0; col < last; )
				{
					header = GetHeader (hdrLine, col);
					if (header->IsVisible())
					{
						width = header->GetDMWidth();
						height = header->GetHeight();
						if (width == 0 || height == 0)
						{
							if (!header->GetText().empty())
							{
								r.SetRect (0, 0, 200, 0);
								DMStyle	*style = report->GetStyle (header->GetStyleID());
								inComposer->MeasureText (header->GetText(), style, r, style->ShouldWrap(), header->IsAttributed(), true, NULL);
							}
							else
								r.SetRect (0, 0, 0, 0);
							if (width == 0)
								width = dframe + r.Width();
							if (height == 0)
								height = dframe + r.Height();
						}
						
						header->SetDMWidth (width);
						lineWidth += width;
						header->SetDMHeight (height);
			//			if (not fixed)
						{
							span = header->GetColSpan();
							width /= span;
							for (span--; span >= 0; span--)
							{
								if (mColWidths [col + span] < width)
									mColWidths [col + span] = width;
							}
						}
						col += header->GetColSpan();
					}
					else
						col++;
				}
			}
			
			// adjust height of top headers
			// mTopRowHeights = new float [mNumTopHeadings];
			mTopRowHeights.assign (mNumTopHeadings, 0.0f);
			for (line = mNumTopHeadings - 1; line >= 0; line--)
			{
				hdrLine = GetHeaderRow (line);
				lineHeight = 0;
				last = hdrLine->size();
				for (col = 0; col < last; )
				{
					header = GetHeader (hdrLine, col);
					if (header->IsVisible())
					{
						height = header->GetDMHeight();
						span = header->GetRowSpan();
						for (span--; span > 0; span--)
							height -= mTopRowHeights [line + span];
						if (height > lineHeight)
							lineHeight = height;
						col += header->GetColSpan();
					}
					else
						col++;
				}
				
				mTopRowHeights [line] = lineHeight;
				mTopHeadingsHeight += lineHeight;
				
				for (col = 0; col < last; )
				{
					header = GetHeader (hdrLine, col);
					if (header->IsVisible())
					{
						height = lineHeight;
						span = header->GetRowSpan();
						for (span--; span > 0; span--)
							height += mTopRowHeights [line + span];
						header->SetDMHeight (height);
						col += header->GetColSpan();
					}
					else
						col++;
				}
			}
		}

//		if (mDrawColumns)
		{
#if 0
			if (not mDrawHeaders && mPosition.Width() > mNumColumns * 10)
			{
				fixed = true;
				width = mPosition.Width() / mNumColumns;
				for (col = 0; col < mNumColumns; col++)
					mColWidths [col] = width;
			}
#endif
			// calculate columns
			for (col = 0; col < mNumColumns; col++)
			{
				column = GetColumn (col);
				width = column->GetDMWidth();
				if (width == 0)
				{
					if ( not mDrawHeaders &&  !column->GetSource().empty()) // pB 2010 - returned back - use headers if available
					{
						r.SetRect (0, 0, 200, 0);
						DMStyle	*style = report->GetStyle (column->GetStyleID());
						width = dframe + inComposer->MeasureText (column->GetSource(), style, r, style->ShouldWrap(), column->IsAttributed(), true, NULL);
					}
					else if (mColWidths [col] == 0)			// no data, no headers...
						width = mPosition.Width() / mNumColumns;
					else
						width = mColWidths [col];			// no data - use header
					column->SetDMWidth (width);
				}
				if (width > mColWidths [col])		// column with specified width, header is taller - expand header
					mColWidths [col] = width;
				else if (width < mColWidths [col])
					column->SetDMWidth (mColWidths [col]);	// header is wider - expand column
				mTopHeadingsWidth += mColWidths [col];
			}
		}
	
		if (mDrawHeaders)
		{
			// adjust headers
			for (line = 0; line < mNumTopHeadings; line++)
			{
				hdrLine = GetHeaderRow (line);
				last = hdrLine->size();
				for (col = 0; col < last; col++)
				{
					header = GetHeader (hdrLine, col);
					if (header->IsVisible())
					{
						span = header->GetColSpan();
						width = 0;
						for (span--; span >= 0; span--)
							width += mColWidths [col + span];
						header->SetDMWidth (width);
					}
//					else
//						header->SetDMWidth (mColWidths [col]);
				}
			}
		}
	
		if (mDrawColumns)
		{
			// calculate row height - every column might use different style
			const RWString	rowTextMeasurement (u"ROW \u00DAg");
			mRowHeightDM = mRowHeight;
			for (col = 0; col < mNumColumns; col++)
			{
				DMStyle	*style;
				r.SetRect (0, 0, 200, 0);
//				if (col >= 0)
					style = report->GetStyle (GetColumn (col)->GetStyleID());
//				else
//					style = report->GetStyle (0);
				width = inComposer->MeasureText (rowTextMeasurement, style, r, false, false, true, NULL);
				height = dframe + r.Height();
				if (mRowHeightDM < height)
					mRowHeightDM = height;
			}
			for (col = 0; col < mNumColumns; col++)
			{
				column = GetColumn (col);
				column->SetDMHeight (mRowHeightDM);
			}
		}

		mColWidthsCalculated = true;

		r.SetRect (0, 0, 0, 0);
		r.bottom += dframe;
		r.bottom += mTopHeadingsHeight;
		r.right += dframe;
		r.right += mTopHeadingsWidth;	// GetColsWidth (0, mNumColumns - 1);
		SRect	rect (r);
		// r is now table frame
		if (mFrame)		// space for frame
			r *= frame;

		// adjust cell bounds
//		if (mFrame)
		{
			if (mDrawHeaders)
			{
				// adjust headers
				for (line = 0; line < mNumTopHeadings; line++)
				{
					r.left = rect.left;
					if (mFrame)		// space for frame
						r.left += frame;
					hdrLine = GetHeaderRow (line);
					last = hdrLine->size();
					int	lastCol = 0;
					for (col = 0; col < last; )
					{
						header = GetHeader (hdrLine, col);
						if (header->IsVisible())
						{
							if (lastCol < col)
								r.left += GetColsWidth (lastCol, col - 1);
							r.right = r.left + header->GetDMWidth();
							r.bottom = r.top + header->GetDMHeight();
							// r is now cell frame
							if (mFrame)		// space for frame
							{
								r *= frame;
								//r.top += frame;
								//r.left += frame;
								r.right += mThickness;
								r.bottom += mThickness;
							}
							header->SetPosition (r);
							r.left = r.right;
							if (mFrame)
							{
								r.top -= frame;
								r.left += frame - mThickness;
							}
							col += header->GetColSpan();
							lastCol = col;
						}
						else
							col++;
					}
					r.top += mTopRowHeights [line];
				}
			}

			if (mDrawColumns)
			{
				r.top = rect.top + mTopHeadingsHeight + frame;
				r.left = rect.left + frame;
				if (mDrawHeaders)
					r.top += mThickness;
				// adjust columns
				for (col = 0; col < mNumColumns; col++)
				{
					column = GetColumn (col);
					r.right = r.left + column->GetDMWidth();
					r.bottom = r.top + mRowHeightDM;
					// r is now cell frame
					if (mFrame)		// space for frame
					{
						r *= frame;
						r.right += mThickness;
						r.bottom += mThickness;
					}
					column->SetPosition (r);
					r.left = r.right;
					if (mFrame)
					{
						r.top -= frame;
						r.left += frame - mThickness;
					}
				}
			}
		}

#if	0 && TARGET_DEBUG
		printf ("\nRW: CalculateAll: DMTABLE %d topHeadings, %d cols\n", mNumTopHeadings, mNumColumns);
		for (line = 0; line < mNumTopHeadings; line++)
		{
			int	last = GetHeaderRow (line)->size();
			printf ("Line %d: %d", line + 1, last);
			width = 0;
			for (col = 0; col < last; col++)
			{
				header = GetHeader (line, col);
				width += header->GetDMWidth();
				printf ("\t%d,%d,%d,%.2f", (int) header->IsVisible(), col + 1, header->GetColSpan(), header->GetDMWidth());
			}
			printf ("\twidth=%.2f\n", width);
		}
		printf ("Column widths: ");
		width = 0;
		for (col = 0; col < mNumColumns; col++)
		{
			column = GetColumn (col);
			width += column->GetDMWidth();
			printf (col > 0 ? ", %.2f" : "%.2f", column->GetDMWidth());
		}
		printf ("\twidth=%.2f\n", width);
		printf ("colWidths    : ");
		width = 0;
		for (col = 0; col < mNumColumns; col++)
		{
			width += mColWidths [col];
			printf (col > 0 ? ", %.2f" : "%.2f", mColWidths [col]);
		}
		printf ("\twidth=%.2f\n\n", width);
		fflush (stdout);
#endif
	
	}

	return;
}

#pragma	mark	-

void
DMBase::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (IsVisible() && (inParent & mDrawRect))
	{
		if (inMode == eDraw_Order)
		{
			GetReport()->DrawFrame (this, inMode, -2, RWStr::FromInteger (mSeqID), mDrawRect, false);
		}
		else if (inMode == eDraw_Size)
		{
			char	str [16];
			snprintf (str, sizeof (str), "%g,%g", mPosition.Width(), mPosition.Height());
			GetReport()->DrawFrame (this, inMode, -1, RWStr::FromASCII (str), mDrawRect, false);
		}
		else // if (inMode == eDraw_ID)
			GetReport()->DrawFrame (this, inMode, -1, mID, mDrawRect, false);
		if (mSelected)
			GetReport()->DrawSelection (this, mDrawRect);
	}
	return;
}

void
DMBase::ParseData (RWPageComposer *inComposer)
{
	return;
}

DMBase::EHitTest
DMBase::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		if (mSelected)
		{
            double squareOffset = GetProxySize(eOut_Handle);
			SRect	r (mDrawRect);
            r *= -squareOffset; //-kSelectionSquareOffset;
			if (inWhere.IsContained (r))
			{
				if (inWhere.IsContained (mDrawRect))
					hit = eHit_Object;
                
				r.SetRect (mDrawRect.top - squareOffset, mDrawRect.left - squareOffset, mDrawRect.top + squareOffset, mDrawRect.left + squareOffset);
				if (inWhere.IsContained (r)) // /* mObjectKind != eObject_Line && */
					hit = eHit_TLH;
				else
				{
					r.left += mDrawRect.Width() / 2;
					r.right = r.left + 2 * squareOffset;
					if (inWhere.IsContained (r))  /* (mObjectKind != eObject_Line || mPosition.Width() == 0) && */
						hit = eHit_TCH;
					else
					{
						r.left = mDrawRect.right - squareOffset;
						r.right = r.left + 2 * squareOffset;
						if (inWhere.IsContained (r))  /* mObjectKind != eObject_Line && */
							hit = eHit_TRH;
						else
						{
							r.top += mDrawRect.Height() / 2;
							r.bottom = r.top + 2 * squareOffset;
							if (inWhere.IsContained (r))  /* (mObjectKind != eObject_Line || mPosition.Height() == 0) && */
								hit = eHit_RCH;
							else
							{
								r.left = mDrawRect.left - squareOffset;
								r.right = r.left + squareOffset;
								if (inWhere.IsContained (r))  /* (mObjectKind != eObject_Line || mPosition.Height() == 0) && */
									hit = eHit_LCH;
								else
								{
									r.SetRect (mDrawRect.bottom - squareOffset, mDrawRect.left - squareOffset, mDrawRect.bottom + squareOffset, mDrawRect.left + squareOffset);
									if (/* mObjectKind != eObject_Line && */ inWhere.IsContained (r))
										hit = eHit_BLH;
									else
									{
										r.left += mDrawRect.Width() / 2;
										r.right = r.left + 2 * squareOffset;
										if (inWhere.IsContained (r))  /* (mObjectKind != eObject_Line || mPosition.Width() == 0) && */
											hit = eHit_BCH;
										else
										{
											r.left = mDrawRect.right - squareOffset;
											r.right = r.left + 2 * squareOffset;
											if (/* mObjectKind != eObject_Line && */ inWhere.IsContained (r))
												hit = eHit_BRH;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (inWhere.IsContained (mDrawRect))
			hit = eHit_Object;

		if (hit != eHit_None)
			outObjectHit = this;
	}
	return hit;
}

void
DMBase::HandleTrackSelect (SRect &inWhere, UInt32 inFlags)
{
	if (IsSelectable())
	{
		bool	select = false;

		if (inWhere & mDrawRect)
		{
			if ((inFlags & 1) == 0)
				select = true;
			else if (inWhere.Contains (mDrawRect))
				select = true;
		}
		SetSelected (select);
	}
	return;
}


void
DMObject::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		if (inMode == eDraw_ID || inMode == eDraw_Order || inMode == eDraw_Size)
			DMBase::Draw (inComposer, inParent, inMode);
		else
		{
			GetReport()->DrawFrame (this, inMode, -1, mName, mDrawRect, false);
			if (mSelected)
				GetReport()->DrawSelection (this, mDrawRect);
		}
	}
	return;
}

void
DMGroup::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	SRect	r (inParent);
	r &= mDrawRect;
	if (not r.IsEmpty())
	{
		DMObject::Draw (inComposer, inParent, inMode);	// draw self
		StClipToRect	clip (inComposer, r);
		PSObjList::iterator	iter;
		for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
		{
			DMBase	*obj = static_cast <DMBase*> (*iter);
			if (obj != NULL && obj->IsVisible()) // pB v1.3.1
				obj->Draw (inComposer, r, inMode);
		}
	}
	return;
}

DMBase::EHitTest
DMGroup::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		hit = DMObject::HitTest (inWhere, outObjectHit);
		if (hit == eHit_Object && not IsLocked())
		{
			PSObjList::reverse_iterator	iter;
			for (iter = mObjects.rbegin(); iter != mObjects.rend(); iter++)
			{
				DMBase	*obj = static_cast <DMBase*> (*iter);
				if (obj != NULL && obj->IsSelectable())
				{
					hit = obj->HitTest (inWhere, outObjectHit);
					if (hit != eHit_None)
						break;
				}
			}
			if (hit == eHit_None)
			{
				hit = eHit_Object;
				outObjectHit = this;
			}
		}
	}

	return hit;
}

void
DMGroup::HandleTrackSelect (SRect &inWhere, UInt32 inFlags)
{
	if (IsSelectable())
	{
		if (not IsLocked())
		{
			SetSelected (false);
			PSObjList::iterator	iter;
			for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
			{
				DMBase	*obj = static_cast <DMBase*> (*iter);
				if (obj != NULL && obj->IsSelectable())
					obj->HandleTrackSelect (inWhere, inFlags);
			}
		}
		else
			DMBase::HandleTrackSelect (inWhere, inFlags);
	}
	return;
}


void
DMLine::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		DMObject::Draw (inComposer, inParent, inMode);	// draw self
		inComposer->DrawLine (mDrawRect, mThickness, mLineColor, mFlags);
	}
	return;
}

DMBase::EHitTest
DMLine::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
#if	1
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		if (mObjectKind != eObject_Line || (mFlags != RWLine_Horizontal && mFlags != RWLine_Vertical))	// subclass should override...
			return DMObject::HitTest (inWhere, outObjectHit);

		SRect		r (mDrawRect);
		r *= -1;
		if (inWhere.IsContained (r))
			hit = eHit_Object;
		if (mSelected && hit == eHit_Object)
		{
			r.SetRect (mDrawRect.top - 1, mDrawRect.left - 1, mDrawRect.top + 1, mDrawRect.left + 1);
			if (inWhere.IsContained (r))
				hit = mFlags == RWLine_Horizontal? eHit_LCH : eHit_TCH; //pB
			else
			{
				r.SetRect (mDrawRect.bottom - 1, mDrawRect.right - 1, mDrawRect.bottom + 1, mDrawRect.right + 1);
				if (inWhere.IsContained (r))
					hit = mFlags == RWLine_Horizontal? eHit_RCH : eHit_BCH;
			}
		}

		if (hit != eHit_None)
			outObjectHit = this;
	}
	return hit;
#else
	return DMObject::HitTest (inWhere, outObjectHit);
#endif
}

void
DMOval::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		DMObject::Draw (inComposer, inParent, inMode);	// draw self
		if (inMode < eDraw_Name || inMode == eDraw_Resource)
			inComposer->DrawOval (mDrawRect, mThickness, mThickness > 0, mLineColor, mFill, mFillColor);
	}
	return;
}


void
DMRect::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	SRect	sect (inParent);
	sect &= mDrawRect;
	if (not sect.IsEmpty())
	{
		DMObject::Draw (inComposer, inParent, inMode);
		if (inMode < eDraw_Name || inMode == eDraw_Resource)
			inComposer->DrawRect (inParent, mDrawRect, mThickness, mLineColor, mFill, mFillColor, mRows, mCols, mFlags);
	}
	return;
}

void
DMPict::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	SRect	r (inParent);
	r &= mDrawRect;
	if (not r.IsEmpty())
	{
		DMObject::Draw (inComposer, inParent, inMode);
		if (inMode < eDraw_Name || inMode == eDraw_Resource)
		{
			StClipToRect	clip (inComposer, r);
			r = mDrawRect;
			if (mFrame)
			{
				inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
				r *= mFrameOffset;
			}
			if (mPicture.GetBlobSize() > 0 && mPicture4D.GetKind() == RWValue::eValue_Undefined)
			{
//				PA_Picture	pict = PA_CreatePicture (mPicture.GetBlobData(), mPicture.GetBlobSize());
				PA_Picture	pict = SR4DData::Get4DPicture (mPicture);
				if (pict)
				{
                    RWScreenPict	sp = reinterpret_cast <RWScreenPict> (PA_CreateNativePictureForScreen (pict));
					mPicture4D.SetPictureRef (sp, true);
					PA_DisposePicture (pict);
				}
				else
					mPicture4D.SetPictureRef (0, true);
			}
			inComposer->DrawPict (r, mPicture4D, EPictFormat (mFormatI), &mComposerPictureData, mObjectRotation, ((float)mFillColor.alpha)/0xFFFF);
		}
	}
}

void
DMText::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		switch (inMode)
		{
			case eDraw_Resource:
				if (mParsedText.IsEmpty())	//mbs 29072011	reuse code in ParseData()
					ParseData (inComposer);

				if (mFrame)
					inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
				GetReport()->DrawFrame (this, inMode, mStyleID, mParsedText, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;

			case eDraw_Normal:
			case eDraw_Name:
			case eDraw_Alias:
			case eDraw_Format:
				if (mFrame)
					inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
				GetReport()->DrawFrame (this, inMode, mStyleID, mText, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			default:
				DMObject::Draw (inComposer, inParent, inMode);
				break;
		}
	}
}

void
DMText::ParseData (RWPageComposer *inComposer)
{
	mParsedText.Free();
	if (mIsDynamic)
	{
		SRDataSource	*ds;
		if (not mText.IsEmpty() && (ds = GetReport()->GetDataSource()) != NULL)
		{
			long	textLen = mText.StrLength();
			long	curPos = 0, delta = 0, endPos;
			
			RWString	result (mText);		// was "(mText, textLen)": the substring *from* textLen, i.e. empty
			RWString	varName;
			RWString	format;
			while (curPos < textLen && RWTools::ParseTextForVar (mIsAttributed, mText, textLen, curPos, endPos, varName, format))
			{
				RWStringView	varname = varName;
				bool			encode = mIsAttributed;
				if (!varname.empty() && varname[0] == u'+')
				{
					varname.remove_prefix (1);
					encode = false;
				}
				RWString	varText;
				if (!varname.empty())
				{
					RWValue		var;
					ds->GetVariable (RWString (varname), var);
					varText = ds->FormatVariable (var, format);
				}
				varName.clear();
				format.clear();
				result.erase (curPos - delta, endPos - curPos);
				if (encode)
					varText = RWTools::EscapeAttributedString (varText);
				result.insert (curPos - delta, varText);
				delta += endPos - curPos - (long) varText.size();
				curPos = endPos;
			}
			mParsedText = result;
		}
	}
	else 
	{
		long	textLen = mText.StrLength();
		RWString	localized;
		if (RWTools::ParseTextForXLIFF (mText, textLen, localized) )
			mParsedText = localized;
		else
			mParsedText = mText;
						
	}
				
}

void
DMVariable::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		switch (inMode)
		{
			case eDraw_Normal:
			case eDraw_Alias:
				if (mFrame)
					inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
				GetReport()->DrawFrame (this, inMode, mStyleID, mAlias.IsEmpty()? mText: mAlias, mDrawRect, false, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_Format:
				if (mFrame)
					inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
				GetReport()->DrawFrame (this, inMode, mStyleID, mFormat, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_Resource:
			{
				SRDataSource	*ds;
				if (not mText.IsEmpty() && (ds = GetReport()->GetDataSource()) != NULL)
				{
					//mbs 29072011	pict support
					if (mParsedText.IsEmpty() && mPicture4D.GetKind() != RWValue::eValue_PictRefScreen)
						ParseData (inComposer);

					GetReport()->DrawFrame (this, inMode, mStyleID, mParsedText, mDrawRect, mIsAttributed, &mStyle);
					if (mFrame)
						inComposer->DrawRect (mDrawRect, mThickness, true, mLineColor, false, cBlackColor);
					if (mPicture4D.GetKind() == RWValue::eValue_PictRefScreen)
					{
						SRect	r (mDrawRect);
						if (mFrame)
							r *= mFrameOffset;
						EPictFormat	fmt = ePictFormat_Normal;
						if (not mFormat.IsEmpty())
							if (mFormat [0] >= '0' && mFormat [0] <= '4' && mFormat[1] == 0)
								fmt = EPictFormat (mFormat [0] - '0');
						inComposer->DrawPict (r, mPicture4D, fmt, &mComposerPictureData, 0, 0);
					}
					if (mSelected)
						GetReport()->DrawSelection (this, mDrawRect);
					break;
				}
				// FALL THROUGH
			}

			default:
				DMText::Draw (inComposer, inParent, inMode);
				break;
		}
	}
}

void
DMVariable::ParseData (RWPageComposer *inComposer)
{
	mParsedText.Free();
	if (mComposerPictureData)	//mbs 29072011	pict support
	{
		delete mComposerPictureData;
		mComposerPictureData = NULL;
	}
	mPicture4D.Free();	//mbs 29072011	pict support

	SRDataSource	*ds;
	if (not mText.IsEmpty() && (ds = GetReport()->GetDataSource()) != NULL)
	{
		RWValue		var;
		if (!mScript.IsEmpty())
			ds->RunScript (mScript, this);
		if (ds->GetVariable (mText, mIndex, var, ECalcType_CurrentValue))
		{
			//mbs 29072011	pict support
			if (var.GetKind() >= RWValue::eValue_PictRefScreen)
			{
				PA_Picture	pict = SR4DData::Get4DPicture (var);
				if (pict)
				{
					RWScreenPict	sp = reinterpret_cast <RWScreenPict> (PA_CreateNativePictureForScreen (pict));
					mPicture4D.SetPictureRef (sp, true);
					PA_DisposePicture (pict);
				}
				else
					mPicture4D.SetPictureRef (0, true);

				SRect	r (mDrawRect);
				inComposer->GetPictBounds (r, mPicture4D, ePictFormat_Normal, &mComposerPictureData, true);
			}
			else
				mParsedText = ds->FormatVariable (var, mFormat);
		}
	}
}

#if	0	//mbs 29072011	identical with DMVariable::Draw()
void
DMField::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		switch (inMode)
		{
			default:
				DMVariable::Draw (inComposer, inParent, inMode);
				break;
		}
	}
}
#endif


void
DMField::ParseData (RWPageComposer *inComposer)
{
	mParsedText.Free();
	if (mComposerPictureData)	//mbs 29072011	pict support
	{
		delete mComposerPictureData;
		mComposerPictureData = NULL;
	}
	mPicture4D.Free();	//mbs 29072011	pict support

	SRDataSource	*ds;
	if (not mText.IsEmpty() && (ds = GetReport()->GetDataSource()) != NULL)
	{
		RWValue		var;
		if (!mScript.IsEmpty())
			ds->RunScript (mScript, this);
		// TODO	pict support
		if (ds->GetField (mText, var, ECalcType_CurrentValue))
		{
			//mbs 29072011	pict support
			if (var.GetKind() >= RWValue::eValue_PictRefScreen)
			{
				PA_Picture	pict = SR4DData::Get4DPicture (var);
				if (pict)
				{
					RWScreenPict	sp = reinterpret_cast <RWScreenPict> (PA_CreateNativePictureForScreen (pict));
					mPicture4D.SetPictureRef (sp, true);
					PA_DisposePicture (pict);
				}
				else
					mPicture4D.SetPictureRef (0, true);
				
				SRect	r;
				inComposer->GetPictBounds (r, mPicture4D, ePictFormat_Normal, &mComposerPictureData, true);
			}
			else
				mParsedText = ds->FormatVariable (var, mFormat);
		}
	}
}


void
DMHeader::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (mVisible && inParent & mDrawRect)
	{
		switch (inMode)
		{
			case eDraw_Normal:
			case eDraw_Alias:
			case eDraw_Format:
			case eDraw_Name:
				GetReport()->DrawFrame (this, inMode, mStyleID, mText, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_ID:
			case eDraw_Order:
			case eDraw_Size:
			case eDraw_Resource:
			default:
				DMBase::Draw (inComposer, inParent, inMode);
				break;
		}
	}
}

DMBase::EHitTest
DMHeader::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		SRect		r (mDrawRect);
		r *= -1;
		if (mSelected && inWhere.IsContained (r))
		{
			hit = eHit_Object;
			r.SetRect (mDrawRect.top - 1, mDrawRect.right - 1, mDrawRect.bottom + 1, mDrawRect.right + 1);
			if (inWhere.IsContained (r))
				hit = eHit_ResizeH;
			else
			{
				r.SetRect (mDrawRect.bottom - 1, mDrawRect.left - 1, mDrawRect.bottom + 1, mDrawRect.right + 1);
				if (inWhere.IsContained (r))
					hit = eHit_ResizeV;
			}
		}
		else if (inWhere.IsContained (mDrawRect))
			hit = eHit_Object;

		if (hit != eHit_None)
			outObjectHit = this;
	}
	return hit;
}


void
DMColumn::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inParent & mDrawRect)
	{
		switch (inMode)
		{
			case eDraw_Alias:
				GetReport()->DrawFrame (this, inMode, mStyleID, mAlias.IsEmpty()? mSource: mAlias, mDrawRect, false, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_Normal:
			case eDraw_Name:
				GetReport()->DrawFrame (this, inMode, mStyleID, mSource, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_Format:
				GetReport()->DrawFrame (this, inMode, mStyleID, mFormat, mDrawRect, mIsAttributed, &mStyle);
				if (mSelected)
					GetReport()->DrawSelection (this, mDrawRect);
				break;
			case eDraw_Order:
				{
					GetReport()->DrawFrame (this, inMode, -2, RWStr::FromInteger (mLevel), mDrawRect, false, &mStyle);
					if (mSelected)
						GetReport()->DrawSelection (this, mDrawRect);
				}
				break;
			case eDraw_ID:
			case eDraw_Size:
			case eDraw_Resource:
			default:
				DMBase::Draw (inComposer, inParent, inMode);
				break;
		}
	}
}

DMBase::EHitTest
DMColumn::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		SRect		r (mDrawRect);
		r *= -1;
		if (mSelected && inWhere.IsContained (r))
		{
			hit = eHit_Object;
			r.SetRect (mDrawRect.top - 1, mDrawRect.right - 1, mDrawRect.bottom + 1, mDrawRect.right + 1);
			if (inWhere.IsContained (r))
				hit = eHit_ResizeH;
			else
			{
				r.SetRect (mDrawRect.bottom - 1, mDrawRect.left - 1, mDrawRect.bottom + 1, mDrawRect.right + 1);
				if (inWhere.IsContained (r))
					hit = eHit_ResizeV;
			}
		}
		else if (inWhere.IsContained (mDrawRect))
			hit = eHit_Object;
		
		if (hit != eHit_None)
			outObjectHit = this;
	}
	return hit;
}


void
DMTable::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (IsVisible())
	{
		SRect	rr (inParent);
		rr &= mDrawRect;
		if (not rr.IsEmpty())
		{
			DMObject::Draw (inComposer, inParent, inMode);	// draw self
			StClipToRect	clip (inComposer, rr);
			SRect			r (mDrawRect);
			DMReport		*report = GetReport();
			float			frame = 0, dframe = 0;

			if (mFrame)		// space for frame
			{
				frame = mThickness + mFrameOffset;
				dframe = 2 * frame;
			}

			r.bottom = r.top + dframe;
			if (mDrawHeaders)
				r.bottom += mTopHeadingsHeight + mThickness;
			if (mDrawColumns)
				r.bottom += mRowHeightDM * 2;
			r.right = r.left + dframe + mTopHeadingsWidth + mThickness;	// GetColsWidth (0, mNumColumns - 1);

			// draw the table frame
			SRGBColor	c;
			if (mFrame > 1)
			{
				c = report->GetStyle (mStyleID)->GetFrameColor();
	//			c.alpha = 16384;	// 25%
				inComposer->DrawRect (r, mThickness, true, c, false, cBlackColor);
			}

			SRect	rect (r);
			if (mFrame)		// space for frame
				r *= frame;
			// r &= inParent;	// need the topLeft position...
			if (r.bottom > inParent.bottom)
				r.bottom = inParent.bottom;
			if (r.right > inParent.right)
				r.right = inParent.right;

			if (mDrawHeaders || mDrawColumns)
			{
				PSObjListD	*hdrLine;
				DMHeader	*header;
				DMColumn	*column;
				int			line, last, col;

				if (mDrawHeaders && mFrame)
				{
					// draw top headers frames
					for (line = 0; line < mNumTopHeadings; line++)
					{
//						r.left = rect.left + frame;
						hdrLine = GetHeaderRow (line);
						last = hdrLine->size();
						for (col = 0; col < last; col++)
						{
							header = GetHeader (hdrLine, col);
							if (header->IsVisible())
							{
								c = report->GetStyle (header->GetStyleID())->GetFrameColor();
//								c.alpha = 16384;	// 25%
//								r.right = r.left + header->GetDMWidth() + dframe;
//								r.bottom = r.top + header->GetDMHeight() + dframe;
//								inComposer->DrawRect (r, mThickness, true, c, false, cBlackColor);
//								r.left = r.right - frame;
								SRect	hr = header->GetDrawPosition();
								hr *= -frame;
//								hr.right += frame/2;
//								hr.bottom += frame/2;
								inComposer->DrawRect (hr, mThickness, true, c, false, cBlackColor);
							}
						}
//						r.top += mTopRowHeights [line];
					}
				}

				if (mDrawColumns && mFrame)
				{
					// draw vertical grid
					r.top = rect.top + mTopHeadingsHeight + frame + mThickness;
					if (mDrawHeaders)
						r.top += mThickness;
					r.left = rect.left + frame;
					r.bottom = rect.bottom - frame - mThickness;
					r.right = r.left;
					inComposer->DrawLine (r, mThickness, mLineColor, RWLine_Vertical);	// report->GetStyle (mStyleID)->GetFrameColor()
//					r.left += mFrameOffset;
					for (col = 0; col < mNumColumns; col++)
					{
						column = GetColumn (col);
						r.left += column->GetDMWidth() + dframe - mThickness;
						r.right = r.left;
						if (col == mNumColumns - 1)
							inComposer->DrawLine (r, mThickness, mLineColor, RWLine_Vertical);	// report->GetStyle (mStyleID)->GetFrameColor()
						else if (column->GetGrid())
							inComposer->DrawLine (r, mThickness, report->GetStyle (column->GetStyleID())->GetFrameColor(), RWLine_Vertical);
					}

					// draw horizontal grid
					r.left = rect.left + frame;
					r.right = r.left + mTopHeadingsWidth + mThickness;
					r.bottom = r.top = rect.top + mTopHeadingsHeight + frame;
					if (mDrawHeaders)
						r.top += mThickness;
					inComposer->DrawLine (r, mThickness, mLineColor, RWLine_Horizontal);	// report->GetStyle (mStyleID)->GetFrameColor()
					r.top += mRowHeightDM - (mThickness - mHGridThickness) / 2;
					r.bottom = r.top;
					if (mHGridThickness > 0)
						inComposer->DrawLine (r, mHGridThickness, mLineColor, RWLine_Horizontal);
					r.bottom = r.top = rect.top + mTopHeadingsHeight + frame + mRowHeightDM + mRowHeightDM;
					inComposer->DrawLine (r, mThickness, mLineColor, RWLine_Horizontal);	// report->GetStyle (mStyleID)->GetFrameColor()
				}

				if (mDrawHeaders)
				{
					// draw top headers
					for (line = 0; line < mNumTopHeadings; line++)
					{
						hdrLine = GetHeaderRow (line);
						last = hdrLine->size();
						for (col = 0; col < last; col++)
						{
							header = GetHeader (hdrLine, col);
							if (header->IsVisible())
								header->Draw (inComposer, rr, inMode);
						}
					}
				}

				if (mDrawColumns)
				{
					// draw columns
					for (col = 0; col < mNumColumns; col++)
					{
						column = GetColumn (col);
						column->Draw (inComposer, rr, inMode);
					}
				}
			}
		}
	}
	return;
}

DMBase::EHitTest
DMTable::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
		hit = DMObject::HitTest (inWhere, outObjectHit);
		if (/* mSelected && */ hit == eHit_Object && (mDrawHeaders || mDrawColumns) && not IsLocked())
		{
			hit = eHit_None;

			int		col;

			if (mDrawHeaders)
			{
				int			line, last;
				for (line = 0; line < mNumTopHeadings && hit == eHit_None; line++)
				{
					PSObjListD	*hdrLine = GetHeaderRow (line);
					last = hdrLine->size();
					for (col = 0; col < last; col++)
					{
						DMHeader	*header = GetHeader (hdrLine, col);
						if ((hit = header->HitTest (inWhere, outObjectHit)) != eHit_None)
						{
							outObjectHit = header;
							break;
						}
					}
				}
			}
			
			if (mDrawColumns)
			{
				for (col = 0; col < mNumColumns && hit == eHit_None; col++)
				{
					DMColumn	*column = GetColumn (col);
					if ((hit = column->HitTest (inWhere, outObjectHit)) != eHit_None)
					{
						outObjectHit = column;
						break;
					}
				}
			}

			if (hit == eHit_None)
				hit = eHit_Object;
		}
	}

	return hit;
}
