
C_LONGINT:C283($color)
If (RW_ColorPicker(PP_TextColor)=1)
	$color:=PP_TextColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "@TextColor"; 0x0000; $color)
End if 

C_LONGINT:C283($err)
PP_TextColor:=((PP_TextOpacity & 0x00FF) << 0x0018) | (PP_TextColor & 0x00FFFFFF)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_TextColor:K7906:71; PP_TextColor)
