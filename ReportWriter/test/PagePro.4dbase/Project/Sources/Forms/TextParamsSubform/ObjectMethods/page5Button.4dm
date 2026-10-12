C_REAL:C285($r)

$r:=0

If (Size of array:C274(PP_TabPosition)>0)
	C_LONGINT:C283($i)
	For ($i; 1; Size of array:C274(PP_TabPosition))
		$r:=$r+PP_TabPosition{$i}
	End for 
	$r:=Round:C94((2*$r+1)/2; 1)
Else 
	$r:=1/2
End if 

APPEND TO ARRAY:C911(PP_TabAlign; pp_TabAlignPopup{1})
APPEND TO ARRAY:C911(PP_TabPosition; $r)

LISTBOX SELECT ROW:C912(PP_TabListBox; Size of array:C274(PP_TabAlign))