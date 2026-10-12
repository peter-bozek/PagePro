//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area
C_TEXT:C284($2; $3)  //selector

If (False:C215)
	C_LONGINT:C283(PPro_ObjectLock; $1)
	C_TEXT:C284(PPro_ObjectLock; $2)  //selector
	C_TEXT:C284(PPro_ObjectLock; $3)  //selector
End if 

C_LONGINT:C283($err; $i; $parent; $area; $lock; $groupLock)
C_BOOLEAN:C305($bool; $groupSel; $locked; $b)
C_TEXT:C284($sel; $scope; $text)

$area:=$1
$sel:=$2
$scope:=$3  //"" - all objects

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($area; $area; "selc"; $alObjects)

$err:=RW_Area_SaveUndo($area; 1)  //start undo block

Case of 
	: ($sel="lock")
		$lock:=PP_LockedMode
		$bool:=False:C215
		$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
		For ($i; 0x0001; Size of array:C274($alObjects))
			$err:=RW_GetProperty($area; $alObjects{$i}; "kind"; ->$text)
			$b:=True:C214
			If ($text="Section")
				$text:=Get localized string:C991("PP_SectionLockMsg")
				CONFIRM:C162($text)
				$b:=(OK=1)
			End if 
			If ($b)
				$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$bool)
				$err:=RW_SetProperty($area; $alObjects{$i}; PProp_Locked:K7906:19; ->$lock)
			End if 
		End for 
		
	: ($sel="unlock")
		
		If ($scope="all")
			// cancel current selection first
			$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
			$bool:=False:C215
			For ($i; 0x0001; Size of array:C274($alObjects))
				$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$bool)
			End for 
			//reload all objects
			$err:=RW_GetObjects($area; $area; "objs"; $alObjects)
			
		Else 
			// get only selected objects
			$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
			
		End if 
		
		// unlock, in case object are in group relock them
		$lock:=0
		$groupLock:=2
		$bool:=True:C214
		$groupSel:=False:C215
		
		
		
		For ($i; 0x0001; Size of array:C274($alObjects))
			$err:=RW_GetProperty($area; $alObjects{$i}; PProp_Locked:K7906:19; ->$locked)
			If (($err=0) & $locked)
				$err:=RW_SetProperty($area; $alObjects{$i}; PProp_Locked:K7906:19; ->$lock)
				$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$bool)
				$err:=RW_GetParent($area; $alObjects{$i}; $parent)
				If ($parent#0x0000)
					$err:=RW_GetProperty($area; $parent; "kind"; ->$text)
					If ($text="Group")
						$err:=RW_SetProperty($area; $alObjects{$i}; PProp_Locked:K7906:19; ->$groupLock)
						$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$groupSel)
						
					End if 
				End if 
			End if 
		End for 
		
End case 
$err:=RW_Area_SaveUndo($area; 0)  //end undo block
