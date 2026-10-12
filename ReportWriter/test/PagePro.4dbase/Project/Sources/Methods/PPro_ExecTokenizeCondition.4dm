//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_BLOB:C604($0)

If (False:C215)
	C_TEXT:C284(PPro_ExecTokenizeCondition; $1)
	C_BLOB:C604(PPro_ExecTokenizeCondition; $0)
End if 

C_TEXT:C284($line)
$line:=$1

C_LONGINT:C283($first; $last)
$first:=1
$last:=Length:C16($line)

C_LONGINT:C283($i)
For ($i; 1; $last)
	If ($line[[$i]]="(")
		$first:=$i+1
		$i:=Length:C16($line)
	End if 
End for 

For ($i; $last; $first; -1)
	If ($line[[$i]]=")")
		$last:=$i-1
		$i:=$first
	End if 
End for 

$line:=Substring:C12($line; $first; $last-$first+1)

C_BLOB:C604($o)
C_LONGINT:C283($err)
$err:=RW_Tokenize($line; $o)
$0:=$o