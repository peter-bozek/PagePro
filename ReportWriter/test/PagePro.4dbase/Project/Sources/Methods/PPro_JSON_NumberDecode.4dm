//%attributes = {"invisible":true}
C_REAL:C285($0)
C_TEXT:C284($1)

If (False:C215)
	C_REAL:C285(PPro_JSON_NumberDecode; $0)
	C_TEXT:C284(PPro_JSON_NumberDecode; $1)
End if 

$0:=Num:C11($1; ".")
