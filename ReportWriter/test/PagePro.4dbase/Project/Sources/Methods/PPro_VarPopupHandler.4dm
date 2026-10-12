//%attributes = {"invisible":true}
C_TEXT:C284($prop; $menu; $text; $parentText)
C_LONGINT:C283($item; $ref)

GET LIST ITEM PARAMETER:C985(*; "PP_SourcePopup"; *; "varName"; $prop)
GET LIST ITEM:C378(*; "PP_SourcePopup"; *; $ref; $text)
$item:=List item parent:C633(*; "PP_SourcePopup"; $ref)
SELECT LIST ITEMS BY REFERENCE:C630(PP_SourcePopup; $item)
GET LIST ITEM:C378(*; "PP_SourcePopup"; *; $ref; $parentText)

If ($prop#"")
	PP_Source:=$prop
	PP_Alias:=$parentText+"."+$text
End if 

C_LONGINT:C283($err)
$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Alias:K7906:81; PP_Alias)
$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Source:K7906:80; PP_Source)
