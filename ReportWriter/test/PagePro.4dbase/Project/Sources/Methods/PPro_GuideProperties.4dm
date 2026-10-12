//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_GuideProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Guide")  //security
			If (PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Type:K7906:17)=0)
				PP_Type:=Get localized string:C991("PP_GuideTypeHor")
			Else 
				PP_Type:=Get localized string:C991("PP_GuideTypeVert")
			End if 
			PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16)
			PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18)
			PP_LockedBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19)
			PP_Point1x:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
		End if 
		
	: ($sel="set")
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Guide")
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			C_LONGINT:C283($err)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; PP_Point1x)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Visible:K7906:18; PP_VisibleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19; PP_LockedBtn)
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 