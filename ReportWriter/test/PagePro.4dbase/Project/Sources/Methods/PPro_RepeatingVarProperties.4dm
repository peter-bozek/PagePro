//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_RepeatingVarProperties; $1)
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
			$repeating:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; "repe")
			If (($repeating#"") & ($repeating#"none"))
				
				If ($text="Var")
					PP_SourcePopup:=PP_VariableList
				Else   //field
					PP_SourcePopup:=PP_TableList
				End if 
				
				PPro_ObjectPropertiesSubform($sel)  //handling of object common properties
				
				C_LONGINT:C283($j)
				$j:=Find in array:C230(PP_RepeatMirror; $repeating)
				If ($j>0)
					PP_RepeatPopup:=$j
				Else 
					PP_RepeatPopup:=1
				End if 
				
				PP_Offset:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; "repo")
				
				PP_Source:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80)
				PP_Alias:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81)
				PP_Format:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41)
				
				If ((PP_Alias="") & (PP_Source#""))
					PP_Alias:=PP_Source
				End if 
				
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
			If (($repeating#"") & ($repeating#"none"))
				PPro_ObjectPropertiesSubform($sel)  //handling of object common properties$err:=RW_Area_SaveUndo (PP_ExtArea;1)  //start undo block
				
				$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "repe"; PP_RepeatMirror{PP_RepeatPopup})
				$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; "repo"; PP_Offset)
				
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81; PP_Alias)
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41; PP_Format)
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80; PP_Source)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51; PP_EmptyPopup-1)
				
				PPro_StylePropertiesHandler("set")
				
				$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			End if 
		End if 
End case 