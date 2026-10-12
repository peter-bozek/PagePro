// modified 01.06.2011/18:14/SDS Software GmbH/Mußhoff
If (Self:C308->=1)
	PP_FillOpacity:=0
End if 

C_LONGINT:C283($err)
PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | ((PP_FillOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_BackColor:K7906:72; PP_BackColor)
