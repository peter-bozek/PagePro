C_TIME:C306($ref)

$ref:=Open document:C264(""; "text")
If (OK=1)
	CLOSE DOCUMENT:C267($ref)
	C_BLOB:C604($o)
	DOCUMENT TO BLOB:C525(Document; $o)
	PP_Script:=BLOB to text:C555($o; UTF8 text without length:K22:17)
End if 