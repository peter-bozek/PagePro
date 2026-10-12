//%attributes = {}
C_POINTER:C301($1)

If (False:C215)
	C_POINTER:C301(PPro_CheckDecimal; $1)
End if 

C_LONGINT:C283($i)
C_TEXT:C284($dec)
GET SYSTEM FORMAT:C994(Decimal separator:K60:1; $dec)
For ($i; 1; Size of array:C274($1->))
	$1->{$i}:=Replace string:C233($1->{$i}; "."; $dec)
End for 
