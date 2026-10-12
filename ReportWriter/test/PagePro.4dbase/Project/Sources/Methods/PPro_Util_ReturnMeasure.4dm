//%attributes = {"invisible":true}
C_POINTER:C301($1)
C_REAL:C285($0)
C_LONGINT:C283($2)

If (False:C215)
	C_POINTER:C301(PPro_Util_ReturnMeasure; $1)
	C_REAL:C285(PPro_Util_ReturnMeasure; $0)
	C_LONGINT:C283(PPro_Util_ReturnMeasure; $2)
End if 

C_REAL:C285($value; $conv)
C_LONGINT:C283($i; $start; $stop; $unit; $round)
C_TEXT:C284($ext)

$value:=$1->

If (Count parameters:C259>1)
	$unit:=$2
Else 
	$unit:=1
End if 

$conv:=1
Case of 
	: ($unit<2)
		$conv:=1
		$ext:=" pt"
		$round:=2
		
	: ($unit=2)
		$conv:=72/25.4
		$ext:=" mm"
		$round:=2
		
	: ($unit=3)
		$conv:=72
		$ext:=" in"
		$round:=4
		
End case 

$0:=Round:C94($value*$conv; $round)
