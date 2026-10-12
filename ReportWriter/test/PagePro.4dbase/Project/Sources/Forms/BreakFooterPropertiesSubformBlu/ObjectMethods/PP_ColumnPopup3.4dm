C_TEXT:C284($prop; $menu; $text; $parentText)
C_LONGINT:C283($item; $ref; $list)

$list:=Self:C308->

GET LIST ITEM PARAMETER:C985(*; "PP_ColumnPopup3"; *; "varName"; $prop)
GET LIST ITEM:C378($list; *; $ref; $text)
$item:=List item parent:C633($list; $ref)
SELECT LIST ITEMS BY REFERENCE:C630($list; $item)
GET LIST ITEM:C378($list; *; $ref; $parentText)

If ($prop#"")
	PP_Source:=$prop
	PP_Alias:=$parentText+"."+$text
End if 