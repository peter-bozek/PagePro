C_LONGINT:C283($from; $to)
GET HIGHLIGHT:C209(PP_Script; $from; $to)
If ($to>$from)
	C_TEXT:C284($value)
	$value:=Substring:C12(PP_Script; $from; $to-$from)
	PPro_SearchBuild($value)
	PP_Search:=$value
End if 