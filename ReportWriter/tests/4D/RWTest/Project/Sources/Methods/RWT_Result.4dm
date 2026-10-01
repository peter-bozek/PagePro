//%attributes = {"invisible":true}
// RWT_Result (name; value): one line "name = value" in results.txt (compared old / new)
#DECLARE($name : Text; $value : Text)

var $doc : Time

$doc:=Append document(vRWT_Out+"results.txt")
If (OK=1)
	SEND PACKET($doc; $name+" = "+$value+Char(Line feed))
	CLOSE DOCUMENT($doc)
End if
RWT_Log($name+" = "+$value)
