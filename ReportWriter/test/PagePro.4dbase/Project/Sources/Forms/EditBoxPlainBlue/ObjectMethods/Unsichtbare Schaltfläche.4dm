// modified 04.07.2014/14:52/SDS Software GmbH/Frank

C_LONGINT:C283($AltColor_L)
C_LONGINT:C283($Back_L)
C_LONGINT:C283($Error_L)
C_LONGINT:C283($Fore_L)
C_TEXT:C284($Value_T)

$AltColor_L:=0
$Back_L:=0
$Error_L:=0
$Fore_L:=0
$Value_T:=""

Case of 
	: (Form event code:C388=On Clicked:K2:4)
		
		OBJECT GET RGB COLORS:C1074(PP_InlineEditPlain; $Fore_L; $Back_L; $AltColor_L)
		
		$Fore_L:=Select RGB color:C956($Fore_L; "Textfarbe wählen")
		// $Value_T:=FUNC_Longint2Hex ($Value_L)
		
		OBJECT SET RGB COLORS:C628(PP_InlineEditPlain; $Fore_L; $Back_L; $AltColor_L)
		
		$Error_L:=PPro_SetPropertyNumber(PP_ExtArea; 0; PProp_TextColor:K7906:71; $Fore_L | 0xFF000000)
		
End case 
