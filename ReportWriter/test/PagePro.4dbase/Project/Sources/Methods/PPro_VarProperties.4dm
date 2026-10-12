//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_VarProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s; $text; $repeating)
		C_LONGINT:C283($err; $i; $j)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If (($text="Var") | ($text="Field"))  //
			
			OBJECT SET VISIBLE:C603(*; "field@"; ($text#"Field"))
			
			$repeating:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; "repe")
			If (($repeating="") | ($repeating="none"))
				
				PPro_ObjectPropertiesSubform($sel)  //handling of object common properties
				
				If ($text="Var")
					PP_SourcePopup:=PP_VariableList
				Else   //field
					PP_SourcePopup:=PP_TableList
				End if 
				
				PP_Script:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54)
				
				PP_Source:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80)
				PP_Alias:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81)
				PP_Format:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41)
				
				If ((PP_Alias="") & (PP_Source#""))
					PP_Alias:=PP_Source
				End if 
				
				PP_CalcPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_CalculationType:K7906:83)+1
				PP_Element:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Element:K7906:82)
				
				If (PP_TextTabs>0)
					PPro_TextTabsVisibility(PP_TextTabs)
				Else 
					PPro_TextTabsVisibility(1)
				End if 
				
				PP_Content:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
				
				PP_MultiStyleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48)
				PP_DynamicBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47)
				PP_KeepTogetherBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50)
				PP_ExpandableBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86)
				
				PP_EmptyPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51)+1
				
				PP_FrameBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42)
				PP_FrameOffset:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43)
				PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44)
				PP_FrameColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45))
				PP_TransparentFrameColor:=Num:C11((PP_FrameColor >> 0x0018)=0)
				C_LONGINT:C283($color)
				$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
				OBJECT SET RGB COLORS:C628(*; "@FrameColor"; $color; $color)
				
				PPro_StylePropertiesHandler("get")
				
			End if 
		End if 
		
	: ($sel="set")
		
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If (($text="Var") | ($text="Field"))  //
			$repeating:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; "repe")
			If (($repeating="") | ($repeating="none"))
				PPro_ObjectPropertiesSubform($sel)  //handling of object common properties$err:=RW_Area_SaveUndo (PP_ExtArea;1)  //start undo block
				
				$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81; PP_Alias)
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80; PP_Source)
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41; PP_Format)
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54; PP_Script)
				
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_CalculationType:K7906:83; PP_CalcPopup-1)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Element:K7906:82; PP_Element)
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; PP_Content)
				
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47; PP_DynamicBtn)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50; PP_KeepTogetherBtn)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86; PP_ExpandableBtn)
				
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51; PP_EmptyPopup-1)
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Name:K7906:3; PP_ObjectName)
				
				PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | (0xFF000000*(1-PP_TransparentFrameColor))
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45; PP_FrameColor)
				$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44; PP_LineWidth)
				$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43; PP_FrameOffset)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42; PP_FrameBtn)
				
				PPro_StylePropertiesHandler("set")
				
				$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			End if 
		End if 
End case 