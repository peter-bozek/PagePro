/*
 *  DMReport.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 17.10.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

# include	"DMReport.h"
# include	"DMArea.h"
# include	"PSObjProps.h"
# include	<algorithm>
# include	<sstream>
# include	"SRLicense.h"

# define	kFirstClonedStyle		100000
 // # define	kSelectionSquareSize	3.0f moved inside a getter pB v 1.4.2
// # define	kSelectionSquareOffset	2.0f

# define	CURRENT_VERSION	1.0

//mbs 20062010	mPageHeight/mPageWidth are always paper, not logical page

// A4, half inch margins
# define	DEF_PAGE_WIDTH	(595)	// - 72)
# define	DEF_PAGE_HEIGHT	(842)	// - 72)
# define	DEF_PAGE_MARGIN	(36)
#if	__APPLE__
# define	Default_Style_0_XML	u"<Style name=\"Default\" id=\"0\" font=\"Lucida Grande\" size=\"12\"/>"
# define	Editor_Style_1_XML	u"<Style name=\"Editor1\" id=\"1\" font=\"Lucida Grande\" size=\"12\" backColor=\"transparent\"/>"
# define	Editor_Style_2_XML	u"<Style name=\"Editor2\" id=\"2\" font=\"Lucida Grande\" size=\"12\" backColor=\"transparent\" align=\"right\"/>"
#else
# define	Default_Style_0_XML	u"<Style name=\"Default\" id=\"0\" font=\"Verdana\" size=\"10\"/>"
# define	Editor_Style_1_XML	u"<Style name=\"Editor1\" id=\"1\" font=\"Verdana\" size=\"10\" backColor=\"transparent\"/>"
# define	Editor_Style_2_XML	u"<Style name=\"Editor2\" id=\"2\" font=\"Verdana\" size=\"10\" backColor=\"transparent\" align=\"right\"/>"
#endif

# define	ALL_BaseProps	\
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true		},	\
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true					},	\
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }							},	\
{ PSObjPropOrder,			true,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 0, LONG_MAX }, false		},	\
{ PSObjPropVisible,			true,	PSProps_Attribute,	PSProps_Boolean,	"visible",			{ NULL, 1, 0, 1 }					},	\
{ PSObjPropLocked,			true,	PSProps_Attribute,	PSProps_Integer,	"locked",			{ NULL, 0, 0, 2 }					},	\
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }					},	\
{ PSObjPropDrawingRect,		false,	PSProps_Attribute,	PSProps_Rect,		"drawRect",			{ NULL }, true						},

//{ PSObjPropPosTop,			true,	PSProps_Attribute,	PSProps_Real,		"r.top",			{ NULL, 0, INT_MIN, INT_MAX }, true	},
//{ PSObjPropPosLeft,			true,	PSProps_Attribute,	PSProps_Real,		"r.left",			{ NULL, 0, INT_MIN, INT_MAX }, true	},
//{ PSObjPropPosBottom,		true,	PSProps_Attribute,	PSProps_Real,		"r.bottom",			{ NULL, 0, INT_MIN, INT_MAX }, true	},
//{ PSObjPropPosRight,		true,	PSProps_Attribute,	PSProps_Real,		"r.right",			{ NULL, 0, INT_MIN, INT_MAX }, true	},


//{ PSObjPropPosWidth,		true,	PSProps_Attribute,	PSProps_Real,		"r.width",			{ NULL, 0, 1, INT_MAX }, true		},
# define	ALL_PositionProps	\
{ PSObjPropRect,			true,	PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }							},	\
{ PSObjPropPosHeight,		true,	PSProps_Attribute,	PSProps_Real,		"r.height",			{ NULL, 0, 1, INT_MAX }, true		},

//{ PSObjPropFixH,			true,	PSProps_Attribute,	PSProps_Boolean,	"fixH",				{ NULL, 0, 0, 1 } 					},

# define	ALL_ObjProps	\
ALL_BaseProps	\
ALL_PositionProps	\
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }, true					},	\
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},	\
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height",			{ NULL, 0, 0, INT_MAX }			},	\
{ PSObjPropExpandV,			true,	PSProps_Attribute,	PSProps_Boolean,	"fixedHeight",		{ NULL, 0, 0, 1 },				},	\
{ PSObjPropMinSpace,		true,	PSProps_Attribute,	PSProps_Real,		"minSpace",			{ NULL, 0, 0, 512 }				},	\
{ PSObjPropDraw,			true,	PSProps_Attribute,	PSProps_Boolean,	"draw", 			{ NULL, 1, 0, 1 }				},	\
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},	\
{ PSObjPropPageThrow,		true,	PSProps_Attribute,	PSProps_List,		"pageThrow",		{ sPageThrow, ePageThrow_None }	},	\
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	\
{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }, true				},	\
{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }, true				},	\
{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }, true				},	\
{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }, true				},	\
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }, true				},	\
{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }, true				},	\
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }, true				},	\
{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }, true				},	\
{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }, true				},	\
{ PSObjPropObjects,			false,	PSProps_Childs,		PSProps_Objects,	"Objects",				{ NULL }					},	\
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",			{ NULL }, true					},	\
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",			{ NULL }, true					},


const PSObject::PSObjProps	DMSection::sProperties[] = {
ALL_ObjProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMHeaderFooterSection::sProperties[] = {
ALL_ObjProps
{ PSObjPropBind,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindToBottom",		{ NULL, 1, 0, 1 }				},
{ PSObjPropFixed,			true,	PSProps_Attribute,	PSProps_Real,		"fixed",			{ NULL, -1, -1, 512 }			},
{ PSObjPropFirstPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"firstPage",		{ NULL, 1, 0, 1 }				},
{ PSObjPropEvenPage,		true,	PSProps_Attribute,	PSProps_Integer,	"evenPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropOddPage,			true,	PSProps_Attribute,	PSProps_Integer,	"oddPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropLastPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"lastPage",			{ NULL, 1, 0, 1 }				},
{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Boolean,	"fillPage",			{ NULL, 0, 0, 1 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMBreakSection::sProperties[] = {
ALL_ObjProps
{ PSObjPropBind,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindToBottom",		{ NULL, 0, 0, 1 }				},
{ PSObjPropBreakOnField,	true,	PSProps_Attribute,	PSProps_String,		"breakOnField",		{ NULL }						},
{ PSObjPropBreakOnVariable,	true,	PSProps_Attribute,	PSProps_String,		"breakOnVariable",	{ NULL }						},
{ PSObjPropBreakOnArray,	true,	PSProps_Attribute,	PSProps_String,		"breakOnArray",		{ NULL }						},
{ PSObjPropBreakLevel,		true,	PSProps_Attribute,	PSProps_Integer,	"level",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropPrintAlways,		true,	PSProps_Attribute,	PSProps_Boolean,	"always",			{ NULL, 0, 0, 1 }				},
{ PSObjPropBreakOn,			true,	PSProps_Attribute,	PSProps_String,		"breakOn",			{ NULL }, true					},
{ PSObjPropBreakType,		true,	PSProps_Attribute,	PSProps_List,		"breakType",		{ sBreakType, eBreakOn_None }, true	},
{ PSObjPropAlias,			true,	PSProps_Attribute,	PSProps_String,		"alias",			{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMScrapSection::sProperties[] = {
ALL_BaseProps
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }, true					},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }, true				},
{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }, true				},
{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }, true				},
{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }, true				},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }, true				},
{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }, true				},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }, true				},
{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }, true				},
{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }, true				},
{ PSObjPropObjects,			false,	PSProps_Childs,		PSProps_Objects,	"Objects",				{ NULL }					},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",			{ NULL }, true					},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMPageSection::sProperties[] = {
ALL_BaseProps
{ PSObjPropRect,			true,	PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }						},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }, true					},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }, true				},
{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }, true				},
{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }, true				},
{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }, true				},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }, true				},
{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }, true				},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }, true				},
{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }, true				},
{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }, true				},
{ PSObjPropObjects,			false,	PSProps_Childs,		PSProps_Objects,	"Objects",				{ NULL }					},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",			{ NULL }, true					},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",			{ NULL }, true					},
//{ 'pgor',					true,	PSProps_Attribute,	PSProps_String,		"Orientation",		{ NULL }						},
//{ 'pgsi',					true,	PSProps_Attribute,	PSProps_String,		"Size",				{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMBodySection::sProperties[] = {
ALL_ObjProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMWatermarkSection::sProperties[] = {
ALL_BaseProps
{ PSObjPropRect,			true,	PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }						},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }, true					},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropDraw,			true,	PSProps_Attribute,	PSProps_Boolean,	"draw", 			{ NULL, 1, 0, 1 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropFirstPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"firstPage",		{ NULL, 1, 0, 1 }				},
{ PSObjPropEvenPage,		true,	PSProps_Attribute,	PSProps_Integer,	"evenPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropOddPage,			true,	PSProps_Attribute,	PSProps_Integer,	"oddPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropLastPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"lastPage",			{ NULL, 1, 0, 1 }				},
{ PSObjPropOnTop,			true,	PSProps_Attribute,	PSProps_Boolean,	"onTop",			{ NULL, 0, 0, 1 }				},
{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }, true				},
{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }, true				},
{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }, true				},
{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }, true				},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }, true				},
{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }, true				},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }, true				},
{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }, true				},
{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }, true				},
{ PSObjPropObjects,			false,	PSProps_Childs,		PSProps_Objects,	"Objects",				{ NULL }					},
{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",			{ NULL }, true					},
{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

#undef	ALL_ObjProps

const PSObject::PSObjProps	DM4DDataSource::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true				},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_List,		"type",				{ s4DKind, -1 }					},
{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_List,		"source",			{ sSource, eDataSource_Undefined }	},
{ PSObjPropIterations,		true,	PSProps_Attribute,	PSProps_Integer,	"iterations",		{ NULL, -1, 1, INT_MAX }		},
{ PSObjPropTableID,			true,	PSProps_Attribute,	PSProps_Integer,	"tableID",			{ NULL, 0, 1, INT_MAX }			},	// main table!
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropRelateOne,		true,	PSProps_Attribute,	PSProps_List,		"relateOne",		{ sRelate, eRelate_Manual }		},
{ PSObjPropRelateMany,		true,	PSProps_Attribute,	PSProps_List,		"relateMany",		{ sRelate, eRelate_Manual }		},
{ PSObjPropCallback,		true,	PSProps_Attribute,	PSProps_String,		"callback",			{ NULL }						},
{ PSObjPropStartScript,		true,	PSProps_OneChild,	PSProps_String,		"StartScript",		{ NULL }						},
{ PSObjPropBodyScript,		true,	PSProps_OneChild,	PSProps_String,		"BodyScript",		{ NULL }						},
{ PSObjPropEndScript,		true,	PSProps_OneChild,	PSProps_String,		"EndScript",		{ NULL }						},
{ PSObjPropSRPCompatibility,true,	PSProps_Attribute,	PSProps_Boolean,	"SRPCompatibility",	{ NULL, 0, 0, 1 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMGuide::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true				},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropOrder,			true,	PSProps_None,		PSProps_Integer,	"order",			{ NULL, 0, 0, LONG_MAX }, true	},
{ PSObjPropLocked,			true,	PSProps_Attribute,	PSProps_Integer,	"locked",			{ NULL, 0, 0, 2 }				},
{ PSObjPropSelected,		true,	PSProps_Attribute,	PSProps_Boolean,	"selected", 		{ NULL, 0, 0, 1 }				},
{ PSObjPropDrawingRect,		false,	PSProps_Attribute,	PSProps_Rect,		"drawRect",			{ NULL }, true					},
{ PSObjPropXML,				false,	PSProps_Attribute,	PSProps_Rect,		"xml",				{ NULL }, true					},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_Boolean,	"type",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropData,			true,	PSProps_Attribute,	PSProps_Real,		"pos",				{ NULL, -INFINITY, -4096, 4096 }	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMReport::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }, true	},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }, true				},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropVersion,			true,	PSProps_Attribute,	PSProps_Real,		"version",			{ NULL, -1, 0, INFINITY } },
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"simple",			{ NULL, 0, 0, 1 } 				},
//{ PSObjPropRect,			true,	PSProps_Attribute,	PSProps_Rect,		"r",				{ NULL }						},
{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"pageWidth",		{ NULL, 100, 100, 4096 }		},
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"pageHeight",		{ NULL, 100, 100, 4096 }		},
{ PSObjPropPaper,			true,	PSProps_Attribute,	PSProps_Boolean,	"usePhysical",		{ NULL, 0, 0, 1 }				},
{ PSObjPropMargins,			true,	PSProps_Attribute,	PSProps_Rect,		"pageMargins",		{ NULL }						},

	// v 1.4 properties
//{ PSObjPropLabel,			true,	PSProps_Attribute,	PSProps_Boolean,	"label",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropLabelH,			true,	PSProps_Attribute,	PSProps_Integer,	"LabelH",			{ NULL, 0, 0, 100 }				},
//{ PSObjPropLabelV,			true,	PSProps_Attribute,	PSProps_Integer,	"labelV",			{ NULL, 0, 0, 100 }				},
//
//{ PSObjPropLabelMTop,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginTop",	{ NULL, 0, -100, 1024 }			},
//{ PSObjPropLabelMLeft,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginLeft",	{ NULL, 0, -100, 1024 }			},
//{ PSObjPropLabelMBottom,	true,	PSProps_Attribute,	PSProps_Real,		"labelMarginBottom", { NULL, 0, -100, 1024 }			},
//{ PSObjPropLabelMRight,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginRight",	{ NULL, 0, -100, 1024 }			},
	
{ PSObjPropObjectRotation,	true,	PSProps_Attribute,	PSProps_Boolean,	"rotation",			{ NULL, 0, 0, 1 }               },
{ PSObjPropMirror,			true,	PSProps_Attribute,	PSProps_Boolean,	"mirror",           { NULL, 0, 0, 1 }               },

{ PSObjPropEditor,			true,	PSProps_OneContainer, PSProps_Objects,	"Editor",			{ NULL }						},
{ PSObjPropOGuides,			true,	PSProps_OneContainer, PSProps_Objects,	"Guides",			{ NULL }						},
{ PSObjPropDataSource,		true,	PSProps_OneChild,	PSProps_Objects,	"DataSource",		{ NULL }						},

	//{ PSObjPropPageSetup,		true,	PSProps_OneChild,	PSProps_BLOB,		"PageSetup",		{ NULL }						},
{ PSObjPropPageFormat,		true,	PSProps_OneChild,	PSProps_BLOB,		"PageFormat",		{ NULL }						},
{ PSObjPropPrintSettings,	true,	PSProps_OneChild,	PSProps_BLOB,		"PrintSettings",	{ NULL }						},
{ PSObjPropDevMode,			true,	PSProps_OneChild,	PSProps_BLOB,		"DevMode",			{ NULL }						},
{ PSObjPropDeviceNames,		true,	PSProps_OneChild,	PSProps_BLOB,		"DeviceNames",		{ NULL }						},
{ PSObjPropPageSetupDlg,	true,	PSProps_OneChild,	PSProps_BLOB,		"PageSetupDlg",		{ NULL }						},
{ PSObjPropPrintDlg,		true,	PSProps_OneChild,	PSProps_BLOB,		"PrintDlg",			{ NULL }						},
{ PSObjPropStyleSet,		true,	PSProps_OneContainer, PSProps_Objects,	"StyleSet",			{ NULL }						},
{ PSObjPropHeaderSection,	true,	PSProps_Childs,		PSProps_Objects,	"Header",			{ NULL }						},
{ PSObjPropBrkHdrSection,	true,	PSProps_Childs,		PSProps_Objects,	"BreakHeader",		{ NULL }, true					},
{ PSObjPropScrapSection,	true,	PSProps_Childs,		PSProps_Objects,	"Scrap",			{ NULL }, true					},	// REMOVE!?!
{ PSObjPropPageSection,		true,	PSProps_Childs,		PSProps_Objects,	"Page",				{ NULL }, true					},
{ PSObjPropBodySection,		true,	PSProps_Childs,		PSProps_Objects,	"Body",				{ NULL }, true					},
{ PSObjPropBrkFtrSection,	true,	PSProps_Childs,		PSProps_Objects,	"BreakFooter",		{ NULL }, true					},
{ PSObjPropFooterSection,	true,	PSProps_Childs,		PSProps_Objects,	"Footer",			{ NULL }, true					},
{ PSObjPropWatermarkSection,true,	PSProps_Childs,		PSProps_Objects,	"Watermark",		{ NULL }, true					},

// for property name lookup only
{ PSObjPropShowMargins,		false,	PSProps_Attribute,	PSProps_Boolean,	"showMargins",		{ NULL, 1, 0, 1 }, true			},
{ PSObjPropShowRuler,		false,	PSProps_Attribute,	PSProps_Boolean,	"showRulers",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropRulerUnits,		false,	PSProps_Attribute,	PSProps_Integer,	"rulerUnits",		{ NULL, 0, 0, 3 }, true			},
{ PSObjPropGridSize,		false,	PSProps_Attribute,	PSProps_Real,		"gridSize",			{ NULL, 32, 4, 256 }, true		},
{ PSObjPropShowGrid,		false,	PSProps_Attribute,	PSProps_Boolean,	"showGrid",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropSnapToGrid,		false,	PSProps_Attribute,	PSProps_Boolean,	"snapToGrid",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropShowGuides,		false,	PSProps_Attribute,	PSProps_Boolean,	"showGuides",		{ NULL, 1, 0, 1 }, true			},
{ PSObjPropLockGuides,		false,	PSProps_Attribute,	PSProps_Boolean,	"lockGuides",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropSnapToGuides,	false,	PSProps_Attribute,	PSProps_Boolean,	"snapToGuide",		{ NULL, 0, 0, 1 }, true			},
//{ PSObjPropShowSections,	false,	PSProps_Attribute,	PSProps_Boolean,	"showSections",		{ NULL, 0, 0, 1 }, true			},
//{ PSObjPropLockSections,	false,	PSProps_Attribute,	PSProps_Boolean,	"lockSections",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropShowObjBorders,	true,	PSProps_Attribute,	PSProps_Boolean,	"showObjBorders",	{ NULL, 1, 0, 1 }, true			},
{ PSObjPropScale,			false,	PSProps_Attribute,	PSProps_Real,		"scale",			{ NULL, 1, 0.1, 10.0 }, true		},
{ PSObjPropGridColor,		false,	PSProps_Attribute,	PSProps_Color,		"gridColor",		{ NULL }, true		},
{ PSObjPropGridRadius,		false,	PSProps_Attribute,	PSProps_Real,		"gridRadius",		{ NULL, 1, 0.1, 10.0 }, true		},
{ PSObjPropGuideColor,		false,	PSProps_Attribute,	PSProps_Integer,	"guideColor",		{ NULL,  }, true		},
{ PSObjPropGuideWidth,		false,	PSProps_Attribute,	PSProps_Color,		"guideWidth",		{ NULL, 1, 0.1, 10.0 }, true		},

// and Area's properties
{ 'drmo',					false,	PSProps_Attribute,	PSProps_Integer,	"drawingMode",		{ NULL, eDraw_Normal, eDraw_Normal, eDraw_Last - 1 }, true		},
{ 'scrl',					false,	PSProps_Attribute,	PSProps_Integer,	"scrollLeft",		{ NULL, 0, 0, 4096 }, true		},
{ 'scrt',					false,	PSProps_Attribute,	PSProps_Integer,	"scrollTop",		{ NULL, 0, 0, 4096 }, true		},
{ 'tool',					false,	PSProps_Attribute,	PSProps_Integer,	"tool",				{ NULL, DMArea::eTool_Select, DMArea::eTool_Select, DMArea::eTool_last - 1 }, true		},
{ 'rund',					false,	PSProps_Attribute,	PSProps_Integer,	"rounding",			{ NULL, 10, 0.1, 10000 }, true		},
{ 'snap',					false,	PSProps_Attribute,	PSProps_Integer,	"snapLimit",		{ NULL, 2.5, 0.05, 10 }, true		},
{ 'scrw',					false,	PSProps_Attribute,	PSProps_Integer,	"scrollWheel",		{ NULL, 1, 1, 1000 }, true		},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMReport::sPropertiesEditor[] = {
{ PSObjPropShowMargins,		true,	PSProps_Attribute,	PSProps_Boolean,	"showMargins",		{ NULL, 1, 0, 1 }				},
{ PSObjPropShowRuler,		true,	PSProps_Attribute,	PSProps_Boolean,	"showRulers",		{ NULL, 0, 0, 1 }				},
{ PSObjPropRulerUnits,		true,	PSProps_Attribute,	PSProps_Integer,	"rulerUnits",		{ NULL, 0, 0, 3 }				},
{ PSObjPropGridSize,		true,	PSProps_Attribute,	PSProps_Real,		"gridSize",			{ NULL, 32, 4, 256 }			},
{ PSObjPropShowGrid,		true,	PSProps_Attribute,	PSProps_Boolean,	"showGrid",			{ NULL, 0, 0, 1 }				},
{ PSObjPropSnapToGrid,		true,	PSProps_Attribute,	PSProps_Boolean,	"snapToGrid",		{ NULL, 0, 0, 1 }				},
{ PSObjPropShowGuides,		true,	PSProps_Attribute,	PSProps_Boolean,	"showGuides",		{ NULL, 1, 0, 1 }				},
{ PSObjPropLockGuides,		true,	PSProps_Attribute,	PSProps_Boolean,	"lockGuides",		{ NULL, 0, 0, 1 }				},
{ PSObjPropSnapToGuides,	true,	PSProps_Attribute,	PSProps_Boolean,	"snapToGuide",		{ NULL, 0, 0, 1 }				},
//{ PSObjPropShowSections,	true,	PSProps_Attribute,	PSProps_Boolean,	"showSections",		{ NULL, 0, 0, 1 }				},
//{ PSObjPropLockSections,	true,	PSProps_Attribute,	PSProps_Boolean,	"lockSections",		{ NULL, 0, 0, 1 }				},
{ PSObjPropShowObjBorders,	true,	PSProps_Attribute,	PSProps_Boolean,	"showObjBorders",	{ NULL, 1, 0, 1 }				},
{ PSObjPropScale,			true,	PSProps_Attribute,	PSProps_Real,		"scale",			{ NULL, 1, 0.1, 10.0 }			},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMReport::sPropertiesGuides[] = {
{ PSObjPropOGuideH,			true,	PSProps_Childs,		PSProps_Objects,	"Horizontal",		{ NULL }						},
{ PSObjPropOGuideV,			true,	PSProps_Childs,		PSProps_Objects,	"Vertical",			{ NULL }, true					},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	DMReport::sPropertiesStyles[] = {
{ PSObjPropOStyle,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Style], { NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

#pragma	mark	sUserProperties

const DMBase::UserProps	DMStyle::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropSelected,		true	},
{ PSObjPropName,			true	},

{ PSObjPropBaseID,			false	},
{ PSObjPropFlags,			false	},
{ PSObjPropFontName,		true	},
//{ PSObjPropPSName,			true	},
{ PSObjPropSize,			true	},
//{ PSObjPropStyleF,			true	},
{ PSObjPropStyleB,			true	},
{ PSObjPropStyleI,			true	},
{ PSObjPropStyleU,			true	},
//{ PSObjPropStyleS,			true	},
{ PSObjPropRotation,		true	},
//{ PSObjPropObjectRotation,		true	},
//{ PSObjPropHorizontalOffset,true	},
//{ PSObjPropVerticalOffset,	true	},
{ PSObjPropWrap,			true	},
//{ PSObjPropFrame,			true	},
{ PSObjPropAlign,			true	},
{ PSObjPropVertAlign,		true	},
{ PSObjPropTextColor,		true	},
{ PSObjPropBackColor,		true	},
{ PSObjPropFrameColor,		true	},

{ PSObjPropBaseLineShift,	true	},
{ PSObjPropHorizontalScale,	true	},
{ PSObjPropLineSpacing,		true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMHeaderFooterSection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
//{ PSObjPropRect,			false	},
//{ PSObjPropPosBottom,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
{ PSObjPropExpandV,			true	},	//mbs 09072010	instead of PSObjPropHeight
{ PSObjPropMinSpace,		true	},
{ PSObjPropDraw,			true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropBind,			true	},
{ PSObjPropPageThrow,		true	},
{ PSObjPropScript,			true	},

//{ PSObjPropFixed,			true	},
{ PSObjPropFirstPage,		true	},
{ PSObjPropEvenPage,		true	},
{ PSObjPropOddPage,			true	},
{ PSObjPropLastPage,		true	},
{ PSObjPropFill,			true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMBreakSection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			false	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
//{ PSObjPropRect,			false	},
//{ PSObjPropPosBottom,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
{ PSObjPropExpandV,			true	},	//mbs 09072010	instead of PSObjPropHeight
{ PSObjPropMinSpace,		true	},
{ PSObjPropDraw,			true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropBind,			true	},
{ PSObjPropPageThrow,		true	},
{ PSObjPropScript,			true	},

{ PSObjPropBreakOn,			true	},
{ PSObjPropBreakType,		true	},
{ PSObjPropBreakOnField,	true	},
{ PSObjPropBreakOnVariable,	true	},
{ PSObjPropBreakOnArray,	true	},
{ PSObjPropAlias,			true	},
{ PSObjPropBreakLevel,		true	},
{ PSObjPropPrintAlways,		true	},
{ 0, 						false	}
};


const DMBase::UserProps	DMScrapSection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMPageSection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
//{ PSObjPropRect,			false	},
//{ PSObjPropPosBottom,		true	},
//{ PSObjPropPosHeight,		true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
{ PSObjPropScript,			true	},

//{ 'pgor',					true	},
//{ 'pgsi',					true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMBodySection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
//{ PSObjPropRect,			false	},
//{ PSObjPropPosBottom,		true	},
{ PSObjPropPosHeight,		true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
{ PSObjPropExpandV,			true	},	//mbs 09072010	instead of PSObjPropHeight
{ PSObjPropMinSpace,		true	},
{ PSObjPropDraw,			true	},
{ PSObjPropKeepTogether,	true	},
{ PSObjPropPageThrow,		true	},
{ PSObjPropScript,			true	},

//{ 'pgor',					true	},
//{ 'pgsi',					true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMWatermarkSection::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropOrder,			true	},
{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
//{ PSObjPropDrawingRect,		false	},
//{ PSObjPropRect,			false	},
//{ PSObjPropPosBottom,		true	},
//{ PSObjPropPosHeight,		true	},
{ PSObjPropType,			false	},
{ PSObjPropName,			true	},
//{ PSObjPropHeight,			true	},
//{ PSObjPropMinSpace,		true	},
{ PSObjPropDraw,			true	},
//{ PSObjPropKeepTogether,	true	},
//{ PSObjPropBind,			true	},
//{ PSObjPropPageThrow,		true	},
{ PSObjPropScript,			true	},

//{ PSObjPropFixed,			true	},
{ PSObjPropFirstPage,		true	},
{ PSObjPropEvenPage,		true	},
{ PSObjPropOddPage,			true	},
{ PSObjPropLastPage,		true	},

{ PSObjPropOnTop,			true	},
{ 0, 						false	}
};

const DMBase::UserProps	DM4DDataSource::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropType,			false	},
{ PSObjPropSource,			true	},
{ PSObjPropIterations,		true	},
{ PSObjPropTableID,			true	},
{ PSObjPropName,			true	},
{ PSObjPropRelateOne,		true	},
{ PSObjPropRelateMany,		true	},
{ PSObjPropCallback,		true	},
{ PSObjPropStartScript,		true	},
{ PSObjPropBodyScript,		true	},
{ PSObjPropEndScript,		true	},
{ PSObjPropSRPCompatibility, true	},
{ 0, 						false	}
};

const DMBase::UserProps	DMGuide::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
{ PSObjPropSelected,		true	},
{ PSObjPropType,			false	},
{ PSObjPropData,			true	},
{ 0, 						false	}
};


const DMBase::UserProps	DMReport::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
//{ PSObjPropVisible,			true	},
{ PSObjPropLocked,			true	},
{ PSObjPropSelected,		true	},
{ PSObjPropVersion,			false	},
{ PSObjPropName,			true	},
{ PSObjPropDynamic,			true	},
//{ PSObjPropRect,			true	},
{ PSObjPropWidth,			true	},
{ PSObjPropHeight,			true	},
{ PSObjPropPaper,			true	},
{ PSObjPropMargins,			true	},

// v 1.4
{ PSObjPropLabel,			true	},
{ PSObjPropLabelH,			true	},
{ PSObjPropLabelV,			true	},

{ PSObjPropLabelMTop,		true	},
{ PSObjPropLabelMLeft,		true	},
{ PSObjPropLabelMBottom,	true	},
{ PSObjPropLabelMRight,		true	},
	
//{ PSObjPropPageSetup,		true	},
{ PSObjPropPageFormat,		true	},
{ PSObjPropPrintSettings,	true	},
{ PSObjPropDevMode,			true	},
{ PSObjPropDeviceNames,		true	},
{ PSObjPropPageSetupDlg,	true	},
{ PSObjPropPrintDlg,		true	},

{ PSObjPropShowMargins,		true	},
{ PSObjPropShowRuler,		true	},
{ PSObjPropRulerUnits,		true	},
{ PSObjPropGridSize,		true	},
{ PSObjPropShowGrid,		true	},
{ PSObjPropSnapToGrid,		true	},
{ PSObjPropShowGuides,		true	},
{ PSObjPropLockGuides,		true	},
{ PSObjPropSnapToGuides,	true	},
//{ PSObjPropShowSections,	true	},
//{ PSObjPropLockSections,	true	},
{ PSObjPropShowObjBorders,	true	},
{ PSObjPropScale,			true	},
{ PSObjPropGridColor,		true	},
{ PSObjPropGridRadius,		true	},
{ PSObjPropGuideColor,		true	},
{ PSObjPropGuideWidth,		true	},
{ 0, 						false	}
};

long	DMStyle::GetUserProperties (const UserProps* &outProps) const				{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMHeaderFooterSection::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMBreakSection::GetUserProperties (const UserProps* &outProps) const		{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMScrapSection::GetUserProperties (const UserProps* &outProps) const		{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMPageSection::GetUserProperties (const UserProps* &outProps) const			{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMBodySection::GetUserProperties (const UserProps* &outProps) const			{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMWatermarkSection::GetUserProperties (const UserProps* &outProps) const	{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DM4DDataSource::GetUserProperties (const UserProps* &outProps) const		{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMGuide::GetUserProperties (const UserProps* &outProps) const				{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }
long	DMReport::GetUserProperties (const UserProps* &outProps) const				{ outProps = sUserProperties; return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1; }


#pragma	mark	-

// ---------------------------------------------------------------------------
// FindStyle														  [public]
// ---------------------------------------------------------------------------

RWStyle*
PSStyleListD::FindStyle (long inID)
const
{
	if (size() > 0)
	{
		const_iterator	iter;
		const RWStyle	*style = NULL;
		for (iter = begin(); iter != end(); iter++)
		{
			style = static_cast <const RWStyle*> (static_cast <const DMStyle*> (static_cast <const DMBase*> (*iter)));	// style = dynamic_cast <const DMStyle*> (*iter);
			if (style->GetID() == inID)
				return const_cast <RWStyle*> (style);
		}
		style = static_cast <const RWStyle*> (static_cast <const DMStyle*> (static_cast <const DMBase*> (*begin())));// style = dynamic_cast <const DMStyle*> (*begin());
		return const_cast <RWStyle*> (style);
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// GetNewID															  [public]
// ---------------------------------------------------------------------------

long
PSStyleListD::GetNewID ()
const
{
	if (size() > 0)
	{
		const RWStyle	*style = static_cast <const RWStyle*> (static_cast <const DMStyle*> (static_cast <const DMBase*> (*(end() - 1))));
		return style->GetID() + 1;
	}
	return 1;
}


// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMStyle*
DMStyle::Create (RWStyleContainer *inContainer, DMBase *inParent, RWXmlNode inNode)
{
	DMStyle	*src = new DMStyle (inContainer, inParent);
	if (inNode)
		src->LoadXML (inNode);
	
	return src;
}


// ---------------------------------------------------------------------------
// Clone															  [public]
// ---------------------------------------------------------------------------

DMStyle*
DMStyle::Clone (RWStyleContainer *inContainer, DMBase *inParent, long inNewID)
{
	DMStyle	*src = new DMStyle (inContainer, inParent, mId);
	RWValue	v (inNewID);
	src->SetProperty (PSObjPropID, v);

	return src;
}


// ---------------------------------------------------------------------------
// DMStyle									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMStyle::DMStyle (RWStyleContainer *inContainer, DMBase *inParent)
	:	DMBase (inParent, eObject_Style),
		RWStyle (inContainer, RWXmlNode())
{
}


// ---------------------------------------------------------------------------
// DMStyle														   [protected]
// ---------------------------------------------------------------------------

DMStyle::DMStyle (RWStyleContainer *inContainer, DMBase *inParent, long inBaseID)
	:	DMBase (inParent, eObject_Style),
		RWStyle (inContainer, RWXmlNode())
{
	mBaseId = inBaseID;
	mFeatures = stf_Based;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMStyle::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
//		case PSObjPropID:			outValue.SetText (mID); break;
		case PSObjPropSelected:		outValue.SetBoolean (mSelected); break;

		default:					return RWStyle::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMStyle::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropID:
		{
			SetIntegerProperty (inValue, mId, 0);
			inValue.GetTextValue (mID, NULL);
			return true;
			break;
		}

		case PSObjPropSelected:
			if (inValue.CoerceValue (RWValue::eValue_Boolean))
			{
				SetSelected (inValue.GetBoolean());
				return true;
			}
			break;
			
		default:					return RWStyle::SetProperty (id, inValue);
	}
	
	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// DMSection								Constructor			   [protected]
// ---------------------------------------------------------------------------

DMSection::DMSection (DMBase *inParent, ESection_Kind inKind, RWStringView inType)
	:	DMBase (inParent, eObject_Section),
		mSectionKind (inKind),
		mHeight (0),
		mMinSpace (0),
		mDraw (true),
		mKeepTogether (false),
		mFromBottom (false),
		mFixedHeight (false),
		mPageThrowI (ePageThrow_None),
		mLabelRect (0, 0, 0, 0)
{
	mType = inType;
	if (inKind == eSectionKind_Footer)
		mFromBottom = true;

	return;
}


// ---------------------------------------------------------------------------
// DMSection								Destructor				  [public]
// ---------------------------------------------------------------------------

DMSection::~DMSection (void)
{

	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMSection::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
/*
 if (mType)
	{
		free (mType);
		mType = NULL;
	}
*/
	mHeight = 0;
	mMinSpace = 0;
	mDraw = true;
	mKeepTogether = false;
	mFromBottom = (mSectionKind == eSectionKind_Footer);
	mPageThrowI = ePageThrow_None;
	mObjects.clear();
	
	DMBase::LoadXML (inNode, pes);

	// header must not be bound to bottom... and body and break heraders/footers, too...
	if (mSectionKind != eSectionKind_Footer && mSectionKind != eSectionKind_BreakFooter)
		mFromBottom = false;

	AdjustOrder (eOrder_Reset, INT_MAX);
//	std::sort<PSObjListD::iterator, DMObjectCompareOrder> (mObjects.begin(), mObjects.end(), DMObjectCompareOrder());
	
	return;
}


// ---------------------------------------------------------------------------
// CompareOrder														  [public]
// ---------------------------------------------------------------------------

int
DMSection::CompareOrder (const DMBase *other)
const
{
	// order is header, break header by level, body, break footer by descending level, fill footer, footer
	const	DMSection	*rh = static_cast <const DMSection*> (other);

	if (mSectionKind == eSectionKind_Watermark)
		if (static_cast <const DMWatermarkSection*> (this)->IsOnTop())
			return 1;
		else
			return -1;
	if (rh->mSectionKind == eSectionKind_Watermark)
		if (static_cast <const DMWatermarkSection*> (rh)->IsOnTop())
			return -1;
		else
			return 1;

	if (mSectionKind < rh->mSectionKind)
		return -1;
	else if (mSectionKind > rh->mSectionKind)
		return 1;

	if (mSectionKind == eSectionKind_BreakHeader || mSectionKind == eSectionKind_BreakFooter)
	{
		const	DMBreakSection	*blh = static_cast <const DMBreakSection*> (this);
		const	DMBreakSection	*brh = static_cast <const DMBreakSection*> (other);
		if (blh->GetBreakLevel() < brh->GetBreakLevel())
			return (mSectionKind == eSectionKind_BreakHeader? -1: 1);
		else if (blh->GetBreakLevel() > brh->GetBreakLevel())
			return (mSectionKind == eSectionKind_BreakHeader? 1: -1);
	}

	if (GetOrder() < other->GetOrder())
		return -1;
	else if (GetOrder() > other->GetOrder())
		return 1;

	return 0;	// should not happen...
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
DMSection::GetObjects (OSType id, PSObjListD* &outList)
{
	if (id == PSObjPropObjects)
	{
		outList = &mObjects;
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropType:			outValue.SetText (mType); break;
		case PSObjPropName:			outValue.SetText (mName); break;
		case PSObjPropHeight:		outValue.SetReal (mHeight); break;
		case PSObjPropExpandV:		outValue.SetBoolean (mFixedHeight); break;	//mbs 09072010
		case PSObjPropMinSpace:		outValue.SetReal (mMinSpace); break;
		case PSObjPropDraw:			outValue.SetBoolean (mDraw); break;
		case PSObjPropKeepTogether:	outValue.SetBoolean (mKeepTogether); break;
		case PSObjPropBind:
			if (mSectionKind != eSectionKind_Footer && mSectionKind != eSectionKind_BreakFooter)
				return false;
			outValue.SetBoolean (mFromBottom);
			break;
		case PSObjPropPageThrow:
			if(outValue.GetKind() == RWValue::eValue_Integer)
				outValue.SetInteger (mPageThrowI); 
			else
				outValue.SetText (RWStr::FromASCII (sPageThrow [mPageThrowI]));
			break;
		case PSObjPropScript:		outValue.SetText (mScript); break;
		case PSObjPropObjects:		outValue.SetInteger (mObjects.size()); break;

		default:					return DMBase::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMSection::SetProperty (OSType id, RWValue &inValue)
{
	float	h;
	switch (id)
	{
		case PSObjPropPosLeft:
		case PSObjPropPosRight:
		case PSObjPropPosWidth:
		case PSObjPropRelPosLeft:
		case PSObjPropRelPosRight:
		case PSObjPropRelMoveH:
		case PSObjPropRelMoveV:
			break;

		case PSObjPropName:			return SetStringProperty (inValue, mName);
		case PSObjPropHeight:
			if (SetRealProperty (inValue, h, 0, 4096))
			{
				mHeight = h;
				DMBase::SetProperty (PSObjPropPosHeight, inValue);
				return true;
			}
			break;
		case PSObjPropExpandV:	//mbs 09072010
			return SetBooleanProperty (inValue, mFixedHeight); // PB 2010-9
		case PSObjPropRect:
		case PSObjPropPosHeight:
			if (DMBase::SetProperty (id, inValue))
			{
				mHeight = mPosition.Height();
				return true;
			}
			break;
		case PSObjPropMinSpace:		return SetRealProperty (inValue, mMinSpace, 0, 4096);
		case PSObjPropDraw:			return SetBooleanProperty (inValue, mDraw);
		case PSObjPropKeepTogether:	return SetBooleanProperty (inValue, mKeepTogether);
		case PSObjPropBind:
			if (mSectionKind != eSectionKind_Footer && mSectionKind != eSectionKind_BreakFooter)
				break;
			return SetBooleanProperty (inValue, mFromBottom);
		case PSObjPropPageThrow:	return SetListProperty (inValue, sPageThrow, mPageThrowI);
		case PSObjPropScript:		return SetStringProperty (inValue, mScript);

		default:					return DMBase::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
DMSection::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
//	DMObject::LoadXMLObjects (pes, inNode);

	GetReport()->ParseObjects (pes, inNode, this, mObjects);

	return;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMSection::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
    RWXmlNode	me = DMBase::WriteXML (inParent, pes);
	me.SetName (mType);

	return me;
}


// ---------------------------------------------------------------------------
// AdjustDrawingPosition											  [public]
// ---------------------------------------------------------------------------

void
DMSection::AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent)
{
	DMBase::AdjustDrawingPosition (inComposer, inParent);
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

DMHeaderFooterSection*
DMHeaderFooterSection::Create (DMBase *inParent, RWXmlNode inNode, ESection_Kind inKind, RWStringView inType)
{
	DMHeaderFooterSection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMHeaderFooterSection (inParent, inKind, !inType.empty() ? RWString (inType) : inNode.Name());
		if (inNode)
			sec->LoadXML (inNode);
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMHeaderFooterSection					Constructor			   [protected]
// ---------------------------------------------------------------------------

DMHeaderFooterSection::DMHeaderFooterSection (DMBase *inParent, ESection_Kind inKind, RWStringView inType)
	:	DMSection (inParent, inKind, inType),
		mFixed (-1),
		mFirstPage (true),
		mEvenPage (1),
		mOddPage (1),
		mLastPage (true),
		mFillPage (false)
{
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMHeaderFooterSection::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	mFixed = -1;
	mFirstPage = true;
	mEvenPage = 1;
	mOddPage = 1;
	mLastPage = true;
	mFillPage = false;

	DMSection::LoadXML (inNode, pes);

	if (mSectionKind != eSectionKind_Footer)
		mFillPage = false;

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMHeaderFooterSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFixed:		outValue.SetReal (mFixed); break;
		case PSObjPropFirstPage:	outValue.SetBoolean (mFirstPage); break;
		case PSObjPropEvenPage:		outValue.SetInteger (mEvenPage); break;
		case PSObjPropOddPage:		outValue.SetInteger (mOddPage); break;
		case PSObjPropLastPage:		outValue.SetBoolean (mLastPage); break;
		case PSObjPropFill:
			if (mSectionKind != eSectionKind_Footer)
				return false;
			outValue.SetBoolean (mFillPage);
			break;

		default:					return DMSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMHeaderFooterSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFixed:		return SetRealProperty (inValue, mFixed, -1, 4096);
		case PSObjPropFirstPage:	return SetBooleanProperty (inValue, mFirstPage);
		case PSObjPropEvenPage:		return SetIntegerProperty (inValue, mEvenPage, 0, 2);
		case PSObjPropOddPage:		return SetIntegerProperty (inValue, mOddPage, 0, 2);
		case PSObjPropLastPage:		return SetBooleanProperty (inValue, mLastPage);
		case PSObjPropFill:
			if (mSectionKind != eSectionKind_Footer)
				break;
			return SetBooleanProperty (inValue, mFillPage);

		default:					return DMSection::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMBreakSection*
DMBreakSection::Create (DMBase *inParent, RWXmlNode inNode, ESection_Kind inKind, RWStringView inType)
{
	DMBreakSection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMBreakSection (inParent, inKind, !inType.empty() ? RWString (inType) : inNode.Name());
		if (inNode)
			sec->LoadXML (inNode);
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMBreakSection							Constructor			   [protected]
// ---------------------------------------------------------------------------
DMBreakSection::DMBreakSection (DMBase *inParent, ESection_Kind inKind, RWStringView inType)
	:	DMSection (inParent, inKind, inType),
		mLevel (0),
		mPrintAlways (false),
		mBreakTypeI (eBreakOn_None)
{
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMBreakSection::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	mLevel = 0;
	mPrintAlways = false;
	mBreakTypeI = eBreakOn_None;
	mBreakOn.clear();
	
	DMSection::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBreakSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropBreakOnField:
			if (mBreakTypeI != eBreakOn_Field)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakOnVariable:
			if (mBreakTypeI != eBreakOn_Variable)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakOnArray:
			if (mBreakTypeI != eBreakOn_Array)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakLevel:		outValue.SetInteger (mLevel); break;
		case PSObjPropPrintAlways:
			if (mSectionKind != eSectionKind_BreakHeader)
				return false;
			outValue.SetBoolean (mPrintAlways);
			break;

		case PSObjPropBreakOn:			outValue.SetText (mBreakOn); break;
		case PSObjPropBreakType:		if(outValue.GetKind() == RWValue::eValue_Integer)
											outValue.SetInteger (mBreakTypeI); 
										else
											outValue.SetText (RWStr::FromASCII (sBreakType [mBreakTypeI]));
										break;
		case PSObjPropAlias:			outValue.SetText (mAlias); break;

		default:						return DMSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBreakSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
//		case PSObjPropOrder:
//			break;

		case PSObjPropBreakOnField:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakTypeI = eBreakOn_Field;
				return true;
			}
			break;
		case PSObjPropBreakOnVariable:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakTypeI = eBreakOn_Variable;
				return true;
			}
			break;
		case PSObjPropBreakOnArray:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakTypeI = eBreakOn_Array;
				return true;
			}
			break;
		case PSObjPropBreakOn:			return SetStringProperty (inValue, mBreakOn);
		case PSObjPropBreakType:		return SetListProperty (inValue, sBreakType, mBreakTypeI);
		case PSObjPropAlias:			return SetStringProperty (inValue, mAlias);

		case PSObjPropBreakLevel:
		{
			int	level = 0;
			if (SetIntegerProperty (inValue, level, /* mSectionKind == eSectionKind_BreakHeader? 1: */ 0))
			{
				if (mParent)
					GetReport()->AdjustSections (this, level);
				else
					SetBreakLevel (level);
				return true;
			}
			break;
		}

		case PSObjPropPrintAlways:		return mSectionKind == eSectionKind_BreakHeader? SetBooleanProperty (inValue, mPrintAlways): false;

		default:						return DMSection::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMScrapSection*
DMScrapSection::Create (DMBase *inParent, RWXmlNode inNode, RWStringView inType)
{
	DMScrapSection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMScrapSection (inParent, !inType.empty() ? RWString (inType) : inNode.Name());
		sec->mName = u"Scrap";
		if (inNode)
			sec->LoadXML (inNode);
		sec->mDraw = false;
		sec->mVisible = false;
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMScrapSection							Constructor			   [protected]
// ---------------------------------------------------------------------------
DMScrapSection::DMScrapSection (DMBase *inParent, RWStringView inType)
	:	DMSection (inParent, eSectionKind_Scrap, inType)
{
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMPageSection*
DMPageSection::Create (DMBase *inParent, RWXmlNode inNode, RWStringView inType)
{
	DMPageSection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMPageSection (inParent, !inType.empty() ? RWString (inType) : inNode.Name());
		if (inNode)
			sec->LoadXML (inNode);
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMPageSection							Constructor			   [protected]
// ---------------------------------------------------------------------------
DMPageSection::DMPageSection (DMBase *inParent, RWStringView inType)
	:	DMSection (inParent, eSectionKind_Page, inType)
{
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMPageSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
//		case 'pgor':			outValue.SetText (mPageOrientation); break;
//		case 'pgsi':			outValue.SetText (mPageSize); break;

		default:				return DMSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMPageSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
//		case 'pgor':		return SetStringProperty (inValue, mPageOrientation);
//		case 'pgsi':		return SetStringProperty (inValue, mPageSize);

		default:			return DMSection::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMBodySection*
DMBodySection::Create (DMBase *inParent, RWXmlNode inNode, RWStringView inType)
{
	DMBodySection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMBodySection (inParent, !inType.empty() ? RWString (inType) : inNode.Name());
		if (inNode)
			sec->LoadXML (inNode);
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMBodySection							Constructor			   [protected]
// ---------------------------------------------------------------------------
DMBodySection::DMBodySection (DMBase *inParent, RWStringView inType)
	:	DMSection (inParent, eSectionKind_Body, inType)
{
}


#if	0
// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBodySection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		default:				return DMSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMBodySection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		default:			return DMSection::SetProperty (id, inValue);
	}

	return false;
}
#endif

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMWatermarkSection*
DMWatermarkSection::Create (DMBase *inParent, RWXmlNode inNode, RWStringView inType)
{
	DMWatermarkSection	*sec = NULL;
	if (inNode || !inType.empty())
	{
		sec = new DMWatermarkSection (inParent, !inType.empty() ? RWString (inType) : inNode.Name());
		if (inNode)
			sec->LoadXML (inNode);
	}

	return sec;
}


// ---------------------------------------------------------------------------
// DMWatermarkSection						Constructor			   [protected]
// ---------------------------------------------------------------------------
DMWatermarkSection::DMWatermarkSection (DMBase *inParent, RWStringView inType)
	:	DMHeaderFooterSection (inParent, eSectionKind_Watermark, inType),
		mOnTop (false)
{
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMWatermarkSection::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	mOnTop = false;

	DMHeaderFooterSection::LoadXML (inNode, pes);

	mHeight = 0;
	mMinSpace = 0;
	mKeepTogether = false;
	mFromBottom = false;
	mPageThrowI = ePageThrow_None;
	mFillPage = false;
	mFixed = -1;

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMWatermarkSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropHeight:
		case PSObjPropMinSpace:
		case PSObjPropKeepTogether:
		case PSObjPropBind:
		case PSObjPropPageThrow:
		case PSObjPropExpandV:
		case PSObjPropFixed:
		case PSObjPropFill:			return false;

		case PSObjPropOnTop:		outValue.SetBoolean (mOnTop); break;

		default:					return DMHeaderFooterSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMWatermarkSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropHeight:
		case PSObjPropMinSpace:
		case PSObjPropKeepTogether:
		case PSObjPropBind:
		case PSObjPropPageThrow:
		case PSObjPropExpandV:
		case PSObjPropFixed:
		case PSObjPropFill:			break;
		case PSObjPropOnTop:
		{
			bool	onTop = mOnTop;
			if (SetBooleanProperty (inValue, onTop))
			{
				if (mOnTop != onTop)
				{
					mOnTop = onTop;
					if (mParent)
						GetReport()->AdjustSections (NULL, 0);
				}
				return true;
			}
			break;
		}
			
		default:					return DMHeaderFooterSection::SetProperty (id, inValue);
	}

	return false;
}

DMBase::EHitTest	DMWatermarkSection::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	if (IsOnTop ())
		return DMSection::HitTest(inWhere, outObjectHit);
	else 
		return DMBase::eHit_None;
	

}
#pragma	mark	-

// ---------------------------------------------------------------------------
// DMDataSource								Constructor			   [protected]
// ---------------------------------------------------------------------------

DMDataSource::DMDataSource (DMBase *inParent)
	:	DMBase (inParent, eObject_DataSource),
		mRealDataSource (0)
{
}


// ---------------------------------------------------------------------------
// ~DMDataSource							Destructor				  [public]
// ---------------------------------------------------------------------------

DMDataSource::~DMDataSource (void)
{
	ClearDataSource();
	return;
}



// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DM4DDataSource*
DM4DDataSource::Create (DMBase *inParent, RWXmlNode inNode)
{
	DM4DDataSource	*src = NULL;
	if (inParent && inNode)
	{
assert (RWStr::EqualsNoCase (inNode.Attr (RWStr::FromASCII (FindPropertyByID (PSObjPropType, sProperties)->name)), s4DKind [0]));
		src = new DM4DDataSource (inParent);
		src->LoadXML (inNode);
	}
	else if (inParent)
		src = new DM4DDataSource (inParent);

	return src;
}


// ---------------------------------------------------------------------------
// DM4DDataSource							Constructor			   [protected]
// ---------------------------------------------------------------------------

DM4DDataSource::DM4DDataSource (DMBase *inParent)
	:	DMDataSource (inParent),
		mSourceI (eDataSource_Fixed),
		mNumIterations (1),
		mMainTable (0),
		mRelateOneI (eRelate_Manual),
		mRelateManyI (eRelate_None)
{
}


// ---------------------------------------------------------------------------
// DM4DDataSource							Destructor				  [public]
// ---------------------------------------------------------------------------

DM4DDataSource::~DM4DDataSource (void)
{
	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DM4DDataSource::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
assert (RWStr::EqualsNoCase (inNode.Attr (RWStr::FromASCII (FindPropertyByID (PSObjPropType, GetProperties())->name)), s4DKind [0]));

	mSourceI = eDataSource_Undefined;
	mNumIterations = -1;
	mMainTable = 0;
	mRelateOneI = eRelate_Manual;
	mRelateManyI = eRelate_None;
	mStartScript.Free();
	mBodyScript.Free();
	mEndScript.Free();
	mName.clear();
	mCallBackName.clear();

	DMBase::LoadXML (inNode, pes);
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DM4DDataSource::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropType:			outValue.SetText (RWStr::FromASCII (s4DKind [0])); break;
		case PSObjPropSource:		if(outValue.GetKind() == RWValue::eValue_Integer)
										outValue.SetInteger (mSourceI); 
									else
										outValue.SetText (RWStr::FromASCII (sSource [mSourceI]));
									break;
		case PSObjPropIterations:	outValue.SetInteger (mNumIterations); break;
		case PSObjPropTableID:		outValue.SetInteger (mMainTable); break;
		case PSObjPropName:			outValue.SetText (mName); break;
		case PSObjPropRelateOne:	if(outValue.GetKind() == RWValue::eValue_Integer)
										outValue.SetInteger (mRelateOneI); 
									else
										outValue.SetText (RWStr::FromASCII (sRelate [mRelateOneI]));
									break;
		case PSObjPropRelateMany:	if(outValue.GetKind() == RWValue::eValue_Integer)
									   outValue.SetInteger (mRelateManyI); 
									else
									   outValue.SetText (RWStr::FromASCII (sRelate [mRelateManyI]));
									break;
		case PSObjPropCallback:		outValue.SetText (mCallBackName); break;
		case PSObjPropStartScript:	outValue.SetText (mStartScript); break;
		case PSObjPropBodyScript:	outValue.SetText (mBodyScript); break;
		case PSObjPropEndScript:	outValue.SetText (mEndScript); break;
		case PSObjPropSRPCompatibility:	outValue.SetBoolean (mSRPCompatibility); break;

		default:					return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DM4DDataSource::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropType:			break;
		case PSObjPropSource:		return SetListProperty (inValue, sSource, mSourceI);
		case PSObjPropIterations:	return SetIntegerProperty (inValue, mNumIterations, 0);
		case PSObjPropTableID:		return SetIntegerProperty (inValue, mMainTable, 0);
		case PSObjPropName:			return SetStringProperty (inValue, mName);
		case PSObjPropRelateOne:	return SetListProperty (inValue, sRelate, mRelateOneI);
		case PSObjPropRelateMany:	return SetListProperty (inValue, sRelate, mRelateManyI);
		case PSObjPropCallback:		return SetStringProperty (inValue, mCallBackName);
		case PSObjPropStartScript:	return SetStringProperty (inValue, mStartScript);
		case PSObjPropBodyScript:	return SetStringProperty (inValue, mBodyScript);
		case PSObjPropEndScript:	return SetStringProperty (inValue, mEndScript);
		case PSObjPropSRPCompatibility:	return SetBooleanProperty (inValue, mSRPCompatibility);

		default:					return PSObject::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

DMGuide*
DMGuide::Create (DMBase *inParent, RWXmlNode inNode, bool inVertical)
{
	DMGuide	*guide = new DMGuide (inParent, inVertical);
	if (inNode)
		guide->LoadXML (inNode);

	return guide;
}

// ---------------------------------------------------------------------------
// DMGuide									Constructor			   [protected]
// ---------------------------------------------------------------------------

DMGuide::DMGuide (DMBase *inParent, bool inVertical)
	:	DMBase (inParent, eObject_Guide),
		mVertical (inVertical),
		mPos (-INFINITY)
{
}


// ---------------------------------------------------------------------------
// DMGuide									Destructor				  [public]
// ---------------------------------------------------------------------------

DMGuide::~DMGuide (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMGuide::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropData:			outValue.SetReal (mPos); break;
		case PSObjPropType:			outValue.SetBoolean (mVertical); break;

		default:					return DMBase::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMGuide::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropRelPosTop:
		case PSObjPropRelPosLeft:
		case PSObjPropRelPosBottom:
		case PSObjPropRelPosRight:
			break;
		case PSObjPropRelMoveH:
		case PSObjPropRelMoveV:
			if (mVertical == (id == PSObjPropRelMoveH))
			{
				float	fVal;
				if (SetRealProperty (inValue, fVal, -4096, 4096) && fVal != 0)
				{
					mPos += fVal;
					if (mVertical)
						mPosition.SetRect (0., mPos, 4096., mPos);
					else
						mPosition.SetRect (mPos, 0., mPos, 4096.);
					return true;
				}
			}				
			break;

		case PSObjPropRect:
			if (not DMBase::SetProperty (id, inValue))
				break;
			if (mVertical)
				inValue.SetReal (mPosition.left);
			else
				inValue.SetReal (mPosition.top);
			// fall through
		case PSObjPropData:
			if (SetRealProperty (inValue, mPos, -4096, 4096))
			{
				if (mVertical)
					mPosition.SetRect (0., mPos, 4096., mPos);
				else
					mPosition.SetRect (mPos, 0., mPos, 4096.);
				return true;
			}
			break;

		default:					return DMBase::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMGuide::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
    RWXmlNode	me = DMBase::WriteXML (inParent, pes);
	me.SetName (mVertical ? u"Vertical" : u"Horizontal");

	return me;
}

#pragma	mark	-


PSObjListD	DMReport::sReports;


// ---------------------------------------------------------------------------
// DMReport									Constructor				  [public]
// ---------------------------------------------------------------------------

DMReport::DMReport (RWXmlDocument *inXML)
	:	DMBase (NULL, eObject_Document),
		mSimple (false),
		mPageWidth (DEF_PAGE_WIDTH),
		mPageHeight (DEF_PAGE_HEIGHT),
		mUsePhysical (false),
		mPageMargins (DEF_PAGE_MARGIN, DEF_PAGE_MARGIN, DEF_PAGE_MARGIN, DEF_PAGE_MARGIN),
		mDataSource (0),
		mMaxBreakHeaderLevel (-1),
		mMaxBreakFooterLevel (-1),
		mShowMargins (true),
		mShowRuler (false),
		mRulerUnits (1),
		mGridSize (12),
		mShowGrid (false),
		mSnapToGrid (false),
		mShowGuides (true),
		mLockGuides (false),
		mSnapToGuide (false),
		mLockSections (false),
		mShowObjBorders (true),
		mScale (1.0),
// pB added 
		mGridRadius (0.75),
		mGridColor (cGrayColor),
		mGuideColor (cGreenColor),
		mGuideWidth (0.75),

//        mLabelReport (false),
//        mLabelH(0),
//        mLabelV(0),
//        mLabelMarginTop(0),
//        mLabelMarginLeft(0),
//        mLabelMarginBottom(0),
//        mLabelMarginRight(0),

        mReportRotation(false),
        mReportMirror(false),

		mDirty (false),
		mComposer (0)



{
	memset (mSeqIDs, 0, sizeof (mSeqIDs));
	sReports.push_back (this);

	mDataSource = DM4DDataSource::Create (this, RWXmlNode());

	{
		RWXmlDocument	doc;
		doc.LoadString (Editor_Style_1_XML);
		DMStyle	*style = DMStyle::Create (&mEditorStyles, this, doc.Root());
		style->SetOrder (1);
		mEditorStyles.push_back (static_cast <DMBase*> (style));
		doc.LoadString (Editor_Style_2_XML);
		style = DMStyle::Create (&mEditorStyles, this, doc.Root());
		style->SetOrder (2);
		mEditorStyles.push_back (static_cast <DMBase*> (style));
	}

	if (inXML)
	{
        RWXmlNode	elem = inXML->Root();
		if (elem)
			LoadXML (elem);
	}

	//mbs 26072010	copied here from LoadXMLObjects()
	if (mStyles.size() == 0 || GetStyle (0)->GetID() != 0)
	{
		RWXmlDocument	doc;
		doc.LoadString (Default_Style_0_XML);
		DMStyle	*style = DMStyle::Create (&mStyles, this, doc.Root());
		mStyles.insert (mStyles.begin(), static_cast <DMBase*> (style));
	}				
}


// ---------------------------------------------------------------------------
// ~DMReport								Destructor				  [public]
// ---------------------------------------------------------------------------

DMReport::~DMReport (void)
{
	if (sReports.size() > 0)
	{
		PSObject			*obj = static_cast <PSObject*> (this);
		PSObjList::iterator	iter = std::find (sReports.begin(), sReports.end(), obj);
		if (iter != sReports.end())
			sReports.erase (iter);
	}

	delete mDataSource;
	return;
}


// ---------------------------------------------------------------------------
// AddObject													   [protected]
// ---------------------------------------------------------------------------

void
DMReport::AddObject (DMBase *inObject)
{
	mObjects.push_back (inObject);

	return;
}


// ---------------------------------------------------------------------------
// DeleteObject													   [protected]
// ---------------------------------------------------------------------------

void
DMReport::DeleteObject (DMBase *inObject)
{
	if (inObject->GetSelected())
		RemoveSelectedObject (inObject);
	if (mObjects.size() > 0)
	{
		PSObject			*obj = static_cast <PSObject*> (inObject);
		PSObjList::iterator	iter = std::find (mObjects.begin(), mObjects.end(), obj);
		if (iter != mObjects.end())
			mObjects.erase (iter);
	}

	return;
}


// ---------------------------------------------------------------------------
// AddSelectedObject											   [protected]
// ---------------------------------------------------------------------------

void
DMReport::AddSelectedObject (DMBase *inObject)
{
	PSObject			*obj = static_cast <PSObject*> (inObject);
	PSObjList::iterator	iter = std::find (mSelectedObjects.begin(), mSelectedObjects.end(), obj);
	if (iter == mSelectedObjects.end())
		mSelectedObjects.push_back (inObject);

	return;
}


// ---------------------------------------------------------------------------
// RemoveSelectedObject											   [protected]
// ---------------------------------------------------------------------------

void
DMReport::RemoveSelectedObject (DMBase *inObject)
{
	if (mSelectedObjects.size() > 0)
	{
		PSObject			*obj = static_cast <PSObject*> (inObject);
		PSObjList::iterator	iter = std::find (mSelectedObjects.begin(), mSelectedObjects.end(), obj);
		if (iter != mSelectedObjects.end())
			mSelectedObjects.erase (iter);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetObject														  [public]
// ---------------------------------------------------------------------------

DMBase *
DMReport::GetObject (long inObject)
const
{
	if (inObject == 0L)
		return NULL;

	PSObject	*obj = GetMappedObject (inObject); //reinterpret_cast <const DMBase*> (inObject);
	if (this != obj && mObjects.size() > 0)
	{
//		const PSObject				*obj = static_cast <const PSObject*> (dmObj);
		PSObjList::const_iterator	iter = std::find (mObjects.begin(), mObjects.end(), obj);
		if (iter == mObjects.end())
			obj = NULL;
	}
	return reinterpret_cast <DMBase*> (obj);
}


// ---------------------------------------------------------------------------
// GetObjectByID													  [public]
// ---------------------------------------------------------------------------

DMBase *
DMReport::GetObjectByID (RWString &inName)
const
{
	PSObjList::const_iterator	iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
	{
		const DMBase	*obj = static_cast <const DMBase*> (*iter);
		if (obj != NULL)
			if (inName == static_cast <const RWString&> (obj->mID))
				return const_cast <DMBase*> (static_cast <const DMBase*> (obj));
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// GetReportObject											 [static] [public]
// ---------------------------------------------------------------------------

DMReport *
DMReport::GetReportObject (long inObject)
{
	if (inObject == 0L)
		return NULL;

	if (sReports.size() > 0)
	{
		PSObject					*obj = GetMappedObject (inObject);
//		DMBase						*dmObj = reinterpret_cast <DMBase*> (inObject);
//		PSObject					*obj = static_cast <PSObject*> (dmObj);
		PSObjList::const_iterator	iter = std::find (sReports.begin(), sReports.end(), obj);
		if (iter == sReports.end())
			obj = NULL;
		return const_cast <DMReport*> (static_cast <const DMReport*> (obj));
	}
	return NULL;
}


// ---------------------------------------------------------------------------
// GetReportOfObject										 [static] [public]
// ---------------------------------------------------------------------------

DMReport *
DMReport::GetReportOfObject (long inObject)
{
	if (inObject == 0L)
		return NULL;

	const DMReport	*rep = GetReportObject (inObject);
	if (rep != NULL)
		return const_cast <DMReport*> (rep);

	PSObjList::const_iterator	iter;
	for (iter = sReports.begin(); iter != sReports.end(); iter++)
	{
		rep = static_cast <const DMReport*> (*iter);
		DMBase	*obj = rep->GetObject (inObject);
		if (obj != NULL)
			return const_cast <DMReport*> (rep);
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// FindObject												 [static] [public]
// ---------------------------------------------------------------------------

DMBase *
DMReport::FindObject (long inObject)
{
	if (inObject == 0L)
		return NULL;

	const DMReport	*rep = GetReportObject (inObject);
	if (rep != NULL)
		return const_cast <DMBase*> (static_cast <const DMBase*> (rep));

	PSObjList::const_iterator	iter;
	for (iter = sReports.begin(); iter != sReports.end(); iter++)
	{
		rep = static_cast <const DMReport*> (*iter);
		DMBase	*obj = rep->GetObject (inObject);
		if (obj != NULL)
			return obj;
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// FindObjectByID											 [static] [public]
// ---------------------------------------------------------------------------

DMBase *
DMReport::FindObjectByID (RWString &inName)
{
	PSObjList::const_iterator	iter;
	for (iter = sReports.begin(); iter != sReports.end(); iter++)
	{
		const DMReport	*rep = static_cast <const DMReport*> (*iter);
		if (inName == static_cast <const RWString&> (rep->mID))
			return const_cast <DMBase*> (static_cast <const DMBase*> (rep));

		DMBase	*obj = rep->GetObjectByID (inName);
		if (obj != NULL)
			return obj;
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// SetReport														  [public]
// ---------------------------------------------------------------------------

void
DMReport::SetReport (RWXmlDocument *inXML)
{
//	mObjects.clear();
	mSelectedObjects.clear();
	mGuides.clear();
	mStyles.clear();
	mSections.clear();

	delete mDataSource;
	mID.clear();
	mName.clear();

	mMaxBreakHeaderLevel = -1;
	mMaxBreakFooterLevel = -1;

	mSelected = false;
	mSimple = false;
	mPageWidth = DEF_PAGE_WIDTH;
	mPageHeight = DEF_PAGE_HEIGHT;
	mUsePhysical = false;
	mShowObjBorders = true;  // pB default
	mPageMargins.SetRect (DEF_PAGE_MARGIN, DEF_PAGE_MARGIN, DEF_PAGE_MARGIN, DEF_PAGE_MARGIN);
	mDataSource = 0;
	memset (mSeqIDs, 0, sizeof (mSeqIDs));
	mDataSource = DM4DDataSource::Create (this, RWXmlNode());
    
//    mLabelReport = false;
//    mLabelH = 0;
//    mLabelV = 0;
//    mLabelMarginTop = 0;
//    mLabelMarginLeft = 0;
//    mLabelMarginBottom = 0;
//    mLabelMarginRight = 0;
    
    mReportRotation = false;
    mReportMirror = false;
    
	if (inXML)
	{
        RWXmlNode	elem = inXML->Root();
		if (elem)
			LoadXML (elem);
	}
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

void
DMReport::GetReport (RWXmlDocument &outXML)
{
	outXML.AddDeclaration (u"utf-8", true);
	WriteXML (outXML.Node());
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
DMReport::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	DMBase::LoadXML (inNode, pes);
	if (mSections.begin() != mSections.end())
		static_cast <DMBase*> (*mSections.begin())->AdjustOrder (eOrder_Sequentially, 0);
	std::sort<PSObjListD::iterator, DMObjectCompareOrder> (mSections.begin(), mSections.end(), DMObjectCompareOrder());
	AdjustSections (NULL, 0);

	//mbs 26072010	moved here from LoadXMLObjects()
	if (mStyles.size() == 0 || GetStyle (0)->GetID() != 0)
	{
		RWXmlDocument	doc;
		doc.LoadString (Default_Style_0_XML);
		DMStyle	*style = DMStyle::Create (&mStyles, this, doc.Root());
		mStyles.insert (mStyles.begin(), static_cast <DMBase*> (style));
	}				

	if (mSelectedObjects.size() == 0)
		SetSelected (true);
	CalculatePosition();
	return;
}


// ---------------------------------------------------------------------------
// CalculatePosition											   [protected]
// ---------------------------------------------------------------------------

void
DMReport::CalculatePosition (void)
{
	mPosition.SetRect (0, 0, DEF_PAGE_WIDTH, DEF_PAGE_HEIGHT);
	if (mPageWidth > 0 && mPageHeight > 0)
	{
		if (mUsePhysical)
		{
//			mPosition.SetRect (0., 0., mPageHeight + mPageMargins.top + mPageMargins.bottom, mPageWidth + mPageMargins.left + mPageMargins.right);
			mPosition.SetRect (0., 0., mPageHeight, mPageWidth);
		}
		else
		{
//			mPosition.SetRect (0., 0., mPageHeight, mPageWidth);
			mPosition.SetRect (0., 0., mPageHeight - mPageMargins.top - mPageMargins.bottom, mPageWidth - mPageMargins.left - mPageMargins.right);
		}
	}
	return;
}


// ---------------------------------------------------------------------------
// CreateObject														  [public]
// ---------------------------------------------------------------------------

DMBase*
DMReport::CreateObject (OSType inKind, long inParent, RWXmlNode inNode)
{
	DMBase *parent = NULL;
	if (inParent)
		parent = GetObject (inParent);
	return CreateObject (inKind, parent, inNode);
}

DMBase*
DMReport::CreateObject (OSType inKind, DMBase *inParent, RWXmlNode inNode)
{
	DMSection	*sec = NULL;
	DMBase		*obj = NULL;
	bool		special = false;	//mbs 06082010	assign ID to Guides and Styles
	PSObjListD	*objList = NULL;
	if (inParent)
		inParent->GetObjects (PSObjPropObjects, objList);

	// drawing objects live in a section or group: without one their constructors
	// dereferenced the missing parent (RW_NewObject with a wrong parent reference crashed 4D)
	switch (inKind)
	{
		case PSObjPropOGroup:	case PSObjPropOLine:	case PSObjPropORect:
		case PSObjPropOOval:	case PSObjPropOPict:	case PSObjPropOText:
		case PSObjPropOVar:		case PSObjPropOFld:		case PSObjPropOTable:
			if (inParent == NULL || objList == NULL)
				return NULL;
			break;
		default:
			break;
	}

	switch (inKind)
	{
		case PSObjPropOGuideH:
		case PSObjPropOGuideV:
		{
			DMGuide	*guide = DMGuide::Create (this, inNode, inKind == PSObjPropOGuideV);
			mGuides.push_back (guide);
			guide->SetOrder (mGuides.size());
			obj = guide;
			special = true;
			break;
		}
		case PSObjPropOStyle:
		{
			DMStyle	*style = DMStyle::Create (&mStyles, this, inNode);
			mStyles.push_back (static_cast <DMBase*> (style));
			obj = style;
			special = true;
			break;
		}

		case PSObjPropDataSource:
		{
			if (mDataSource)
				delete mDataSource;
			mDataSource = 0;
			mDataSource = DM4DDataSource::Create (this, RWXmlNode());
			return mDataSource;
			break;
		}

		case PSObjPropHeaderSection:
			if (not mSimple)
				sec = DMHeaderFooterSection::Create (this, inNode, DMSection::eSectionKind_Header, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;
		case PSObjPropBrkHdrSection:
			if (not mSimple)
			{
				sec = DMBreakSection::Create (this, inNode, DMSection::eSectionKind_BreakHeader, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
				static_cast <DMBreakSection*> (sec)->SetBreakLevel (++mMaxBreakHeaderLevel);
			}
			break;
		case PSObjPropScrapSection:
			sec = DMScrapSection::Create (this, inNode, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;
		case PSObjPropPageSection:
			if (mSimple)
				sec = DMPageSection::Create (this, inNode, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;
		case PSObjPropBodySection:
			if (not mSimple)
				sec = DMBodySection::Create (this, inNode, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;
		case PSObjPropBrkFtrSection:
			if (not mSimple)
			{
				sec = DMBreakSection::Create (this, inNode, DMSection::eSectionKind_BreakFooter, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
				static_cast <DMBreakSection*> (sec)->SetBreakLevel (++mMaxBreakFooterLevel);
			}
			break;
		case PSObjPropFooterSection:
			if (not mSimple)
				sec = DMHeaderFooterSection::Create (this, inNode, DMSection::eSectionKind_Footer, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;
		case PSObjPropWatermarkSection:
			sec = DMWatermarkSection::Create (this, inNode, RWStr::FromASCII (FindPropertyByID (inKind, GetProperties())->name));
			break;

		case PSObjPropOGroup:	obj = DMGroup::Create (inParent, inNode); break;
		case PSObjPropOLine:	obj = DMLine::Create (inParent, inNode); break;
		case PSObjPropORect:	obj = DMRect::Create (inParent, inNode); break;
		case PSObjPropOOval:	obj = DMOval::Create (inParent, inNode); break;
		case PSObjPropOPict:	obj = DMPict::Create (inParent, inNode); break;
		case PSObjPropOText:	obj = DMText::Create (inParent, inNode); break;
		case PSObjPropOVar:		obj = DMVariable::Create (inParent, inNode); break;
		case PSObjPropOFld:		obj = DMField::Create (inParent, inNode); break;
		case PSObjPropOTable:	obj = DMTable::Create (inParent, inNode); break;
		default:
			throw -1L;
	}

	if (special)
		;
	else if (sec)
	{
		SRect	pos = sec->GetPosition();
		pos.SetRect (0., 0., pos.Height(), 4096.);
		sec->SetPosition (pos);
		mSections.push_back (sec);
		sec->SetOrder (mSections.size());
		obj = sec;
		AdjustSections (dynamic_cast <DMBreakSection*> (sec), -1);
	}
	else if (obj)
	{
		if (objList)
		{
			objList->push_back (obj);
			obj->SetOrder (objList->size());
		}
		else
		{
			delete obj;
			obj = NULL;
		}
	}

	if (obj)
	{
		RWValue	id;
		if (obj->GetProperty (PSObjPropID, id))
		{
			if (id.IsEmpty())
			{
				if (obj->GetKind() == eObject_Style)
				{
					long	last = 0;
					if (mStyles.size() > 0)
					{
						PSStyleListD::iterator	iter;
						RWStyle	*style = NULL;
						long	cur;
						for (iter = mStyles.begin(); iter != mStyles.end(); iter++)
						{
							style = static_cast <RWStyle*> (static_cast <DMStyle*> (static_cast <DMBase*> (*iter)));	// style = dynamic_cast <const DMStyle*> (*iter);
							cur = style->GetID();
							if (cur < kFirstClonedStyle)
							{
								if (cur > last) 
									last = cur;
							}
							else
								break;
						}
					}
					id.SetInteger (last + 1);
				}
				else
				{
					char	buf [32];
					snprintf (buf, sizeof (buf), "%s_%ld", sKind [obj->GetKind()], ++mSeqIDs [obj->GetKind()]);
					id.SetText (RWStr::FromASCII (buf));
				}
				obj->SetProperty (PSObjPropID, id);
			}
		}
	}
	
	return obj;
}


// ---------------------------------------------------------------------------
// CreateObject														  [public]
// ---------------------------------------------------------------------------

DMBase*
DMReport::CreateObject (RWXmlNode inNode, long inParent)
{
	static const PSObject::PSObjProps	sAllowed[] = {
		{ PSObjPropOStyle,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Style],	{ NULL }					},

		{ PSObjPropOGuideH,			true,	PSProps_Childs,		PSProps_Objects,	"Horizontal",			{ NULL }					},
		{ PSObjPropOGuideV,			true,	PSProps_Childs,		PSProps_Objects,	"Vertical",				{ NULL }					},

		{ PSObjPropHeaderSection,	true,	PSProps_Childs,		PSProps_Objects,	"Header",				{ NULL }					},
		{ PSObjPropBrkHdrSection,	true,	PSProps_Childs,		PSProps_Objects,	"BreakHeader",			{ NULL }					},
		{ PSObjPropScrapSection,	true,	PSProps_Childs,		PSProps_Objects,	"Scrap",				{ NULL }					},
		{ PSObjPropPageSection,		true,	PSProps_Childs,		PSProps_Objects,	"Page",					{ NULL }					},
		{ PSObjPropBodySection,		true,	PSProps_Childs,		PSProps_Objects,	"Body",					{ NULL }					},
		{ PSObjPropBrkFtrSection,	true,	PSProps_Childs,		PSProps_Objects,	"BreakFooter",			{ NULL }					},
		{ PSObjPropFooterSection,	true,	PSProps_Childs,		PSProps_Objects,	"Footer",				{ NULL }					},
		{ PSObjPropWatermarkSection,true,	PSProps_Childs,		PSProps_Objects,	"Watermark",			{ NULL }					},

		{ PSObjPropOGroup,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Group],	{ NULL }					},
		{ PSObjPropOLine,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Line],	{ NULL }					},
		{ PSObjPropORect,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Rect],	{ NULL }					},
		{ PSObjPropOOval,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Oval],	{ NULL }					},
		{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Pict],	{ NULL }					},
		{ PSObjPropOText,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Text],	{ NULL }					},
		{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Var],		{ NULL }					},
		{ PSObjPropOFld,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Fld],		{ NULL }					},
		{ PSObjPropOTable,			true,	PSProps_Childs,		PSProps_Objects,	sKind[eObject_Table],	{ NULL }					},
		{ PSObjPropOPict,			true,	PSProps_Childs,		PSProps_Objects,	"Picture",				{ NULL }					},
		{ PSObjPropOVar,			true,	PSProps_Childs,		PSProps_Objects,	"Variable",				{ NULL }					},
		{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,					{ NULL }					}
	};

	for (RWXmlNode elem = inNode; elem; elem = elem.NextElement())
	{
		if (!elem.IsElement())
			continue;
		const PSObjProps*	pes = FindPropertyByName (elem.Name(), sAllowed);
		if (pes)
		{
			DMBase	*obj = CreateObject (pes->id, inParent, elem);
			return obj;
		}
	}

	return NULL;
}


// ---------------------------------------------------------------------------
// RemoveObject														  [public]
// ---------------------------------------------------------------------------

bool
DMReport::RemoveObject (DMBase *inObject)
{
	PSObjListD	*objects = NULL;
	DMBase		*parent = inObject->GetParent();
	if (parent == this)
	{
		switch (inObject->GetKind())
		{
			case eObject_Style:		objects = &mStyles; break;
			case eObject_Guide:		objects = &mGuides; break;
			case eObject_Section:	objects = &mSections; break;
			default:	break;	// to shut up compiler
		}
	}
	else
		parent->GetObjects (PSObjPropObjects, objects);

	if (objects != NULL)
	{
		PSObject			*obj = NULL;
		DMArea				*a = NULL;
		if (mComposer != NULL)
			a = static_cast <DMArea*> (this);
		if (inObject->GetSelected())
			inObject->SetSelected (false);
        if (a != NULL)
            a->CheckCurrentObject(inObject);
		if (a != NULL && a->IsRecordingUndo())	//mbs 22122009	need to record all deleted objects... (a is NULL without an editor area)
		{
			PSObjListD	*subobjects = NULL;
			if (inObject->GetObjects (PSObjPropObjects, subobjects))
			{
				PSObjList::reverse_iterator	rit;
				for (rit = subobjects->rbegin(); rit != subobjects->rend(); rit++)
					RemoveObject (static_cast <DMBase*> (*rit));
			}
		}
        obj = static_cast <PSObject*> (inObject);
		PSObjList::iterator	iter = std::find (objects->begin(), objects->end(), obj);
		if (iter != objects->end())
			objects->erase (iter);
//		if (parent != this)
			parent->AdjustOrder (eOrder_Deleted, 0);
		if (inObject->GetKind() == eObject_Section && dynamic_cast <DMBreakSection*> (inObject) != NULL)
			AdjustSections (NULL, 0);
		if (a != NULL && inObject->GetKind() == PSObject::eObject_Style)	//mbs 28122009	notify renderer
			a->GetPageComposer()->StyleChanged (static_cast <DMStyle*> (inObject));
        if (a != NULL) {
			a->AddUndoDelete (inObject);
        }
		delete inObject;
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// ChangeObjectParent												  [public]
// ---------------------------------------------------------------------------

bool
DMReport::ChangeObjectParent (DMBase *inObject, long inParent)
{
	DMBase		*parent = inObject->GetParent();
	DMBase		*newParent = GetObject (inParent);
	if (parent == this || parent == NULL || newParent == NULL || newParent == this)
		return false;

	PSObjListD	*objects = NULL;
	PSObjListD	*newObjects = NULL;
	parent->GetObjects (PSObjPropObjects, objects);
	newParent->GetObjects (PSObjPropObjects, newObjects);
	
	if (objects != NULL && newObjects != NULL)
	{
		PSObjList::iterator	iter = std::find (objects->begin(), objects->end(), inObject);
		if (iter != objects->end())
			objects->erase (iter);
		parent->AdjustOrder (eOrder_Deleted, 0);
		newObjects->push_back (inObject);
		inObject->SetParent (newParent);
		inObject->AdjustOrder (eOrder_Set, newObjects->size());
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// AdjustSections													  [public]
// ---------------------------------------------------------------------------

void
DMReport::AdjustSections (DMBreakSection *inSection, int inLevel)
{
	DMSection			*sec = NULL;
	DMBreakSection		*bsec = NULL;
	PSObjList::iterator	iter;

	if (inSection == NULL || inLevel < 0)
	{
		mMaxBreakHeaderLevel = -1;
		mMaxBreakFooterLevel = -1;

		for (iter = mSections.begin(); iter != mSections.end(); iter++)
		{
			sec = static_cast <DMSection*> (*iter);
			if (sec->GetSectionKind() == DMSection::eSectionKind_BreakHeader)
			{
				bsec = static_cast <DMBreakSection*> (*iter);
				if (bsec->GetBreakLevel() > mMaxBreakHeaderLevel)
					mMaxBreakHeaderLevel = bsec->GetBreakLevel();
			}
			if (sec->GetSectionKind() == DMSection::eSectionKind_BreakFooter)
			{
				bsec = static_cast <DMBreakSection*> (*iter);
				if (bsec->GetBreakLevel() > mMaxBreakFooterLevel)
					mMaxBreakFooterLevel = bsec->GetBreakLevel();
			}
		}
	}
	else if (inSection->GetBreakLevel() != inLevel)
	{
		DMSection::ESection_Kind	kind = inSection->GetSectionKind();
		if (kind == DMSection::eSectionKind_BreakHeader && inLevel > mMaxBreakHeaderLevel)
		{
			inSection->SetBreakLevel (inLevel);
			mMaxBreakHeaderLevel = inLevel;
		}
		else if (kind == DMSection::eSectionKind_BreakFooter && inLevel > mMaxBreakFooterLevel)
		{
			inSection->SetBreakLevel (inLevel);
			mMaxBreakFooterLevel = inLevel;
		}
		else
		{
			bool	goingUp = inSection->GetBreakLevel() > inLevel;
			if (kind == DMSection::eSectionKind_BreakFooter)
				goingUp = not goingUp;
			inSection->SetBreakLevel (inLevel);
			iter = std::find (mSections.begin(), mSections.end(), inSection);
			if (goingUp)
			{
				PSObjList::reverse_iterator	riter;
				for (riter = PSObjList::reverse_iterator (iter); riter != mSections.rend(); riter++)
				{
					sec = static_cast <DMSection*> (*riter);
					if (sec->GetSectionKind() != kind)
						break;
					bsec = static_cast <DMBreakSection*> (sec);
					if (bsec == inSection)
						;
					else if (bsec->GetBreakLevel() == inLevel)
						if (kind == DMSection::eSectionKind_BreakFooter)
							bsec->SetBreakLevel (--inLevel);
						else
							bsec->SetBreakLevel (++inLevel);
				}
			}
			else
			{
				for ( ; iter != mSections.end(); iter++)
				{
					sec = static_cast <DMSection*> (*iter);
					if (sec->GetSectionKind() != kind)
						break;
					bsec = static_cast <DMBreakSection*> (sec);
					if (bsec == inSection)
						;
					else if (bsec->GetBreakLevel() == inLevel)
						if (kind == DMSection::eSectionKind_BreakFooter)
							bsec->SetBreakLevel (++inLevel);
						else
							bsec->SetBreakLevel (--inLevel);
				}
			}
			AdjustSections (NULL, 0);
			return;
		}
	}

	if (mSections.size() > 0)
	{
		std::sort<PSObjListD::iterator, DMObjectCompareOrder> (mSections.begin(), mSections.end(), DMObjectCompareOrder());
		static_cast <DMBase*> (*mSections.begin())->AdjustOrder (eOrder_Sequentially, 0);
	}
}

#if 0
// ---------------------------------------------------------------------------
// CloneStyle														  [public]
// ---------------------------------------------------------------------------

DMStyle*
DMReport::CloneStyle (DMBase *inObject, DMStyle *inStyle)
{
	long	id = mStyles.GetNewID (kFirstClonedStyle);
	DMStyle	*style = inStyle->Clone (&mStyles, this, id);
	mStyles.push_back (static_cast <DMBase*> (style));
	if (mComposer != NULL)
		static_cast <DMArea*> (this)->AddUndoCreate (style);

	RWValue	v;
	inObject->GetProperty (PSObjPropID, v);
	style->SetProperty (PSObjPropName, v);
	return style;
}
#endif

// ---------------------------------------------------------------------------
// ParseObjects														  [public]
// ---------------------------------------------------------------------------

void
DMReport::ParseObjects (const PSObjProps* pes, RWXmlNode inNode, DMBase *inParent, PSObjListD &objList)
{
	DMObject	*obj = NULL;
	switch (pes->id)
	{
		case PSObjPropOGroup:	obj = DMGroup::Create (inParent, inNode); break;
		case PSObjPropOLine:	obj = DMLine::Create (inParent, inNode); break;
		case PSObjPropORect:	obj = DMRect::Create (inParent, inNode); break;
		case PSObjPropOOval:	obj = DMOval::Create (inParent, inNode); break;
		case PSObjPropOPict:	obj = DMPict::Create (inParent, inNode); break;
		case PSObjPropOText:	obj = DMText::Create (inParent, inNode); break;
		case PSObjPropOVar:		obj = DMVariable::Create (inParent, inNode); break;
		case PSObjPropOFld:		obj = DMField::Create (inParent, inNode); break;
		case PSObjPropOTable:	obj = DMTable::Create (inParent, inNode); break;
	}
	if (obj)
	{
		objList.push_back (obj);
		++mSeqIDs [obj->GetKind()];
	}
}


// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
DMReport::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
	DMSection	*sec = NULL;

	switch (pes->id)
	{
		case PSObjPropEditor:	// just a container
			PSObject::LoadXML (inNode, sPropertiesEditor);
			break;

		case PSObjPropOGuides:	// just a container
			PSObject::LoadXML (inNode, sPropertiesGuides);
			break;

		case PSObjPropOGuideH:
		case PSObjPropOGuideV:
		{
			DMGuide	*guide = DMGuide::Create (this, inNode, pes->id == PSObjPropOGuideV);
			mGuides.push_back (guide);
			guide->SetOrder (mGuides.size());
			++mSeqIDs [guide->GetKind()];
			break;
		}

		case PSObjPropDataSource:
		{
			if (RWStr::EqualsNoCase (inNode.Attr (u"type"), s4DKind [0]))
			{
				delete mDataSource;
				mDataSource = 0;
				mDataSource = DM4DDataSource::Create (this, inNode);
				++mSeqIDs [mDataSource->GetKind()];
			}
			break;
		}

/*
		case PSObjPropPageSetup:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mPageSetup.SetBlob (data, true);
			break;
		}
*/

		case PSObjPropPageFormat:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mPageFormat.SetBlob (data, true);
#if	MACVER
			if (GetPageComposer())
			{
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetPageFormat (data);
				if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
					CalculatePosition();
			}
#endif
			break;
		}

		case PSObjPropPrintSettings:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mPrintSettings.SetBlob (data, true);
#if	MACVER
			if (GetPageComposer())
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetPrintSettings (data);
#endif
			break;
		}

		case PSObjPropDevMode:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mDevMode.SetBlob (data, true);
#if	WINVER
			if (GetPageComposer())
			{
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetDevMode (data);
				/*if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
					CalculatePosition();*/
			}
#endif
			break;
		}

		case PSObjPropDeviceNames:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mDeviceNames.SetBlob (data, true);
#if	WINVER
			if (GetPageComposer())
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetDeviceNames (data);
#endif
			break;
		}

		case PSObjPropPageSetupDlg:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mPageSetupDialog.SetBlob (data, true);
#if	WINVER
			if (GetPageComposer())
			{
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetPageSetupDialog (data);
				if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
					CalculatePosition();
			}
#endif
			break;
		}

		case PSObjPropPrintDlg:
		{
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			mPrintDialog.SetBlob (data, true);
#if	WINVER
			if (GetPageComposer())
				static_cast <RWNativePageComposer*> (GetPageComposer())->SetPrintDialog (data);
#endif
			break;
		}


		case PSObjPropStyleSet:	// just a container
			PSObject::LoadXML (inNode, sPropertiesStyles);
			break;

		case PSObjPropOStyle:
		{
			DMStyle	*style = DMStyle::Create (&mStyles, this, inNode);
			mStyles.push_back (static_cast <DMBase*> (style));
			break;
		}

		case PSObjPropHeaderSection:	if (not mSimple)	sec = DMHeaderFooterSection::Create (this, inNode, DMSection::eSectionKind_Header, RWStringView()); break;
		case PSObjPropBrkHdrSection:	if (not mSimple)	sec = DMBreakSection::Create (this, inNode, DMSection::eSectionKind_BreakHeader, RWStringView()); break;
		case PSObjPropScrapSection:							sec = DMScrapSection::Create (this, inNode, RWStringView()); break;
		case PSObjPropPageSection:		if (mSimple)		sec = DMPageSection::Create (this, inNode, RWStringView()); break;
		case PSObjPropBodySection:		if (not mSimple)	sec = DMBodySection::Create (this, inNode, RWStringView()); break;
		case PSObjPropBrkFtrSection:	if (not mSimple)	sec = DMBreakSection::Create (this, inNode, DMSection::eSectionKind_BreakFooter, RWStringView()); break;
		case PSObjPropFooterSection:	if (not mSimple)	sec = DMHeaderFooterSection::Create (this, inNode, DMSection::eSectionKind_Footer, RWStringView()); break;
		case PSObjPropWatermarkSection:						sec = DMWatermarkSection::Create (this, inNode, RWStringView()); break;
	}

	if (sec)
	{
		mSections.push_back (sec);
		sec->SetOrder (mSections.size());
		++mSeqIDs [sec->GetKind()];
	}
	return;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

PSObjList *
DMReport::GetObjects (OSType id)
{
	PSObjList	*l = NULL;

	switch (id)
	{
		case PSObjPropObjects:			l = new PSObjList (mObjects); break;
		case PSObjPropSelected:			l = new PSObjList (mSelectedObjects); break;
		case PSObjPropOStyle:			l = new PSObjList (mEditorStyles); break;
		case PSObjPropDataSource:		if (mDataSource != NULL) l = new PSObjList (1, mDataSource); break;

		case PSObjPropOGuides:			l = new PSObjList (mGuides); break;
		case PSObjPropStyleSet:			l = new PSObjList (mStyles); break;
		case PSObjPropHeaderSection:
		case PSObjPropBrkHdrSection:
		case PSObjPropScrapSection:
		case PSObjPropPageSection:
		case PSObjPropBodySection:
		case PSObjPropBrkFtrSection:
		case PSObjPropFooterSection:
		case PSObjPropWatermarkSection:	l = new PSObjList (mSections); break;
	}

	return l;
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
DMReport::GetObjects (OSType id, PSObjListD* &outList)
{
	switch (id)
	{
		case PSObjPropOGuides:			outList = &mGuides; return true;
		case PSObjPropStyleSet:			outList = &mStyles; return true;
		case PSObjPropHeaderSection:
		case PSObjPropBrkHdrSection:
		case PSObjPropScrapSection:
		case PSObjPropPageSection:
		case PSObjPropBodySection:
		case PSObjPropBrkFtrSection:
		case PSObjPropFooterSection:
		case PSObjPropWatermarkSection:	outList = &mSections; return true;
	}

	return false;
}


// ---------------------------------------------------------------------------
// WriteXMLObjects												   [protected]
// ---------------------------------------------------------------------------

bool
DMReport::WriteXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
	switch (pes->id)
	{
		case PSObjPropEditor:	// just a container
			PSObject::WriteXML (inNode, sPropertiesEditor);
			break;

		case PSObjPropOGuides:	// just a container
			break;

		case PSObjPropDataSource:
			if (mDataSource == NULL)
				return false;
			mDataSource->WriteXML (inNode);
			break;

/*
		case PSObjPropPageSetup:
			if (mPageSetup.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Classic");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mPageSetup.GetBlob());
			}
			else
				return false;
			break;
*/

		case PSObjPropPageFormat:
			if (mPageFormat.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Carbon");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mPageFormat.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropPrintSettings:
			if (mPrintSettings.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Carbon");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mPrintSettings.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropDevMode:
			if (mDevMode.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Win32");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mDevMode.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropDeviceNames:
			if (mDeviceNames.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Win32");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mDeviceNames.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropPageSetupDlg:
			if (mPageSetupDialog.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Win32");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mPageSetupDialog.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropPrintDlg:
			if (mPrintDialog.GetBlobSize() > 0)
			{
				inNode.SetAttr (u"kind", u"Win32");
				inNode.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (inNode, mPrintDialog.GetBlob());
			}
			else
				return false;
			break;

		case PSObjPropStyleSet:	// just a container
			break;

		case PSObjPropOStyle:
		case PSObjPropHeaderSection:
			break;
		case PSObjPropBrkHdrSection:
		case PSObjPropScrapSection:
		case PSObjPropPageSection:
		case PSObjPropBodySection:
		case PSObjPropBrkFtrSection:
		case PSObjPropFooterSection:
		case PSObjPropWatermarkSection:
			return false;
	}
	return true;
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
DMReport::WriteXML (RWXmlNode inParent, const PSObjProps* pes)
{
    RWXmlNode	me = DMBase::WriteXML (inParent, pes);
//	me->SetAttribute (FindPropertyByID (PSObjPropVersion, GetProperties())->name, CURRENT_VERSION);
	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMReport::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropVersion:			outValue.SetReal (CURRENT_VERSION); break;
		case PSObjPropName:				outValue.SetText (mName); break;
		case PSObjPropDynamic:			outValue.SetBoolean (mSimple); break;
		case PSObjPropWidth:			outValue.SetReal (mPageWidth); break;
		case PSObjPropHeight:			outValue.SetReal (mPageHeight); break;
		case PSObjPropPaper:			outValue.SetBoolean (mUsePhysical); break;
		case PSObjPropMargins:			outValue.SetText (mPageMargins.ToString()); break;
//		case PSObjPropPageSetup:		outValue.Attach (mPageSetup); break;
		case PSObjPropPageFormat:		outValue.Attach (mPageFormat); break;
		case PSObjPropPrintSettings:	outValue.Attach (mPrintSettings); break;
		case PSObjPropDevMode:			outValue.Attach (mDevMode); break;
		case PSObjPropDeviceNames:		outValue.Attach (mDeviceNames); break;
		case PSObjPropPageSetupDlg:		outValue.Attach (mPageSetupDialog); break;
		case PSObjPropPrintDlg:			outValue.Attach (mPrintDialog); break;

// 1.4
//		case PSObjPropLabel:			outValue.SetBoolean (mLabelReport); break;
//		case PSObjPropLabelH:			outValue.SetInteger (mLabelH); break;
//		case PSObjPropLabelV:			outValue.SetInteger (mLabelV); break;
//
//		case PSObjPropLabelMTop:		outValue.SetInteger (mLabelMarginTop); break;
//		case PSObjPropLabelMLeft:		outValue.SetInteger (mLabelMarginLeft); break;
//		case PSObjPropLabelMBottom:		outValue.SetInteger (mLabelMarginBottom); break;
//		case PSObjPropLabelMRight:		outValue.SetInteger (mLabelMarginRight); break;

        case PSObjPropObjectRotation:    outValue.SetBoolean (mReportRotation); break;
        case PSObjPropMirror:            outValue.SetBoolean (mReportMirror); break;

//		case PSObjPropDataSource:		outValue.SetInteger ((long) mDataSource); break;

		// Editor properties
		case PSObjPropShowMargins:		outValue.SetBoolean (mShowMargins); break;
		case PSObjPropShowRuler:		outValue.SetBoolean (mShowRuler); break;
		case PSObjPropRulerUnits:		outValue.SetInteger (mRulerUnits); break;
		case PSObjPropGridSize:			outValue.SetReal (mGridSize); break;
		case PSObjPropShowGrid:			outValue.SetBoolean (mShowGrid); break;
		case PSObjPropSnapToGrid:		outValue.SetBoolean (mSnapToGrid); break;
		case PSObjPropShowGuides:		outValue.SetBoolean (mShowGuides); break;
		case PSObjPropLockGuides:		outValue.SetBoolean (mLockGuides); break;
		case PSObjPropSnapToGuides:		outValue.SetBoolean (mSnapToGuide); break;
		case PSObjPropLockSections:		outValue.SetBoolean (mLockSections); break;
		case PSObjPropShowObjBorders:	outValue.SetBoolean (mShowObjBorders); break;
			// pB added
		case PSObjPropGridColor:		outValue.SetText (mGridColor.ToString()); break;
		case PSObjPropGridRadius:		outValue.SetReal (mGridRadius); break;
		case PSObjPropGuideColor:		outValue.SetText (mGuideColor.ToString()); break;
		case PSObjPropGuideWidth:		outValue.SetReal (mGuideWidth); break;
			
		case PSObjPropScale:			outValue.SetReal (mScale); break;

		default:						return DMBase::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMReport::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropXML:			break;
		case PSObjPropPosLeft:
		case PSObjPropPosRight:
		case PSObjPropPosWidth:
		case PSObjPropRelPosLeft:
		case PSObjPropRelPosRight:
		case PSObjPropRelMoveH:
		case PSObjPropRelMoveV:
			break;
			
		case PSObjPropName:			return SetStringProperty (inValue, mName);
		case PSObjPropDynamic:
		{
			bool	simple = mSimple;
			if (SetBooleanProperty (inValue, simple))
			{
				if (simple != mSimple)
				{
					PSObjList::iterator	iter;
					for (iter = mSections.begin(); iter != mSections.end(); iter++)
					{
						DMSection	*sec = static_cast <DMSection*> (*iter);
						if (sec != NULL && sec->GetSectionKind() != DMSection::eSectionKind_Watermark)
						{
							if (simple != (sec->GetSectionKind() == DMSection::eSectionKind_Page))
								return false;
						}
					}
					mSimple = simple;
				}
				return true;
			}
			break;
		}

		case PSObjPropWidth:		return SetRealProperty (inValue, mPageWidth, 32, 4096);
		case PSObjPropHeight:		return SetRealProperty (inValue, mPageHeight, 32, 4096);
		case PSObjPropPaper:		return SetBooleanProperty (inValue, mUsePhysical);
		case PSObjPropMargins:		return SetRectProperty (inValue, mPageMargins);

//		case PSObjPropLabel:			return SetBooleanProperty (inValue, mLabelReport);
//		case PSObjPropLabelH:			return SetIntegerProperty (inValue, mLabelH);
//		case PSObjPropLabelV:			return SetIntegerProperty (inValue, mLabelV);

        case PSObjPropObjectRotation:    return SetBooleanProperty (inValue, mReportRotation);
        case PSObjPropMirror:            return SetBooleanProperty (inValue, mReportMirror);
/*
		case PSObjPropPageSetup:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageSetup.Clone (inValue);
				return true;
			}
			break;
*/

		case PSObjPropPageFormat:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageFormat.Clone (inValue);
#if	MACVER
				if (GetPageComposer())
				{
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetPageFormat (mPageFormat.GetBlob());
					if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
						CalculatePosition();
				}
#else
				{
					// report made on the Mac: page size and margins from its page format
					SRect	paperRect, pageRect;
					if (RWTools::ParseMacPageFormat (mPageFormat.GetBlobData(), mPageFormat.GetBlobSize(), paperRect, pageRect))
					{
						mPageWidth = paperRect.Width();
						mPageHeight = paperRect.Height();
						mPageMargins.SetRect (pageRect.top - paperRect.top, pageRect.left - paperRect.left,
											  paperRect.bottom - pageRect.bottom, paperRect.right - pageRect.right);
						CalculatePosition();
					}
				}
#endif
				return true;
			}
			break;

		case PSObjPropPrintSettings:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPrintSettings.Clone (inValue);
#if	MACVER
				if (GetPageComposer())
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetPrintSettings (mPrintSettings.GetBlob());
#else
				//••• TODO •••
#endif
				return true;
			}
			break;

		case PSObjPropDevMode:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mDevMode.Clone (inValue);
#if	WINVER
				if (GetPageComposer())
				{
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetDevMode (mDevMode.GetBlob());
					/* if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
						CalculatePosition(); */ // pB 1.3.2 removed as this resets page data 
				}
#else
				//••• TODO •••
#endif
				return true;
			}
			break;

		case PSObjPropDeviceNames:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mDeviceNames.Clone (inValue);
#if	WINVER
				if (GetPageComposer())
				{
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetDeviceNames (mDeviceNames.GetBlob());
					/* if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
						CalculatePosition(); */
				}
#else
				//••• TODO •••
#endif
				return true;
			}
			break;

		case PSObjPropPageSetupDlg:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageSetupDialog.Clone (inValue);
#if	WINVER
				if (GetPageComposer())
				{
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetPageSetupDialog (mPageSetupDialog.GetBlob());
					/* if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
						CalculatePosition(); */
				}
#else
				//••• TODO •••
#endif
				return true;
			}
			break;

		case PSObjPropPrintDlg:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPrintDialog.Clone (inValue);
#if	WINVER
				if (GetPageComposer())
				{
					static_cast <RWNativePageComposer*> (GetPageComposer())->SetPrintDialog (mPrintDialog.GetBlob());
					/* if (GetPageComposer()->GetPaperMetrics (mPageWidth, mPageHeight, mPageMargins))
						CalculatePosition();*/
				}
#else
				//••• TODO •••
#endif
				return true;
			}
			break;

		// Editor properties
		case PSObjPropShowMargins:		return SetBooleanProperty (inValue, mShowMargins);
		case PSObjPropShowRuler:		return SetBooleanProperty (inValue, mShowRuler);
		case PSObjPropRulerUnits:		return SetIntegerProperty (inValue, mRulerUnits, 1, 3);
		case PSObjPropGridSize:			return SetRealProperty (inValue, mGridSize, 4, 256);
		case PSObjPropShowGrid:			return SetBooleanProperty (inValue, mShowGrid);
		case PSObjPropSnapToGrid:		return SetBooleanProperty (inValue, mSnapToGrid);
		case PSObjPropShowGuides:		return SetBooleanProperty (inValue, mShowGuides);
		case PSObjPropLockGuides:		return SetBooleanProperty (inValue, mLockGuides);
		case PSObjPropSnapToGuides:		return SetBooleanProperty (inValue, mSnapToGuide);
//		case PSObjPropShowSections:		return SetBooleanProperty (inValue, mShowSections);
		case PSObjPropLockSections:		return SetBooleanProperty (inValue, mLockSections);
		case PSObjPropShowObjBorders:	return SetBooleanProperty (inValue, mShowObjBorders);
			// pB added
		case PSObjPropGridColor:		return SetColorProperty (inValue, mGridColor);
		case PSObjPropGridRadius:		return SetRealProperty (inValue, mGridRadius);
		case PSObjPropGuideColor:		return SetColorProperty (inValue, mGuideColor);
		case PSObjPropGuideWidth:		return SetRealProperty (inValue, mGuideWidth);
		case PSObjPropScale:
			if (SetRealProperty (inValue, mScale, 0.1, 10.0))
			{
				ScaleChanged();
				return true;
			}
			break;
			
		default:						return DMBase::SetProperty (id, inValue);
	}

	return false;
}

DMStyle*
DMReport::GetStyle (long inStyleID)
const
{
	const PSStyleListD	*ld;
	if (inStyleID < 0 || mStyles.size() == 0)
		ld = &mEditorStyles;
	else
		ld = &mStyles;
	if (inStyleID < 0)
		inStyleID = -inStyleID;

/*
	PSObjListD::const_iterator	iter;
	const DMStyle				*style = NULL;
	for (iter = ld->begin(); iter != ld->end(); iter++)
	{
		style = static_cast <const DMStyle*> (static_cast <const DMBase*> (*iter));	// style = dynamic_cast <const DMStyle*> (*iter);
		if (style->GetID() == inStyleID)
			break;
	}
	if (iter == ld->end())
		style = static_cast <const DMStyle*> (static_cast <const DMBase*> (*ld->begin()));// style = dynamic_cast <const DMStyle*> (*ld->begin());

	return const_cast <DMStyle*> (style);
*/
	RWStyle	*style = ld->FindStyle (inStyleID);
	return static_cast <DMStyle*> (style);
}


// ---------------------------------------------------------------------------
// AdjustDrawingPosition											  [public]
// ---------------------------------------------------------------------------

void
DMReport::AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent)
{
	DMBase::AdjustDrawingPosition (inComposer, inParent);

	SPoint	p (mDrawRect.TopLeft());
//	p.v += 20;	// I want to see Report's name/id ;-)

	PSObjList::iterator	iter;
	for (iter = mGuides.begin(); iter != mGuides.end(); iter++)
	{
		DMBase	*obj = static_cast <DMBase*> (*iter);
		if (obj != NULL)
			obj->AdjustDrawingPosition (inComposer, p);
	}

	float	height = 0;
	for (iter = mSections.begin(); iter != mSections.end(); iter++)
	{
		DMBase	*sec = static_cast <DMBase*> (*iter);
		if (sec != NULL && sec->IsVisible())
		{
			if (mSimple || static_cast <DMSection*> (sec)->GetSectionKind() == DMSection::eSectionKind_Watermark)
			{
				sec->SetPosition (mPosition);
				sec->AdjustDrawingPosition (inComposer, p);
				continue;
			}

			float	thisHeight = sec->GetPosition().Height();
			SRect	newPos (height, 0., height + thisHeight, 4096.);
			sec->SetPosition (newPos);
//			p.v += 20;	// I want to see Section's name/id ;-)
			sec->AdjustDrawingPosition (inComposer, p);
			height += thisHeight;
//			p.v += 2;
		}
	}
}


// ---------------------------------------------------------------------------
// DeselectAll														  [public]
// ---------------------------------------------------------------------------

void
DMReport::DeselectAll (void)
{
	if (mSelectedObjects.size() > 0)
	{
		std::unique_ptr<PSObjList> l (new PSObjList (mSelectedObjects));
		mSelectedObjects.clear();
		PSObjList::iterator	iter;
		for (iter = l->begin(); iter != l->end(); iter++)
		{
			DMBase	*obj = static_cast <DMBase*> (*iter);
			if (obj != NULL)
				obj->SetSelected (false);
		}
		Modified();
	}
}


// ---------------------------------------------------------------------------
// SetPageMetrics														  [public]
// ---------------------------------------------------------------------------

void
DMReport::SetPageMetrics (float inPageWidth, float inPageHeight, const SRect inMargins)
{
	mPageWidth = inPageWidth;
	mPageHeight = inPageHeight;
	mPageMargins = inMargins;
	if (GetPageComposer())
		CalculatePosition();
}

// ---------------------------------------------------------------------------
// GetMappedObject											 [static] [public]
// ---------------------------------------------------------------------------

PSObject *
DMReport::GetMappedObject (long inObject)
{
	std::map<long, PSObject*>::iterator it = sObjectMap.find(inObject);
	if (it == sObjectMap.end()) {
		return reinterpret_cast <PSObject*> (inObject); // ??
	} else {
		return it->second;
	}

}

// ---------------------------------------------------------------------------
// IsValidObject                                             [static] [public]
// ---------------------------------------------------------------------------

bool
DMReport::IsValidObject (DMBase * inObject)
{
    if (mObjects.size() > 0)
    {
        PSObjList::iterator    iter = std::find (mObjects.begin(), mObjects.end(), inObject);
        if (iter != mObjects.end())
            return true;
    }
    return false;
}

#pragma	mark	-


static	void DrawSquare (RWPageComposer *inComposer, SPoint where)
{
	SRect	r (where.v - 1.5f, where.h - 1.5f, where.v + 1.5f, where.h + 1.5f);
	inComposer->DrawRect (r, 0.75, true, cBlackColor, true, cRedColor);
}

static	void DrawSquare (RWPageComposer *inComposer, SPoint where, bool locked)
{
	SRect	r (where.v - 1.5f, where.h - 1.5f, where.v + 1.5f, where.h + 1.5f);
	if (locked)
	{
		SRGBColor lightRed (0xAAAA, 0x4444, 0x4444, 0xFFFF);
		inComposer->DrawRect (r, 0.75, true, cGrayColor, true, lightRed);	
	}
	else
	{
		inComposer->DrawRect (r, 0.75, true, cBlackColor, true, cRedColor);
	}
}


void
DMSection::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (IsVisible())
	{
		SRect	r (inParent);
//		mDrawRect.top -= 20;
		r &= mDrawRect;
//		mDrawRect.top += 20;
		if (not r.IsEmpty())
		{
			if (false)
			{
				StClipToRect	clip (inComposer, r);
				RWPrintText		*rwt = NULL;
				SRect			rr (mDrawRect.top - 18, inParent.left + 2, mDrawRect.top, inParent.left + 2);
				inComposer->MeasureText (inMode == eDraw_ID? mID: mName, GetReport()->GetStyle (-2), rr, true, false, true, &rwt);
				if (rr.right > inParent.right)
					rr.right = inParent.right;
				r.right = rr.right + 2;
				if (r.right > inParent.right)
					r.right = inParent.right;
				mLabelRect = r;
				mLabelRect.bottom = mLabelRect.top + 20;
				inComposer->DrawRect (r, 1, true, cBlueColor, true, cWhiteColor);
				inComposer->DrawTextBox (inMode == eDraw_ID? mID: mName, GetReport()->GetStyle (-2), rr, true, false, false, &rwt);
				delete rwt;
				r = inParent;
				r &= mDrawRect;
			}

//			inComposer->DrawRect (r, 1, true, cBlueColor, true, cWhiteColor);
//			r *= 1;
			StClipToRect	clip (inComposer, r);

				//pB 2010-6
			SRGBColor	fillColor	( 0x0C0000AA );
			switch (mSectionKind) {
				case eSectionKind_Page:
				case eSectionKind_Watermark:
					break; // no drawing of section page
				case eSectionKind_Header:
				case eSectionKind_Footer:		// same as header 
					inComposer->DrawRect (mDrawRect, 0.25, true, cBlueColor, true, fillColor, 2.5, 2.5);
					break;
				case eSectionKind_Body:
					fillColor = 0x0CAAAA00 ;
					inComposer->DrawRect (mDrawRect, 0.25, true, cYellowColor, true, fillColor, 2.5, 2.5);
					break;
				case eSectionKind_BreakHeader:					
				case eSectionKind_BreakFooter:
					fillColor = 0x0CAA0000;
					inComposer->DrawRect (mDrawRect, 0.25, true, cRedColor, true, fillColor, 2.5, 2.5);
					break;
				default:
					inComposer->DrawRect (mDrawRect, 0.25, true, cBlueColor, true, fillColor, 2.5, 2.5);
					break;
			}

			if (true)
			{
				RWPrintText	*rwt = NULL;
				SRect		rr (mDrawRect.top + 1, inParent.left + 2, mDrawRect.top + 21, inParent.left + 2);
				inComposer->MeasureText (inMode == eDraw_ID? mID: mName, GetReport()->GetStyle (-2), rr, true, false, true, &rwt);
				if (rr.right < inParent.right - 1)
					rr.left += inParent.right - rr.right - 1;
				rr.right = inParent.right;
				inComposer->DrawTextBox (inMode == eDraw_ID? mID: mName, GetReport()->GetStyle (-2), rr, true, false, false, &rwt);
				delete rwt;
			}

			PSObjList::iterator	iter;
			for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
			{
				DMBase	*obj = static_cast <DMBase*> (*iter);
				if (obj != NULL && obj->IsVisible())
					obj->Draw (inComposer, r, inMode);
			}

			if (false && mSelected)		// to draw or not to draw?
			{
				DrawSquare (inComposer, r.TopLeft());
				SPoint	pt (r.right, r.top);
				DrawSquare (inComposer, pt);
				DrawSquare (inComposer, r.BottomRight());
				pt = SPoint(r.left, r.bottom);
				DrawSquare (inComposer, pt);
			}
		}
	}
	return;
}

void
DMSection::ParseData (RWPageComposer *inComposer)
{
			
	PSObjList::iterator	iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); iter++)
	{
		DMBase	*obj = static_cast <DMBase*> (*iter);
		if (obj != NULL)
			obj->ParseData (inComposer);
	}
			
	return;
}

DMBase::EHitTest	DMSection::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	
	if (IsSelectable())
	{
        double proximityOffset = GetProxySize(eOut_Proximity);
        
		if (inWhere.IsContained (mLabelRect))
		{
			hit = eHit_Object;
			outObjectHit = this;
		}
		else if (mSelected)
		{
			SRect	r (mDrawRect.bottom - proximityOffset, mDrawRect.left - proximityOffset, mDrawRect.bottom + proximityOffset, mDrawRect.right + proximityOffset);
			if (inWhere.IsContained (r))
			{
				hit = eHit_ResizeV;
				outObjectHit = this;
			}
		}

		if (hit == eHit_None && inWhere.IsContained (mDrawRect))
		{
			if (not IsLocked())
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
			}
			if (hit == eHit_None)
			{
				SRect	r (mDrawRect.bottom - proximityOffset, mDrawRect.left - proximityOffset, mDrawRect.bottom + proximityOffset, mDrawRect.right + proximityOffset);
				if (inWhere.IsContained (r))
					hit = eHit_ResizeV;
				else
					hit = eHit_Object;
				outObjectHit = this;
			}
		}
	}

	return hit;
}

void
DMSection::HandleTrackSelect (SRect &inWhere, UInt32 inFlags)
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
DMGuide::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode, double inWidth, SRGBColor inColor)
{
	if (IsVisible())
	{
		SRect	r (inParent);
		r &= mDrawRect;
		if (not mDrawRect.IsEmpty())
		{
			inComposer->DrawLine (r, inWidth, inColor, mVertical? RWLine_Vertical: RWLine_Horizontal, 2, 2);
			if (false && mSelected)		// to draw or not to draw?
			{
				DrawSquare (inComposer, r.TopLeft());
				DrawSquare (inComposer, r.BottomRight());
			}
		}
	}
	return;
}


DMBase::EHitTest	DMGuide::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest	hit = eHit_None;
	if (IsSelectable())
	{
        double proximityOffset = GetProxySize(eOut_Proximity);

        SRect		r (mDrawRect.top - proximityOffset, mDrawRect.left - proximityOffset, mDrawRect.bottom + proximityOffset, mDrawRect.right + proximityOffset);
		if (mSelected)
		{
			if (inWhere.IsContained (r))
			{
				if (mVertical)
					hit = eHit_ResizeH;
				else
					hit = eHit_ResizeV;
				outObjectHit = this;
			}
		}
		if (hit == eHit_None && inWhere.IsContained (r))
		{
			hit = eHit_Object;
			outObjectHit = this;
		}
	}

	return hit;
}

void
DMReport::ParseData (RWPageComposer *inComposer)
{
	if (inComposer)
		mComposer = inComposer;
	
	if (mComposer)
	{
		
		// draw sections
		PSObjList::iterator	iter;
		for (iter = mSections.begin(); iter != mSections.end(); iter++)
		{
			DMBase	*obj = static_cast <DMBase*> (*iter);
			if (obj != NULL)
				obj->ParseData (mComposer);
		}
		
			
	}
	return;
}

void
DMReport::Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode)
{
	if (inComposer)
		mComposer = inComposer;

	if (mComposer)
	{
		StClipToRect	clip (inComposer, inParent);

		// draw paper
		mComposer->DrawRect (mDrawRect, 0, false, cWhiteColor, true, cWhiteColor);

/*
		if (inParent & mDrawRect)
		{
//			DrawFrame (this, inMode, 0, inMode == eDraw_ID? mID: mName, mDrawRect, true);
			inComposer->DrawRect (mDrawRect, 1, true, cRedColor, false, cWhiteColor, 2, 2);
			inComposer->DrawTextBox (inMode == eDraw_ID? mID: mName, GetStyle (-1), mDrawRect, true, false, false, NULL);
		}
*/

		// draw grid
		if (mShowGrid && mGridSize >= 3)
		{
			float	top = mDrawRect.top + mGridSize * long ((inParent.top - mDrawRect.top) / mGridSize);
			float	left = mDrawRect.left + mGridSize * long ((inParent.left - mDrawRect.left) / mGridSize);
			float	x, y;
			for (x = left + mGridSize; x <= inParent.right; x += mGridSize)
			{
				if (x < inParent.left)
					continue;
				for (y = top + mGridSize; y <= inParent.bottom; y += mGridSize)
				{
					if (y < inParent.top)
						continue;
					SRect	dot (y - mGridRadius / 2, x - mGridRadius / 2, y + mGridRadius / 2, x + mGridRadius / 2);
					mComposer->DrawRect (dot, 0, false, cWhiteColor, true, mGridColor);
				}
			}
		}

		// draw sections
		PSObjList::iterator	iter;
		for (iter = mSections.begin(); iter != mSections.end(); iter++)
		{
			DMBase	*obj = static_cast <DMBase*> (*iter);
			if (obj != NULL && obj->IsVisible())
				obj->Draw (mComposer, inParent, inMode);
		}

		// draw guides
		if (mShowGuides)
			for (iter = mGuides.begin(); iter != mGuides.end(); iter++)
			{
				DMGuide	*obj = static_cast <DMGuide*> (*iter);
				if (obj != NULL && obj->IsVisible())
					obj->Draw (mComposer, inParent, inMode, mGuideWidth, mGuideColor);
			}

		// draw physical margins
		if (mUsePhysical && mShowMargins)
		{
			//mbs 12082010	fixed!
			SRect	l (mPageMargins.top, mPageMargins.left, mPageHeight - mPageMargins.bottom, mPageWidth - mPageMargins.right);
			l += mDrawRect.TopLeft();
			mComposer->DrawRect (l, 0.5, true, cRedColor, false, cWhiteColor, 1, 1);
		}

		RW_CheckLicense (inComposer, &inParent, false);
	}
	return;
}

DMBase::EHitTest	DMReport::HitTest (SPoint &inWhere, DMBase* &outObjectHit)
{
	EHitTest					hit = eHit_None;
	PSObjList::reverse_iterator	iter;

	if (not mLockGuides)
		for (iter = mGuides.rbegin(); iter != mGuides.rend(); iter++)
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
		for (iter = mSections.rbegin(); iter != mSections.rend(); iter++)
		{
			DMBase	*obj = static_cast <DMBase*> (*iter);
			if (obj != NULL && obj->IsSelectable())
			{
				hit = obj->HitTest (inWhere, outObjectHit);
				if (hit != eHit_None)
					break;
			}
		}
	}

	return hit;
}

void
DMReport::HandleTrackSelect (SRect &inWhere, UInt32 inFlags)
{
	PSObjList::iterator	iter;
	DMBase				*obj;

	if (false && not mLockGuides)
		for (iter = mGuides.begin(); iter != mGuides.end(); iter++)
		{
			obj = static_cast <DMBase*> (*iter);
			if (obj != NULL && obj->IsSelectable())
				obj->HandleTrackSelect (inWhere, inFlags);
		}
	
	for (iter = mSections.begin(); iter != mSections.end(); iter++)
	{
		obj = static_cast <DMBase*> (*iter);
		if (obj != NULL && obj->IsSelectable())
			obj->HandleTrackSelect (inWhere, inFlags);
	}
	return;
}

DMBase*
DMReport::GetParentAt (SPoint &inWhere)
{
	EHitTest	hit = eHit_None;
	DMBase		*obj = NULL;
	PSObjList::reverse_iterator	iter;
	for (iter = mSections.rbegin(); iter != mSections.rend(); iter++)
	{
		obj = static_cast <DMBase*> (*iter);
		if (obj != NULL)
		{
			hit = obj->HitTest (inWhere, obj);
			if (hit != eHit_None)
				break;
		}
	}
	if (hit != eHit_None)
	{
		while (obj && obj->GetKind() != eObject_Section && obj->GetKind() != eObject_Group)
			obj = obj->GetParent();
	}

	return obj;
}


void
DMReport::DrawFrame (const DMBase *inObject, EDrawDM inMode, long inStyleID, const RWString inText, const SRect &inRect, bool inAttributed, RWStyle* inStyle)
{
	RWStyle	*style;
	if (inStyle && inStyle->GetFeatures())
		style = inStyle;
	else
		style = GetStyle (inStyleID);
	
	if (mShowObjBorders || inObject->GetSelected())
	{
		SRect		r (inRect);
		r *= -0.25;
		SRGBColor	frameColor;
		float		width = 0.5;
		const DMVariable * var;
		switch (inObject->GetKind()) 
		{
			case eObject_Text:
				frameColor = cBlueColor;				
				break;
			case eObject_Var:
				frameColor = cRedColor;
				var = dynamic_cast<const DMVariable*> (inObject);
				if (var->HasScript ())
				{
					width = 1.0;
					r *= -0.5;
					frameColor = cDarkRedColor;
				}
				break;
			case eObject_Fld:
				frameColor = cOrangeColor;
				var = dynamic_cast<const DMField*> (inObject);
				if (var->HasScript ())
				{
					width = 1.0;
					r *= -0.5;
					frameColor = cDarkOrangeColor;
				}
				break;
			default:
				frameColor = cGrayColor;
				break;
		}
		mComposer->DrawRect (r, width, true, frameColor, true, cWhite50Color);
	}
//	if (not inText.IsEmpty())
		mComposer->DrawTextBox (inText, style, inRect, style->ShouldWrap(), inAttributed, false, NULL);
}


void
DMReport::DrawSelection (const DMBase *inObject, const SRect &inRect)
{
	bool	isLocked = inObject->IsLocked();
	switch (inObject->GetKind())
	{
		case eObject_DataSource:
		case eObject_Style:
			// should not occur
			break;
			
		case eObject_Document:		// inObject == this
			break;	// nothing for report
		case eObject_Guide:
			break;	// nothing for guide
		case eObject_TblHdr:
		case eObject_TblCol:
			DrawSquare (mComposer, inRect.TopLeft(), isLocked);
			DrawSquare (mComposer, SPoint (inRect.right, inRect.top), isLocked);
			DrawSquare (mComposer, inRect.BottomRight(), isLocked);
			DrawSquare (mComposer, SPoint (inRect.left, inRect.bottom), isLocked);
			break;
		case eObject_Section:
		{
//			SPoint	pt (inRect.left + inRect.Width() / 2, inRect.bottom);
//			DrawSquare (mComposer, pt);
			break;
		}
		case eObject_Line:
			if (static_cast <const DMLine*> (inObject)->GetFlags() == RWLine_Horizontal || static_cast <const DMLine*> (inObject)->GetFlags() == RWLine_Vertical)
			{
				DrawSquare (mComposer, inRect.TopLeft());
				DrawSquare (mComposer, inRect.BottomRight());
				break;
			}
			// FALL THROUGH
		default:
		{
			SPoint	pt (inRect.TopLeft());
			DrawSquare (mComposer, pt, isLocked);
			pt.h += inRect.Width() / 2;
			DrawSquare (mComposer, pt, isLocked);
			pt.h = inRect.right;
			DrawSquare (mComposer, pt, isLocked);
			pt.v += inRect.Height() / 2;
			DrawSquare (mComposer, pt, isLocked);
			pt.v = inRect.bottom;
			DrawSquare (mComposer, pt, isLocked);
			pt.h -= inRect.Width() / 2;
			DrawSquare (mComposer, pt, isLocked);
			pt.h = inRect.left;
			DrawSquare (mComposer, pt, isLocked);
			pt.v -= inRect.Height() / 2;
			DrawSquare (mComposer, pt, isLocked);
			break;
		}
	}
}


//mbs 31052010	"preview" support
SRDataSource*
DMReport::GetDataSource (void)
{
	if (mDataSource != NULL)
		return mDataSource->GetRealDataSource();
	return NULL;
}


SRDataSource*
DMDataSource::GetRealDataSource (void)
{
	if (mRealDataSource != NULL)
		return mRealDataSource;

	RWXmlDocument	xml;
	RWXmlNode		root = xml.Node().Append (u"Report");

	this->WriteXML (root);
	mRealDataSource = new SRDataSource;
	mRealDataSource->ParseReport (root);
	mRealDataSource->Reset();
	mRealDataSource->FetchNextRecord();
	return mRealDataSource;
}


void
DMDataSource::ClearDataSource (void)
{
	if (mRealDataSource != NULL)
	{
		delete mRealDataSource;
		mRealDataSource = NULL;
	}
}
