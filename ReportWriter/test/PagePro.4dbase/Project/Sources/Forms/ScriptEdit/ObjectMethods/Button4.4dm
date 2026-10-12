C_TIME:C306($ref)

$ref:=Create document:C266(""; "text")
If (OK=1)
	CLOSE DOCUMENT:C267($ref)
	C_BLOB:C604($o)
	TEXT TO BLOB:C554(PP_Script; $o; UTF8 text without length:K22:17)
	BLOB TO DOCUMENT:C526(Document; $o)
End if 