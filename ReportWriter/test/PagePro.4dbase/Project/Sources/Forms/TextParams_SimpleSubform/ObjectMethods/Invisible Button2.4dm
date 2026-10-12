C_LONGINT:C283($color)
If (RW_ColorPicker(PP_BackColor)=1)
	$color:=PP_BackColor & 0x00FFFFFF  // alpha
	OBJECT SET RGB COLORS:C628(*; "PP_BackColor"; 0x0000; $color)
End if 