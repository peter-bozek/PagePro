C_LONGINT:C283($color)
If (RW_ColorPicker(PP_LineColor)=1)
	$color:=PP_LineColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "PP_LineColor"; 0x0000; $color)
End if 