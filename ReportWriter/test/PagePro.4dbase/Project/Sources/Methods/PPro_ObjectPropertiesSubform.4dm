//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_ObjectPropertiesSubform; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//common procedure called from all objects
		C_TEXT:C284($s)
		
		PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16)
		PP_PrintOrder:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4)
		
		PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18)
		PP_LockedBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19)
		PP_FixVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31)
		PP_BindVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32)
		
		PP_PrintPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52)+1
		PP_AlignPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Alignment:K7906:33)+1
		
	: ($sel="set")
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
		
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4; PP_PrintOrder)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18; PP_VisibleBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19; PP_LockedBtn*PP_LockedMode)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31; PP_FixVBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32; PP_BindVBtn)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52; PP_PrintPopup-1)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Alignment:K7906:33; PP_AlignPopup-1)
		$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //start undo block
		
End case 