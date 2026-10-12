// StylesEdit.LB_Styles
// 

// created 11.06.2011/07:26/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_TEXT:C284($Ergebnis_T)
// —— Initialisation ———————————————
$Ergebnis_T:=""
// —— Main ———————————————

Case of 
	: (Form event code:C388=On Selection Change:K2:29)
		
		$Ergebnis_T:=PPro_EditStyles("Event: On Style Selected")
		$Ergebnis_T:=PPro_EditStyles("Load Style Data")
		$Ergebnis_T:=PPro_EditStyles("Update Display")
		
		
	Else 
		// sc only
End case 
