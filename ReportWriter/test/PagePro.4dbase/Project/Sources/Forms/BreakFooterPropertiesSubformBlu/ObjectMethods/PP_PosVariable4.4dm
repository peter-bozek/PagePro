
If (Length:C16(PP_Source)>0)
	If (PP_Source[[1]]#"[")
		
		C_LONGINT:C283($pos)
		$pos:=Position:C15("."; PP_Source)
		If ($pos>0)
			//possible name in virtual structure?
			C_TEXT:C284($prop; $text; $parentText)
			C_LONGINT:C283($item; $ref)
			
			C_LONGINT:C283($idx)
			For ($idx; 1; Count list items:C380(PP_SourcePopup))
				SELECT LIST ITEMS BY POSITION:C381(PP_SourcePopup; $idx)
				GET LIST ITEM PARAMETER:C985(PP_SourcePopup; *; "varName"; $prop)
				
				If ($prop#"")
					GET LIST ITEM:C378(PP_SourcePopup; *; $ref; $text)
					$item:=List item parent:C633(PP_SourcePopup; $ref)
					SELECT LIST ITEMS BY REFERENCE:C630(PP_SourcePopup; $item)
					GET LIST ITEM:C378(PP_SourcePopup; *; $ref; $parentText)
					
					If (PP_Source=($parentText+"."+$text))
						PP_Source:=$prop
					End if 
				End if 
			End for 
		End if 
	End if 
End if 
