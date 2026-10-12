//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3; $4)  //init, loop and exit conddition

If (False:C215)
	C_TEXT:C284(PPro_ExecTokenizeForCondition; $1)
	C_POINTER:C301(PPro_ExecTokenizeForCondition; $2; $3; $4)
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
C_TEXT:C284($part1; $part2; $part3; $part4; $s)
C_LONGINT:C283($idx)
$idx:=1
While ($line#"")
	C_LONGINT:C283($pos)
	$pos:=Position:C15(";"; $line)
	If ($pos>0)
		$s:=Substring:C12($line; 1; $pos-1)
		$line:=Substring:C12($line; $pos+1)
	Else 
		$s:=$line
		$line:=""
	End if 
	Case of 
		: ($idx=1)
			$part1:=$s
		: ($idx=2)
			$part2:=$s
		: ($idx=3)
			$part3:=$s
		: ($idx=4)
			$part4:=$s
	End case 
	$idx:=$idx+1
End while 

C_BLOB:C604($o)
If (($part1#"") & ($part2#"") & ($part3#""))
	C_LONGINT:C283($step)
	If ($part4#"")
		$step:=Num:C11($part4)
	Else 
		$step:=1
	End if 
	
	C_LONGINT:C283($err)
	$line:=$part1+":="+$part2
	$err:=RW_Tokenize($line; $o)
	$2->:=$o
	
	If ($step>0)
		$line:=$part1+":="+$part1+"+"+String:C10($step)
	Else 
		$line:=$part1+":="+$part1+"-"+String:C10(Abs:C99($step))
	End if 
	$err:=RW_Tokenize($line; $o)
	$3->:=$o
	
	If ($step>0)
		$line:=$part1+"<="+$part3
	Else 
		$line:=$part1+">="+$part3
	End if 
	$err:=RW_Tokenize($line; $o)
	$4->:=$o
	
End if 
