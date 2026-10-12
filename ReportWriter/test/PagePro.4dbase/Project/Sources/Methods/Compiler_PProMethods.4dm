//%attributes = {"invisible":true}
C_LONGINT:C283(PPro_CreateDocumentTree; $0)
C_LONGINT:C283(PPro_CreateDocumentTree; $1)
C_POINTER:C301(PPro_CreateDocumentTree; $2)
C_LONGINT:C283(PPro_CreateDocumentTree; $3)

C_LONGINT:C283(PPro_SetExtAreaReference; $1)

C_LONGINT:C283(PPro_Util_TypeToLong; $0)
C_TEXT:C284(PPro_Util_TypeToLong; $1)
C_TEXT:C284(PPro_Util_PathToFolder$0; $1)

C_LONGINT:C283(PPro_CheckSublistState; $1; $2)
C_BOOLEAN:C305(PPro_CheckSublistState; $0)

C_LONGINT:C283(PPro_LoadObjects; $1)
C_LONGINT:C283(PPro_LoadObjects; $2)
C_LONGINT:C283(PPro_LoadObjects; $3)

C_LONGINT:C283(PPro_ObjectGroup; $1)
C_TEXT:C284(PPro_ObjectGroup; $2)  //selector
C_LONGINT:C283(PPro_ObjectProperty; $1)
C_TEXT:C284(PPro_ObjectProperty; $2; $3)
C_LONGINT:C283(PPro_ObjectOrder; $1)
C_TEXT:C284(PPro_ObjectOrder; $2)
C_LONGINT:C283(PPro_ObjectEdit; $1)

C_LONGINT:C283(PPro_XMLObjects; $1)
C_TEXT:C284(PPro_XMLObjects; $2)
C_LONGINT:C283(PPro_LoadTable; $1)
C_LONGINT:C283(PPro_LoadTable; $2)
C_LONGINT:C283(PPro_LoadTable; $3)

C_TEXT:C284(PPro_CommandDispatch; $1)
C_LONGINT:C283(PPro_NewReportDlg; $1)
C_BOOLEAN:C305(PPro_NewReportDlg; $0)  //success
C_TEXT:C284(PPro_NewReportDlg; $2)  // Type of report ("new" "static" or "dynamic")

C_LONGINT:C283(PPro_OpenReportDlg; $1)
C_BOOLEAN:C305(PPro_OpenReportDlg; $0)

C_LONGINT:C283(PPro_SaveReport; $1)
C_LONGINT:C283(PPro_SaveReport; $2)
C_BOOLEAN:C305(PPro_SaveReport; $0)

C_LONGINT:C283(PPro_PrintReport; $1)
C_LONGINT:C283(PPro_PrintReport; $2)
C_TEXT:C284(PPro_PrintReport; $3)  //
C_TEXT:C284(PPro_PreviewRW; $1)
C_LONGINT:C283(PPro_PreviewRW; $2; $0)
C_TEXT:C284(PPro_SearchBuild; $1)

C_LONGINT:C283(PPro_ToolsSetButtons; $1; $2)

C_LONGINT:C283(PPro_Util_ComponentsToColor; $0)
C_LONGINT:C283(PPro_Util_ComponentsToColor; $1; $2; $3)
C_REAL:C285(PPro_Util_ComponentsToColor; $4)

C_TEXT:C284(PPro_EditorFull; $1; $2; $3)  //report to edit, callback, reference
C_TEXT:C284(PPro_EditorFull; $0)  //returned report
C_LONGINT:C283(PPro_EditorFull; $4)

C_TEXT:C284(PPro_EditorBlue; $1; $2; $3)  //report to edit, callback, reference
C_LONGINT:C283(PPro_EditorBlue; $4)  //menu bar to restore
C_TEXT:C284(PPro_EditorBlue; $0)  //returned report

C_TEXT:C284(PPro_EditScript; $0; $1)

C_LONGINT:C283(PPro_SetPropertyString; $1)
C_LONGINT:C283(PPro_SetPropertyString; $2)
C_TEXT:C284(PPro_SetPropertyString; $3)
C_TEXT:C284(PPro_SetPropertyString; $4)
C_LONGINT:C283(PPro_SetPropertyString; $0)

C_LONGINT:C283(PPro_SetPropertyInteger; $1)
C_LONGINT:C283(PPro_SetPropertyInteger; $2)
C_TEXT:C284(PPro_SetPropertyInteger; $3)
C_LONGINT:C283(PPro_SetPropertyInteger; $4)
C_LONGINT:C283(PPro_SetPropertyInteger; $0)

C_LONGINT:C283(PPro_EditLineDlg; $1)  //line object to edit

C_LONGINT:C283(PPro_GetPropertyString; $1)
C_LONGINT:C283(PPro_GetPropertyString; $2)
C_TEXT:C284(PPro_GetPropertyString; $3)
C_TEXT:C284(PPro_GetPropertyString; $0)

C_LONGINT:C283(PPro_GetPropertyInteger; $1)
C_LONGINT:C283(PPro_GetPropertyInteger; $2)
C_TEXT:C284(PPro_GetPropertyInteger; $3)
C_LONGINT:C283(PPro_GetPropertyInteger; $0)

C_TEXT:C284(PPro_Util_RectToCoord; $1)
C_POINTER:C301(PPro_Util_RectToCoord; $2; $3; $4; $5)
C_TEXT:C284(PPro_Util_HexToNum; $1)
C_LONGINT:C283(PPro_Util_HexToNum; $0)

C_LONGINT:C283(PPro_SetPropertyNumber; $1)
C_LONGINT:C283(PPro_SetPropertyNumber; $2)
C_TEXT:C284(PPro_SetPropertyNumber; $3)
C_REAL:C285(PPro_SetPropertyNumber; $4)
C_LONGINT:C283(PPro_SetPropertyNumber; $0)

C_LONGINT:C283(PPro_GetPropertyNumber; $1)
C_LONGINT:C283(PPro_GetPropertyNumber; $2)
C_TEXT:C284(PPro_GetPropertyNumber; $3)
C_REAL:C285(PPro_GetPropertyNumber; $0)

C_LONGINT:C283(PPro_ToggleProperty; $1)
C_LONGINT:C283(PPro_ToggleProperty; $2)
C_TEXT:C284(PPro_ToggleProperty; $3)
C_LONGINT:C283(PPro_ToggleProperty; $0)

C_TEXT:C284(PPro_Util_ColorToComponents; $1)
C_POINTER:C301(PPro_Util_ColorToComponents; $2; $3; $4; $5)
C_LONGINT:C283(PPro_Util_CoordToRect; $0)
C_REAL:C285(PPro_Util_CoordToRect; $2; $3; $4; $5)  //left, top, right, bottom
C_POINTER:C301(PPro_Util_CoordToRect; $1)  //text with rectangle
C_BOOLEAN:C305(PPro_Util_CoordToRect; $6)  //normalize coordinates? default = true

C_LONGINT:C283(PPro_EditLineDlg; $1)
C_LONGINT:C283(PPro_EditRectDlg; $1)
C_LONGINT:C283(PPro_EditOvalDlg; $1)

C_POINTER:C301(PPro_CheckDecimal; $1)

C_POINTER:C301(PPro_AreaKeyHandler; $1)  //area
C_BOOLEAN:C305(PPro_AreaKeyHandler$0)  //area needs redraw

C_LONGINT:C283(PPro_ScaleUp; $1)  //area
C_LONGINT:C283(PPro_ScaleDown; $1)  //area
C_LONGINT:C283(PPro_ScaleTo; $1)  //area
C_REAL:C285(PPro_ScaleTo; $2)  //area

C_POINTER:C301(PPro_AreaMouseHandler; $1)  //area
C_BOOLEAN:C305(PPro_AreaMouseHandler$0)  //area needs redraw

C_LONGINT:C283(PPro_PropertiesReload; $1)  //area

C_POINTER:C301(PPro_AreaKeyHandler; $1)  //area
C_BOOLEAN:C305(PPro_AreaKeyHandler$0)  //area needs redraw

C_LONGINT:C283(PPro_AreaFormData; $1)  //area

C_TEXT:C284(PPro_ObjectCommandsDispatch; $1)
C_TEXT:C284(PPro_TextCommandDispatch; $1)

C_LONGINT:C283(PPro_ObjectsDuplicate; $1)

C_POINTER:C301(PPro_AreaKeyHandler; $1)  //area
C_BOOLEAN:C305(PPro_AreaKeyHandler$0)  //area needs redraw

C_LONGINT:C283(PPro_DeleteObject; $1)  // RefID of the external area (0=PP_ExtArea)
C_LONGINT:C283(PPro_DeleteObject; $2)  // The object reference
C_LONGINT:C283(PPro_DeleteObject; $0)  // 0=No error otherwise #0

C_LONGINT:C283(PPro_IsLeaf; $1)  //area
C_LONGINT:C283(PPro_IsLeaf; $2)  //object
C_BOOLEAN:C305(PPro_IsLeaf; $0)

C_LONGINT:C283(PPro_HasTextAttributes; $1)  //area
C_LONGINT:C283(PPro_HasTextAttributes; $2)  //object
C_BOOLEAN:C305(PPro_HasTextAttributes; $0)

C_LONGINT:C283(PPro_ObjectsAlign; $1)
C_TEXT:C284(PPro_ObjectsAlign; $2)  //selector

C_LONGINT:C283(PPro_AreaReloadHList; $1)  //area

C_TEXT:C284(PPro_MenuIterator; $1)

C_LONGINT:C283(PPro_TextGetProperties; $1; $2)
C_POINTER:C301(PPro_TextGetProperties; $3; $4; $5; $6; $7)

C_LONGINT:C283(PPro_PropertiesReload; $1; $2)  //area

C_LONGINT:C283(PPro_ObjectGetIcon; $1; $2; $0)

C_LONGINT:C283(PPro_SubformVisibility; $1)

C_LONGINT:C283(PPro_ObjectLock; $1)
C_TEXT:C284(PPro_ObjectLock; $2)  //selector
C_TEXT:C284(PPro_ObjectLock; $3)  //se

C_LONGINT:C283(PPro_EditMenuHandler; $1)
C_LONGINT:C283(PPro_EditMenuHandler; $2)
C_LONGINT:C283(PPro_EditMenuHandler; $3)
C_BOOLEAN:C305(PPro_EditMenuHandler; $0)

C_TEXT:C284(PPro_ReportProperty; $1)
C_LONGINT:C283(PPro_ReportProperty; $2)
C_BOOLEAN:C305(PPro_ReportProperty; $0)

C_TEXT:C284(PPro_ReportAddSection; $1)
C_LONGINT:C283(PPro_ReportAddSection; $2)
C_LONGINT:C283(PPro_ReportAddSection; $3)
C_TEXT:C284(PPro_ReportAddSection; $4)
C_LONGINT:C283(PPro_ReportAddSection; $5)
C_LONGINT:C283(PPro_ReportAddSection; $0)

C_LONGINT:C283(PPro_ObjectPosition; $1)
C_TEXT:C284(PPro_ObjectPosition; $2)
C_REAL:C285(PPro_ObjectPosition; $3)

C_POINTER:C301(PPro_TextHandleEditing; $1)
C_LONGINT:C283(PPro_TextHandleEditing; $2; $3)

C_REAL:C285(PPro_ToolbarPositionSet; $2)  //value
C_POINTER:C301(PPro_ToolbarPositionSet; $1)  //text
C_LONGINT:C283(PPro_ToolbarPositionSet; $3)  //selector of unit

C_POINTER:C301(PPro_Util_ReturnMeasure; $1)
C_REAL:C285(PPro_Util_ReturnMeasure; $0)
C_LONGINT:C283(PPro_Util_ReturnMeasure; $2)

C_LONGINT:C283(PPro_ConfirmDelete; $1; $2)
C_BOOLEAN:C305(PPro_ConfirmDelete; $0)

C_LONGINT:C283(PPro_UnitsDlg; $1)
C_BOOLEAN:C305(PPro_UnitsDlg; $0)

C_LONGINT:C283(PPro_TextTabsVisibility; $1)

C_TEXT:C284(PPro_ResolveAlias; $0; $1)

C_TEXT:C284(PPro_LoadMenu; $1)
C_TEXT:C284(PPro_LoadMenu; $2)
C_TEXT:C284(PPro_LoadMenuBar; $0)
C_TEXT:C284(PPro_LoadMenuBar; $1)  //root (menu bar)
C_TEXT:C284(PPro_LoadMenuBar; $2)  //file name

C_LONGINT:C283(PPro_NewStyle; $1)
C_TEXT:C284(PPro_Palette; $1; $7)
C_LONGINT:C283(PPro_Palette; $2; $3; $4; $5; $6)

C_TEXT:C284(PPro_ResolveTableName; $1)
C_LONGINT:C283(PPro_ResolveTableName; $0)

C_BOOLEAN:C305(PPro_EditorCallback; $0)  //event handled
C_TEXT:C284(PPro_EditorCallback; $1; $2)  //type of event, event
C_LONGINT:C283(PPro_EditorCallback; $3; $4)  // paramters: report and object or window

C_BOOLEAN:C305(PPro_CallbackTemplate; $0)  //event handled
C_TEXT:C284(PPro_CallbackTemplate; $1; $2; $3)  // reference, type of event, event
C_LONGINT:C283(PPro_CallbackTemplate; $4; $5)  // paramters: report and object or window

C_LONGINT:C283(PPro_SetEditorVariable; $0)
C_TEXT:C284(PPro_SetEditorVariable; $1)
C_POINTER:C301(PPro_SetEditorVariable; $2)

C_LONGINT:C283(PPro_GetEditorVariable; $0)
C_TEXT:C284(PPro_GetEditorVariable; $1)
C_POINTER:C301(PPro_GetEditorVariable; $2)

C_TEXT:C284(PPro_StylePropertiesHandler; $1)

C_TEXT:C284(PPro_VirtualStructure; $1)  // selector - field or var?
C_TEXT:C284(PPro_VirtualStructure; $2)  //action selector
C_TEXT:C284(PPro_VirtualStructure; $3)  //item name
C_POINTER:C301(PPro_VirtualStructure; $4)  //pointer to array if items
C_POINTER:C301(PPro_VirtualStructure; $5; $6)  // pointers to longint arrays of table no / field no
C_LONGINT:C283(PPro_VirtualStructure; $0)  //error number
C_LONGINT:C283(PPro_VirtStructCreateFieldList; $0)
C_LONGINT:C283(PPro_VirtStructCreateVarList; $0)
C_LONGINT:C283(PPro_VirtStructCreateCommList; $0)

C_LONGINT:C283(PPro_ResetSelection; $1)  //area

C_LONGINT:C283(PPro_JoinLists; $1)
C_LONGINT:C283(PPro_JoinLists; $0)

C_LONGINT:C283(PPro_NewObjectProperties; $1; $2)

C_TEXT:C284(PPro_ExportReport; $1)

C_TEXT:C284(PPro_ObjectPropertiesSubform; $1)
C_TEXT:C284(PPro_ColumnProperties; $1)
C_TEXT:C284(PPro_DataSourceProperties; $1)
C_TEXT:C284(PPro_GroupProperties; $1)
C_TEXT:C284(PPro_GuideProperties; $1)
C_TEXT:C284(PPro_LineProperties; $1)
C_TEXT:C284(PPro_OvalProperties; $1)
C_TEXT:C284(PPro_PictureProperties; $1)
C_TEXT:C284(PPro_RectangleProperties; $1)
C_TEXT:C284(PPro_ReportProperties; $1)
C_TEXT:C284(PPro_HeaderProperties; $1)
C_TEXT:C284(PPro_StyleProperties; $1)
C_LONGINT:C283(PPro_StyleProperties; $2)
C_TEXT:C284(PPro_TableProperties; $1)
C_TEXT:C284(PPro_TextProperties; $1)
C_TEXT:C284(PPro_VarProperties; $1)
C_TEXT:C284(PPro_SelectionProperties; $1)
C_LONGINT:C283(PPro_SelectionProperties; $2)
C_TEXT:C284(PPro_RepeatingVarProperties; $1)

C_TEXT:C284(PPro_Util_SaveMethod; $1)
C_TEXT:C284(PPro_Util_FindPlainText; $1)
C_POINTER:C301(PPro_Util_FindPlainText; $2; $3)

C_LONGINT:C283(PPro_SnapCoordinate; $1)
C_REAL:C285(PPro_SnapCoordinate; $2; $0)
C_TEXT:C284(PPro_SnapCoordinate; $3)
C_BOOLEAN:C305(PPro_SnapCoordinate; $4)

C_LONGINT:C283(PPro_EditOpacity; $2; $3)
C_REAL:C285(PPro_EditOpacity; $0; $1)

C_LONGINT:C283(PPro_LoadHList; $0)
C_TEXT:C284(PPro_LoadHList; $1)  //root (menu bar)
C_TEXT:C284(PPro_LoadHList; $2)  //file name

C_LONGINT:C283(PPro_LoadList; $1)
C_TEXT:C284(PPro_LoadList; $2)
C_LONGINT:C283(PPro_LoadList; $3; $0)

C_LONGINT:C283(PPro_SRP_CompareReports; $1)  //SuperReportPro external area reference
C_TEXT:C284(PPro_SRP_CompareReports; $2)  // name of report
C_BLOB:C604(PPro_SRP_CompareReports; $3)  // blob with SRP report - if $1 = 0

C_LONGINT:C283(PPro_ConvertRepeatingObject; $0; $1; $2)
C_LONGINT:C283(PPro_ConvertRepeatingLine; $0; $1; $2)

C_LONGINT:C283(PPro_LoadVarHList; $0)
C_TEXT:C284(PPro_LoadVarHList; $1)  //XML text 

C_LONGINT:C283(PPro_LoadVarList; $1)
C_TEXT:C284(PPro_LoadVarList; $2)
C_LONGINT:C283(PPro_LoadVarList; $3; $0)

C_POINTER:C301(PPro_OpacityButtonScript; $1)
C_TEXT:C284(PPro_Menu; $1)

C_TEXT:C284(PPro_EditStyles; $1)
C_LONGINT:C283(PPro_EditStyles; $2)
C_TEXT:C284(PPro_EditStyles; $3)
C_TEXT:C284(PPro_EditStyles; $0)

C_LONGINT:C283(PPro_DeselectAll; $1)
C_LONGINT:C283(PPro_FreezeValues; $1)
C_LONGINT:C283(PPro_FreezeSection; $1; $2)
C_LONGINT:C283(PPro_FormExit; $1)

C_TEXT:C284(PPro_MapFormats; $0; $1; $2; $3)
C_LONGINT:C283(PPro_MapFormatsObject; $1; $2)
C_TEXT:C284(PPro_MapFormatsObject; $3; $4)

C_LONGINT:C283(PPro_OnDragOver; $0)
C_REAL:C285(PPro_NativeDataDrop; $1; $2)
C_LONGINT:C283(PPro_NativeDataDrop; $3)
C_REAL:C285(PPro_TextDataDrop; $1; $2)
C_LONGINT:C283(PPro_TextDataDrop; $3)
C_REAL:C285(PPro_PictDataDrop; $1; $2)
C_LONGINT:C283(PPro_PictDataDrop; $3)

C_TEXT:C284(PPro_Util_Fix4DNode; $1; $0)

C_LONGINT:C283(PPro_PlacePicture; $1; $0)  // report, created object
C_PICTURE:C286(PPro_PlacePicture; $3)
C_REAL:C285(PPro_PlacePicture; $4; $5; $6; $7)  // pos and dimensions
C_LONGINT:C283(PPro_PlacePicture; $8)  // format between 0 and 4 
C_TEXT:C284(PPro_PlacePicture; $2; $9)  // section ID, 'front' or 'back' 

C_REAL:C285(PPro_BlueHandler; $1; $2)
C_LONGINT:C283(PPro_BlueHandler; $3)

C_REAL:C285(PPro_ToPt; $0; $1)
C_TEXT:C284(PPro_ToPt; $2)
C_LONGINT:C283(PPro_ToPt; $3)

C_LONGINT:C283(PPro_PlaceText; $1; $0)  // report, section, created object
C_TEXT:C284(PPro_PlaceText; $3)
C_REAL:C285(PPro_PlaceText; $4; $5; $6; $7)  // pos and dimensions
C_TEXT:C284(PPro_PlaceText; $8)  // style ID 
C_TEXT:C284(PPro_PlaceText; $2; $9)  // 

C_LONGINT:C283(PPro_OffscreenReport; $2; $0)
C_TEXT:C284(PPro_OffscreenReport; $1)

// exec methods
C_TEXT:C284(PPro_ExecBuildIndentedText; $0)
C_POINTER:C301(PPro_ExecBuildIndentedText; $1; $2)
C_LONGINT:C283(PPro_ExecFindBackInArray; $1; $2; $0)
C_TEXT:C284(PPro_ExecInsertLine; $1)
C_TEXT:C284(PPro_ExecSplitToLines; $1)
C_POINTER:C301(PPro_ExecSplitToLines; $2; $3)
C_TEXT:C284(PPro_ExecTokenizeCondition; $1)
C_BLOB:C604(PPro_ExecTokenizeCondition; $0)
C_TEXT:C284(PPro_ExecTokenizeForCondition; $1)
C_POINTER:C301(PPro_ExecTokenizeForCondition; $2; $3; $4)
C_TEXT:C284(PPro_ExecDataHandler; $1)
C_LONGINT:C283(PPro_ExecDataHandler; $2; $0)

// JSON methods
C_TEXT:C284(PPro_JSON_DecodeObject; $1)
C_POINTER:C301(PPro_JSON_DecodeObject; $2; $3)
C_TEXT:C284(PPro_JSON_DecodeObject; $1)
C_POINTER:C301(PPro_JSON_DecodeObject; $2; $3)
C_TEXT:C284(PPro_JSON_GetNextChunk; $0; $1)
C_POINTER:C301(PPro_JSON_GetNextChunk; $2)
C_REAL:C285(PPro_JSON_NumberDecode; $0)
C_TEXT:C284(PPro_JSON_NumberDecode; $1)
C_REAL:C285(PPro_JSON_NumberEncode; $1)
C_TEXT:C284(PPro_JSON_NumberEncode; $0)
C_TEXT:C284(PPro_JSON_StringDecode; $0; $1)
C_TEXT:C284(PPro_JSON_StringEncode; $0; $1)
C_TEXT:C284(PPro_JSON_GetNextChar; $1; $2)
C_LONGINT:C283(PPro_JSON_GetNextChar; $0; $3)

// v1.4
C_TEXT:C284(PPro_TabArraysFlatten; $0)
C_TEXT:C284(PPro_TabArraysBuild; $1)

C_TEXT:C284(PPro_EditMenu; $1)

C_REAL:C285(PPro_PictFileDrop; $1; $2)
C_LONGINT:C283(PPro_PictFileDrop; $3)
