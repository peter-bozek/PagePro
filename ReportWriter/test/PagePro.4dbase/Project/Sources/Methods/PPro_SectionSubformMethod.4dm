//%attributes = {"invisible":true}

Case of 
	: (Form event code:C388=On Bound Variable Change:K2:52)
		C_TEXT:C284($s; $text)
		C_REAL:C285($r1; $r2)
		C_LONGINT:C283($l; $err)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Section")  //security
			PP_Type:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Type:K7906:17)
			
			PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16)
			PP_GrowBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86)
			PP_OnTopBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_OnTop:K7906:101)
			PP_FillBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Fill:K7906:36)
			PP_DrawBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52)
			PP_KeepTogetherBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50)
			PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18)
			
			PP_Script:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54)
			
			PP_PageBreakPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_ThrowPage:K7906:87)+1
			PP_BindBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindToBottom:K7906:88)
			PP_AlwaysBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintAlways:K7906:100)
			
			PP_BreakOnPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BreakType:K7906:96)+1
			PP_Source:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_BreakOn:K7906:95)
			PP_Level:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BreakLevel:K7906:94)
			
			PP_FirstBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FirstPage:K7906:89)
			PP_LastBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_LastPage:K7906:90)
			
			C_LONGINT:C283($treeState)
			$treeState:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_OddPage:K7906:91)
			PP_OddBtn:=Num:C11($treeState>0)
			PP_Odd2Btn:=Num:C11($treeState=2)
			
			$treeState:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_EvenPage:K7906:92)
			PP_EvenBtn:=Num:C11($treeState>0)
			PP_Even2Btn:=Num:C11($treeState=2)
			
			PP_Point1x:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_MinimumSpace:K7906:85)
		End if 
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Clicked:K2:4))
		C_TEXT:C284($text)
		C_LONGINT:C283($err)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Section")
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FixedHeight:K7906:86; PP_GrowBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_OnTop:K7906:101; PP_OnTopBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Fill:K7906:36; PP_FillBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52; PP_DrawBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_ThrowPage:K7906:87; PP_PageBreakPopup-1)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BindToBottom:K7906:88; PP_BindBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_PrintAlways:K7906:100; PP_AlwaysBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BreakType:K7906:96; PP_BreakOnPopup-1)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_BreakOn:K7906:95; PP_Source)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BreakLevel:K7906:94; PP_Level)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50; PP_KeepTogetherBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18; PP_VisibleBtn)
			
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54; PP_Script)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FirstPage:K7906:89; PP_FirstBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_LastPage:K7906:90; PP_LastBtn)
			
			C_LONGINT:C283($treeState)
			$treeState:=PP_OddBtn+(PP_OddBtn*PP_Odd2Btn)  //instead of two ifs
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_OddPage:K7906:91; $treeState)
			
			$treeState:=PP_EvenBtn+(PP_EvenBtn*PP_Even2Btn)  //instead of two ifs
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_EvenPage:K7906:92; $treeState)
			
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_MinimumSpace:K7906:85; PP_Point1x)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			
		End if 
End case 