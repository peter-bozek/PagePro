//%attributes = {"invisible":true}
C_REAL:C285($1)
C_TEXT:C284($0)

If (False:C215)
	C_REAL:C285(PPro_JSON_NumberEncode; $1)
	C_TEXT:C284(PPro_JSON_NumberEncode; $0)
End if 

$0:=Replace string:C233(String:C10($1); ","; ".")
