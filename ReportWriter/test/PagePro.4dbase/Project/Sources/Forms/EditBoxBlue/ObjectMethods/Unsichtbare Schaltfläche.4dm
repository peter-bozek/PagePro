// modified 04.07.2014/14:52/SDS Software GmbH/Frank

C_LONGINT:C283($End_L)
C_LONGINT:C283($Start_L)
C_LONGINT:C283($Value_L)
C_TEXT:C284($Value_T)

$End_L:=0
$Start_L:=0
$Value_L:=0
$Value_T:=""

Case of 
	: (Form event code:C388=On Clicked:K2:4)
		
		GET HIGHLIGHT:C209(PP_InlineEdit; $Start_L; $End_L)
		If ($End_L>$Start_L)
			
			ST GET ATTRIBUTES:C1094(PP_InlineEdit; $Start_L; $End_L; Attribute text color:K65:7; $Value_L)
			
			$Value_L:=Select RGB color:C956($Value_L; "Textfarbe wählen")
			$Value_T:="#"+Substring:C12(String:C10($Value_L; "&$"); 2)
			
			ST SET ATTRIBUTES:C1093(PP_InlineEdit; $Start_L; $End_L; Attribute text color:K65:7; $Value_T)
			
		End if 
		
		
End case 
