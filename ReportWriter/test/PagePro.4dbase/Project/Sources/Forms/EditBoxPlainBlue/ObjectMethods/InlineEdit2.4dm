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
				$pos:=Length:C16(PP_InlineEditPlain)+1
			Else 
				$pos:=$pos+1
			End if 
			ST SET PLAIN TEXT:C1136(PP_InlineEditPlain; $t; $pos)
		End if 
		
		
	: (Form event code:C388=On Load:K2:1)
		
		C_LONGINT:C283($Found_L)
		C_TEXT:C284($Text_T)
		C_LONGINT:C283($Value_L)
		
		// Font
		$Text_T:=OBJECT Get font:C1069(PP_InlineEditPlain)
		$Found_L:=Find in array:C230(PP_Fonts; $Text_T)
		If ($Found_L>0)
			PP_Fonts:=$Found_L
		Else 
			PP_Fonts:=0
		End if 
		
		// Size
		$Value_L:=OBJECT Get font size:C1070(PP_InlineEditPlain)
		$Found_L:=Find in array:C230(PP_FontSizePopup; String:C10($Value_L))
		If ($Found_L>0)
			PP_FontSizePopup:=$Found_L
		Else 
			PP_FontSizePopup:=0
		End if 
		
		// Bold, Italic, Underline
		$Value_L:=OBJECT Get font style:C1071(PP_InlineEditPlain)
		Case of 
			: ($Value_L=0)
				PP_StyleB:=0
				PP_StyleI:=0
				PP_StyleU:=0
			: ($Value_L=Bold:K14:2)
				PP_StyleB:=1
				PP_StyleI:=0
				PP_StyleU:=0
			: ($Value_L=Italic:K14:3)
				PP_StyleB:=0
				PP_StyleI:=1
				PP_StyleU:=0
			: ($Value_L=Underline:K14:4)
				PP_StyleB:=0
				PP_StyleI:=0
				PP_StyleU:=1
			: ($Value_L=Bold:K14:2+Italic:K14:3)
				PP_StyleB:=1
				PP_StyleI:=1
				PP_StyleU:=0
			: ($Value_L=Bold:K14:2+Underline:K14:4)
				PP_StyleB:=1
				PP_StyleI:=0
				PP_StyleU:=1
			: ($Value_L=Italic:K14:3+Underline:K14:4)
				PP_StyleB:=0
				PP_StyleI:=1
				PP_StyleU:=1
			: ($Value_L=Bold:K14:2+Italic:K14:3+Underline:K14:4)
				PP_StyleB:=1
				PP_StyleI:=1
				PP_StyleU:=1
		End case 
		
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Losing Focus:K2:8) | (Form event code:C388=On Unload:K2:2))
		
		C_TEXT:C284($Text_T; $Value_T)
		C_LONGINT:C283($Error_L)
		
		PP_InlineEditPlain:=Get edited text:C655
		$Value_T:=PP_InlineEditPlain
		
		$Error_L:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$Text_T)
		$Error_L:=RW_Area_SaveUndo(PP_ExtArea; 10)
		Case of 
			: ($Text_T="Text")
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$Value_T)
			: ($Text_T="Var")
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$Value_T)
				$Value_T:=PPro_ResolveAlias($Value_T)
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$Value_T)
			: ($Text_T="Field")
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$Value_T)
				$Value_T:=PPro_ResolveAlias($Value_T)
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$Value_T)
				
			: ($Text_T="Header")
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$Value_T)
			: ($Text_T="Column")
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "alis"; ->$Value_T)
				$Value_T:=PPro_ResolveAlias($Value_T)
				$Error_L:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "src"; ->$Value_T)
		End case 
		$Error_L:=RW_Area_SaveUndo(PP_ExtArea; 0)
		
		
End case 
