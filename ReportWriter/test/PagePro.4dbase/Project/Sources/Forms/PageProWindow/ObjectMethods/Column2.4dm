C_LONGINT:C283($err; $i; $table; $col; $row)
C_TEXT:C284($text)
C_LONGINT:C283($table)

LISTBOX GET CELL POSITION:C971(PP_PropertyPalette; $col; $row)
PP_PropValue:=$row

$err:=RW_Area_SaveUndo(PP_ExtArea; 10)
For ($i; 0x0001; Size of array:C274(PP_ItemsToSelect))
	$err:=RW_GetProperty(PP_ExtArea; PP_ItemsToSelect{$i}; "kind"; ->$text)
	If (($text="Table") | ($text="Header") | ($text="Column"))
		If ($text="Table")
			$table:=PP_ItemsToSelect{$i}
		Else 
			$err:=RW_GetParent(PP_ExtArea; PP_ItemsToSelect{$i}; $table)
		End if 
		//$err:=RW_Area_SaveUndo (PP_ExtArea;11)
		$err:=RW_Area_AddUndo(PP_ExtArea; $table; 1)  // add old state of the table
		$err:=RW_Area_SaveUndo(PP_ExtArea; -1)  // pause recording
		$err:=RW_SetProperty(PP_ExtArea; PP_ItemsToSelect{$i}; PP_PropID{PP_PropValue}; ->PP_PropValue{PP_PropValue})
		$err:=RW_Area_SaveUndo(PP_ExtArea; -2)  // resume recording
		$err:=RW_Area_AddUndo(PP_ExtArea; $table; 0)  // add new state of the table
	Else 
		$err:=RW_SetProperty(PP_ExtArea; PP_ItemsToSelect{$i}; PP_PropID{PP_PropValue}; ->PP_PropValue{PP_PropValue})
	End if 
End for 

$err:=RW_Area_SaveUndo(PP_ExtArea; 0)
$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)
SET TIMER:C645(-1)
