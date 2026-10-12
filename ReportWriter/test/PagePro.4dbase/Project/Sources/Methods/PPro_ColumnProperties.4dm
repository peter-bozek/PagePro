//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_ColumnProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Column")
			
			PP_ColumnNo:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4)
			
			PP_Script:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54)
			PP_Source:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80)
			PP_Alias:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81)
			PP_Format:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41)
			PP_Level:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Level:K7906:63)
			
			PP_RowNoBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RowNumber:K7906:64)
			PP_DuplicatesBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Duplicates:K7906:65)
			
			PP_MultiStyleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48)
			PP_GridBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Grid:K7906:62)
			
			PP_ColumnWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6)
			
			PPro_StylePropertiesHandler("get")
		End if 
		
		
	: ($sel="set")
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Column")
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80; PP_Source)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81; PP_Alias)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41; PP_Format)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Level:K7906:63; PP_Level)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54; PP_Script)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RowNumber:K7906:64; PP_RowNoBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Duplicates:K7906:65; PP_DuplicatesBtn)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Grid:K7906:62; PP_GridBtn)
			
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6; PP_ColumnWidth)
			
			PPro_StylePropertiesHandler("set")
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			
		End if 
End case 