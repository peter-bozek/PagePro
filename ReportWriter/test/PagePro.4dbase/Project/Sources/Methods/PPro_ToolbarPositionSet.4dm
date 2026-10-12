//%attributes = {"invisible":true}
C_REAL:C285($2)
C_POINTER:C301($1)
C_LONGINT:C283($3)

If (False:C215)
	C_REAL:C285(PPro_ToolbarPositionSet; $2)  //value
	C_POINTER:C301(PPro_ToolbarPositionSet; $1)  //text
	C_LONGINT:C283(PPro_ToolbarPositionSet; $3)  //selector of unit
End if 

C_REAL:C285($value; $conv; $disp)
C_LONGINT:C283($stop; $i; $round)
C_LONGINT:C283($unit)
C_TEXT:C284($ext)

$disp:=$1->
$value:=$2
If (Count parameters:C259>2)
	$unit:=$3
Else 
	$unit:=1
End if 

$conv:=1
$round:=3
Case of 
	: ($unit<2)  // default or points
		$conv:=1
		$ext:=" pt"
		$round:=2
		
	: ($unit=2)  //mm
		$conv:=72/25.4
		$ext:=" mm"
		$round:=2
		
	: ($unit=3)  //inch
		$conv:=72
		$ext:=" in"
		$round:=4
		
End case 

$1->:=Round:C94($value/$conv; $round)


