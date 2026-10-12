//%attributes = {"shared":true}
// PPro_MenuCreate
//      without parameters or return value
//
// method installs PagePro menu
// should be used to install PagePro when using PagePro in subform

C_TEXT:C284($newMenuReference)
C_BOOLEAN:C305($b)

If (PPro_EditorCallback("menu"; "init"; 0; 0))
	PP_OldMenuRef:=Get menu bar reference:C979(Current process:C322)
	$newMenuReference:=PPro_LoadMenuBar(PP_DefaultMenu; "PageProMenus.xml")
	If ($newMenuReference#"")
		SET MENU BAR:C67($newMenuReference)
		PP_MenuRef:=$newMenuReference
		$b:=PPro_EditorCallback("menu"; "load"; 0; 0)
	End if 
End if 