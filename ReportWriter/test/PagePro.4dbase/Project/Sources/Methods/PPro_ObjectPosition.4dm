//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_TEXT:C284($2)
C_REAL:C285($3)

If (False:C215)
	C_LONGINT:C283(PPro_ObjectPosition; $1)
	C_TEXT:C284(PPro_ObjectPosition; $2)
	C_REAL:C285(PPro_ObjectPosition; $3)
End if 

C_LONGINT:C283($area; $err; $i)
C_TEXT:C284($command; $prop; $oldProp; $altprop)
C_BOOLEAN:C305($bool; $bRelative)
C_REAL:C285($value; $oldValue; $move)

ARRAY LONGINT:C221($alObjects; 0x0000)

$area:=$1
$command:=$2
$value:=$3

$err:=RW_GetObjects($area; $area; "selc"; $alObjects)

$bRelative:=False:C215
Case of 
	: ($command="posX")
		$prop:="movH"
		$bRelative:=True:C214
		$oldProp:="posl"
		
	: ($command="posY")
		$prop:="movV"
		$bRelative:=True:C214
		$oldProp:="post"
		
	: ($command="posR")
		$prop:="posr"
		
	: ($command="posB")
		$prop:="posb"
		
	: ($command="width")
		$prop:="posw"
		If (PP_LockProportions=1)
			$altprop:="posh"
		End if 
		
	: ($command="height")
		$prop:="posh"
		If (PP_LockProportions=1)
			$altprop:="posw"
		End if 
		
End case 

$err:=RW_Area_SaveUndo($area; 1)  //start undo block


$bool:=True:C214
For ($i; 0x0001; Size of array:C274($alObjects))
	Case of 
		: ($bRelative)
			$err:=RW_GetProperty($area; $alObjects{$i}; $oldProp; ->$oldValue)
			$move:=Round:C94($value-$oldValue; 4)
		: ($altprop#"")
			$err:=RW_GetProperty($area; $alObjects{$i}; $prop; ->$oldValue)
			$move:=$value
		Else 
			$move:=$value
	End case 
	$err:=RW_SetProperty($area; $alObjects{$i}; $prop; ->$move)
	If (($altprop#"") & ($oldValue#0))
		$err:=RW_GetProperty($area; $alObjects{$i}; $altprop; ->$move)
		$move:=Round:C94($move*$value/$oldValue; 2)
		$err:=RW_SetProperty($area; $alObjects{$i}; $altprop; ->$move)
	End if 
End for 

$err:=RW_Area_SaveUndo($area; 0)  //end  undo block
