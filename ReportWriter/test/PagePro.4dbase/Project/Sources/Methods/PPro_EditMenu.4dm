//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_EditMenu; $1)
End if 

Case of 
	: ($1="set")
		If (PPro_EditorCallback("menu"; "editStart"; 0; 0))
			DISABLE MENU ITEM:C150(1; 0)
			DISABLE MENU ITEM:C150(3; 0)
			DISABLE MENU ITEM:C150(5; 0)
			DISABLE MENU ITEM:C150(6; 0)
		End if 
		
	: ($1="restore")
		If (PPro_EditorCallback("menu"; "editEnd"; 0; 0))
			ENABLE MENU ITEM:C149(1; 0)
			ENABLE MENU ITEM:C149(3; 0)
			ENABLE MENU ITEM:C149(5; 0)
			ENABLE MENU ITEM:C149(6; 0)
		End if 
End case 