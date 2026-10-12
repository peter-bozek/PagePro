// PagePro.Unsichtbare Schaltfläche1
// 

// created 12.06.2011/14:43/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_LONGINT:C283($Bottom_L)
C_LONGINT:C283($FormHeight_L)
C_LONGINT:C283($FormWidth_L)
C_LONGINT:C283($Left_L)
C_LONGINT:C283($LeftPos_L)
C_LONGINT:C283($Right_L)
C_LONGINT:C283($Top_L)
C_LONGINT:C283($TopPos_L)
C_LONGINT:C283($WinHeight_L)
C_LONGINT:C283($WinRef_L)
C_LONGINT:C283($WinWidth_L)
// —— Initialisation ———————————————
$Bottom_L:=0
$FormHeight_L:=0
$FormWidth_L:=0
$Left_L:=0
$LeftPos_L:=0
$Right_L:=0
$Top_L:=0
$TopPos_L:=0
$WinHeight_L:=0
$WinRef_L:=0
$WinWidth_L:=0
// —— Main ———————————————

Case of 
	: (Form event code:C388=On Clicked:K2:4)
		
		GET WINDOW RECT:C443($Left_L; $Top_L; $Right_L; $Bottom_L)
		$WinWidth_L:=$Right_L-$Left_L
		$WinHeight_L:=$Bottom_L-$Top_L
		
		FORM GET PROPERTIES:C674("AboutBlue"; $FormWidth_L; $FormHeight_L)
		$LeftPos_L:=$Left_L+Int:C8(($WinWidth_L-$FormWidth_L)/2)
		$TopPos_L:=$Top_L+Int:C8(($WinHeight_L-$FormHeight_L)/2)-20
		
		$WinRef_L:=Open form window:C675("AboutBlue"; Pop up form window:K39:11; $LeftPos_L; $TopPos_L)
		DIALOG:C40("AboutBlue")
		CLOSE WINDOW:C154($WinRef_L)
		
	Else 
		// sc only
End case 
