C_LONGINT:C283($color)
If (RW_ColorPicker(PP_FrameColor)=1)
	$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
	OBJECT SET RGB COLORS:C628(*; "@FrameColor"; 0x0000; $color)  // Frank 01.06.11
End if 

C_LONGINT:C283($err)
PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | ((PP_FrameOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_FrameColor:K7906:45; PP_FrameColor)
