Case of 
	: (Form event code:C388=On Drop:K2:12)
		C_LONGINT:C283($pos)
		$pos:=Drop position:C608
		C_BLOB:C604($o)
		GET PASTEBOARD DATA:C401("com.4d.private.text.utf8"; $o)
		C_TEXT:C284($t)
		If (BLOB size:C605($o)>0)
			$t:=BLOB to text:C555($o; UTF8 text without length:K22:17)
		Else 
			$t:=Get text from pasteboard:C524
		End if 
		If ($t#"")
			If ($pos<0)
				$pos:=Length:C16(PP_InlineEdit)+1
			Else 
				$pos:=$pos+1
			End if 
			ST SET PLAIN TEXT:C1136(PP_InlineEdit; $t; $pos)
		End if 
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Losing Focus:K2:8) | (Form event code:C388=On Unload:K2:2))
		C_TEXT:C284($text; $value)
		C_LONGINT:C283($err; $color)
		C_REAL:C285($scale; $size)
		C_TEXT:C284($font)
		C_LONGINT:C283($style; $prop; $styleID; $color)
		C_LONGINT:C283($platform)
		_O_PLATFORM PROPERTIES:C365($platform)
		
		PP_InlineEdit:=Get edited text:C655
		
		//$value:= OBJECT Get styled text(Self->)
		$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$text)
		PPro_TextGetProperties(PP_ExtArea; PP_CurrentObject; ->$font; ->$size; ->$style; ->$scale; ->$color)
		If ($platform=Windows:K25:3)
			$scale:=$scale*0.75
		End if 
		$err:=RW_RemoveStyle($value; PP_InlineEdit; 1; Length:C16(PP_InlineEdit)+1; $style; $font; $size; $color; ($color & 0x00FFFFFF); 1/$scale; 0)
		
		$err:=RW_Area_SaveUndo(PP_ExtArea; 10)
		Case of 
			: ($text="Text")
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$value)
			: ($text="Var")
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				$value:=PPro_ResolveAlias($value)
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
			: ($text="Field")
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				$value:=PPro_ResolveAlias($value)
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
				
			: ($text="Header")
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$value)
			: ($text="Column")
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$value)
				$value:=PPro_ResolveAlias($value)
				$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$value)
		End case 
		$err:=RW_Area_SaveUndo(PP_ExtArea; 0)
		//$err:=RW_GetProperties (PP_ExtArea;0x0000;PP_PropID;PP_PropValue;PP_PropName)
		
	: ((Form event code:C388=On Load:K2:1) | (Form event code:C388=On Selection Change:K2:29))
		
		C_LONGINT:C283($End_L)
		C_LONGINT:C283($Found_L)
		C_LONGINT:C283($Start_L)
		C_TEXT:C284($Text_T)
		C_LONGINT:C283($Value_L)
		
		If (Form event code:C388=On Load:K2:1)
			$Start_L:=1
			$End_L:=1
		Else 
			GET HIGHLIGHT:C209(PP_InlineEdit; $Start_L; $End_L)
		End if 
		
		If ($End_L=$Start_L)
			If ($End_L<Length:C16(PP_InlineEdit))
				$End_L:=$Start_L+1
			Else 
				If ($Start_L>1)
					$Start_L:=$Start_L-1
				End if 
			End if 
		End if 
		
		// Font
		ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute font name:K65:5; $Text_T)
		$Found_L:=Find in array:C230(PP_Fonts; $Text_T)
		If ($Found_L>0)
			PP_Fonts:=$Found_L
		Else 
			PP_Fonts:=0
		End if 
		
		// Size
		ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute text size:K65:6; $Value_L)
		$Found_L:=Find in array:C230(PP_FontSizePopup; String:C10($Value_L))
		If ($Found_L>0)
			PP_FontSizePopup:=$Found_L
		Else 
			PP_FontSizePopup:=0
		End if 
		
		// Bold
		ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute bold style:K65:1; $Value_L)
		Case of 
			: ($Value_L=0)
				PP_StyleB:=0
			: ($Value_L=1)
				PP_StyleB:=1
			Else 
				PP_StyleB:=0
		End case 
		
		// Underline
		ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute italic style:K65:2; $Value_L)
		Case of 
			: ($Value_L=0)
				PP_StyleI:=0
			: ($Value_L=1)
				PP_StyleI:=1
			Else 
				PP_StyleI:=0
		End case 
		
		// Underline
		ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute underline style:K65:4; $Value_L)
		Case of 
			: ($Value_L=0)
				PP_StyleU:=0
			: ($Value_L=1)
				PP_StyleU:=1
			Else 
				PP_StyleU:=0
		End case 
		
		
End case 