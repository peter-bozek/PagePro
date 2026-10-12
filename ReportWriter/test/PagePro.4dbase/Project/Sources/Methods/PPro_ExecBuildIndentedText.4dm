//%attributes = {"invisible":true}
C_TEXT:C284($0)
C_POINTER:C301($1; $2)

If (False:C215)
	C_TEXT:C284(PPro_ExecBuildIndentedText; $0)
	C_POINTER:C301(PPro_ExecBuildIndentedText; $1; $2)
End if 

C_LONGINT:C283($start; $end)
$start:=$1->
$end:=$2->


C_LONGINT:C283($newSel)
C_TEXT:C284($out)
$out:=""

C_LONGINT:C283($size; $newSel)
$size:=Size of array:C274(IN4_Line)

C_LONGINT:C283($j)
For ($j; 1; $size)
	IN4_Position{$j}:=Length:C16($out)
	If (IN4_LineIndent{$j}>0)
		$out:=$out+("  "*IN4_LineIndent{$j})
	End if 
	$out:=$out+IN4_Line{$j}
	If ($j<$size)
		$out:=$out+Char:C90(Carriage return:K15:38)
	End if 
	If ($start=$j)
		If ((IN4_LineIndent{$j}*2)>=$end)
			$end:=(IN4_LineIndent{$j}*2)+1
		End if 
		$newSel:=IN4_Position{$j}+$end
		If ($newSel>Length:C16($out))
			$newSel:=Length:C16($out)
		End if 
	End if 
End for 

$0:=$out
$1->:=$newSel
$2->:=$newSel