//%attributes = {"invisible":true}
// RWT_Log (text): appends a line to log.txt in the output folder (written at once, survives a crash)
#DECLARE($text : Text)

var $doc : Time

$doc:=Append document(vRWT_Out+"log.txt")
If (OK=1)
	SEND PACKET($doc; $text+Char(Line feed))
	CLOSE DOCUMENT($doc)
End if
