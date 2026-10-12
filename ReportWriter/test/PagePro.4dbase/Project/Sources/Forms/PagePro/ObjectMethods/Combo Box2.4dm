If (PP_Units=0)
	PP_Units:=1
End if 
C_LONGINT:C283($err)
$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_RulerUnit:K7906:115; PP_Units)
PPro_AreaFormData(PP_ExtArea)
