//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_TEXT:C284($2; $3)

If (False:C215)
	C_LONGINT:C283(PPro_ObjectProperty; $1)
	C_TEXT:C284(PPro_ObjectProperty; $2; $3)
End if 

C_LONGINT:C283($area; $err; $value; $i)
C_TEXT:C284($command; $prop; $selector; $sValue)
C_BOOLEAN:C305($select; $bool; $isInteger)

ARRAY LONGINT:C221($alObjects; 0x0000)
ARRAY LONGINT:C221($alObjects2; 0x0000)

$area:=$1
$command:=$2
$selector:=$3

If ($selector="sel")
	$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
Else 
	$err:=RW_GetObjects($area; $area; "objs"; $alObjects)
End if 

$isInteger:=True:C214  //most properties are integer
Case of 
	: ($command="lock")
		$prop:=PProp_Locked:K7906:19
		$value:=2
		$select:=False:C215
		
	: ($command="unlock")
		$prop:=PProp_Locked:K7906:19
		$value:=0
		$select:=True:C214
		
	: ($command="hide")
		$prop:="visi"
		$value:=0
		$select:=False:C215
		
	: ($command="show")
		$prop:="visi"
		$value:=1
		$select:=True:C214
		
	: ($command="dontprint")
		$prop:="draw"
		$sValue:="no"
		$select:=False:C215
		$isInteger:=False:C215
		
	: ($command="print")
		$prop:="draw"
		$sValue:="yes"
		$select:=True:C214
		$isInteger:=False:C215
		
End case 

$err:=RW_Area_SaveUndo($area; 1)  //start undo block

If ($select)
	$err:=RW_GetObjects($area; $area; "selc"; $alObjects2)
	$bool:=False:C215
	For ($i; 0x0001; Size of array:C274($alObjects2))
		$err:=RW_SetProperty($area; $alObjects2{$i}; "selc"; ->$bool)
	End for 
End if 

$bool:=True:C214
C_LONGINT:C283($oldValue)
C_TEXT:C284($sOldvalue)
For ($i; 0x0001; Size of array:C274($alObjects))
	If ($isInteger)
		
		$err:=RW_GetProperty($area; $alObjects{$i}; $prop; ->$oldValue)
		If (($oldValue#$value) & ($err=0))
			$err:=RW_SetProperty($area; $alObjects{$i}; $prop; ->$value)
			If ($select)
				$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$bool)
			End if 
		End if 
		
	Else 
		
		$err:=RW_GetProperty($area; $alObjects{$i}; $prop; ->$sOldValue)
		If (($sOldValue#$sValue) & ($err=0))
			$err:=RW_GetProperty($area; $alObjects{$i}; $prop; ->$sValue)
			If ($select)
				$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$bool)
			End if 
		End if 
		
	End if 
End for 

$err:=RW_Area_SaveUndo($area; 0)  //end  undo block
