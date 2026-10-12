//%attributes = {"shared":true}
C_TEXT:C284($1)

C_BOOLEAN:C305($b)

If (False:C215)
	C_TEXT:C284(PPro_CommandDispatch; $1)
End if 

C_LONGINT:C283($err)

Case of 
		//menu commands
	: ($1="new")
		$b:=PPro_NewReportDlg(PP_ExtArea; "new")
		PPro_ToolsReset
		PPro_StylesLoadArray
		PPro_ResetSelection(PP_ExtArea)
		$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
		
	: ($1="new_static")
		$b:=PPro_NewReportDlg(PP_ExtArea; "static")
		PPro_ToolsReset
		PPro_StylesLoadArray
		PPro_ResetSelection(PP_ExtArea)
		$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
		
	: ($1="new_dynamic")
		$b:=PPro_NewReportDlg(PP_ExtArea; "dynamic")
		PPro_ToolsReset
		PPro_StylesLoadArray
		PPro_ResetSelection(PP_ExtArea)
		$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
		
	: ($1="open")
		$b:=PPro_OpenReportDlg(PP_ExtArea)
		PPro_ToolsSet
		PPro_StylesLoadArray
		PPro_ResetSelection(PP_ExtArea)
		$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
		
	: ($1="save")
		$b:=PPro_SaveReport(PP_ExtArea; 1)
		
	: ($1="saveAs")
		$b:=PPro_SaveReport(PP_ExtArea; 0)
		
	: ($1="revert")
		$err:=RW_ParseReport(PP_ExtArea; PP_Report; 0x0000)  // 
		
	: ($1="pageSetup")
		If (Shift down:C543)
			$err:=RW_PrintSettings(PP_ExtArea; PP_AskPageSetup:K7907:12 | PP_AskJobSetup:K7907:13 | PP_ValidatePageSetup:K7907:6)
		Else 
			$err:=RW_PrintSettings(PP_ExtArea; PP_AskJobSetup:K7907:13)  // PP_AskPageSetup | PP_ValidatePageSetup)
		End if 
		
	: ($1="print")
		PPro_PrintReport(PP_ExtArea; PP_AskPageSetup:K7907:12 | PP_AskJobSetup:K7907:13)  // PP_Default)
		
	: ($1="preview")
		PPro_PrintReport(PP_ExtArea; PP_DestinationPreview:K7907:4)
		
	: ($1="printRW")
		PPro_PreviewRW(""; PP_Default:K7907:17)
		
	: ($1="exit")
		If (PPro_EditorCallback("form"; "close"; PP_ExtArea; 0))
			C_TEXT:C284($text)
			$err:=RW_SaveReport(PP_ExtArea; $text; 0x0000)  // third arg means it is a text variable
			If (PP_Report#$text)
				C_LONGINT:C283($WinRef)
				$WinRef:=Open form window:C675("Confirm3Buttons"+PP_Environment; Sheet form window:K39:12; Horizontally centered:K39:1; Vertically centered:K39:4)
				DIALOG:C40("Confirm3Buttons")
				CLOSE WINDOW:C154($WinRef)
				
				If (OK=1)
					If (PP_Answer="OK")
						PP_Report:=$text
						PP_ReportModified:=1
					End if 
					CANCEL:C270
				End if 
			Else 
				CANCEL:C270
			End if 
		End if 
		
		
		//edit commands
	: ($1="prefs")
		//what prefs? 
		
End case 
