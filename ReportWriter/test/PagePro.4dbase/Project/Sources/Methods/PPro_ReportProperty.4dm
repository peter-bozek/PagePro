//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($2)
C_BOOLEAN:C305($0)

If (False:C215)
	C_TEXT:C284(PPro_ReportProperty; $1)
	C_LONGINT:C283(PPro_ReportProperty; $2)
	C_BOOLEAN:C305(PPro_ReportProperty; $0)
End if 

C_TEXT:C284($sel; $prop)
C_LONGINT:C283($area; $err; $value; $newValue; $val)

$sel:=$1
$area:=$2

$val:=-1
$prop:=""
$0:=False:C215
Case of 
	: ($sel="Rulers")
		$prop:=PProp_ShowRulers:K7906:114
	: ($sel="Grid")
		$prop:=PProp_ShowGrid:K7906:116
	: ($sel="ToGrid")
		$prop:=PProp_SnapToGrid:K7906:118
	: ($sel="Guide")
		$prop:=PProp_ShowGuides:K7906:119
	: ($sel="ShowBorders")
		$prop:=PProp_ShowObjectBorders:K7906:122
		
	: ($sel="Normal")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=0
	: ($sel="Alias")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=1
	: ($sel="Names")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=3
	: ($sel="IDs")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=4
	: ($sel="Formats")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=2
	: ($sel="PrintOrder")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=5
	: ($sel="Values")
		$prop:=PProp_Drawingmode:K7906:127
		$val:=7
		
	: ($sel="ShowGuides")
		$prop:=PProp_ShowGuides:K7906:119
	: ($sel="LockGuides")
		$prop:=PProp_LockGuides:K7906:121
	: ($sel="SnapGuides")
		$prop:=PProp_SnapToGuides:K7906:120
	: ($sel="ShowXMLTree")
		$prop:=""  // property of editor that is not saved
		PPro_ToggleRootVisibility
End case 


If ($prop#"")
	$err:=RW_Area_SaveUndo($area; 1)  //start undo block
	$err:=RW_GetProperty($area; $area; $prop; ->$value)
	If ($val=-1)
		$newValue:=1-$value
		$err:=RW_SetProperty($area; $area; $prop; ->$newValue)
	Else 
		$newValue:=$val
		If ($newValue#$value)
			$err:=RW_SetProperty($area; $area; $prop; ->$newValue)
		Else 
			$newValue:=0
			$err:=RW_SetProperty($area; $area; $prop; ->$newValue)
		End if 
	End if 
	$err:=RW_Area_SaveUndo($area; 0)  //end  undo block
	
	$0:=($newValue#$value)
End if 