//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2; $0)

If (False:C215)
	C_LONGINT:C283(PPro_ExecFindBackInArray; $1; $2; $0)
End if 

C_LONGINT:C283($from; $value)

$value:=$1
$from:=$2
$0:=-1

C_LONGINT:C283($i)
For ($i; $from; 1; -1)
	If (IN4_LineIndent{$i}=$value)
		$0:=$i
		$i:=1
	End if 
End for 