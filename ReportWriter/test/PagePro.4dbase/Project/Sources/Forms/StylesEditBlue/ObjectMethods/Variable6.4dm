// StylesEdit.Variable6
// 

// created 12.06.2011/11:09/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_LONGINT:C283($Found_L)
// —— Initialisation ———————————————
$Found_L:=0
// —— Main ———————————————

Case of 
	: (Form event code:C388=On Data Change:K2:15)
		
		If (PP_StyleName{PP_StyleLB}#PP_ObjectName)
			PP_StyleName{PP_StyleLB}:=PP_ObjectName
			LISTBOX SORT COLUMNS:C916(PP_StyleLB; 1; >)
			$Found_L:=Find in array:C230(PP_StyleRef; PP_CurrentObject)
			If ($Found_L>0)
				LISTBOX SELECT ROW:C912(PP_StyleLB; $Found_L; lk replace selection:K53:1)
			Else 
				BEEP:C151
				ALERT:C41("Style #"+String:C10(PP_CurrentObject)+" nicht gefunden. Ärks.")
			End if 
		End if 
		
		
	Else 
		// sc only
End case 
