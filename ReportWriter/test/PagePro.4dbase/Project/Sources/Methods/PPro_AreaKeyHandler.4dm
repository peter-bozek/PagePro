//%attributes = {"shared":true}
C_POINTER:C301($1)  //area
C_BOOLEAN:C305($0)  //area needs redraw

If (False:C215)
	C_POINTER:C301(PPro_AreaKeyHandler; $1)  //area
	C_BOOLEAN:C305(PPro_AreaKeyHandler$0)  //area needs redraw
	
End if 

C_LONGINT:C283($err; $i; $area)
C_LONGINT:C283($modifiers; $key; $value)
C_BOOLEAN:C305($rebuildList)
C_TEXT:C284($text)

ARRAY LONGINT:C221($aPP_CurrentObjects; 0x0000)

$area:=$1->

$err:=RW_GetProperty($area; $area; "evtK"; ->$key)
$err:=RW_GetProperty($area; $area; "evtC"; ->$text)
$err:=RW_GetProperty($area; $area; "evtM"; ->$modifiers)

Case of 
		
	: (($key=0x0095) | ($key=0x0096))  // BS, delete
		$err:=RW_GetObjects($area; $area; "selc"; $aPP_CurrentObjects)
		
		$err:=RW_Area_SaveUndo($area; 1)  //start undo block
		For ($i; 0x0001; Size of array:C274($aPP_CurrentObjects))
			If (PPro_ConfirmDelete($area; $aPP_CurrentObjects{$i}))
				$err:=RW_DeleteObject($area; $aPP_CurrentObjects{$i})
			End if 
		End for 
		$err:=RW_Area_SaveUndo($area; 0)  //end undo block
		
		$rebuildList:=True:C214
		
	: (($key=0x008E) | ($key=0x008F) | ($key=0x0090) | ($key=0x0091))  // left/right/up/down arrow
		$err:=RW_GetObjects($area; $area; "selc"; $aPP_CurrentObjects)
		
		$err:=RW_Area_SaveUndo($area; 2)  //start undo block
		$value:=0x0001
		
		If (($key=0x008E) | ($key=0x0090))  //??
			$value:=-$value
		End if 
		
		If ($modifiers ?? 0x0009)  // shift
			$value:=10*$value
		End if 
		
		
		If ($key<0x0090)
			Case of 
				: ($modifiers ?? 0x000B)  // option == left side only
					$text:="rpol"
				: ($modifiers ?? 0x0008)  // cmd == right side only
					$text:="rpor"
				Else 
					$text:="movH"  // none == move
			End case 
		Else 
			Case of 
				: ($modifiers ?? 0x000B)  // option == top side only
					$text:="rpot"
				: ($modifiers ?? 0x0008)  // cmd == bottom side only
					$text:="rpob"
				Else 
					$text:="movV"  // none == move
			End case 
		End if 
		
		$err:=RW_SetProperty($area; 0x0000; $text; ->$value)  //apply to all selected
		$err:=RW_Area_SaveUndo($area; 0)
		$rebuildList:=True:C214
		
	: ((($text="+") | ($text="-") | ($text="1") | ($text="=")) & ($modifiers ?? 0x0008))  // cmd
		Case of 
			: ($text="+")
				PPro_ScaleUp($area)
			: ($text="-")
				PPro_ScaleDown($area)
			: ($text="1")
				PPro_ScaleTo($area; 1)
			: ($text="=")
				PPro_ScaleTo($area; 1)
		End case 
		
	: (($text="g") & ($modifiers ?? 0x0008))  // cmd
		PPro_ObjectGroup($area; "")
		$rebuildList:=True:C214
		
	: ((($text="d") | ($text="c") | ($text="x") | ($text="v")) & ($modifiers ?? 0x0008))  // cmd
		PPro_XMLObjects($area; $text)
		$rebuildList:=True:C214
		
	: (($text="i") & ($modifiers ?? 0x0008))  // cmd
		$err:=RW_GetObjects($area; $area; "selc"; $aPP_CurrentObjects)
		If (Size of array:C274($aPP_CurrentObjects)>0x0000)
			$err:=RW_GetObjectXML($area; $aPP_CurrentObjects{0x0001}; $text)
			SET TEXT TO PASTEBOARD:C523($text)
		End if 
		
	: (($text="z") & ($modifiers ?? 0x0008) & ($modifiers ?? 0x0009))  // cmd shift
		$err:=RW_Area_Redo($area)
		If ($err#0x0000)
			BEEP:C151
		Else 
			$rebuildList:=True:C214
		End if 
		
	: (($text="z") & ($modifiers ?? 0x0008))  // cmd
		$err:=RW_Area_Undo($area)
		If ($err#0x0000)
			BEEP:C151
		Else 
			$rebuildList:=True:C214
		End if 
		
	: ($modifiers ?? 0x0008)
		//?? 
End case 

$0:=$rebuildList
