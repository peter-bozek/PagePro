// modified 04.07.2014/14:52/SDS Software GmbH/Frank

C_LONGINT:C283($End_L)
C_LONGINT:C283($Start_L)
C_LONGINT:C283($Value_L)

$End_L:=0
$Start_L:=0
$Value_L:=0

GET HIGHLIGHT:C209(PP_InlineEdit; $Start_L; $End_L)
ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute italic style:K65:2; $Value_L)

Case of 
	: ($Value_L=0)
		$Value_L:=1
	: ($Value_L=1)
		$Value_L:=0
	Else 
		$Value_L:=1
End case 

ST SET ATTRIBUTES:C1093(PP_InlineEdit; $Start_L; $End_L; Attribute italic style:K65:2; $Value_L)
