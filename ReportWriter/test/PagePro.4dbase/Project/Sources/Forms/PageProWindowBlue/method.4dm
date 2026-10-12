Case of 
	: (Form event code:C388=On Load:K2:1)
		C_LONGINT:C283($err)
		If (PPro_EditorCallback("form"; "load"; PP_ExtArea; 0))
			
			If (Length:C16(PP_Report)>0x0000)
				$err:=RW_ParseReport(PP_ExtArea; PP_Report; 0x0000)  // 
				// modified 01.08.2011/06:51/SDS Software GmbH/Mußhoff
			Else 
				// modified 18.06.2011/08:04/SDS Software GmbH/Mußhoff
				If (Shift down:C543)
					PPro_CommandDispatch("new_dynamic")
				Else 
					PPro_CommandDispatch("new_static")
				End if 
				// end
			End if 
		End if 
		
		PPro_StylesLoadArray
		
		PPro_ResetSelection(PP_ExtArea)
		$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
		
		PP_ListBox:=0  //highlight
		PP_Dialogs:=1  //
		OBJECT SET VISIBLE:C603(*; "PP_LB@"; False:C215)
		PPro_SubformVisibility(PP_ExtArea)
		PPro_BuildFontMenu
		//FONT LIST(PP_Fonts;*) // pB 2012-8-5 replaced byt internal routine
		PP_FormatPopup:=PPro_LoadHList("PP_Formats"; "PageProMenus.xml")
		
		PP_Scaling:=Round:C94(PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Scale:K7906:123)*100; 2)
		PPro_AreaFormData(PP_ExtArea)
		
		//Preload lists and popups
		STRING LIST TO ARRAY:C511(13001; PP_AlignPopup)
		STRING LIST TO ARRAY:C511(13002; PP_PrintPopup)
		STRING LIST TO ARRAY:C511(13005; PP_PictFormatPopup)
		STRING LIST TO ARRAY:C511(13007; PP_WidthPopup)
		STRING LIST TO ARRAY:C511(13008; PP_EmptyPopup)
		// STRING LIST TO ARRAY(13011;PP_TextTabs)
		STRING LIST TO ARRAY:C511(13102; PP_TextTabs)  // Frank 01.06.11
		//STRING LIST TO ARRAY(13104;PP_TextTabsVar)  // Frank 02.06.11
		STRING LIST TO ARRAY:C511(13012; PP_ElementPopup)
		STRING LIST TO ARRAY:C511(13013; PP_CalcPopup)
		STRING LIST TO ARRAY:C511(13014; PP_DataSourcePopup)
		STRING LIST TO ARRAY:C511(13015; PP_Relate1Popup)
		STRING LIST TO ARRAY:C511(13015; PP_RelateManyPopup)
		STRING LIST TO ARRAY:C511(13017; PP_PageBreakPopup)
		STRING LIST TO ARRAY:C511(13018; PP_BreakOnPopup)
		STRING LIST TO ARRAY:C511(13019; PP_ElementPopup)
		STRING LIST TO ARRAY:C511(13020; PP_Units)
		STRING LIST TO ARRAY:C511(13022; PP_DataSourcePopupMirror)
		STRING LIST TO ARRAY:C511(13023; PP_RelatePopupMirror)
		STRING LIST TO ARRAY:C511(13024; PP_AlignPopupMirror)
		STRING LIST TO ARRAY:C511(13025; PP_PrintPopupMirror)
		STRING LIST TO ARRAY:C511(13026; PP_RepeatPopup)
		STRING LIST TO ARRAY:C511(13027; PP_RepeatMirror)
		
		// pB v1.3 checking proper decimal char
		PPro_CheckDecimal(->PP_WidthPopup)
		
		PP_ColumnPopup:=PPro_JoinLists(1)
		PP_TablePopup:=PPro_JoinLists(2)
		
		PP_ObjectsAlignPopup:=1
		
	: (Form event code:C388=On Drop:K2:12)
		PPro_DragNDropHandler
		
	: (Form event code:C388=On Timer:K2:25)
		SET TIMER:C645(0x0000)
		SELECT LIST ITEMS BY REFERENCE:C630(PP_XMLRoot; 0x0000; PP_ItemsToSelect)
		
	: (Form event code:C388=On Close Box:K2:21)
		If (PPro_EditorCallback("form"; "close"; PP_ExtArea; 0))
			C_TEXT:C284($text)
			$err:=RW_SaveReport(PP_ExtArea; $text; 0x0000)  // third arg means it is a text variable
			If (PP_Report#$text)
				C_LONGINT:C283($WinRef)
				$WinRef:=Open form window:C675("Confirm3ButtonsBlue"; Sheet form window:K39:12; Horizontally centered:K39:1; Vertically centered:K39:4)
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
		
End case 