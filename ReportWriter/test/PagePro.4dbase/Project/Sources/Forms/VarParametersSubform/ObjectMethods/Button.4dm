C_LONGINT:C283($win)

PP_ScriptName:=Get localized string:C991("PP_ScriptVariable")+": "+PP_Alias+" ("+PP_ObjectID+")"
GET PICTURE FROM LIBRARY:C565("PP_VariableIcon"; PP_ScriptPicture)

$win:=Open form window:C675("ScriptEdit"+PP_Environment; Sheet form window:K39:12)
DIALOG:C40("ScriptEdit"+PP_Environment)
CLOSE WINDOW:C154($win)

C_LONGINT:C283($err)
$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Script:K7906:54; PP_Script)
