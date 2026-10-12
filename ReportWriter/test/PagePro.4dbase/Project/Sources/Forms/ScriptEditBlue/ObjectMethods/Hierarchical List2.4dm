C_TEXT:C284($prop; $menu; $text; $parentText)
C_LONGINT:C283($item; $ref; $list)

$list:=PP_CommandList

GET LIST ITEM:C378($list; *; $ref; $text)

If ($text#"")
	C_LONGINT:C283($start; $end)
	If (PP_ScriptEditorStartSel=-1)
		GET HIGHLIGHT:C209(PP_Script; $start; $end)
	Else 
		$start:=PP_ScriptEditorStartSel
		$end:=PP_ScriptEditorEndSel
	End if 
	PP_Script:=Substring:C12(PP_Script; 1; $start-1)+$text+Substring:C12(PP_Script; $end)
	$end:=$start+Length:C16($text)
	HIGHLIGHT TEXT:C210(PP_Script; $end; $end)
	PP_ScriptEditorStartSel:=-1
	PP_ScriptEditorEndSel:=-1
End if 