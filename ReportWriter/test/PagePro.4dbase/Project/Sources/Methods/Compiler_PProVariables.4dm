//%attributes = {"shared":true}
// Declares variables used by PagePro component
// PagePro variables starts with PP_ prefix
//      call before displaying PagePro area in subform

C_TEXT:C284(PP_Report)
C_TEXT:C284(PP_CallbackName; PP_CallbackRef)
C_TEXT:C284(PP_DefaultMenu; PP_MenuRef; PP_OldMenuRef)
C_LONGINT:C283(PP_WindowTop; PP_WindowLeft; PP_WindowType; PP_WindowRef)
C_BOOLEAN:C305(PP_DirtyFlag)

C_LONGINT:C283(PP_ExtArea; PP_XMLRoot; PP_CurrentObject)
C_REAL:C285(PP_Scaling)
C_TEXT:C284(PP_InlineEdit; PP_InlineEditPlain)

C_LONGINT:C283(PP_OK; PP_Cancel; PP_Enter; PP_Esc; PP_Setup; PP_No)

C_LONGINT:C283(PP_ToolCursor; PP_ToolText; PP_ToolVariable; PP_ToolField; PP_ToolRect; PP_ToolOval)
C_LONGINT:C283(PP_ToolLine; PP_ToolTable; PP_ToolPicture)

C_LONGINT:C283(PP_ListBox; PP_Dialogs)
C_LONGINT:C283(PP_Type1; PP_Type2)

C_REAL:C285(PP_Point1x; PP_Point1y; PP_Point2x; PP_Point2y; PP_LineWidth)
C_REAL:C285(PP_width; PP_Height)
C_REAL:C285(PP_Left; PP_Top; PP_Right; PP_Bottom)
C_LONGINT:C283(PP_PaperBtn; PP_StaticReport; PP_ButtonInfo)

C_LONGINT:C283(PP_LockedMode)
C_REAL:C285(PP_X; PP_Y; PP_W; PP_H; PP_R; PP_B)

C_PICTURE:C286(PP_ScriptPicture)
C_TEXT:C284(PP_ScriptName)
C_REAL:C285(PP_GridSize)
C_LONGINT:C283(PP_PageProURLBtn)
C_LONGINT:C283(PP_ButtonMinus; PP_ButtonPlus; PP_Button1; PP_ButtonLock; PP_ButtonGroup)
C_LONGINT:C283(PP_ColorPickerBtn; PP_ColorPicker2Btn)
C_LONGINT:C283(PP_VisibleBtn; PP_LockedBtn; PP_FixVBtn; PP_BindBtn; PP_BindVBtn; PP_FillBtn)
C_LONGINT:C283(PP_StyleB; PP_StyleI; PP_StyleU; PP_AlignLeft; PP_AlignRight; PP_AlignCenter; PP_AlignJustify; PP_AlignFullJustify)
C_LONGINT:C283(PP_VAlignTop; PP_VAlignCenter; PP_VAlignBottom)
C_LONGINT:C283(PP_WrapBtn; PP_DynamicBtn; PP_KeepTogetherBtn; PP_MultiStyleBtn)
C_LONGINT:C283(PP_Rotation0; PP_Rotation90; PP_Rotation180; PP_Rotation270; PP_Rotation)
C_LONGINT:C283(PP_TransparentBackColor; PP_TransparentFrameColor)
C_LONGINT:C283(PP_TextColor; PP_BackColor; PP_FrameColor)
C_LONGINT:C283(PP_ExpandableBtn; PP_EditBtn; PP_SRPComaptibility; PP_Add2Section)
C_LONGINT:C283(PP_GrowBtn; PP_DrawBtn; PP_NewPageBtn; PP_DeleteStyle)
C_LONGINT:C283(PP_OnTopBtn; PP_FirstBtn; PP_LastBtn; PP_OddBtn; PP_Odd2Btn; PP_EvenBtn; PP_Even2Btn; PP_AlwaysBtn)
C_LONGINT:C283(PP_HeaderBtn; PP_HeaderLines; PP_ColumnsBtn; PP_Columns; PP_DoubleFrameBtn)
C_LONGINT:C283(PP_Rows; PP_Level; PP_ColumnNo)
C_LONGINT:C283(PP_rowNoBtn; PP_GridBtn; PP_DuplicatesBtn)
C_REAL:C285(PP_RowHeight; PP_RowGridWidth; PP_CellHeight; PP_ColumnWidth)
C_LONGINT:C283(PP_Columns; PP_Rows)
C_LONGINT:C283(PP_AddStyle)

// opacities
C_LONGINT:C283(PP_TextOpacity; PP_FrameOpacity; PP_FillOpacity; PP_FillColor)
C_LONGINT:C283(PP_LineOpacity; PP_TextOpacityBtn; PP_LineColor)

C_TEXT:C284(PP_Script; PP_ScriptOld)
C_TEXT:C284(PP_StartScript; PP_BodyScript; PP_EndScript)
C_LONGINT:C283(PP_Element; PP_Iterations; PP_Offset)

C_TEXT:C284(PP_Source; PP_Format; PP_Name; PP_Type; PP_Alias)

C_REAL:C285(PP_LineSpacing; PP_HorScale; PP_ProximityLimit; PP_FontSize)

C_TEXT:C284(PP_ObjectID; PP_ObjectName; PP_Content)
C_LONGINT:C283(PP_PrintOrder; PP_FramePopup)

C_LONGINT:C283(PP_LBHeader1; PP_LBHeader2; PP_LBHeader3)
C_LONGINT:C283(PP_FrameBtn; PP_PictureLoad)
C_TEXT:C284(PP_ImageSize; PP_PictSize; PP_Callback)
C_REAL:C285(PP_FrameOffset)

C_TEXT:C284(PP_DocumentPath; PP_DocumentFolder)

C_LONGINT:C283(PP_SourcePopup)
C_LONGINT:C283(PP_FormatPopup)
C_LONGINT:C283(PP_TablePopup)
C_LONGINT:C283(PP_Splitter; PP_Splitter1; PP_Splitter2; PP_Splitter3)
C_LONGINT:C283(PP_ColumnPopup)
C_LONGINT:C283(PP_ObjectsAlignPopup)

C_LONGINT:C283(PP_TableList; PP_VariableList; PP_CommandList)
C_LONGINT:C283(PP_DefaultMenuID)
C_LONGINT:C283(PP_ReportModified)

C_LONGINT:C283(PP_TypeAhead; PP_Test)
C_LONGINT:C283(PP_ScriptEditorStartSel; PP_ScriptEditorEndSel)

C_REAL:C285(PP_Opacity)
C_PICTURE:C286(PP_Opacity_SVG)
C_LONGINT:C283(PP_LockProportions)

// palette
C_LONGINT:C283(PP_PaletteProcess; PP_HierList; PP_PaletteIcon; PP_PaletteList)

// printing flags
C_LONGINT:C283(PP_ExportBreaks; PP_ExportHeaders; PP_ExportTexts; PP_ExportVars; PP_ExportTabs; PP_ExportCVS)
C_LONGINT:C283(PP_ExportFlag)

// version
C_TEXT:C284(PP_VersionString; PP_PluginVersionString)

// localization
C_TEXT:C284(PP_CurrentLanguage)

// exec variables

ARRAY TEXT:C222(IN4_Line; 0)
ARRAY TEXT:C222(IN4_Token; 0)

ARRAY LONGINT:C221(IN4_LineIndent; 0)
ARRAY LONGINT:C221(IN4_CommandOffset; 0)
ARRAY LONGINT:C221(IN4_ConditionOffset; 0)
ARRAY LONGINT:C221(IN4_LineIfTrue; 0)
ARRAY LONGINT:C221(IN4_LineIfFalse; 0)
ARRAY LONGINT:C221(IN4_ForStack; 0)

ARRAY LONGINT:C221(IN4_Position; 0)

C_LONGINT:C283(IN4_NextLevel)
C_BLOB:C604(IN4_ObjectBlob)

// style editor
ARRAY BOOLEAN:C223(PP_StyleLB; 0)
ARRAY LONGINT:C221(PP_StyleID; 0)
ARRAY TEXT:C222(PP_StyleName; 0)
ARRAY TEXT:C222(PP_StyleProps; 0)
ARRAY LONGINT:C221(PP_StyleRef; 0)
C_LONGINT:C283(PP_CurrentStyleID)

// added editor properties
C_LONGINT:C283(PP_ObjectFrame; PP_PrintableArea; PP_ShowRuler; PP_ShowGrid; PP_AlignToGrid; PP_ShowGuides; PP_SnapToGuides)
C_LONGINT:C283(PP_LockGuides)
C_TEXT:C284(PP_ReportType)
C_LONGINT:C283(PP_ToolData; PP_ToolStyle; PP_ToolRefresh; PP_ToolAlignLeft; PP_ToolAlignRight; PP_ToolAlignVCentre; PP_ToolalignTop; PP_ToolAlignBottom; PP_ToolAlignHCentre)
C_LONGINT:C283(PP_ToolBringToFront; PP_ToolSendToBack; PP_ShowXMLRoot)
C_BOOLEAN:C305(PP_XMLIsDisplayed)

C_TEXT:C284(PP_Answer)

// LB
C_LONGINT:C283(PP_StylesHdr01)
C_LONGINT:C283(PP_StylesHdr02)
C_LONGINT:C283(PP_StylesHdr03)
C_LONGINT:C283(PP_StylesHdr04)

C_TEXT:C284(PP_Environment)
C_TEXT:C284(PP_UnitName)

// Search
C_TEXT:C284(PP_Search)
C_LONGINT:C283(PP_SearchNext; PP_SearchBack; PP_SearchSet)
// palette communication
C_TEXT:C284(<>PP_PaletteMessage)

// SizeLink
C_LONGINT:C283(PP_SizeLink_bt)

// v1.4
// tab support
C_LONGINT:C283(PP_TabHeader1; PP_TabHeader2)
C_LONGINT:C283(PP_GridX; PP_GridY)
C_REAL:C285(PP_LabelLeft; PP_LabelTop; PP_LabelRight; PP_LabelBottom)
C_LONGINT:C283(PP_LabelReport)

C_LONGINT:C283(PP_TabAdd; PP_TabRemove)

C_LONGINT:C283(PP_ObjectRotation; PP_ObjectSkewing; PP_Dimming)  // could be real as well
C_LONGINT:C283(PP_ObjectMirrorBtn)

C_LONGINT:C283(PP_EditWidth; PP_EditHeight)
C_LONGINT:C283(PP_AcceptEdit; PP_CancelEdit)