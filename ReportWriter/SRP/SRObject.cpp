# include	"SRObject.h"
# include	"SRReportData.h"
# include	"SRReportWriter.h"
# include	"SRDataSource.h"
# include	"PSObjProps.h"

# define	ALL_ObjProps	\
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},	\
{ PSObjPropKind,				false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},	\
{ PSObjPropName,				true,		PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},	\
{ PSObjPropID,					true,		PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},	\
{ PSObjPropOrder,				false,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 1, LONG_MAX }		},	\
{ PSObjPropRect,				true,		PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }						},	\
{ PSObjPropFixV,				true,		PSProps_Attribute,	PSProps_Boolean,	"fixV",				{ NULL, 0, 0, 1 } 				},	\
{ PSObjPropBindV,				true,		PSProps_Attribute,	PSProps_Boolean,	"bindV",			{ NULL, 0, 0, 1 } 				},	\
{ PSObjPropAlign,				true,		PSProps_Attribute,	PSProps_Integer,	"align",			{ NULL, 0, 0, 4}, true			},	\
{ PSObjPropAlign,				true,		PSProps_Attribute,	PSProps_List,		"align",			{ sAlignment, 0, }				},	\
{ PSObjPropAlign,				true,		PSProps_Attribute,	PSProps_List,		"align",			{ sAlignment2, 0, }		},	\
{ PSObjPropDraw,				true,		PSProps_Attribute,	PSProps_List,		"draw",				{ sDraw, 1 }					},	\
{ PSObjPropPosTop,				true,		PSProps_Attribute,	PSProps_Real,		"top",				{ NULL, 0, INT_MIN, INT_MAX }	},	\
{ PSObjPropPosLeft,				true,		PSProps_Attribute,	PSProps_Real,		"left",				{ NULL, 0, INT_MIN, INT_MAX }	},	\
{ PSObjPropPosBottom,			true,		PSProps_Attribute,	PSProps_Real,		"bottom",			{ NULL, 0, INT_MIN, INT_MAX }	},	\
{ PSObjPropPosRight,			true,		PSProps_Attribute,	PSProps_Real,		"right",			{ NULL, 0, INT_MIN, INT_MAX }	},	\
{ PSObjPropPosWidth,			true,		PSProps_Attribute,	PSProps_Real,		"width",			{ NULL, 0, 1, INT_MAX }			},	\
{ PSObjPropPosHeight,			true,		PSProps_Attribute,	PSProps_Real,		"height",			{ NULL, 0, 1, INT_MAX }			},	\
{ PSObjPropObjectRotation,		true,		PSProps_Attribute,	PSProps_Real,       "rotation",         { NULL, 0, -360, 360 }			},	\
{ PSObjPropMirror,				true,		PSProps_Attribute,	PSProps_Boolean,	"mirroring",		{ sMirror, 0, 1 }			},	\
//{ PSObjPropSkew,				true,		PSProps_Attribute,	PSProps_Real,		"skewing",			{ NULL, 0, 0, INT_MAX }			},	\

//{ PSObjPropFixH,			true,	PSProps_Attribute,	PSProps_Boolean,	"fixH",				{ NULL, 0, 0, 1 } 				},
//{ PSObjPropBindH,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindH",			{ NULL, 0, 0, 1 } 				},

const PSObject::PSObjProps	SRObject::sProperties[] = {
ALL_ObjProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRGroup::sProperties[] = {
ALL_ObjProps
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
{ PSObjPropObjects,			false,	PSProps_Value,		PSProps_Objects,	"Objects",			{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRLine::sProperties[] = {
ALL_ObjProps
{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"flags",			{ NULL, RWLine_Horizontal, 0, RWLine_Full }	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SROval::sProperties[] = {
ALL_ObjProps
{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},

{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRRect::sProperties[] = {
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

const PSObject::PSObjProps	SRPict::sProperties[] = {
ALL_ObjProps
//{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
//{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
// { PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
//{ PSObjPropObjectRotation,  true,   PSProps_Attribute,  PSProps_Real,       "rotation",         { NULL, 0, -360, 360 }          },
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_List,		"format",			{ sPictFormat, ePictFormat_Normal, ePictFormat_First, ePictFormat_Last-1 }, true },
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_Integer,	"format",			{ NULL, ePictFormat_Normal, ePictFormat_First, ePictFormat_Last-1 }				},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_None,		PSProps_BLOB,		"ImageData",		{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

static const char*	sPictDataProperties[] = { "ImageData", "format", "encoding", "base64" };


const PSObject::PSObjProps	SRText::sProperties[] = {
ALL_ObjProps
//{ PSObjPropThickness,		true,	PSProps_Attribute,	PSProps_Real,		"thickness",		{ NULL, 1, 0, 10 }				},
//{ PSObjPropLineColor,		true,	PSProps_Attribute,	PSProps_Color,		"lineColor",		{ NULL }						},
//{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Integer,	"fillPattern",		{ NULL, 0, 0, 255 }				},
//{ PSObjPropFillColor,		true,	PSProps_Attribute,	PSProps_Color,		"fillColor",		{ NULL }						},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},

//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},

{ PSObjPropTabStops,		true,	PSProps_Value,		PSProps_String,		"tabStops",			{ NULL }						},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }, true					},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }, true		},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }, true			},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }, true		},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }	, true		},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }, true					},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }, true					},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }, true	},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }, true	},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }, true		},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }, true		},
	
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRVariable::sProperties[] = {
ALL_ObjProps
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},

{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_String,		"source",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
{ PSObjPropElement,			true,	PSProps_Attribute,	PSProps_Integer,	"elem",				{ NULL, SR4DVariable::SR4DVariable_Variable, SR4DVariable::SR4DVariable_Variable, INT_MAX }		},
{ PSObjPropCalcType,		true,	PSProps_Attribute,	PSProps_Integer,	"calc",				{ NULL, ECalcType_None, ECalcType_None, ECalcType_Last-1 }		},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat, 0 }					},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat2, 0 }, true			},
{ PSObjPropRepeatOffset,	true,	PSProps_Attribute,	PSProps_Real,		"repeatOffset",		{ NULL, 0, 0, 128 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	// child element(s)

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }, true					},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }, true		},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }, true			},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }, true		},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }	, true		},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }, true					},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }, true					},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }, true	},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }, true	},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }, true		},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }, true		},
	
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRField::sProperties[] = {
ALL_ObjProps
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
//{ PSObjPropExpandH, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandH",			{ NULL, 0, 0, 1 }				},
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"expandV",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawEmpty,		true,	PSProps_Attribute,	PSProps_List,		"empty", 			{ sEmpty, eEmpty_Draw, eEmpty_Draw, eEmpty_RemoveRow }	},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Boolean,	"frame",			{ NULL, 0, 0, 1 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},

{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_String,		"source",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
//{ PSObjPropElement,		true,	PSProps_Attribute,	PSProps_Integer,	"elem",				{ NULL, SR4DVariable::SR4DVariable_Variable, SR4DVariable::SR4DVariable_Variable, INT_MAX }		},
{ PSObjPropCalcType,		true,	PSProps_Attribute,	PSProps_Integer,	"calc",				{ NULL, ECalcType_None, ECalcType_None, ECalcType_Last-1 }		},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat, 0 }					},
{ PSObjPropRepeat,			true,	PSProps_Attribute,	PSProps_List,		"repeat",			{ sRepeat2, 0 }, true			},
{ PSObjPropRepeatOffset,	true,	PSProps_Attribute,	PSProps_Real,		"repeatOffset",		{ NULL, 0, 0, 128 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	// child element(s)

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }, true					},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }, true		},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }, true			},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }, true		},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }	, true		},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }, true					},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }, true					},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }, true	},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }, true	},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }, true		},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }, true		},
	
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRTable::sProperties[] = {
ALL_ObjProps
{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, 0, 0, INT_MAX }			},
{ PSObjPropFrame,			true,	PSProps_Attribute,	PSProps_Integer,	"frame",			{ NULL, 1, 0, 2 }				},
{ PSObjPropFrameOffset,		true,	PSProps_Attribute,	PSProps_Real,		"frameOffset",		{ NULL, 2, 0, 32 }				},
{ PSObjPropFrameThickness,	true,	PSProps_Attribute,	PSProps_Real,		"frameThickness",	{ NULL, 1, 0, 10 }				},
{ PSObjPropFrameColor,		true,	PSProps_Attribute,	PSProps_Color,		"frameColor",		{ NULL }						},
{ PSObjPropHGridThickness,	true,	PSProps_Attribute,	PSProps_Real,		"hGridThickness",	{ NULL, 0.5, 0, 10 }			},

{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height", 			{ NULL, 0, 0, 512 }				},
{ PSObjPropNumCols,			false,	PSProps_Attribute,	PSProps_Integer,	"cols",				{ NULL }						},
{ PSObjPropNumHeadings,		false,	PSProps_Attribute,	PSProps_Integer,	"hdrs",				{ NULL }						},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropHeader,			true,	PSProps_OneChild,	PSProps_Objects,	"Head",				{ NULL }						},
{ PSObjPropColumn,			true,	PSProps_OneChild,	PSProps_Objects,	"Columns",			{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRObject									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRObject::SRObject (SRReportData *inReport, long inOrder, EObject_Kind inKind)
	:	PSObject (inKind),
		mReportData (inReport),
		mSeqID (inOrder),
		mPosition (0, 0, 0, 0),
//		mFixedH (false),
		mFixedV (false),
//		mBindH (false),
		mBindV (false),
		mAlignment (eAlign_None),
		mDraw (eDraw_Yes)
{
}


// ---------------------------------------------------------------------------
// ~SRObject								Destructor			   [protected]
// ---------------------------------------------------------------------------

SRObject::~SRObject (void)
{
}


// ---------------------------------------------------------------------------
// GetReportData													  [public]
// ---------------------------------------------------------------------------

SRReportData*
SRObject::GetReportData (void)
const
{
	return mReportData;
}


// ---------------------------------------------------------------------------
// GetReportWriter													  [public]
// ---------------------------------------------------------------------------

SRReportWriter*
SRObject::GetReportWriter (void)
const
{
	return mReportData->GetReportWriter();
}


// ---------------------------------------------------------------------------
// GetDataSource													  [public]
// ---------------------------------------------------------------------------

SRDataSource&
SRObject::GetDataSource (void)
const
{
	return mReportData->GetReportWriter()->GetDataSource();
}


// ---------------------------------------------------------------------------
// ComparePosition													  [public]
// ---------------------------------------------------------------------------

int
SRObject::ComparePosition (const SRObject *other)
const
{
	if (mPosition.top < other->mPosition.top)
		return -1;
	else if (mPosition.top > other->mPosition.top)
		return 1;
	else if (mPosition.left < other->mPosition.left)
		return -1;
	else if (mPosition.left > other->mPosition.left)
		return 1;
	return 0;
}


// ---------------------------------------------------------------------------
// GetPosition														  [public]
// ---------------------------------------------------------------------------

SRect
SRObject::GetPosition (void)
const
{
	return mPosition;
}


// ---------------------------------------------------------------------------
// CompareOrder														  [public]
// ---------------------------------------------------------------------------

int
SRObject::CompareOrder (const SRObject *other)
const
{
	if (mSeqID < other->mSeqID)
		return -1;
	else if (mSeqID > other->mSeqID)
		return 1;

	return 0;	// should not happen - seqID should be unique within a section...
}


// ---------------------------------------------------------------------------
// GetOrder															  [public]
// ---------------------------------------------------------------------------

long
SRObject::GetOrder (void)
const
{
	return mSeqID;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
SRObject::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	if (inNode != NULL)
		PSObject::LoadXML (inNode);

/*
	if (mAlignment != eAlign_None)
	{
		mPosition.right -= mPosition.left;
		mPosition.left = 0;
	}
*/

	return;
}


// ---------------------------------------------------------------------------
// GetVariableText													  [public]
// ---------------------------------------------------------------------------

RWTextValue
SRObject::GetVariableText (const CText inVariableName, const CText inFormat)
const
{
	RWValue		var;
	RWTextValue	result;

//	if (GetReportWriter()->GetVariable (inVariableName, SR4DVariable::SR4DVariable_Variable, var, ECalcType_CurrentValue))
	GetReportWriter()->GetVariable (inVariableName, var);
		result = GetReportWriter()->FormatVariable (var, inFormat);
//	else
//		result.Copy ("Unknown Variable");

	return result;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
SRObject::Reset (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FetchValue														  [public]
// ---------------------------------------------------------------------------

void
SRObject::FetchValue (bool inUseOld)
{
	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
SRObject::FetchCalcValue (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FindCalculatedObject												  [public]
// ---------------------------------------------------------------------------

const SRObject*
SRObject::FindCalculatedObject (const CText inName)
const
{
	return NULL;
}


// ---------------------------------------------------------------------------
// CreateCalculatedObject									 [static] [public]
// ---------------------------------------------------------------------------

SRObject*
SRObject::CreateCalculatedObject (SRReportData *inReport, long inOrder, const CText inName)
{
	SRVariable	*object = NULL;

	if (inName[0] == '[')
		object = SRField::Create (inReport, NULL, inOrder);
	else
		object = SRVariable::Create (inReport, NULL, inOrder);
	object->mDraw = eDraw_No;
	object->mSource = inName;

	return object;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRObject::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRect	pos (mPosition);
//	if (mAlignment != eAlign_None)
//		pos.right = pos.left;
	fprintf (fd, "<%s r=\"%s\"", inObjectType, (const char*) pos);
	if (not mName.IsEmpty())
	{
		fprintf (fd, " name=\"");
		RWTools::WriteText (fd, mName);
		fprintf (fd, "\"");
	}
	if (not mID.IsEmpty())
	{
		fprintf (fd, " id=\"");
		RWTools::WriteText (fd, mID);
		fprintf (fd, "\"");
	}
	//	if (mFixedH)
//		fprintf (fd, " fixH=\"1\"");
	if (mFixedV)
		fprintf (fd, " fixV=\"1\"");
//	if (mBindH)
//		fprintf (fd, " bindH=\"1\"");
	if (mBindV)
		fprintf (fd, " bindV=\"1\"");
	if (mAlignment != eAlign_None)
		fprintf (fd, " align=\"%d\"", (int) mAlignment);	// sAlignment [mAlignment]
	if (mDraw != eDraw_Yes)
		fprintf (fd, " draw=\"%d\"", (int) mDraw);			// sDraw [mDraw]
#if	TARGET_DEBUG
	fprintf (fd, " oid=\"%ld\"", mSeqID);
#endif

	return;
}


XMLElement*
SRObject::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
    XMLElement	* elem = inParent->GetDocument()->NewElement (inObjectType);
	SRect	pos (mPosition);
//	if (mAlignment != eAlign_None)
//		pos.right = pos.left;
	elem->SetAttribute ("r", (const char*) pos);
	if (not mName.IsEmpty())
	{
		CXMLText	name = mName.ToXML();
		elem->SetAttribute ("name", name.c_str());
		mName.FreeXML (name);
	}
	if (not mID.IsEmpty())
	{
		CXMLText	name = mID.ToXML();
		elem->SetAttribute ("id", name.c_str());
		mID.FreeXML (name);
	}
	//	if (mFixedH)
//		elem.SetAttribute ("fixH", 1);
	if (mFixedV)
		elem->SetAttribute ("fixV", 1);
//	if (mBindH)
//		elem.SetAttribute ("bindH", 1);
	if (mBindV)
		elem->SetAttribute ("bindV", 1);
	if (mAlignment != eAlign_None)
		elem->SetAttribute ("align", (int) mAlignment);		// sAlignment [mAlignment]
	if (mDraw != eDraw_Yes)
		elem->SetAttribute ("draw", (int) mDraw);			// sDraw [mDraw]
#if	TARGET_DEBUG
	elem->SetAttribute ("oid", mSeqID);
#endif
	
	XMLNode	*node = inParent->InsertEndChild (elem);

	return node->ToElement();
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRObject::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropName:			outValue.SetText (mName); break;	//mbs 15112010
		case PSObjPropID:			outValue.SetText (mID); break;		//mbs 15112010

		case PSObjPropOrder:		outValue.SetInteger (mSeqID); break;
		case PSObjPropRect:			outValue.SetXMLText ((const char*) mPosition); break;
//		case PSObjPropFixH:			outValue.SetBoolean (mFixedH); break;
		case PSObjPropFixV:			outValue.SetBoolean (mFixedV); break;
//		case PSObjPropBindH:		outValue.SetBoolean (mBindH); break;
		case PSObjPropBindV:		outValue.SetBoolean (mBindV); break;
		case PSObjPropAlign:		outValue.SetXMLText (sAlignment [mAlignment]);break;
		case PSObjPropDraw:			outValue.SetXMLText (sDraw [mDraw]);break;						
			
		case PSObjPropPosTop:		outValue.SetReal (mPosition.top); break;
		case PSObjPropPosLeft:		outValue.SetReal (mPosition.left); break;
		case PSObjPropPosBottom:	outValue.SetReal (mPosition.bottom); break;
		case PSObjPropPosRight:		outValue.SetReal (mPosition.right); break;
		case PSObjPropPosWidth:		outValue.SetReal (mPosition.Width()); break;
		case PSObjPropPosHeight:	outValue.SetReal (mPosition.Height()); break;

		default:					return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRObject::SetProperty (OSType id, RWValue &inValue)
{
	long	lVal;
	float	fVal;

	switch (id)
	{
		case PSObjPropName:			return GetReportWriter()->IsExport()? SetStringProperty (inValue, mName): false;	//mbs 15112010
		case PSObjPropID:			return GetReportWriter()->IsExport()? SetStringProperty (inValue, mID): false;		//mbs 15112010

		case PSObjPropOrder:		return SetIntegerProperty (inValue, mSeqID, 1);
		case PSObjPropRect:			return SetRectProperty (inValue, mPosition);
//		case PSObjPropFixH:			return SetBooleanProperty (inValue, mFixedH);
		case PSObjPropFixV:			return SetBooleanProperty (inValue, mFixedV);
//		case PSObjPropBindH:		return SetBooleanProperty (inValue, mBindH);
		case PSObjPropBindV:		return SetBooleanProperty (inValue, mBindV);
		case PSObjPropAlign:
			if ((lVal = SetListProperty (inValue, sAlignment)) >= 0 || (lVal = SetListProperty (inValue, sAlignment2)) >= 0)
			{
				mAlignment = EAlignment (lVal);
				return true;
			}
			break;
		case PSObjPropDraw:
			if ((lVal = SetListProperty (inValue, sDraw)) >= 0)
			{
				mDraw = EDraw (lVal);
				return true;
			}
			break;

		case PSObjPropPosTop:
			if (SetRealProperty (inValue, mPosition.top))
			{
				if (mPosition.top > mPosition.bottom)
					mPosition.bottom = mPosition.top;
				return true;
			}
			break;
		case PSObjPropPosLeft:
			if (SetRealProperty (inValue, mPosition.left))
			{
				if (mPosition.left > mPosition.right)
					mPosition.right = mPosition.left;
				return true;
			}
			break;
		case PSObjPropPosBottom:
			if (SetRealProperty (inValue, mPosition.bottom))
			{
				if (mPosition.top > mPosition.bottom)
					mPosition.bottom = mPosition.top;
				return true;
			}
			break;
		case PSObjPropPosRight:
			if (SetRealProperty (inValue, mPosition.right))
			{
				if (mPosition.left > mPosition.right)
					mPosition.right = mPosition.left;
				return true;
			}
			break;
		case PSObjPropPosWidth:
			if (SetRealProperty (inValue, fVal) && fVal >= 0)
			{
				mPosition.right = mPosition.left + fVal;
				return true;
			}
			break;
		case PSObjPropPosHeight:
			if (SetRealProperty (inValue, fVal) && fVal > 0)
			{
				mPosition.bottom = mPosition.top + fVal;
				return true;
			}
			break;

		default:
			return false;	// PSObject::SetProperty (id, inValue);		ignore XML property!
			break;
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRGroup*
SRGroup::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRGroup	*group = new SRGroup (inReport, inOrder);
	group->LoadXML (inNode);

	return group;
}


// ---------------------------------------------------------------------------
// SRGroup									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRGroup::SRGroup (SRReportData *inReport, long inOrder)
	:	SRObject (inReport, inOrder, eObject_Group),
//		mExpandH (false),
		mExpandV (false)
{
}


// ---------------------------------------------------------------------------
// ~SRGroup									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRGroup::~SRGroup (void)
{
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
SRGroup::Reset (void)
{
	SRObjListD::const_iterator	it;
	SRObject					*obj;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->Reset();
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchValue														  [public]
// ---------------------------------------------------------------------------

void
SRGroup::FetchValue (bool inUseOld)
{
	SRObjListD::const_iterator	it;
	SRObject					*obj;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->FetchValue (inUseOld);
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
SRGroup::FetchCalcValue (void)
{
	SRObjListD::const_iterator	it;
	SRObject					*obj;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->FetchCalcValue();
	}

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRGroup::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	SRObjListD::const_iterator	it;
	SRObject					*obj;

	WriteSelf (fd, "Group");
	
	fprintf (fd, ">\r\n");
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->Write (fd, inIsInBody, inUseCalculator);
	}
	fprintf (fd, "</Group>\r\n");
	
	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRGroup::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
	SRObjListD::const_iterator	it;
	SRObject					*obj;

    XMLElement	*me = WriteSelf (inParent, "Group");
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->Write (me, inIsInBody, inUseCalculator);
	}

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRGroup::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRObject::WriteSelf (fd, inObjectType);

//	if (mExpandH)
//		fprintf (fd, " expandH=\"1\"");
	if (mExpandV)
		fprintf (fd, " expandV=\"1\"");

	return;
}

XMLElement*
SRGroup::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);
//	if (mExpandH)
//		me->SetAttribute ("expandH", 1);
	if (mExpandV)
		me->SetAttribute ("expandV", 1);

	return me;
}


SRObjListD *
SRGroup::GetObjects (void)
{
	return &mObjects;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRGroup::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
//		case PSObjPropExpandH:		outValue.SetBoolean (mExpandH); break;
		case PSObjPropExpandV:		outValue.SetBoolean (mExpandV); break;
		case PSObjPropObjects:		outValue.SetInteger (mObjects.size()); break;

		default:					return SRObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRGroup::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
//		case PSObjPropExpandH:		return SetBooleanProperty (inValue, mExpandH);
		case PSObjPropExpandV:		return SetBooleanProperty (inValue, mExpandV);

		default:					return SRObject::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------
#if	0
PSObjList *
SRGroup::GetObjects (OSType id)
const
{
	PSObjList	*l = NULL;
	if (id == PSObjPropObjects)
	{
		l = new PSObjList (mObjects.size());
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			l->push_back (static_cast <PSObject*> (*it));
		}
	}
	return l;
}
#endif


// ---------------------------------------------------------------------------
// FindCalculatedObject												  [public]
// ---------------------------------------------------------------------------

const SRObject*
SRGroup::FindCalculatedObject (const CText inName)
const
{
	const SRObject				*obj = NULL;
	SRObjListD::const_iterator	it;
	
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = (*it)->FindCalculatedObject (inName);
		if (obj != NULL)
			return obj;
	}
	return NULL;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRLine*
SRLine::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRLine	*line = new SRLine (inReport, inOrder);
	line->LoadXML (inNode);

	return line;
}


// ---------------------------------------------------------------------------
// SRLine									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRLine::SRLine (SRReportData *inReport, long inOrder)
	:	SRObject (inReport, inOrder, eObject_Line),
		mThickness (1),
		mLineColor (cBlackColor),
		mFlags (RWLine_Horizontal)
{
}

// ---------------------------------------------------------------------------
// ~SRLine									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRLine::~SRLine (void)
{
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRLine::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	WriteSelf (fd, "Line");
	fprintf (fd, " />\r\n");

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRLine::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
    XMLElement	*me = WriteSelf (inParent, "Line");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRLine::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRObject::WriteSelf (fd, inObjectType);

	if (mThickness != 1)
		fprintf (fd, " thickness=\"%g\"", mThickness);
	if (mLineColor != cBlackColor)
		fprintf (fd, " lineColor=\"%s\"", (const char*) mLineColor);
	if (mFlags != 0)
		fprintf (fd, " flags=\"%d\"", mFlags);

	return;
}

XMLElement*
SRLine::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);
	if (mThickness != 1)
		me->SetAttribute ("thickness", mThickness);
	if (mLineColor != cBlackColor)
		me->SetAttribute ("lineColor", (const char*) mLineColor);
	if (mFlags != 0)
		me->SetAttribute ("flags", (int) mFlags);

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRLine::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropThickness:	outValue.SetReal (mThickness); break;
		case PSObjPropLineColor:	outValue.SetXMLText ((const char*) mLineColor); break;
		case PSObjPropFlags:		outValue.SetInteger (mFlags); break;

		default:					return SRObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRLine::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropThickness:		return SetRealProperty (inValue, mThickness, 0, 64);
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
			
		default:						return SRObject::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SROval*
SROval::Create (SRReportData *inReport,XMLElement  *inNode, long inOrder)
{
	SROval	*oval = new SROval (inReport, inOrder);
	oval->LoadXML (inNode);

	return oval;
}


// ---------------------------------------------------------------------------
// SROval									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SROval::SROval (SRReportData *inReport, long inOrder)
	:	SRLine (inReport, inOrder),
		mFill (false),
		mFillColor (cBlackColor)
{
	mObjectKind = eObject_Oval;
}

// ---------------------------------------------------------------------------
// ~SROval									Destructor			   [protected]
// ---------------------------------------------------------------------------

SROval::~SROval (void)
{
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SROval::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	WriteSelf (fd, "Oval");
	fprintf (fd, " />\r\n");

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SROval::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
    XMLElement	*me = WriteSelf (inParent, "Oval");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SROval::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRObject::WriteSelf (fd, inObjectType);

	if (mThickness != 1)
		fprintf (fd, " thickness=\"%g\"", mThickness);
	if (mLineColor != cBlackColor)
		fprintf (fd, " lineColor=\"%s\"", (const char*) mLineColor);
	if (mFill /* && mFillColor != cWhiteColor */)
		fprintf (fd, " fillColor=\"%s\"", (const char*) mFillColor);

	return;
}

XMLElement*
SROval::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);

	if (mThickness != 1)
		me->SetAttribute ("thickness", mThickness);
	if (mLineColor != cBlackColor)
		me->SetAttribute ("lineColor", (const char*) mLineColor);
	if (mFill /* && mFillColor != cWhiteColor */)
		me->SetAttribute ("fillColor", (const char*) mFillColor);

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SROval::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFlags:		return false;

		case PSObjPropFill:			outValue.SetInteger (mFill); break;
		case PSObjPropFillColor:	outValue.SetXMLText ((const char*) mFillColor); break;

		default:					return SRLine::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SROval::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFlags:		break;

		case PSObjPropFill:			return SetBooleanProperty (inValue, mFill);
		case PSObjPropFillColor:	return SetColorProperty (inValue, mFillColor);

		default:					return SRLine::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRRect*
SRRect::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRRect	*rect = new SRRect (inReport, inOrder);
	rect->LoadXML (inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// SRRect									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRRect::SRRect (SRReportData *inReport, long inOrder)
	:	SROval (inReport, inOrder),
		mRows (1),
		mCols (1),
		mFlags (RWRect_Full)
{
	mObjectKind = eObject_Rect;
}

// ---------------------------------------------------------------------------
// ~SRRect									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRRect::~SRRect (void)
{
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRRect::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	WriteSelf (fd, "Rect");
	fprintf (fd, " />\r\n");

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRRect::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
    XMLElement	*me = WriteSelf (inParent, "Rect");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRRect::WriteSelf (FILE *fd, const char* inObjectType)
{
	SROval::WriteSelf (fd, inObjectType);
	if (mRows != 1)
		fprintf (fd, " rows=\"%ld\"", mRows);
	if (mCols != 1)
		fprintf (fd, " cols=\"%ld\"", mCols);
	if (mFlags != RWRect_Full)
		fprintf (fd, " flags=\"%d\"", mFlags);

	return;
}

XMLElement*
SRRect::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
    XMLElement	*me = SROval::WriteSelf (inParent, inObjectType);

	if (mRows != 1)
		me->SetAttribute ("rows", (int)mRows);
	if (mCols != 1)
		me->SetAttribute ("cols", (int)mCols);
	if (mFlags != RWRect_Full)
		me->SetAttribute ("flags", mFlags);

	return me;
}




// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRRect::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropRows:			outValue.SetInteger (mRows); break;
		case PSObjPropCols:			outValue.SetInteger (mCols); break;
		case PSObjPropFlags:		outValue.SetInteger (mFlags); break;

		default:					return SROval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRRect::SetProperty (OSType id, RWValue &inValue)
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

		default:					return SROval::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRPict*
SRPict::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRPict	*pict = new SRPict (inReport, inOrder);
	pict->LoadXML (inNode);

	return pict;
}


// ---------------------------------------------------------------------------
// SRPict									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRPict::SRPict (SRReportData *inReport, long inOrder)
	:	SROval (inReport, inOrder),
//		mExpandH (false),
		mExpandV (false),
		mFormat (ePictFormat_Normal),
		mFrame (false),
		mFrameOffset (2),
		mDataID (0),
        mObjectRotation (0)
{
	mObjectKind = eObject_Pict;
//	mPicture.Init();
}

// ---------------------------------------------------------------------------
// ~SRPict									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRPict::~SRPict (void)
{
	mPicture.Free();
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
SRPict::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	SROval::LoadXML (inNode);

	if (inNode)
	{
        XMLElement	*elem = inNode->FirstChildElement (sPictDataProperties [0]);
		if (elem)
		{
			int				kind = RWValue::eValue_BLOB;
			const CXMLText	fmt = elem->Attribute (sPictDataProperties [1]);
			if (!fmt.empty())
			{
				long	lVal = RWTools::FindInList (fmt, RWValue::GetPictFormats());
				if (lVal >= 0)
					kind = RWValue::EValue_Kind (RWValue::eValue_BLOB + lVal);
			}
			SBlob	pictData;
			pictData.Init();
			RWTools::ReadData (elem, pictData);
			mPicture.SetPicture (RWValue::EValue_Kind (kind), pictData, true);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

#if 0
void
SRPict::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	WriteSelf (fd, "Pict");
	fprintf (fd, " />\r\n");

	return;
}
#endif

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRPict::Write ( XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
    XMLElement	*me = WriteSelf (inParent, "Pict");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

#if 0
void
SRPict::WriteSelf (FILE *fd, const char* inObjectType)
{
	if (mDataID == 0 && mPicture.GetKind() >= RWValue::eValue_PictRefScreen)
	{
		mDataID = GetDataSource().EmitPicture (mPicture);
		mPicture.Free();
	}

	SROval::WriteSelf (fd, inObjectType);
//	if (mExpandH)
//		fprintf (fd, " expandH=\"1\"");
	if (mExpandV)
		fprintf (fd, " expandV=\"1\"");
	if (mDataID != 0)
		fprintf (fd, " dataID=\"%lu\"", mDataID);
	if (mFormat != 0)
		fprintf (fd, " format=\"%d\"", mFormat);
    if (mObjectRotation != 0)
        fprintf (fd, " rotation=\"%g\"", mObjectRotation);
    if (mFillColor != cEmptyColor)
        fprintf (fd, " fillColor=\"%s\"", (const char*) mFillColor);

	if (mFrame)
	{
		fprintf (fd, " frame=\"1\"");
		if (mFrameOffset != 2)
			fprintf (fd, " frameOffset=\"%g\"", mFrameOffset);
		if (mThickness != 1)
			fprintf (fd, " frameThickness=\"%g\"", mThickness);
		if (mLineColor != cBlackColor)
			fprintf (fd, " frameColor=\"%s\"", (const char*) mLineColor);
	}
	return;
}
#endif

XMLElement*
SRPict::WriteSelf (XMLElement *inParent, const char* inObjectType)
{
	if (mDataID == 0 && mPicture.GetKind() >= RWValue::eValue_PictRefScreen)
	{
		mDataID = GetDataSource().EmitPicture (mPicture);
		mPicture.Free();
	}

    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);
//	if (mExpandH)
//		me->SetAttribute ("expandH", 1);
	if (mExpandV)
		me->SetAttribute ("expandV", 1);
	if (mDataID != 0)
		me->SetAttribute ("dataID", (int)mDataID);
	if (mFormat != 0)
		me->SetAttribute ("format", mFormat);
    if (mObjectRotation != 0)
        me->SetAttribute ("rotation", mObjectRotation);
    if (mFillColor != cEmptyColor )
        me->SetAttribute ("fillColor", (const char*) mFillColor);

	if (mFrame)
	{
		me->SetAttribute ("frame", 1);
		if (mFrameOffset != 2)
			me->SetAttribute ("frameOffset", mFrameOffset);
		if (mThickness != 1)
			me->SetAttribute ("frameThickness", mThickness);
		if (mLineColor != cBlackColor)
			me->SetAttribute ("frameColor", (const char*) mLineColor);
	}

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRPict::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	outValue.SetReal (mThickness); break;	// same as PSObjPropThickness in SROval
		case PSObjPropFrameColor:		outValue.SetXMLText ((const char*) mLineColor); break;	// same as PSObjPropLineColor in SROval

//		case PSObjPropExpandH:			outValue.SetBoolean (mExpandH); break;
		case PSObjPropExpandV:			outValue.SetBoolean (mExpandV); break;
		case PSObjPropFormat:			outValue.SetInteger (mFormat); break;
		case PSObjPropFrame:			outValue.SetBoolean (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropData:				outValue.Attach (mPicture); break;
        case PSObjPropObjectRotation:   outValue.SetReal (mObjectRotation); break;
        case PSObjPropBackColor:        outValue.SetXMLText ((const char*) mFillColor); break;

		default:						return SROval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRPict::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	return SROval::SetProperty (PSObjPropThickness, inValue);	// same as PSObjPropThickness in SROval
		case PSObjPropFrameColor:		return SROval::SetProperty (PSObjPropLineColor, inValue);	// same as PSObjPropLineColor in SROval
//		case PSObjPropExpandH:			return SetBooleanProperty (inValue, mExpandH);
		case PSObjPropExpandV:			return SetBooleanProperty (inValue, mExpandV);
		case PSObjPropFormat:
		{
			long	lVal;
			if ((lVal = SetListProperty (inValue, sPictFormat)) >= 0)
			{
				mFormat = EPictFormat (lVal);
				return true;
			}
			break;
		}
		case PSObjPropFrame:			return SetBooleanProperty (inValue, mFrame);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 256);
		case PSObjPropData:
			if (inValue.GetKind() >= RWValue::eValue_PictRefScreen)
			{
				mPicture.Clone (inValue);
				return true;
			}
			break;
        case PSObjPropObjectRotation:   return SetRealProperty (inValue, mObjectRotation, -360, 360);
        case PSObjPropBackColor:        return SetColorProperty (inValue, mFillColor);

		default:						return SROval::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-


// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRText*
SRText::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRText	*text = new SRText (inReport, inOrder);
	text->LoadXML (inNode);

	return text;
}


// ---------------------------------------------------------------------------
// SRText									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRText::SRText (SRReportData *inReport, long inOrder)
	:	SROval (inReport, inOrder),
		mStyleID (0),
//		mExpandH (false),
		mExpandV (false),
		mIsDynamic (false),
		mIsAttributed (false),
		mKeepTogether (false),
		mDrawIfEmpty (eEmpty_Draw),
//		mDrawIfEmpty (true),
		mFrame (false),
		mFrameOffset (2)
{
	mObjectKind = eObject_Text;
}


// ---------------------------------------------------------------------------
// SRText										Constructor		[protected]
// ---------------------------------------------------------------------------

//SRText::SRText ()
//	:	SROval (),
//	mStyleID (0),
//	//		mExpandH (false),
//	mExpandV (false),
//	mIsDynamic (false),
//	mIsAttributed (false),
//	mKeepTogether (false),
//	mDrawIfEmpty (eEmpty_Draw),
//	//		mDrawIfEmpty (true),
//	mFrame (false),
//	mFrameOffset (2)
//{
//	mObjectKind = eObject_Text;
//}

// ---------------------------------------------------------------------------
// ~SRText									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRText::~SRText (void)
{
	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
SRText::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	SROval::LoadXML (inNode);

	//mbs 07052010	support attributed text
	if (	mIsDynamic
		&&	(	mText.IsEmpty()
			 || (mIsAttributed &&  TEXT_STR (mText, "&lt;%") == STR_NOTFOUND)
			 || (not mIsAttributed && TEXT_STR (mText, "<%") == STR_NOTFOUND)
			 )
		)
		mIsDynamic = false;

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

#if 0
void
SRText::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	RWTextValue	text;
	bool		oldDynamic = mIsDynamic;
	if (mIsDynamic)
		text = ParseText (mIsDynamic);
	else
		text = LocalizeText();

	WriteSelf (fd, "Text");

	if (text && *text)
	{
		fprintf (fd, ">");
		RWTools::WriteText (fd, text);
		fprintf (fd, "</Text>\r\n");
	}
	else
		fprintf (fd, " />\r\n");

	mIsDynamic = oldDynamic;
	if (mIsDynamic)
		text.Free();
	else
		mText.Attach (text.Detach());	// no need to copy...
	
	
	return;
}
#endif

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRText::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
	RWTextValue		text;
	bool			oldDynamic = mIsDynamic;

	if (mIsDynamic)
		text = ParseText (mIsDynamic);
	else
		text = LocalizeText();


    XMLElement	*me = WriteSelf (inParent, "Text");

	if (!text.IsEmpty())
		RWTools::WriteText (me, text);

	mIsDynamic = oldDynamic;
	if (mIsDynamic)
		text.Free();
	else
		mText.Attach (text.Detach());	// no need to copy...

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

#if 0
void
SRText::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRObject::WriteSelf (fd, inObjectType);

	if (mStyleID != 0)
		fprintf (fd, " style=\"%ld\"", mStyleID);
//	if (mExpandH)
//		fprintf (fd, " expandH=\"1\"");
	if (mExpandV)
		fprintf (fd, " expandV=\"1\"");
	if (mIsDynamic)
		fprintf (fd, " dynamic=\"1\"");
	if (mIsAttributed)
		fprintf (fd, " attributed=\"1\"");
	if (mKeepTogether)
		fprintf (fd, " keepTogether=\"1\"");
	if (mDrawIfEmpty != eEmpty_Draw)
		fprintf (fd, " empty=\"%d\"", (int) mDrawIfEmpty);	// sEmpty [mDrawIfEmpty]
	if (mFrame)
	{
		fprintf (fd, " frame=\"1\"");
		if (mFrameOffset != 2)
			fprintf (fd, " frameOffset=\"%g\"", mFrameOffset);
		if (mThickness != 1)
			fprintf (fd, " frameThickness=\"%g\"", mThickness);
		if (mLineColor != cBlackColor)
			fprintf (fd, " frameColor=\"%s\"", (const char*) mLineColor);
	}

	return;
}
#endif

XMLElement*
SRText::WriteSelf (XMLElement *inParent, const char *inObjectType)
{
    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);

	if (mStyleID != 0)
		me->SetAttribute ("style", (int)mStyleID);
//	if (mExpandH)
//		me->SetAttribute ("expandH", 1);
	if (mExpandV)
		me->SetAttribute ("expandV", 1);
	if (mIsDynamic)
		me->SetAttribute ("dynamic", 1);
	if (mIsAttributed)
		me->SetAttribute ("attributed", 1);
	if (mKeepTogether)
		me->SetAttribute ("keepTogether", 1);
	if (mDrawIfEmpty != eEmpty_Draw)
		me->SetAttribute ("empty", (int) mDrawIfEmpty);		// sEmpty [mDrawIfEmpty]
	if (mFrame)
	{
		me->SetAttribute ("frame", 1);
		if (mFrameOffset != 2)
			me->SetAttribute ("frameOffset", mFrameOffset);
		if (mThickness != 1)
			me->SetAttribute ("frameThickness", mThickness);
		if (mLineColor != cBlackColor)
			me->SetAttribute ("frameColor", (const char*) mLineColor);
	}

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRText::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	outValue.SetReal (mThickness); break;	// same as PSObjPropThickness in SROval
		case PSObjPropFrameColor:		outValue.SetXMLText ((const char*) mLineColor); break;	// same as PSObjPropLineColor in SROval

		case PSObjPropStyle:			outValue.SetInteger (mStyleID); break;
//		case PSObjPropExpandH:			outValue.SetBoolean (mExpandH); break;
		case PSObjPropExpandV:			outValue.SetBoolean (mExpandV); break;
		case PSObjPropDynamic:			outValue.SetBoolean (mIsDynamic); break;
		case PSObjPropAttributed:		outValue.SetBoolean (mIsAttributed); break;
		case PSObjPropKeepTogether:		outValue.SetBoolean (mKeepTogether); break;
		case PSObjPropDrawEmpty:		outValue.SetInteger (mDrawIfEmpty); break;
		case PSObjPropFrame:			outValue.SetBoolean (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropData:				outValue.SetText (mText); break;

		default:						return SROval::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRText::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFrameThickness:	return SROval::SetProperty (PSObjPropThickness, inValue);	// same as PSObjPropThickness in SROval
		case PSObjPropFrameColor:		return SROval::SetProperty (PSObjPropLineColor, inValue);	// same as PSObjPropLineColor in SROval
//		case PSObjPropExpandH:			return SetBooleanProperty (inValue, mExpandH);
		case PSObjPropExpandV:			return SetBooleanProperty (inValue, mExpandV);
		case PSObjPropDynamic:			return SetBooleanProperty (inValue, mIsDynamic);
		case PSObjPropAttributed:		return SetBooleanProperty (inValue, mIsAttributed);
		case PSObjPropKeepTogether:		return SetBooleanProperty (inValue, mKeepTogether);
		case PSObjPropDrawEmpty:		return SetIntegerProperty (inValue, mDrawIfEmpty, eEmpty_Draw, eEmpty_RemoveRow);
		case PSObjPropFrame:			return SetBooleanProperty (inValue, mFrame);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 256);
		case PSObjPropData:				return SetStringProperty (inValue, mText);

			// pB special handling of style properties
		case PSObjPropStyle:
		{
			if (SetIntegerProperty (inValue, mStyleID, 0))
			{
				RWStyleList * styles = GetReportData()->GetStyles();
				RWStyle	*style = new RWStyle (styles, NULL);
				if (mStyleID > GetReportData()->GetLastStyle())	//mbs 20052011
					mStyleID = 0;
				style->Clear (mStyleID);
				mStyleID = style->GetID();
				styles->insert (pair<long,RWStyle*> (style->GetID (), style));
				return true;
			}
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
			RWStyleList * styles = GetReportData()->GetStyles();
			RWStyle* style = styles->FindStyle(mStyleID);
			return style->SetProperty (id, inValue);
		}
			// pB
			
		default:						return SROval::SetProperty (id, inValue);
	}

	return false;
}

// ---------------------------------------------------------------------------
// LocalizeText													   [protected]
// ---------------------------------------------------------------------------
// :resource_num,resource_id	¥¥¥TODO¥¥¥	PA_LocaliseStringByID()
// :xliff:resource_name		¥¥¥TODO¥¥¥	PA_LocaliseString()


RWTextValue
SRText::LocalizeText (void)
const
{
	RWTextValue	text;
	
	if (not mText.IsEmpty())
	{
		long	textLen = mText.StrLength();
		CText	localized (mText, textLen);
		
		(void) RWTools::ParseTextForXLIFF (mText, textLen, localized);
		text.Attach (localized.c_str());
	}
	
	return text;
}

// ---------------------------------------------------------------------------
// ParseText													   [protected]
// ---------------------------------------------------------------------------
// <% report_variable [ ; format ] %>
// <% variable [ ; format ] %>

RWTextValue
SRText::ParseText (bool& outStillDynamic)
const
{
	outStillDynamic = false;	//mbs 30122009
	RWTextValue	text;

	//mbs 30042010	support attributed text
	if (not mText.IsEmpty())
	{
		long	textLen = mText.StrLength();
		long	curPos = 0, delta = 0, endPos;

		CText	result (mText, textLen);
		CText   varName;
		CText   format;
	
		while (curPos < textLen && RWTools::ParseTextForVar (mIsAttributed, mText, textLen, curPos, endPos, varName, format))
		{
			if (GetReportWriter()->IsRWReportVariable (varName))
			{
				varName.clear();
				format.clear();
				outStillDynamic = true;
				curPos = endPos;
			}
			else
			{
				const CText	varname = varName;
				bool		encode = mIsAttributed;
				if (!varname.empty() && varname[0] == '+')
				{
                    varname.erase(0, 1);;
					encode = false;
				}
				RWTextValue	varText;
				if (!varname.empty())
					varText = GetVariableText (varname, format;
				varName.clear();
				format.clear();
				size_t	varLen;
				if (varText)
					varLen = varText.StrLength();
				else
					varLen = 0;
				result.erase (curPos - delta, endPos - curPos);
				if (varLen > 0)
				{
					if (encode)
					{
						CXMLText	encoded;
						CText	    us (varText, varLen);
						TiXmlBase::PutString ((const char*) us.GetUTF8(), &encoded);
						us.AssignUTF8 ((const UTF8Char*) encoded.c_str(), encoded.length());
						result.Insert (curPos - delta, us);
						varLen = us.StrLength();
					}
					else
						result.Insert (curPos - delta, varText, varLen);
				}
				delta += endPos - curPos - varLen;
				curPos = endPos;
			}
		}
		text.Attach (result.Release());
	}

	return text;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRVariable*
SRVariable::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRVariable	*var = new SRVariable (inReport, inOrder);
	var->LoadXML (inNode);

	if (var->mSource)
		var->mVar = var->GetReportWriter()->CreateVariable (var->mSource, var->mIndex, var->mCalcType);

	return var;
}


// ---------------------------------------------------------------------------
// SRVariable								Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRVariable::SRVariable (SRReportData *inReport, long inOrder)
	:	SRText (inReport, inOrder),
		mIndex (SR4DVariable::SR4DVariable_Variable),
//		mCalcShow (false),
		mCalcType (ECalcType_None),
//		mRecordCalcInto (0),
		mRepeat (eRepeat_None),
		mRepeatOffset (0),
//		mScript (0),
		mVar (0),
		mDataID (0)
{
	mObjectKind = eObject_Var;
}

// ---------------------------------------------------------------------------
// ~SRVariable								Destructor			   [protected]
// ---------------------------------------------------------------------------

SRVariable::~SRVariable (void)
{
//	if (mScript)
//		free (mScript);
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
SRVariable::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	SRText::LoadXML (inNode);

	mIsDynamic = false;
	if (mRepeat != eRepeat_None)
		mIndex = SR4DVariable::SR4DVariable_ArrayAuto;

	return;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
SRVariable::Reset (void)
{
	if (mRepeat != eRepeat_None)
		mDataID = 0;

	return;
}


// ---------------------------------------------------------------------------
// FetchValue														  [public]
// ---------------------------------------------------------------------------

void
SRVariable::FetchValue (bool inUseOld)
{
	if (mScript != NULL)
		GetDataSource().RunScript (mScript, this);

//	if (mCalcShow)
	if (mCalcType != ECalcType_None)
		;	// GetReportWriter()->GetVariable (mSource, mIndex, mValue, mCalcType);
	else
	{
		GetReportWriter()->GetVariable (mSource, mIndex, mValue, (inUseOld  &&  (mIndex == SR4DVariable::SR4DVariable_ArrayAuto)) ? ECalcType_OldValue : ECalcType_CurrentValue);
		// clear "cache" if needed
		if (mDataID != 0 && mVar != NULL && mVar->IsChanged())
			mDataID = 0;
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
SRVariable::FetchCalcValue (void)
{
//	if (mCalcShow)
	if (mCalcType != ECalcType_None)
		GetReportWriter()->GetVariable (mSource, mIndex, mValue, mCalcType);
//	else
//		GetReportWriter()->GetVariable (mSource, mIndex, mValue, inUseOld ? ECalcType_OldValue : ECalcType_CurrentValue);
	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRVariable::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	
	if (mRepeat != eRepeat_None)
	{
		// output a Table object
		if (mDataID == 0)
			mDataID = GetDataSource().EmitRepeating (mVar, mRepeat == eRepeat_Horizontally);
		SRObject::WriteSelf (fd, "Table");
		fprintf (fd, " dataID=\"%lu\"", mDataID);
		if (not mFrame)
			fprintf (fd, " frame=\"0\"");
		if (mRepeat == eRepeat_Horizontally)
		{
			fprintf (fd, " cols=\"%ld\">\r\n", mVar->GetSize());
			fprintf (fd, "<Columns height=\"%g\">\r\n", mPosition.Height());
			fprintf (fd, "<col id=\"0\" grid=\"0\" width=\"%g\"", mPosition.Width() + mRepeatOffset);
		}
		else
		{
			fprintf (fd, " cols=\"1\">\r\n");
			fprintf (fd, "<Columns height=\"%g\">\r\n", mPosition.Height() + mRepeatOffset);
			fprintf (fd, "<col id=\"0\" grid=\"0\" width=\"%g\"", mPosition.Width());
		}
		if (not mFormat.IsEmpty())
		{
			CXMLText		name = mFormat.ToXML();
			TIXML_STRING	tsname (name);
			mFormat.FreeXML (name);
			TIXML_STRING	encoded;
			TiXmlBase::PutString (tsname, &encoded);
			fprintf (fd, " format=\"%s\"", encoded.c_str());
		}
		if (mStyleID != 0)
			fprintf (fd, " style=\"%ld\"", mStyleID);
		fprintf (fd, " />\r\n");
		fprintf (fd, "</Columns>\r\n");
		fprintf (fd, "</Table>\r\n");
	}
	else if (GetReportWriter()->IsRWReportVariable (mSource))
	{
		WriteSelf (fd, "Var");
		fprintf (fd, " source=\"");
		RWTools::WriteText (fd, mSource);
		fputc ('"', fd);
		if (not mFormat.IsEmpty())
		{
			fprintf (fd, " format=\"");
			RWTools::WriteText (fd, mFormat);
			fputc ('"', fd);
		}
		fprintf (fd, " />\r\n");
	}
	else
	{
		const char*	type;

		if (mCalcType != ECalcType_None)
		{
			if (inUseCalculator)
			{
				WriteSelf (fd, type = "Var");
				fprintf (fd, " source=\"");
				RWTools::WriteText (fd, mSource);
				fprintf (fd, "\" calc=\"%ld\"", long (mCalcType));
				if (not mFormat.IsEmpty() && mValue.GetKind() < RWValue::eValue_PictRefScreen)
				{
					fprintf (fd, " format=\"");
					RWTools::WriteText (fd, mFormat);
					fputc ('"', fd);
				}
			}
			else
				WriteSelf (fd, type = "Text");
		}
		else if (mValue.GetKind() >= RWValue::eValue_PictRefScreen)
		{
			if (mDataID == 0)	// || (mVar != NULL && mVar->IsChanged()))
				mDataID = GetDataSource().EmitPicture (mValue);
			SRObject::WriteSelf (fd, type = "Pict");
			if (mDataID != 0)
				fprintf (fd, " dataID=\"%lu\"", mDataID);
			if (not mFormat.IsEmpty())
			{
				CXMLText		name = mFormat.ToXML();
				TIXML_STRING	tsname (name);
				mFormat.FreeXML (name);
				TIXML_STRING	encoded;
				TiXmlBase::PutString (tsname, &encoded);
				fprintf (fd, " format=\"%s\"", encoded.c_str());
			}
			if (mFrame)
				fprintf (fd, " frame=\"1\"");
//			if (mExpandH)
//				fprintf (fd, " expandH=\"1\"");
			if (mExpandV)
				fprintf (fd, " expandV=\"1\"");
		}
		else
			WriteSelf (fd, type = "Text");

		if (inIsInBody && inUseCalculator && GetReportWriter()->IsCalculatedVariable (mSource))
		{
			fprintf (fd, " var=\"");
			RWTools::WriteText (fd, mSource);
			fputc ('"', fd);

			switch (mValue.GetKind())
			{
				case RWValue::eValue_Boolean:
				case RWValue::eValue_Integer:
				case RWValue::eValue_DateTime:
				case RWValue::eValue_Date:
				case RWValue::eValue_Time:
					fprintf (fd, " val=\"%ld\"", mValue.GetInteger());
					break;

				case RWValue::eValue_Real:
					fprintf (fd, " val=\"%.15lg\"", mValue.GetReal());
					break;

				default:	// to shut up compiler - calculated value can't be of any other kind...
					break;
			}
		}

		if (mValue.GetKind() < RWValue::eValue_PictRefScreen)
		{
			mText.Free();
			mText = GetReportWriter()->FormatVariable (mValue, mFormat);

			if (not mText.IsEmpty())
			{
				fprintf (fd, ">");
				RWTools::WriteText (fd, mText);
				fprintf (fd, "</%s>\r\n", type);
			}
			else
				fprintf (fd, " />\r\n");
		}
		else
			fprintf (fd, " />\r\n");
	}

	
	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRVariable::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
    XMLElement	*me = NULL;
	CXMLText		xml;

	if (mRepeat != eRepeat_None)
	{
		// output a Table object
		if (mDataID == 0)
			mDataID = GetDataSource().EmitRepeating (mVar, mRepeat == eRepeat_Horizontally);
		me = SRObject::WriteSelf (inParent, "Table");
		me->SetAttribute ("dataID", mDataID);
		if (not mFrame)
			me->SetAttribute ("frame", 0);
		me->SetAttribute ("cols", mRepeat == eRepeat_Horizontally ? mVar->GetSize() : 1);

        XMLElement	elem ("col");
		elem.SetAttribute ("id", 0);
		elem.SetAttribute ("grid", 0);
		elem.SetAttribute ("width", mRepeat == eRepeat_Horizontally ? mPosition.Width() + mRepeatOffset : mPosition.Width());
		if (not mFormat.IsEmpty())
		{
			xml = mFormat.ToXML();
			elem.SetAttribute ("format", xml);
			mFormat.FreeXML (xml);
		}
		if (mStyleID != 0)
			elem.SetAttribute ("style", mStyleID);
        XMLElement columns ("Columns");
		columns.SetAttribute ("height", mRepeat == eRepeat_Horizontally ? mPosition.Height() : mPosition.Height() + mRepeatOffset);
		me->InsertEndChild (columns)->InsertEndChild (elem);
	}
	else if (GetReportWriter()->IsRWReportVariable (mSource))
	{
		me = WriteSelf (inParent, "Var");
		xml = mSource.ToXML();
		me->SetAttribute ("source", xml);
		mSource.FreeXML (xml);
		if (not mFormat.IsEmpty())
		{
			xml = mFormat.ToXML();
			me->SetAttribute ("format", xml);
			mFormat.FreeXML (xml);
		}
	}
	else
	{
		if (mCalcType != ECalcType_None)
		{
			if (inUseCalculator)
			{
				me = WriteSelf (inParent, "Var");
				xml = mSource.ToXML();
				me->SetAttribute ("source", xml);
				mSource.FreeXML (xml);
				me->SetAttribute ("calc", long (mCalcType));
				if (not mFormat.IsEmpty() && mValue.GetKind() < RWValue::eValue_PictRefScreen)
				{
					xml = mFormat.ToXML();
					me->SetAttribute ("format", xml);
					mFormat.FreeXML (xml);
				}
			}
			else
				me = WriteSelf (inParent, "Text");
		}
		else if (mValue.GetKind() >= RWValue::eValue_PictRefScreen)
		{
			if (mDataID == 0)	// || (mVar != NULL && mVar->IsChanged()))
				mDataID = GetDataSource().EmitPicture (mValue);
			me = SRObject::WriteSelf (inParent, "Pict");
			if (mDataID != 0)
				me->SetAttribute ("dataID", mDataID);
			if (not mFormat.IsEmpty())
			{
				xml = mFormat.ToXML();
				me->SetAttribute ("format", xml);
				mFormat.FreeXML (xml);
			}
			if (mFrame)
				me->SetAttribute ("frame", 1);
//			if (mExpandH)
//				me->SetAttribute ("expandH", 1);
			if (mExpandV)
				me->SetAttribute ("expandV", 1);
		}
		else
			me = WriteSelf (inParent, "Text");

		if (inIsInBody && inUseCalculator && GetReportWriter()->IsCalculatedVariable (mSource))
		{
			xml = mSource.ToXML();
			me->SetAttribute ("var", xml);
			mSource.FreeXML (xml);
			switch (mValue.GetKind())
			{
				case RWValue::eValue_Boolean:
				case RWValue::eValue_Integer:
				case RWValue::eValue_DateTime:
				case RWValue::eValue_Date:
				case RWValue::eValue_Time:
					me->SetAttribute ("val", mValue.GetInteger());
					break;

				case RWValue::eValue_Real:
					me->SetAttribute ("val", mValue.GetReal());
					break;

				default:	// to shut up compiler - calculated value can't be of any other kind...
					break;
			}
		}

		if (mValue.GetKind() < RWValue::eValue_PictRefScreen)
		{
			mText.Free();
			if ((mValue.GetKind() == RWValue::eValue_Boolean) && mFormat.IsEmpty() && !mDraw) {
				RWTextValue buf("True;");
				mText = GetReportWriter()->FormatVariable (mValue, buf);				
			} else {
				mText = GetReportWriter()->FormatVariable (mValue, mFormat);
			}
			if (not mText.IsEmpty())
				RWTools::WriteText (me, mText);
		}
	}

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRVariable::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropDynamic:			return false;

		case PSObjPropSource:			outValue.SetText (mSource); break;
		case PSObjPropFormat:			outValue.SetText (mFormat); break;
		case PSObjPropElement:			outValue.SetInteger (mIndex); break;
		case PSObjPropCalcType:			outValue.SetInteger (long (mCalcType)); break;
		case PSObjPropRepeat:			outValue.SetXMLText (sRepeat [mRepeat]);break;						
		case PSObjPropRepeatOffset:		outValue.SetReal (mRepeatOffset); break;
		case PSObjPropScript:			outValue.SetText (mScript); break;

		default:						return SRText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRVariable::SetProperty (OSType id, RWValue &inValue)
{
	long	lVal;
	switch (id)
	{
		case PSObjPropDynamic:		break;
		case PSObjPropData:			break;

		case PSObjPropSource:		return SetStringProperty (inValue, mSource);
		case PSObjPropFormat:		return SetStringProperty (inValue, mFormat);
		case PSObjPropElement:		return SetIntegerProperty (inValue, mIndex, SR4DVariable::SR4DVariable_Variable);
		case PSObjPropCalcType:
			if ((lVal = SetListProperty (inValue, sCalcType)) >= 0)
			{
				mCalcType = ECalcType (lVal);
				return true;
			}
			break;
		case PSObjPropRepeat:
			if ((lVal = SetListProperty (inValue, sRepeat)) >= 0 || (lVal = SetListProperty (inValue, sRepeat2)) >= 0)
			{
				mRepeat = ERepeat (lVal);
				return true;
			}
			break;
		case PSObjPropRepeatOffset:	return SetRealProperty (inValue, mRepeatOffset, 0, 1024);
		case PSObjPropScript:		return SetStringProperty (inValue, mScript);

		default:					return SRText::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// FindCalculatedObject												  [public]
// ---------------------------------------------------------------------------

const SRObject*
SRVariable::FindCalculatedObject (const CText inName)
const
{
	SConstText	name (inName);
	if (mCalcType == ECalcType_None && name == SConstText (mSource))
		return this;
	return NULL;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRField*
SRField::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
{
	SRField	*field = new SRField (inReport, inOrder);
	field->LoadXML (inNode);

	if (field->mSource)
		field->mVar = field->GetReportWriter()->CreateField (field->mSource, field->mCalcType);

	return field;
}


// ---------------------------------------------------------------------------
// SRField									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRField::SRField (SRReportData *inReport, long inOrder)
	:	SRVariable (inReport, inOrder)
{
	mObjectKind = eObject_Fld;
	return;
}

// ---------------------------------------------------------------------------
// ~SRField									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRField::~SRField (void)
{
}


// ---------------------------------------------------------------------------
// FetchValue														  [public]
// ---------------------------------------------------------------------------

void
SRField::FetchValue (bool inUseOld)
{
	if (mScript != NULL)
		GetDataSource().RunScript (mScript, this);

//	if (mCalcShow)
	if (mCalcType != ECalcType_None)
		;	// GetReportWriter()->GetField (mSource, mValue, mCalcType);
	else
	{
		GetReportWriter()->GetField (mSource, mValue, (inUseOld  &&  mScript.IsEmpty()) ? ECalcType_OldValue : ECalcType_CurrentValue);
		// clear "cache" if needed
		if (mDataID != 0 && mVar != NULL && mVar->IsChanged())
			mDataID = 0;
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
SRField::FetchCalcValue (void)
{
//	if (mCalcShow)
	if (mCalcType != ECalcType_None)
		GetReportWriter()->GetField (mSource, mValue, mCalcType);
//	else
//		GetReportWriter()->GetField (mSource, mValue, inUseOld ? ECalcType_OldValue : ECalcType_CurrentValue);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRField::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropElement:			return false;

		default:						return SRVariable::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRField::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropElement:			break;

		default:						return SRVariable::SetProperty (id, inValue);
	}

	return false;
}
