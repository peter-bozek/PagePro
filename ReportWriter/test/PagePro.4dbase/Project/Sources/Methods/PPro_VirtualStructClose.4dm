//%attributes = {"shared":true}
// PPro_VirtualStructClose
//      without parameters or return value
//
// clears hierarchical lists that are used to display
// fields, variables and 4D commands 

If (PPro_EditorCallback("virtualStruc"; "close"; 0; 0))
	
	PPro_VirtualStructure("field"; "init")
	PPro_VirtualStructure("var"; "init")
	
End if 