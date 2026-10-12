//%attributes = {"invisible":true}
C_TEXT:C284($1; $2)
C_LONGINT:C283($0; $3)

If (False:C215)
	C_TEXT:C284(PPro_JSON_GetNextChar; $1; $2)
	C_LONGINT:C283(PPro_JSON_GetNextChar; $0; $3)
End if 

C_TEXT:C284($t)
$t:=$1
$0:=Length:C16($t)+1

C_LONGINT:C283($idx)
$idx:=$3
While ($idx<=Length:C16($t))
	If ($t[[$idx]]=$2)
		$0:=$idx
		$idx:=Length:C16($t)
	End if 
	$idx:=$idx+1
End while 