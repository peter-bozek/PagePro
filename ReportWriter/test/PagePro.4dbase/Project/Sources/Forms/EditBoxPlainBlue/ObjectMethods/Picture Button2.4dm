// modified 04.07.2014/14:52/SDS Software GmbH/Frank

C_LONGINT:C283($Error_L)
C_LONGINT:C283($Value_L)

$Error_L:=0
$Value_L:=0

$Value_L:=OBJECT Get font style:C1071(PP_InlineEditPlain)

If (PP_StyleU=0)
	$Value_L:=$Value_L-Underline:K14:4
Else 
	$Value_L:=$Value_L+Underline:K14:4
End if 

OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; $Value_L)

$Error_L:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_FontStyleU:K7906:70; PP_StyleB)
