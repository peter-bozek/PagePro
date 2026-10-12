//this form is unused, was used as a template 
// for properties that appear in many objects


Case of 
	: (Form event code:C388=On Bound Variable Change:K2:52)
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s)
		
		PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16)
		PP_PrintOrder:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4)
		
		PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18)
		PP_LockedBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19)
		PP_FixVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31)
		PP_BindVBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32)
		
		PP_PrintPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52)+1
		PP_AlignPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Alignment:K7906:33)+1
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Clicked:K2:4))
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintOrder:K7906:4; PP_PrintOrder)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18; PP_VisibleBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19; PP_LockedBtn*2)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedVerticalPos:K7906:31; PP_FixVBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindVerticalPos:K7906:32; PP_BindVBtn)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52; PP_PrintPopup-1)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Alignment:K7906:33; PP_AlignPopup-1)
		
End case 