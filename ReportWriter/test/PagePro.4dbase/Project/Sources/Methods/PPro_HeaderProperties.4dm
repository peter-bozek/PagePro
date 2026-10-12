//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_HeaderProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a table header object
		C_TEXT:C284($text)
		
		$err:=RW_GetProperty(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; "kind"; ->$text)
		If ($text="Header")
			
			PP_PrintOrder:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4)
			PP_Content:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
			
			PP_MultiStyleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48)
			PP_DynamicBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47)
			
			PP_Columns:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_ColSpan:K7906:60)
			PP_Rows:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RowSpan:K7906:61)
			
			PP_CellHeight:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7)
			PP_ColumnWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6)
			
			PPro_StylePropertiesHandler("get")
		End if 
		
	: ($sel="set")
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		$err:=RW_GetProperty(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; "kind"; ->$text)
		If ($text="Header")
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; PP_Content)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47; PP_DynamicBtn)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_ColSpan:K7906:60; PP_Columns)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RowSpan:K7906:61; PP_Rows)
			
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7; PP_CellHeight)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6; PP_ColumnWidth)
			
			PPro_StylePropertiesHandler("set")
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 