//%attributes = {"shared":true}
// PPro_MenuClose
//      without parameters or return value
//
// methods that de-installs PagePro menu

If (PPro_EditorCallback("menu"; "close"; 0; 0))
	RELEASE MENU:C978(PP_MenuRef)
	If (PP_DefaultMenuID>0)
		SET MENU BAR:C67(PP_DefaultMenuID)
	Else 
		// workaround around situation where 4D destroy menu bar stored in PP_OldMenuRef
		If (PP_OldMenuRef#"")
			ARRAY TEXT:C222($asMenus; 0)
			ARRAY TEXT:C222($asIDs; 0)
			GET MENU ITEMS:C977(PP_OldMenuRef; $asMenus; $asIDs)
			If (Size of array:C274($asMenus)>0)
				SET MENU BAR:C67(PP_OldMenuRef)
			End if 
		End if 
	End if 
End if 