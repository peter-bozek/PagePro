C_LONGINT:C283($col; $row)
LISTBOX GET CELL POSITION:C971(PP_TabListBox; $col; $row)

If (($col>0) & ($col<=Size of array:C274(PP_TabAlign)))
	DELETE FROM ARRAY:C228(PP_TabAlign; $col; 1)
	DELETE FROM ARRAY:C228(PP_TabPosition; $col; 1)
End if 
