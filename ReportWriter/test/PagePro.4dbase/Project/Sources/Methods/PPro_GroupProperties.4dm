//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_GroupProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		
		C_TEXT:C284($s; $text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Group")  //
			
			PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16)
			PP_PrintOrder:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4)
			
			PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18)
			PP_LockedBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19)
			PP_FixVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31)
			PP_BindVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32)
			PP_ExpandableBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86)
			
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; "draw")
			PP_PrintPopup:=Find in array:C230(PP_PrintPopupMirror; $s)
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; "algn")
			PP_AlignPopup:=Find in array:C230(PP_AlignPopupMirror; $s)
		End if 
		
	: ($sel="set")
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Group")  //
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4; PP_PrintOrder)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18; PP_VisibleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19; PP_LockedBtn*2)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31; PP_FixVBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32; PP_BindVBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86; PP_ExpandableBtn)
			
			$s:=PP_PrintPopupMirror{PP_PrintPopup}
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "draw"; $s)
			$s:=PP_AlignPopupMirror{PP_AlignPopup}
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "algn"; $s)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
		
End case 