//%attributes = {"invisible":true}
C_TEXT:C284($0; $1)

If (False:C215)
	C_TEXT:C284(PPro_JSON_StringDecode; $0; $1)
End if 

C_TEXT:C284($t)

$t:=$1
If (Length:C16($t)>0)
	If (($t[[Length:C16($t)]]="\"") & ($t[[1]]="\""))
		$t:=Substring:C12($t; 2; Length:C16($t)-1)
	End if 
	
	$t:=Replace string:C233($t; "\\\\"; "\\")
	$t:=Replace string:C233($t; "\\\""; "\"")
	$t:=Replace string:C233($t; "\\/"; "/")
	$t:=Replace string:C233($t; "\\r"; Char:C90(Carriage return:K15:38))
End if 

$0:=$t