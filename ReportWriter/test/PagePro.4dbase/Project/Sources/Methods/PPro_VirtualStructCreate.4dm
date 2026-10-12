//%attributes = {"shared":true}
// PPro_VirtualStructCreate
//      without parameters or return value
//
// method creates hierarchical lists that are used to display
// fields, variables and 4D commands 

C_TEXT:C284($newMenuReference)
C_BOOLEAN:C305($b)

If (PPro_EditorCallback("virtualStruct"; "init"; 0; 0))
	
	PPro_VirtualStructure("field"; "init")
	PPro_VirtualStructure("var"; "init")
	PPro_VirtualStructure("commands"; "init")
	PPro_VirtualStructure("field"; "default")
	PPro_VirtualStructure("var"; "default")
	PPro_VirtualStructure("commands"; "default")
	
	$b:=PPro_EditorCallback("virtualStruc"; "load"; 0; 0)
	
End if 

PP_ColumnPopup:=PPro_JoinLists(1)
PP_TablePopup:=PPro_JoinLists(2)
