Case of 
	: (Form event code:C388=On Load:K2:1)
		PPro_BuildFontMenu
		
		C_TEXT:C284($text; $value)
		C_LONGINT:C283($err)
		
		$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$text)
		Case of 
			: ($text="Text")
				$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$value)
			: ($text="Var")
				$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				If ($value=PPro_ResolveAlias($value))
					$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
				End if 
			: ($text="Field")
				$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				If ($value=PPro_ResolveAlias($value))
					$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
				End if 
			: ($text="Header")
				$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$value)
			: ($text="Column")
				$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				If ($value=PPro_ResolveAlias($value))
					$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
				End if 
		End case 
		
		C_LONGINT:C283($size; $style; $color)
		C_REAL:C285($scale)
		C_TEXT:C284($font)
		C_LONGINT:C283($platform)
		_O_PLATFORM PROPERTIES:C365($platform)
		
		PPro_TextGetProperties(PP_ExtArea; PP_CurrentObject; ->$font; ->$size; ->$style; ->$scale; ->$color)
		If ($platform=Windows:K25:3)
			$scale:=$scale*0.75
		End if 
		If (True:C214)  // does not work in all versions o 4D
			OBJECT SET FONT:C164(PP_InlineEditPlain; $font)
			OBJECT SET FONT SIZE:C165(PP_InlineEditPlain; $size*$scale)  // ushen passing back
			If ($style ?? 0)
				OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Bold:K14:2)
			End if 
			If ($style ?? 1)
				OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Italic:K14:3)
			End if 
			If ($style ?? 2)
				OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Underline:K14:4)
			End if 
		Else 
			$err:=RW_AddStyle($text; $value; 1; Length:C16(PP_InlineEditPlain)+1; $style; $font; $size; 0; 0; $scale; 2; 0)
		End if 
		
		PP_InlineEditPlain:=$value
		GOTO OBJECT:C206(PP_InlineEditPlain)
		
	: (Form event code:C388=On Deactivate:K2:10)
		CANCEL:C270
		
End case 