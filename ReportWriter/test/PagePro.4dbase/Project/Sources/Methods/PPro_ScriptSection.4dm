//%attributes = {"invisible":true}
C_LONGINT:C283($win)

PP_ScriptName:=Get localized string:C991("PP_ScriptSection")+": "+PP_Type
If (PP_ObjectID#"")
	PP_ScriptName:=PP_ScriptName+" ("+PP_ObjectID+")"
End if 
GET PICTURE FROM LIBRARY:C565("PP_SectionIcon"; PP_ScriptPicture)

$win:=Open form window:C675("ScriptEdit"+PP_Environment; Sheet form window:K39:12)
DIALOG:C40("ScriptEdit"+PP_Environment)
CLOSE WINDOW:C154($win)
