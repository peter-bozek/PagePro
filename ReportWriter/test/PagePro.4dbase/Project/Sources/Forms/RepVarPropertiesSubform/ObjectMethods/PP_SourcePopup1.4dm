C_TEXT:C284($prop; $menu; $text; $parentText)
C_LONGINT:C283($item; $ref)

GET LIST ITEM PARAMETER:C985(*; "PP_SourcePopup1"; *; "varName"; $prop)
GET LIST ITEM:C378(*; "PP_SourcePopup1"; *; $ref; $text)
$item:=List item parent:C633(*; "PP_SourcePopup1"; $ref)
SELECT LIST ITEMS BY REFERENCE:C630(PP_SourcePopup; $item)
GET LIST ITEM:C378(*; "PP_SourcePopup1"; *; $ref; $parentText)

If ($prop#"")
	PP_Source:=$prop
	PP_Alias:=$parentText+"."+$text
End if 