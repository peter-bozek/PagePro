Case of 
	: (Form event code:C388=On Load:K2:1)
		
		PPro_BuildFontMenu
		//FONT LIST(PP_Fonts;*) // pB 2012-8-5 replaced byt internal routine
		PP_FormatPopup:=PPro_LoadHList("PP_Formats"; "PageProMenus.xml")
		
		C_LONGINT:C283($err)
		If (PPro_EditorCallback("form"; "load"; PP_ExtArea; 0))
			If (Length:C16(PP_Report)>0x0000)
				$err:=RW_ParseReport(PP_ExtArea; PP_Report; 0x0000)  // 
			Else 
				PPro_CommandDispatch("new")
			End if 
		End if 
		
		PPro_InitDocument
		PP_ObjectsAlignPopup:=1  // force display
		
	: (Form event code:C388=On Bound Variable Change:K2:52)
		// form is included as a subform in parent form
		// when content of the report changes, force redraw of its subforms
		SELECT LIST ITEMS BY REFERENCE:C630(PP_XMLRoot; 0x0000; PP_ItemsToSelect)
		
	: (Form event code:C388=On Timer:K2:25)
		SET TIMER:C645(0x0000)
		SELECT LIST ITEMS BY REFERENCE:C630(PP_XMLRoot; 0x0000; PP_ItemsToSelect)
		
	: (Form event code:C388=On Drop:K2:12)
		PPro_DragNDropHandler
		
	: (Form event code:C388=On Close Box:K2:21)
		If (PPro_EditorCallback("form"; "close"; PP_ExtArea; 0))
			C_TEXT:C284($text)
			$err:=RW_SaveReport(PP_ExtArea; $text; 0x0000)  // third arg means it is a text variable
			If (PP_Report#$text)
				C_LONGINT:C283($WinRef)
				$WinRef:=Open form window:C675("Confirm3Buttons"; Sheet form window:K39:12; Horizontally centered:K39:1; Vertically centered:K39:4)
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
		
	: (Form event code:C388=On Unload:K2:2)
		CLEAR LIST:C377(PP_ColumnPopup)
		CLEAR LIST:C377(PP_TablePopup)
		CLEAR LIST:C377(PP_FormatPopup)
		
		C_BOOLEAN:C305($b)
		$b:=PPro_EditorCallback("form"; "unload"; PP_ExtArea; 0)
		
		PPro_Palette("quit")
		
	: (Form event code:C388=On Deactivate:K2:10)
		PPro_Palette("deactivate")
		
	: (Form event code:C388=On Activate:K2:9)
		// PPro_Palette ("deactivate")
		
End case 