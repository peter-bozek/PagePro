//%attributes = {"invisible":true}
C_TEXT:C284($0; $1)

If (False:C215)
	C_TEXT:C284(PPro_JSON_StringEncode; $0; $1)
End if 

C_TEXT:C284($t)

$t:=$1
$t:=Replace string:C233($t; "\\"; "\\\\")
$t:=Replace string:C233($t; "\""; "\\\"")
$t:=Replace string:C233($t; "/"; "\\/")
$t:=Replace string:C233($t; Char:C90(Carriage return:K15:38); "\\r")

$0:="\""+$t+"\""