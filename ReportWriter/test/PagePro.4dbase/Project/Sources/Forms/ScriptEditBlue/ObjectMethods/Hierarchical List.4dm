C_TEXT:C284($prop; $menu; $text; $parentText)
C_LONGINT:C283($item; $ref; $list)

$list:=PP_TableList

GET LIST ITEM PARAMETER:C985($list; *; "varName"; $prop)

If ($prop="")  //pB 2010-9
	GET LIST ITEM:C378($list; *; $ref; $prop)
	$prop:="["+$prop+"]"
End if 

If ($prop#"")
	C_LONGINT:C283($start; $end)
	GET HIGHLIGHT:C209(PP_Script; $start; $end)
	PP_Script:=Substring:C12(PP_Script; 1; $start-1)+$prop+Substring:C12(PP_Script; $end)
	$end:=$start+Length:C16($prop)
	HIGHLIGHT TEXT:C210(PP_Script; $end; $end)
End if 