//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_TextProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err; $i)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Text")  //
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
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
			PP_FrameOpacity:=(PP_FrameColor >> 0x0018) & 0x00FF
			PP_TransparentFrameColor:=Num:C11(PP_FrameOpacity=0)
			C_LONGINT:C283($color)
			$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "@FrameColor"; 0x0000; $color)
			
			PPro_StylePropertiesHandler("get")
			
		End if 
		
	: ($sel="set")
		
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Text")
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; PP_Content)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47; PP_DynamicBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50; PP_KeepTogetherBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86; PP_ExpandableBtn)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51; PP_EmptyPopup-1)
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Name:K7906:3; PP_ObjectName)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72; PP_BackColor)
			PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | ((PP_FrameOpacity & 0x00FF) << 0x0018)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45; PP_FrameColor)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44; PP_LineWidth)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43; PP_FrameOffset)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42; PP_FrameBtn)
			
			PPro_StylePropertiesHandler("set")
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			
			PP_Content:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
		End if 
End case 