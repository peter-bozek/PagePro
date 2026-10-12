//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_TextCommandDispatch; $1)
End if 

C_LONGINT:C283($err; $size; $start; $end)
C_BOOLEAN:C305($b)
C_TEXT:C284($type; $text; $font)
C_REAL:C285($scale)

If (PP_CurrentObject>0)
	$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$type)
	If (Focus object:C278=(->PP_InlineEdit))
		//inside editing session
		//set property for character run
		If ($type="Text")
			GET HIGHLIGHT:C209(PP_InlineEdit; $start; $end)
			Case of 
				: ($1="stPlain")
					$err:=RW_RemoveStyle($text; PP_InlineEdit; $start; $end; Attribute bold style:K65:1+Attribute italic style:K65:2+Attribute underline style:K65:4; ""; 0; 0; 0; 0; 0)
					PP_InlineEdit:=$text
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute bold style;0)
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute italic style;0)
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute underline style;0)
					
				: ($1="stBold")
					$err:=RW_AddStyle($text; PP_InlineEdit; $start; $end; Attribute bold style:K65:1; ""; 0; 0; 0; 0; 0; 1)
					PP_InlineEdit:=$text
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute bold style;1)
					
				: ($1="stItalic")
					$err:=RW_AddStyle($text; PP_InlineEdit; $start; $end; Attribute italic style:K65:2; ""; 0; 0; 0; 0; 0; 1)
					PP_InlineEdit:=$text
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute italic style;1)
					
				: ($1="stUnderline")
					$err:=RW_AddStyle($text; PP_InlineEdit; $start; $end; Attribute underline style:K65:4; ""; 0; 0; 0; 0; 0; 1)
					PP_InlineEdit:=$text
					//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute underline style;1)
					
				: ($1="stLower")
					$text:=ST Get text:C1116(PP_InlineEdit; $start; $end)
					$text:=Lowercase:C14($text)
					ST SET TEXT:C1115(PP_InlineEdit; $text; $start; $end)
					
				: ($1="stUpper")
					$text:=ST Get text:C1116(PP_InlineEdit; $start; $end)
					$text:=Uppercase:C13($text)
					ST SET TEXT:C1115(PP_InlineEdit; $text; $start; $end)
					
				: ($1="stSize@")
					$size:=Num:C11(Substring:C12($1; 7))
					If ($size>0)
						$scale:=PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; "scal")
						$err:=RW_AddStyle($text; PP_InlineEdit; $start; $end; 0; ""; Round:C94($size*$scale; 2); 0; 0; 0; 0; 1)
						PP_InlineEdit:=$text
						//OBJECT SET STYLED TEXT ATTRIBUTES(PP_InlineEdit;$start;$end;Attribute text size;round($size*$scale;2))
					End if 
					
				: ($1="stBigger")
					If ($type="Text")
						ST GET ATTRIBUTES:C1094(PP_InlineEdit; $start; $end; Attribute text size:K65:6; $size)
						If ($size>0)
							$size:=Round:C94($size*1.1; 0)
							ST SET ATTRIBUTES:C1093(PP_InlineEdit; $start; $end; Attribute text size:K65:6; $size)
						End if 
					End if 
					
				: ($1="stSmaller")
					If ($type="Text")
						ST GET ATTRIBUTES:C1094(PP_InlineEdit; $start; $end; Attribute text size:K65:6; $size)
						If ($size>0)
							$size:=Round:C94($size/1.1; 0)
							ST SET ATTRIBUTES:C1093(PP_InlineEdit; $start; $end; Attribute text size:K65:6; $size)
						End if 
					End if 
					
				: ($1="menuI=@")
					$font:=Substring:C12($1; 7)
					ST SET ATTRIBUTES:C1093(PP_InlineEdit; $start; $end; Attribute font name:K65:5; $font)
					
				: ($1="txColor")
					C_LONGINT:C283($color)
					If (RW_ColorPicker($color)=1)
						$err:=RW_AddStyle($text; PP_InlineEdit; $start; $end; 0; ""; 0; $color; 1; 0; 0; 0)
						PP_InlineEdit:=$text
					End if 
					
			End case 
			HIGHLIGHT TEXT:C210(PP_InlineEdit; $start; $end)
			
		End if 
		
	Else 
		
		Case of 
			: ($1="stPlain")
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styB"; 0)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styI"; 0)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styU"; 0)
				
			: ($1="stBold")
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styB"; 1)
				
			: ($1="stItalic")
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styI"; 1)
				
			: ($1="stUnderline")
				$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "styU"; 1)
				
			: ($1="stLower")
				If ($type="Text")
					$text:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; "data")
					$text:=Lowercase:C14($text)
					$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "data"; $text)
				End if 
				
			: ($1="stUpper")
				If ($type="Text")
					$text:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; "data")
					$text:=Uppercase:C13($text)
					$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "data"; $text)
				End if 
				
			: ($1="stSize@")
				$size:=Num:C11(Substring:C12($1; 7))
				If ($size>0)
					$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "size"; $size)
				End if 
				
			: ($1="stBigger")
				If ($type="Text")
					$size:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; "size")
					$size:=Round:C94($size*1.1; 0)
					$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "size"; $size)
				End if 
				
			: ($1="haLeft")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "algn"; "left")
			: ($1="haRight")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "algn"; "right")
			: ($1="haCenter")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "algn"; "center")
			: ($1="haJustify")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "algn"; "justify")
				
			: ($1="vaTop")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "valg"; "top")
			: ($1="vaCenter")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "valg"; "center")
			: ($1="vaBottom")
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "valg"; "bottom")
				
			: ($1="txWrap")
				$err:=PPro_ToggleProperty(PP_ExtArea; PP_CurrentObject; "wrap")
				
			: ($1="txColor")
				C_LONGINT:C283($color)
				$color:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; "tclr")
				If (RW_ColorPicker($color)=1)
					$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; "tclr"; $color)
				End if 
				
			: ($1="txSpace+")
				C_REAL:C285($spacing)
				$spacing:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; "lisp")
				$spacing:=Round:C94($spacing+0.2; 2)
				PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; "lisp"; $spacing)
				
			: ($1="txSpace-")
				C_REAL:C285($spacing; $realValue)
				$spacing:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; "lisp")
				$spacing:=Round:C94($realValue-0.2; 2)
				PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; "lisp"; $spacing)
				
			: ($1="menuI=@")
				$font:=Substring:C12($1; 7)
				$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; "fnam"; $font)
				
		End case 
	End if 
End if 
