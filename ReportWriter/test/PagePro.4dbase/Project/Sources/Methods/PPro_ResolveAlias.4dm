//%attributes = {"invisible":true}
C_TEXT:C284($1; $0)

If (False:C215)
	C_TEXT:C284(PPro_ResolveAlias; $0; $1)
End if 

$0:=$1

C_TEXT:C284($alias)
$alias:=$1
If (Length:C16($alias)>0)
	If ($alias[[1]]#"[")
		
		C_LONGINT:C283($pos)
		$pos:=Position:C15("."; $alias)
		If ($pos>0)
			//possible name in virtual structure?
			C_TEXT:C284($prop; $text; $parentText)
			C_LONGINT:C283($item; $ref)
			
			C_LONGINT:C283($idx)
			For ($idx; 1; Count list items:C380(PP_SourcePopup; *))
				SELECT LIST ITEMS BY POSITION:C381(PP_SourcePopup; $idx)
				GET LIST ITEM PARAMETER:C985(PP_SourcePopup; *; "varName"; $prop)
				
				If ($prop#"")
					GET LIST ITEM:C378(PP_SourcePopup; *; $ref; $text)
					$item:=List item parent:C633(PP_SourcePopup; $ref)
					SELECT LIST ITEMS BY REFERENCE:C630(PP_SourcePopup; $item)
					GET LIST ITEM:C378(PP_SourcePopup; *; $ref; $parentText)
					
					If ($alias=($parentText+"."+$text))
						$0:=$prop
					End if 
				End if 
			End for 
		End if 
	End if 
End if 
