//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($0)
C_LONGINT:C283($i; $j; $l)

$j:=4
If (Length:C16($1)<4)
	$j:=Length:C16($1)
End if 

$l:=0
For ($i; 1; $j)
	$l:=($l << 0x0008) | (Character code:C91($1[[$i]]) & 0x00FF)
End for 
$0:=$l