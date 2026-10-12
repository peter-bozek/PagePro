C_LONGINT:C283($err)
PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | ((PP_FillOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_BackColor:K7906:72; PP_BackColor)
