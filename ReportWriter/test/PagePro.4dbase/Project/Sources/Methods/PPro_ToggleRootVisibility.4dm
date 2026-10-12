//%attributes = {"invisible":true}
// modified 09.06.2011/10:28/SDS Software GmbH/Mußhoff

PP_XMLIsDisplayed:=Not:C34(PP_XMLIsDisplayed)
OBJECT SET VISIBLE:C603(PP_ExtArea; Not:C34(PP_XMLIsDisplayed))
OBJECT SET VISIBLE:C603(PP_XMLRoot; PP_XMLIsDisplayed)

PPro_EditorCallback("area"; "visible"; Num:C11(Not:C34(PP_XMLIsDisplayed)); 0)
