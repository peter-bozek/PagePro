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
		
	: (Form event code:C388=On Validate:K2:3)  //| (Form event=On Losing Focus)
		C_TEXT:C284($text; $value)
		C_LONGINT:C283($err; $color)
		C_REAL:C285($scale; $size)
		C_TEXT:C284($font)
		C_LONGINT:C283($style; $prop; $styleID; $color)
		C_LONGINT:C283($platform)
		_O_PLATFORM PROPERTIES:C365($platform)
		
		If (OBJECT Get pointer:C1124(Object with focus:K67:3)=(->PP_InlineEditPlain))
			PP_InlineEditPlain:=Get edited text:C655
		End if 
		
		$value:=PP_InlineEditPlain
		//$value:= OBJECT Get styled text(Self->)
		$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$text)
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
		
		If (Form event code:C388=On Losing Focus:K2:8)
			// CANCEL
		End if 
		
End case 