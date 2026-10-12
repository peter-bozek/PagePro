C_PICTURE:C286($pict)
C_BLOB:C604($blob)
C_LONGINT:C283($err)

READ PICTURE FILE:C678(""; $pict)
If (OK=1)
	VARIABLE TO BLOB:C532($pict; $blob)
	$err:=RW_SetProperty(PP_ExtArea; PP_CurrentObject; "data"; ->$blob)
End if 