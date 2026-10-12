//%attributes = {"shared":true}
C_POINTER:C301($1)  //area
C_BOOLEAN:C305($0)  //area needs redraw

If (False:C215)
	C_POINTER:C301(PPro_AreaMouseHandler; $1)  //area
	C_BOOLEAN:C305(PPro_AreaMouseHandler$0)  //area needs redraw
	
End if 

C_LONGINT:C283($err; $i; $style; $value)
C_LONGINT:C283($mouseH; $mouseV; $modifiers; $area)
C_BOOLEAN:C305($doubleClick; $rebuildList; $b; $bProperty)
C_TEXT:C284($text)

$area:=$1->
$rebuildList:=True:C214

$err:=RW_GetProperty($area; $area; "evtH"; ->$mouseH)
$err:=RW_GetProperty($area; $area; "evtV"; ->$mouseV)
$err:=RW_GetProperty($area; $area; "evtD"; ->$doubleClick)
$err:=RW_GetProperty($area; $area; "evtM"; ->$modifiers)
$err:=RW_GetProperty($area; $area; "hito"; ->PP_CurrentObject)  // last object hit
$err:=RW_GetObjects($area; $area; "selc"; PP_ItemsToSelect)

If (PP_CurrentObject=0x0000)  // if no current object
	$doubleClick:=False:C215
End if 

PP_LockProportions:=0
If (($doubleClick=False:C215) & (PP_CurrentObject#0x0000))
	$err:=RW_GetProperty($area; $area; "creo"; ->$doubleClick)  // last object hit
	
	If ($doubleClick)
		
		// new object created - handle initialisation of default properties
		PPro_NewObjectProperties($area; PP_CurrentObject)
		// allow callback to modify properties of new object
		$b:=PPro_EditorCallback("object"; "created"; $area; PP_CurrentObject)
	Else 
		// v 1.2.2
		// PP_LockProportions is set for newly selected object
		$err:=RW_GetProperty($area; PP_CurrentObject; "kind"; ->$text)
		If (($text="Pict") | ($text="Rect") | ($text="Oval"))
			PP_LockProportions:=1
		End if 
		// pB 2012-8-26 disallow movement to negative coordinates
		// if applicable
		If (($text#"Guide") & ($text#"Section") & ($text#"Header") & ($text#"Column"))
			C_TEXT:C284($s)
			C_LONGINT:C283($left; $top; $right; $bottom; $width; $height; $parent)
			$err:=RW_GetParent($area; PP_CurrentObject; $parent)
			$err:=RW_GetProperty($area; $parent; PProp_Width:K7906:6; ->$width)
			$err:=RW_GetProperty($area; $parent; PProp_Height:K7906:7; ->$height)
			
			$s:=PPro_GetPropertyString($area; PP_CurrentObject; PProp_Rect:K7906:22)
			PPro_Util_RectToCoord($s; ->$left; ->$top; ->$right; ->$bottom)
			
			If (($bottom>$height) & ($height>0))
				$bottom:=$height-$bottom
				$err:=RW_SetProperty($area; PP_CurrentObject; "movV"; ->$bottom)
				$top:=$top+$bottom
			End if 
			If (($right>$width) & ($width>0))
				$right:=$width-$right
				$err:=RW_SetProperty($area; PP_CurrentObject; "movH"; ->$right)
				$left:=$left-$right
			End if 
			If ($top<0)
				$top:=-$top
				$err:=RW_SetProperty($area; PP_CurrentObject; "movV"; ->$top)
			End if 
			If ($left<0)
				$left:=-$left
				$err:=RW_SetProperty($area; PP_CurrentObject; "movH"; ->$left)
			End if 
		End if   // position fix
	End if 
End if 

// if created or double clicked, edit
If ($doubleClick)
	$rebuildList:=True:C214
	If (PPro_EditorCallback("edit"; "start"; $area; PP_CurrentObject))
		PPro_SubformVisibility($area)
		
		$err:=RW_GetProperty($area; PP_CurrentObject; "kind"; ->$text)
		Case of 
			: ($text="Text")
				PPro_TextHandleEditing($1; $area; PP_CurrentObject)
			: ($text="Var")
				PPro_TextHandleEditing($1; $area; PP_CurrentObject)
			: ($text="Field")
				PPro_TextHandleEditing($1; $area; PP_CurrentObject)
			: ($text="Header")
				PPro_TextHandleEditing($1; $area; PP_CurrentObject)
			: ($text="Column")
				PPro_TextHandleEditing($1; $area; PP_CurrentObject)
			: ($text="Pict")
				C_PICTURE:C286($pict)
				C_BLOB:C604($blob)
				
				READ PICTURE FILE:C678(""; $pict)
				If (OK=1)
					VARIABLE TO BLOB:C532($pict; $blob)
					$err:=RW_SetProperty($area; PP_CurrentObject; "data"; ->$blob)
				End if 
				PP_LockProportions:=1
				
		End case 
		$b:=PPro_EditorCallback("edit"; "end"; $area; PP_CurrentObject)
	End if 
	$bProperty:=False:C215
	$err:=RW_SetProperty($area; $area; "creo"; ->$bProperty)  // clear flag that new object was created
	
Else 
	// v1.4  - 4D no longer sets the edit menu correctly
	ENABLE MENU ITEM:C149(2; 0; Current process:C322)
End if 

$0:=$rebuildList