//%attributes = {"invisible":true,"shared":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3)

If (False:C215)
	C_TEXT:C284(PPro_Util_FindPlainText; $1)
	C_POINTER:C301(PPro_Util_FindPlainText; $2; $3)
End if 

C_TEXT:C284($text)
$text:=$1

C_LONGINT:C283($start; $end)
$start:=$2->
$end:=$3->

C_LONGINT:C283($i)
If ($text[[$start]]="<")
	$start:=$start+1
End if 

For ($i; $start; $end)
	If ($text[[$i]]=">")
		$start:=$i+1
	End if 
	If ($text[[$i]]="<")
		C_LONGINT:C283($temp)
		$temp:=$i-1
		$i:=$end
		$end:=$temp
	End if 
End for 

$2->:=$start
$3->:=$end


