If (Form event code:C388=On Outside Call:K2:11)
	Case of 
		: (<>PP_PaletteMessage="quit")
			CANCEL:C270
	End case 
	<>PP_PaletteMessage:=""
End if 