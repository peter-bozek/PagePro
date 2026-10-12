//%attributes = {"shared":true}
// Declares arrays used by PagePro component
// PagePro arrays starts with PP_ prefix
//      call before displaying PagePro area in subform

ARRAY BOOLEAN:C223(PP_PropertyPalette; 0x0000)
ARRAY TEXT:C222(PP_PropID; 0x0000)
ARRAY TEXT:C222(PP_PropValue; 0x0000)
ARRAY TEXT:C222(PP_PropName; 0x0000)
ARRAY LONGINT:C221(PP_ItemsToSelect; 0x0000)

ARRAY TEXT:C222(PP_Styles; 0x0000)
ARRAY LONGINT:C221(PP_StyleID; 0x0000)

ARRAY TEXT:C222(PP_Units; 0x0000)

ARRAY TEXT:C222(PP_ReportTypes; 0x0000)
ARRAY TEXT:C222(PP_PrintPopup; 0x0000)
ARRAY TEXT:C222(PP_AlignPopup; 0x0000)
ARRAY TEXT:C222(PP_WidthPopup; 0x0000)
ARRAY TEXT:C222(PP_PropertyGroup; 0x0000)
ARRAY TEXT:C222(PP_PictFormatPopup; 0x0000)
ARRAY TEXT:C222(PP_DrawPopup; 0x0000)
ARRAY TEXT:C222(PP_EmptyPopup; 0x0000)
ARRAY TEXT:C222(PP_RulerUnits; 0x0000)
ARRAY TEXT:C222(PP_UnitRounding; 0x0000)
ARRAY TEXT:C222(PP_DataSourcePopupMirror; 0x0000)
ARRAY TEXT:C222(PP_RelatePopupMirror; 0x0000)
ARRAY TEXT:C222(PP_AlignPopupMirror; 0x0000)
ARRAY TEXT:C222(PP_PrintPopupMirror; 0x0000)
ARRAY TEXT:C222(PP_RepeatPopup; 0x0000)
ARRAY TEXT:C222(PP_RepeatMirror; 0x0000)

ARRAY TEXT:C222(PP_TextTabs; 0x0000)
ARRAY TEXT:C222(PP_Fonts; 0x0000)
ARRAY TEXT:C222(PP_FontSizePopup; 0x0000)
ARRAY TEXT:C222(PP_CalcPopup; 0x0000)
ARRAY TEXT:C222(PP_ElementPopup; 0x0000)

ARRAY TEXT:C222(PP_DataSourcePopup; 0x0000)
ARRAY TEXT:C222(PP_Relate1Popup; 0x0000)
ARRAY TEXT:C222(PP_RelateManyPopup; 0x0000)

ARRAY TEXT:C222(PP_PageBreakPopup; 0x0000)
ARRAY TEXT:C222(PP_BreakOnPopup; 0x0000)

ARRAY TEXT:C222(PP_FrameOpacityPopup; 0)
ARRAY TEXT:C222(PP_FillOpacityPopup; 0)
ARRAY TEXT:C222(PP_TextOpacityPopup; 0)

// Search
ARRAY LONGINT:C221(PP_SearchPosition; 0)
ARRAY LONGINT:C221(PP_SearchLen; 0)

// v1.4
ARRAY TEXT:C222(PP_TabAlignPopup; 0)
ARRAY TEXT:C222(PP_TabAlignPopupMirror; 0)
ARRAY TEXT:C222(PP_TabAlign; 0)
ARRAY REAL:C219(PP_TabPosition; 0)

ARRAY BOOLEAN:C223(PP_TabListBox; 0)