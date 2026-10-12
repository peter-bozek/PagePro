//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_ObjectCommandsDispatch; $1)
End if 

Case of 
	: ($1="duplicate")
		PPro_XMLObjects(PP_ExtArea; "d")
		
	: ($1="edit")
		PPro_ObjectEdit(PP_ExtArea)
		
	: ($1="bring2Front")
		PPro_ObjectOrder(PP_ExtArea; "front")
	: ($1="bring2Back")
		PPro_ObjectOrder(PP_ExtArea; "back")
		
	: ($1="group")
		PPro_ObjectGroup(PP_ExtArea; "group")
		PPro_AreaReloadHList(PP_ExtArea)
		PPro_AreaFormData(PP_ExtArea)
		
	: ($1="ungroup")
		PPro_ObjectGroup(PP_ExtArea; "ungroup")
		PPro_AreaReloadHList(PP_ExtArea)
		PPro_AreaFormData(PP_ExtArea)
		
	: ($1="lock")
		PPro_ObjectLock(PP_ExtArea; "lock"; "sel")
	: ($1="unlock")
		PPro_ObjectLock(PP_ExtArea; "unlock"; "all")
		
	: ($1="hide")
		PPro_ObjectProperty(PP_ExtArea; "hide"; "sel")
	: ($1="show")
		PPro_ObjectProperty(PP_ExtArea; "show"; "all")
		
	: ($1="notprint")
		PPro_ObjectProperty(PP_ExtArea; "dontprint"; "sel")
	: ($1="printall")
		PPro_ObjectProperty(PP_ExtArea; "print"; "all")
		
End case 
