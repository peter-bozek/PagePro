//%attributes = {"invisible":true}
C_LONGINT:C283($i)

ARRAY TEXT:C222($aNames; 0x0000)
ARRAY TEXT:C222($aRefs; 0x0000)


//iteration through all menu 
GET MENU ITEMS:C977(PP_MenuRef; $aNames; $aRefs)

For ($i; 1; Size of array:C274($aRefs))
	PPro_MenuIterator($aRefs{$i})
End for 
