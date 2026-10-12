//%attributes = {"invisible":true}
// PPro_EditStyles
// 

// created 10.06.2011/18:03/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_TEXT:C284($1; $Selektor)
C_LONGINT:C283($2; $size)
C_TEXT:C284($3; $dest)
C_TEXT:C284($0; $Return)

If (False:C215)
	C_TEXT:C284(PPro_EditStyles; $1)
	C_LONGINT:C283(PPro_EditStyles; $2)
	C_TEXT:C284(PPro_EditStyles; $3)
	C_TEXT:C284(PPro_EditStyles; $0)
End if 

C_LONGINT:C283($ArraySize)
C_LONGINT:C283($Counter)
C_LONGINT:C283($CurrentObject)
C_TEXT:C284($Ergebnis)
C_LONGINT:C283($Error)
C_LONGINT:C283($Found)
C_LONGINT:C283($WinRef)
// —— Initialisation ———————————————

Case of 
	: (Count parameters:C259=3)
		$Selektor:=$1
		$size:=$2
		$dest:=$3
	: (Count parameters:C259=2)
		$Selektor:=$1
		$size:=$2
		$dest:=""
	: (Count parameters:C259=1)
		$Selektor:=$1
		$size:=0
		$dest:=""
	Else 
		$Selektor:=""
		$size:=0
		$dest:=""
End case 
$Return:=""

$ArraySize:=0
$Counter:=0
$CurrentObject:=0
$Ergebnis:=""
$Error:=0
$Found:=0
$WinRef:=0
// —— Main ———————————————

Case of 
	: ($Selektor="")
		// Entry point for style editor
		
		$Return:=PPro_EditStyles("Dim Arrays"; 0; "All")
		$Return:=PPro_EditStyles("Init Var"; 0; "All")
		
		$CurrentObject:=PP_CurrentObject
		
		$WinRef:=Open form window:C675("StylesEditBlue"; Sheet form window:K39:12)
		DIALOG:C40("StylesEditBlue")
		CLOSE WINDOW:C154($WinRef)
		
		$Return:=PPro_EditStyles("Dim Arrays"; 0)
		
		PP_CurrentObject:=$CurrentObject
		
		
	: ($Selektor="Dim Arrays")
		// Dim Listbox Arrays
		
		If (($dest="Listbox With Styles") | ($dest="All"))
			ARRAY BOOLEAN:C223(PP_StyleLB; $size)
			ARRAY LONGINT:C221(PP_StyleID; $size)
			ARRAY TEXT:C222(PP_StyleName; $size)
			ARRAY TEXT:C222(PP_StyleProps; $size)
			ARRAY LONGINT:C221(PP_StyleRef; $size)
		End if 
		
		
	: ($Selektor="Init Var")
		// $dest = What to init? (area or all)
		
		If (($dest="Listbox With Styles") | ($dest="All"))
			
			$Error:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "STL#"; PP_StyleRef)
			$ArraySize:=Size of array:C274(PP_StyleRef)
			$Return:=PPro_EditStyles("Dim Arrays"; $ArraySize; "Listbox With Styles")
			
			For ($Counter; 1; $ArraySize)
				$Error:=RW_GetProperty(PP_ExtArea; PP_StyleRef{$Counter}; "id"; ->PP_StyleID{$Counter})
				$Error:=RW_GetProperty(PP_ExtArea; PP_StyleRef{$Counter}; "name"; ->PP_StyleName{$Counter})
				PP_StyleLB{$Counter}:=False:C215
				PP_StyleProps{$Counter}:=""
			End for 
			
		End if 
		
		If (($Selektor="Variables") | ($Selektor="All"))
			PP_CurrentStyleID:=0
		End if 
		
		
	: ($Selektor="Add New Style")
		// Add a new stylesheet
		
		PPro_NewStyle(PP_ExtArea)
		
		$Return:=PPro_EditStyles("Dim Arrays"; 0; "Listbox With Styles")
		$Return:=PPro_EditStyles("Init Var"; 0; "Listbox With Styles")
		$Found:=Find in array:C230(PP_StyleRef; PP_CurrentObject)
		If ($Found>0)
			LISTBOX SELECT ROW:C912(PP_StyleLB; $Found; lk replace selection:K53:1)
		Else 
			LISTBOX SELECT ROW:C912(PP_StyleLB; 0; lk remove from selection:K53:3)
			PP_CurrentObject:=0
		End if 
		$Return:=PPro_EditStyles("Load Style Data")
		$Return:=PPro_EditStyles("Update Display")
		
		PPro_StylesLoadArray
		PPro_AreaReloadHList(PP_ExtArea)
		PPro_AreaFormData(PP_ExtArea)
		
		
	: ($Selektor="Delete Style")
		// The selected style has been deleted ...
		
		$Found:=Find in array:C230(PP_StyleRef; PP_CurrentObject)
		LISTBOX DELETE ROWS:C914(PP_StyleLB; $Found)
		$ArraySize:=Size of array:C274(PP_StyleLB)
		If ($Found>$ArraySize)
			$Found:=$ArraySize
		End if 
		If ($Found>0)
			LISTBOX SELECT ROW:C912(PP_StyleLB; $Found; lk replace selection:K53:1)
		Else 
			LISTBOX SELECT ROW:C912(PP_StyleLB; 0; lk remove from selection:K53:3)
		End if 
		
		$Return:=PPro_EditStyles("Event: On Style Selected")
		$Ergebnis:=PPro_EditStyles("Load Style Data")
		$Ergebnis:=PPro_EditStyles("Update Display")
		
		
	: ($Selektor="Event: On Style Selected")
		// Act on user click in the style sheet list ...
		
		If (Count in array:C907(PP_StyleLB; True:C214)=0)
			LISTBOX SELECT ROW:C912(PP_StyleLB; Size of array:C274(PP_StyleLB); lk replace selection:K53:1)
		End if 
		
		If (Count in array:C907(PP_StyleLB; True:C214)=1)
			PP_CurrentObject:=PP_StyleRef{PP_StyleLB}
			PP_CurrentStyleID:=PP_StyleID{PP_StyleLB}
		Else 
			
			PP_CurrentObject:=0
			PP_CurrentStyleID:=0
		End if 
		
		
	: ($Selektor="Load Style Data")
		// Load data into the vars ...
		
		PPro_StyleProperties("get"; PP_CurrentObject)
		
		
	: ($Selektor="Save Style Data")
		// Load data into the vars ...
		
		If (PP_CurrentObject>0)
			PPro_StyleProperties("set"; PP_CurrentObject)
		End if 
		
		
	: ($Selektor="Update Display")
		// Handling of vars and buttons regarding to the style selection ...
		
		OBJECT SET ENABLED:C1123(PP_DeleteStyle; (PP_CurrentStyleID>0))
		
		
	Else 
		BEEP:C151
		ALERT:C41("Unbekannter Selektor \""+$Selektor+"\"")
End case 

// —— Return ———————————————
$0:=$Return
