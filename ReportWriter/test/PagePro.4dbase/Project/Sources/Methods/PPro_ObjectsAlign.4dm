//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area
C_TEXT:C284($2)  //selector

If (False:C215)
	C_LONGINT:C283(PPro_ObjectsAlign; $1)
	C_TEXT:C284(PPro_ObjectsAlign; $2)  //selector
End if 

C_LONGINT:C283($err; $i; $rep)
C_REAL:C285($top; $left; $bottom; $right; $lineWidth)
C_REAL:C285($gtop; $gleft; $gbottom; $gright)
C_TEXT:C284($sel)

$rep:=$1
$sel:=$2  //"" - according to selected objects

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($rep; $rep; "selc"; $alObjects)

$err:=RW_Area_SaveUndo($rep; 1)  //start undo block


If (Size of array:C274($alObjects)>0x0001)  // Group
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
	
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty($rep; $alObjects{$i}; "post"; ->$top)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posl"; ->$left)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posb"; ->$bottom)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "posr"; ->$right)
		$err:=RW_GetProperty($rep; $alObjects{$i}; "thic"; ->$lineWidth)
		
		$top:=$top-$lineWidth
		$left:=$left-$lineWidth
		$right:=$right+$lineWidth
		$bottom:=$bottom+$lineWidth
		
		C_REAL:C285($move)
		Case of 
			: ($sel="top")
				$move:=$gtop-$top
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movV"; ->$move)
			: ($sel="left")
				$move:=$gleft-$left
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movH"; ->$move)
			: ($sel="bottom")
				$move:=$gbottom-$bottom
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movV"; ->$move)
			: ($sel="right")
				$move:=$gright-$right
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movH"; ->$move)
				
			: ($sel="hCenter")
				$move:=(($gbottom+$gtop)-($bottom+$top))/2
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movV"; ->$move)
			: ($sel="vCenter")
				$move:=(($gright+$gleft)-($right+$left))/2
				$err:=RW_SetProperty($rep; $alObjects{$i}; "movH"; ->$move)
		End case 
	End for 
	
	
End if 
