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
		C_TEXT:C284($font)
		C_REAL:C285($scale)
		C_LONGINT:C283($platform)
		_O_PLATFORM PROPERTIES:C365($platform)
		
		PPro_TextGetProperties(PP_ExtArea; PP_CurrentObject; ->$font; ->$size; ->$style; ->$scale; ->$color)
		//If ($platform=Windows)
		//$scale:=$scale*3/4
		//End if 
		$err:=RW_AddStyle($text; $value; 1; Length:C16(PP_InlineEdit)+1; $style; $font; $size; 0; 0; $scale; 2; 0)
		
		PP_InlineEdit:=$text
		
		OBJECT SET FONT:C164(PP_InlineEdit; $font)
		GOTO OBJECT:C206(PP_InlineEdit)
		
		// show object boundaries
		C_LONGINT:C283($left; $top; $right; $bottom)
		OBJECT GET COORDINATES:C663(*; "InlineEdit"; $left; $top; $right; $bottom)
		If (PP_EditWidth<130)
			OBJECT MOVE:C664(*; "InlineEdit"; 0; 0; PP_EditWidth-$right+$left; 0)
		End if 
		
	: (Form event code:C388=On Deactivate:K2:10)
		ACCEPT:C269
		
End case 