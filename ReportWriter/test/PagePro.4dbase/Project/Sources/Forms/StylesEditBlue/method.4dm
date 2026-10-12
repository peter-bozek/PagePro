// StylesEdit
// 

// created 10.06.2011/18:28/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_TEXT:C284($Ergebnis_T)
// —— Initialisation ———————————————
$Ergebnis_T:=""
// —— Main ———————————————

Case of 
	: (Form event code:C388=On Load:K2:1)
		
		If (Size of array:C274(PP_StyleLB)>0)
			LISTBOX SELECT ROW:C912(PP_StyleLB; 1; lk replace selection:K53:1)
			PP_CurrentObject:=PP_StyleRef{1}
		End if 
		
		$Ergebnis_T:=PPro_EditStyles("Load Style Data")
		$Ergebnis_T:=PPro_EditStyles("Update Display")
		
		
	: (Form event code:C388=On Clicked:K2:4)
		
		$Ergebnis_T:=PPro_EditStyles("Save Style Data")
		
		
	: (Form event code:C388=On Data Change:K2:15)
		
		$Ergebnis_T:=PPro_EditStyles("Save Style Data")
		
		
	Else 
		// sc only
End case 
