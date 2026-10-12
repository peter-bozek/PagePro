C_LONGINT:C283($color)
If (RW_ColorPicker(PP_FillColor)=1)
	$color:=PP_FillColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "PP_LineColor1"; 0x0000; $color)
End if 