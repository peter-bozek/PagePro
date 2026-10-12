//%attributes = {"invisible":true}
C_LONGINT:C283($err)

PPro_StylesLoadArray
PPro_ResetSelection(PP_ExtArea)
$err:=PPro_CreateDocumentTree(PP_ExtArea; ->PP_XMLRoot)

PP_ListBox:=0  //highlight
PP_Dialogs:=1  //
OBJECT SET VISIBLE:C603(*; "PP_LB@"; False:C215)
PPro_SubformVisibility(PP_ExtArea)

PP_Scaling:=Round:C94(PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Scale:K7906:123)*100; 2)
PPro_AreaFormData(PP_ExtArea)