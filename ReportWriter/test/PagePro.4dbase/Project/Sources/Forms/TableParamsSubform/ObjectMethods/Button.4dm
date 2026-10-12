C_LONGINT:C283($win)

PP_ScriptName:=Get localized string:C991("PP_ScriptTable")+": "+PP_ObjectID
GET PICTURE FROM LIBRARY:C565("PP_TableIcon"; PP_ScriptPicture)

$win:=Open form window:C675("ScriptEdit"+PP_Environment; Sheet form window:K39:12)
DIALOG:C40("ScriptEdit"+PP_Environment)
CLOSE WINDOW:C154($win)
