C_LONGINT:C283($color)
If (RW_ColorPicker(PP_TextColor)=1)
	$color:=PP_TextColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "PP_TextColor"; 0x0000; $color)  // Frank 01.06.11
End if 