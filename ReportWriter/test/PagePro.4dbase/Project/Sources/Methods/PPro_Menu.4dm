//%attributes = {"shared":true}
// PPro_Menu(menuCommandParameter)
//
// top level menu dispatching method

C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_Menu; $1)
End if 

C_TEXT:C284($s)
C_BOOLEAN:C305($rebuildList)
C_LONGINT:C283($err)

// modified 23.06.2011/10:40/SDS Software GmbH/Mu√ühoff
If (Count parameters:C259>0)
	$s:=$1
Else 
	$s:=Get selected menu item parameter:C1005
End if 
// end

$rebuildList:=False:C215
If (PPro_EditorCallback("command"; $s; 0; 0))
	
	Case of 
			//file menu action`
			// modified 18.06.2011/07:58/SDS Software GmbH/Mu√ühoff
		: ($s="flNewStatic")
			PPro_CommandDispatch("new_static")
			$rebuildList:=True:C214
			
		: ($s="flNewDynamic")
			PPro_CommandDispatch("new_dynamic")
			$rebuildList:=True:C214
			// end
			
		: ($s="flNew")
			PPro_CommandDispatch("new")
			$rebuildList:=True:C214
			
		: ($s="flOpen")
			PPro_CommandDispatch("open")
			$rebuildList:=True:C214
			
		: ($s="flSave")
			PPro_CommandDispatch("save")
		: ($s="flSaveAs")
			PPro_CommandDispatch("saveAs")
		: ($s="flRevert")
			PPro_CommandDispatch("revert")
			$rebuildList:=True:C214
			
		: ($s="flExport@")
			PPro_ExportReport(Substring:C12($s; 9))
			
		: ($s="flPageS")
			PPro_CommandDispatch("pageSetup")
		: ($s="flPrint")
			PPro_CommandDispatch("print")
		: ($s="flPrintPr")
			PPro_CommandDispatch("preview")
		: ($s="flPrintRW")
			PPro_CommandDispatch("printRW")
			
		: ($s="flExit")
			PPro_CommandDispatch("exit")
			
		: ($s="edDuplicate")
			PPro_ObjectCommandsDispatch("duplicate")
			$rebuildList:=True:C214
			
		: ($s="obProps")
			PPro_ObjectCommandsDispatch("edit")
		: ($s="obFront")
			PPro_ObjectCommandsDispatch("bring2Front")
		: ($s="obBack")
			PPro_ObjectCommandsDispatch("bring2Back")
			
		: ($s="obGroup")
			PPro_ObjectCommandsDispatch("group")
		: ($s="obUngroup")
			PPro_ObjectCommandsDispatch("ungroup")
			
		: ($s="obLock")
			PPro_ObjectCommandsDispatch("lock")
		: ($s="obUnlock")
			PPro_ObjectCommandsDispatch("unlock")
			
		: ($s="obHide")
			PPro_ObjectCommandsDispatch("hide")
		: ($s="obShow")
			PPro_ObjectCommandsDispatch("show")
			
		: ($s="obNotPrint")
			PPro_ObjectCommandsDispatch("dontprint")
			
		: ($s="st@")  //style 
			PPro_TextCommandDispatch($s)
			$rebuildList:=False:C215  //no change 
			
		: ($s="ha@")  //horizontal alignment
			PPro_TextCommandDispatch($s)
			$rebuildList:=False:C215  //no change 
			
		: ($s="va@")  //vertical alignment
			PPro_TextCommandDispatch($s)
			$rebuildList:=False:C215  //no change 
			
		: ($s="tx@")  //other text settings
			PPro_TextCommandDispatch($s)
			$rebuildList:=False:C215  //no change 
			
		: ($s="menuI=@")  //font settings
			PPro_TextCommandDispatch($s)
			$rebuildList:=False:C215  //no change 
			
		: ($s="dpSize1")
			PPro_ScaleTo(PP_ExtArea; 1)
			
		: ($s="dpPaper")
			PPro_ScaleTo(PP_ExtArea; -2)
			
		: ($s="dpBig")
			PPro_ScaleUp(PP_ExtArea)
			
		: ($s="dpSmall")
			PPro_ScaleDown(PP_ExtArea)
			
		: ($s="dp@")  //rulers etc settings
			PPro_ReportProperty(Substring:C12($s; 3); PP_ExtArea)
			
		: ($s="unitSetup")  //rulers etc settings"
			PPro_UnitsDlg(PP_ExtArea)
			
		: ($s="inStyle")  //new style"
			PPro_NewStyle(PP_ExtArea)
			PPro_StylesLoadArray
			$rebuildList:=True:C214
			
		: ($s="inSection")  //new section
			PPro_NewSectionDlg
			$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
			$rebuildList:=True:C214
			
		: ($s="refInsert")  //new section
			PPro_InsertReference
			$rebuildList:=False:C215  //no change 
			
		: ($s="refUpdate")  //reload values
			If (PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)=7)  // values are shown
				PPro_ReportProperty("Normal"; PP_ExtArea)  //reset
				PPro_ReportProperty("Values"; PP_ExtArea)  //force update
			End if 
			
		: ($s="refFreeze")  //freeze values
			CONFIRM:C162(Get localized string:C991("PP_ConfirmFreeze"); Get localized string:C991("PP_ConfirmFreezeOK"))
			If (OK=1)
				PPro_FreezeValues(PP_ExtArea)  //freeze
			End if 
			
		Else 
			TRACE:C157
	End case 
	
End if 

If ($rebuildList)
	PPro_AreaReloadHList(PP_ExtArea)
End if 

PPro_AreaFormData(PP_ExtArea)