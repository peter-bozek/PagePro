//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area

If (False:C215)
	C_LONGINT:C283(PPro_ResetSelection; $1)  //area
	
End if 

C_LONGINT:C283($area; $err)
C_BOOLEAN:C305($bValue)

ARRAY LONGINT:C221($aSelection; 0x0000)

$area:=$1

$err:=RW_GetObjects($area; $area; "selc"; $aSelection)  // get selected items

Case of 
	: (Size of array:C274($aSelection)=0)  //nothing selected
		//select report as object selected by default
		$bValue:=True:C214
		$err:=RW_SetProperty($area; $area; "selc"; ->$bValue)
		PP_CurrentObject:=$area
		
	: (Size of array:C274($aSelection)=1)  //this is selected obect
		PP_CurrentObject:=$aSelection{1}
	: (Size of array:C274($aSelection)>1)  //select one
		PP_CurrentObject:=$aSelection{1}
		$bValue:=False:C215
		C_LONGINT:C283($i)
		For ($i; 2; Size of array:C274($aSelection))
			$err:=RW_SetProperty($area; $aSelection{$i}; "selc"; ->$bValue)
		End for 
End case 

ARRAY LONGINT:C221(PP_ItemsToSelect; 1)
PP_ItemsToSelect{1}:=PP_CurrentObject