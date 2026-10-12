C_LONGINT:C283($err)
PP_Alias:=PP_Source
PP_Source:=PPro_ResolveAlias(PP_Alias)

$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81; PP_Alias)
$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80; PP_Source)
