//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($0)

If (False:C215)
	C_TEXT:C284(PPro_ResolveTableName; $1)
	C_LONGINT:C283(PPro_ResolveTableName; $0)
End if 

C_LONGINT:C283($table)
C_TEXT:C284($s)

$0:=0

$s:=$1
$s:=Replace string:C233($s; "["; "")
$s:=Replace string:C233($s; "]"; "")
For ($table; 1; Get last table number:C254)
	If (Is table number valid:C999($table))
		If ($s=Table name:C256($table))
			$0:=$table
			$table:=Get last table number:C254
		End if 
	End if 
End for 
