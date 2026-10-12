//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3)

If (False:C215)
	C_TEXT:C284(PPro_ExecSplitToLines; $1)
	C_POINTER:C301(PPro_ExecSplitToLines; $2; $3)
End if 

C_TEXT:C284($method)
$method:=$1
C_LONGINT:C283($start; $end; $len)
$start:=$2->
$end:=$3->
$len:=$end-$start

C_TEXT:C284($line)
C_LONGINT:C283($lineNo)
$lineNo:=0

C_LONGINT:C283($lineSel; $charSel)
$lineSel:=0
$charSel:=0

C_BOOLEAN:C305($lastCR)
PPro_ExecDataHandler("Init")

While (Length:C16($method)>0)
	$lineNo:=$lineNo+1
	//split to lines
	C_LONGINT:C283($j)
	$j:=Position:C15(Char:C90(Carriage return:K15:38); $method)
	If ($j>0)
		$lastCR:=True:C214
		
		Case of 
			: ($start=0)
			: ($start<=$j)
				$lineSel:=$lineNo
				$charSel:=$start
				$start:=0
			: ($start=($j+1))  //after new line
				$lineSel:=$lineNo+1
				$charSel:=1
				$start:=0
			Else 
				$start:=$start-$j
		End case 
		$line:=Substring:C12($method; 1; $j-1)
		$method:=Substring:C12($method; $j+1)
	Else 
		$lastCR:=False:C215
		If ($start>0)
			$lineSel:=$lineNo
			$charSel:=$start
		End if 
		
		$line:=$method
		$method:=""
	End if 
	
	PPro_ExecInsertLine($line)
	
End while 

If ($lastCR)
	PPro_ExecInsertLine("")
End if 

$2->:=$lineSel
$3->:=$charSel
