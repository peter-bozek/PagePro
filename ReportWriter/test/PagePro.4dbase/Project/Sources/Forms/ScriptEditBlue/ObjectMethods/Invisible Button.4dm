C_LONGINT:C283($size)
$size:=Size of array:C274(PP_SearchPosition)
If ($size>0)
	C_LONGINT:C283($from; $to)
	GET HIGHLIGHT:C209(PP_Script; $from; $to)
	C_LONGINT:C283($i)
	For ($i; 1; $size)
		If (PP_SearchPosition{$i}>=$to)
			HIGHLIGHT TEXT:C210(PP_Script; PP_SearchPosition{$i}; PP_SearchPosition{$i}+PP_SearchLen{$i})
			$i:=$size
		End if 
	End for 
End if 