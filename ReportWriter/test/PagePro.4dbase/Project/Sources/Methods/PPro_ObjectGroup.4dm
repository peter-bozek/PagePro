//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area
C_TEXT:C284($2)  //selector

If (False:C215)
	C_LONGINT:C283(PPro_ObjectGroup; $1)
	C_TEXT:C284(PPro_ObjectGroup; $2)  //selector
End if 

C_LONGINT:C283($err; $group; $i; $parent; $rep; $lock)
C_REAL:C285($top; $left; $bottom; $right; $lineWidth)
C_REAL:C285($gtop; $gleft; $gbottom; $gright)
C_BOOLEAN:C305($bool)
C_TEXT:C284($sel)

$rep:=$1
$sel:=$2  //"" - according to selected objects

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($rep; $rep; "selc"; $alObjects)

$err:=RW_Area_SaveUndo($rep; 1)  //start undo block

If ((Size of array:C274($alObjects)=0x0001) & ($sel#"group"))  //ungroup
	C_TEXT:C284($kind)
	$err:=RW_GetProperty($rep; $alObjects{0x0001}; "kind"; ->$kind)
	If ($kind="Group")  // Ungroup
		$bool:=True:C214
		$group:=$alObjects{0x0001}
		$err:=RW_GetObjects($rep; $group; "objs"; $alObjects)
		$err:=RW_GetProperty($rep; $group; "post"; ->$gtop)
		$err:=RW_GetProperty($rep; $group; "posl"; ->$gleft)
		//$err:=RW_GetProperty ($rep;$group;"posb";->$gbottom)
		//$err:=RW_GetProperty ($rep;$group;"posr";->$gright)
		$err:=RW_GetParent($rep; $group; $parent)
		$lock:=0
		// move objects out of the group
		For ($i; 0x0001; Size of array:C274($alObjects))
			$err:=RW_SetProperty($rep; $alObjects{$i}; "movH"; ->$gleft)
			$err:=RW_SetProperty($rep; $alObjects{$i}; "movV"; ->$gtop)
			$err:=RW_SetProperty($rep; $alObjects{$i}; "selc"; ->$bool)
			$err:=RW_SetProperty($rep; $alObjects{$i}; PProp_Locked:K7906:19; ->$lock)
			$err:=RW_ChangeObjectParent($rep; $alObjects{$i}; $parent)
		End for 
		$err:=RW_DeleteObject($rep; $group)
		ARRAY LONGINT:C221($alObjects; 0x0000)
	Else 
		BEEP:C151
	End if 
End if 

If ((Size of array:C274($alObjects)>0x0001) & ($sel#"ungroup"))  // Group
	// get bounding box
	For ($i; Size of array:C274($alObjects); 0x0001; -1)
		If (PPro_IsLeaf($rep; $alObjects{$i})=False:C215)
			DELETE FROM ARRAY:C228($alObjects; $i; 1)
		End if 
	End for 
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty($rep; $alObjects{$i}; "post"; ->$top)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posl"; ->$left)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posb"; ->$bottom)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posr"; ->$right)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "thic"; ->$lineWidth)
		
		If ($i=0x0001)
			$gtop:=$top-$lineWidth
			$gleft:=$left-$lineWidth
			$gright:=$right+$lineWidth
			$gbottom:=$bottom+$lineWidth
		Else 
			If ($gtop>($top-$lineWidth))
				$gtop:=$top-$lineWidth
			End if 
			If ($gleft>($left-$lineWidth))
				$gleft:=$left-$lineWidth
			End if 
			If ($gbottom<($bottom+$lineWidth))
				$gbottom:=$bottom+$lineWidth
			End if 
			If ($gright<($right+$lineWidth))
				$gright:=$right+$lineWidth
			End if 
		End if 
	End for 
	// move objects into the group
	$err:=RW_GetParent($rep; $alObjects{0x0001}; $parent)
	$err:=RW_NewObject($rep; $group; "GRP#"; $parent)
	$err:=RW_SetProperty($rep; $group; "post"; ->$gtop)
	$err:=RW_SetProperty($rep; $group; "posl"; ->$gleft)
	$err:=RW_SetProperty($rep; $group; "posb"; ->$gbottom)
	$err:=RW_SetProperty($rep; $group; "posr"; ->$gright)
	$bool:=True:C214
	$err:=RW_SetProperty($rep; $group; "expV"; ->$bool)
	$err:=RW_SetProperty($rep; $group; "selc"; ->$bool)
	$gtop:=-$gtop
	$gleft:=-$gleft
	$bool:=False:C215
	$lock:=PP_LockedMode
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_SetProperty($rep; $alObjects{$i}; "movH"; ->$gleft)
		$err:=RW_SetProperty($rep; $alObjects{$i}; "movV"; ->$gtop)
		$err:=RW_SetProperty($rep; $alObjects{$i}; "selc"; ->$bool)
		$err:=RW_ChangeObjectParent($rep; $alObjects{$i}; $group)
		$err:=RW_SetProperty($rep; $alObjects{$i}; PProp_Locked:K7906:19; ->$lock)
	End for 
End if 
$err:=RW_Area_SaveUndo($rep; 0)  //end undo block
