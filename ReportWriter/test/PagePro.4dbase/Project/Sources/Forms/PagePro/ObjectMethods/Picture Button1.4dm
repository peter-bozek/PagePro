If (PP_ButtonGroup=1)
	PPro_ObjectGroup(PP_ExtArea; "group")
Else 
	PPro_ObjectGroup(PP_ExtArea; "ungroup")
End if 
PPro_AreaReloadHList(PP_ExtArea)
PPro_AreaFormData(PP_ExtArea)
