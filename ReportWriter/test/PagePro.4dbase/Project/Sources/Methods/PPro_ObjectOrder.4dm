//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_TEXT:C284($2)

If (False:C215)
	C_LONGINT:C283(PPro_ObjectOrder; $1)
	C_TEXT:C284(PPro_ObjectOrder; $2)
End if 

C_LONGINT:C283($area; $err; $parent; $dummy; $old; $new; $i)
C_TEXT:C284($command; $text)

ARRAY LONGINT:C221($aObjects; 0x0000)
ARRAY LONGINT:C221($aOrder; 0x0000)

$area:=$1
$command:=$2


If (PP_CurrentObject>0)
	$err:=RW_GetProperty($area; PP_CurrentObject; "kind"; ->$text)
	
	Case of 
		: (($text="Report") | ($text="Guide"))
			//objects of tese types cannot be ordered
			
		: ($text="Section")
			$err:=RW_Area_SaveUndo($area; 4)
			C_LONGINT:C283($order)
			$order:=PPro_GetPropertyInteger($area; PP_CurrentObject; PProp_PrintOrder:K7906:4)
			Case of 
				: ($command="front")
					$order:=$order-1
					If ($order=0)
						$order:=1
					End if 
					
				: ($command="back")
					$order:=$order+1
			End case 
			PPro_SetPropertyInteger($area; PP_CurrentObject; PProp_PrintOrder:K7906:4; $order)
			$err:=RW_Area_SaveUndo($area; 0)
			
		Else 
			$err:=RW_Area_SaveUndo($area; 4)
			
			
			$err:=RW_GetParent($area; PP_CurrentObject; $parent)
			$err:=RW_GetObjects($area; $parent; "objs"; $aObjects)
			ARRAY LONGINT:C221($aOrder; Size of array:C274($aObjects))
			For ($i; 1; Size of array:C274($aObjects))
				$aOrder{$i}:=PPro_GetPropertyInteger($area; $aObjects{$i}; PProp_PrintOrder:K7906:4)
			End for 
			
			SORT ARRAY:C229($aOrder; $aObjects)
			
			$old:=Find in array:C230($aObjects; PP_CurrentObject)
			$new:=0  //dont move
			Case of 
				: ($command="front")
					If ($old<Size of array:C274($aObjects))
						$new:=Size of array:C274($aObjects)
					End if 
					
				: ($command="back")
					If ($old>1)
						$new:=1
					End if 
					
				: ($command="up")
					If ($old<Size of array:C274($aObjects))
						$new:=$old+1
					End if 
					
				: ($command="down")
					If ($old>1)
						$new:=$old-1
					End if 
					
			End case 
			
			If ($new>0)
				$dummy:=$aObjects{$old}
				DELETE FROM ARRAY:C228($aObjects; $old; 1)
				INSERT IN ARRAY:C227($aObjects; $new; 1)
				$aObjects{$new}:=$dummy
				//$dummy:=$aObjects{$new}
				//$aObjects{$new}:=$aObjects{$old}
				//$aObjects{$old}:=$dummy
			End if 
			
			For ($i; 1; Size of array:C274($aObjects))
				PPro_SetPropertyInteger($area; $aObjects{$i}; PProp_PrintOrder:K7906:4; $i)
			End for 
			
			$err:=RW_Area_SaveUndo($area; 0)
	End case 
End if 