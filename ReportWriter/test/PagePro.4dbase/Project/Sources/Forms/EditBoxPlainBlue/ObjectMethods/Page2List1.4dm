// modified 04.07.2014/14:52/SDS Software GmbH/Frank

C_LONGINT:C283($Error_L)
C_LONGINT:C283($Value_L)

$Error_L:=0
$Value_L:=0

$Value_L:=Num:C11(PP_FontSizePopup{PP_FontSizePopup})
OBJECT SET FONT SIZE:C165(PP_InlineEditPlain; $Value_L)

$Error_L:=PPro_SetPropertyNumber(PP_ExtArea; 0; PProp_FontSize:K7906:67; $Value_L)