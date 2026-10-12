//%attributes = {"invisible":true}
C_LONGINT:C283($0)  //returns default list of PP variables

If (False:C215)
	C_LONGINT:C283(PPro_VirtStructCreateVarList; $0)
End if 

C_LONGINT:C283($table; $field)
C_LONGINT:C283($hList; $hSubList)

$hList:=New list:C375

$hSubList:=New list:C375
APPEND TO LIST:C376($hSubList; "RWRecord"; -16)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "RWRecord")
APPEND TO LIST:C376($hSubList; "RWDate"; -17)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "RWDate")
APPEND TO LIST:C376($hSubList; "RWTime"; -18)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "RWTime")

C_TEXT:C284($item; $s)
$item:=Get localized string:C991("PP_RWVariables")
APPEND TO LIST:C376($hList; $item; -32; $hSublist; False:C215)

$hSubList:=New list:C375
APPEND TO LIST:C376($hSubList; "PAGE"; -19)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "PAGE")
APPEND TO LIST:C376($hSubList; "PAGES"; -20)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "PAGES")
APPEND TO LIST:C376($hSubList; "SUBPAGE"; -21)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "SUBPAGE")
APPEND TO LIST:C376($hSubList; "SUBPAGES"; -22)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "SUBPAGES")
APPEND TO LIST:C376($hSubList; "FRAME"; -23)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "FRAME")
APPEND TO LIST:C376($hSubList; "FRAMES"; -24)
SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "FRAMES")

C_TEXT:C284($item)
$item:=Get localized string:C991("PP_PrintVariables")
APPEND TO LIST:C376($hList; $item; -33; $hSublist; False:C215)

$0:=$hList
