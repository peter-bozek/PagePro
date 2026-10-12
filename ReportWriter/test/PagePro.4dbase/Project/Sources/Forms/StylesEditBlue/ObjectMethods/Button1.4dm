// StylesEdit.Button1
// 

// created 12.06.2011/11:01/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_TEXT:C284($Ergebnis_T)
// —— Initialisation ———————————————
$Ergebnis_T:=""
// —— Main ———————————————

Case of 
	: (Form event code:C388=On Clicked:K2:4)
		
		$Ergebnis_T:=PPro_EditStyles("Add New Style")
		
		
	Else 
		// sc only
End case 
