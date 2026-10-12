//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_TableProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1


Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Table")
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
			PP_Script:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54)
			
			PP_HeaderBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawHeaders:K7906:58)
			PP_HeaderLines:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Headers:K7906:56)
			PP_ColumnsBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawColumns:K7906:59)
			PP_Columns:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PPRop_Columns:K7906:55)
			PP_RowHeight:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7)
			PP_RowGridWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_GridThickness:K7906:57)
			
			PP_FrameBtn:=Num:C11(PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42)>0)
			PP_DoubleFrameBtn:=Num:C11(PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42)=2)
			PP_FrameOffset:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43)
			PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44)
			PP_FrameColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45))
			PP_FrameOpacity:=(PP_FrameColor >> 0x0018) & 0x00FF
			C_LONGINT:C283($color)
			$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "@FrameColor"; 0x0000; $color)
		End if 
		
		
	: ($sel="set")
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Table")
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54; PP_Script)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawHeaders:K7906:58; PP_HeaderBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Headers:K7906:56; PP_HeaderLines)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawColumns:K7906:59; PP_ColumnsBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PPRop_Columns:K7906:55; PP_Columns)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7; PP_RowHeight)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_GridThickness:K7906:57; PP_RowGridWidth)
			
			PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | ((PP_FrameOpacity & 0x00FF) << 0x0018)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45; PP_FrameColor)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44; PP_LineWidth)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43; PP_FrameOffset)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42; PP_FrameBtn*(1+PP_DoubleFrameBtn))
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 