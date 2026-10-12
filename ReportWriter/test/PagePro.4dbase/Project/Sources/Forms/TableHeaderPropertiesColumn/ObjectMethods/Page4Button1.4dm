C_LONGINT:C283($color)
If (RW_ColorPicker(PP_BackColor)=1)
	$color:=PP_BackColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "@BackColor"; 0x0000; $color)
End if 

C_LONGINT:C283($err)
PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | ((PP_FillOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_BackColor:K7906:72; PP_BackColor)
