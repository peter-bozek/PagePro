//%attributes = {"invisible":true,"shared":true}
C_TEXT:C284($1; $7)
C_LONGINT:C283($2; $3; $4; $5; $6)

If (False:C215)
	C_TEXT:C284(PPro_Palette; $1; $7)
	C_LONGINT:C283(PPro_Palette; $2; $3; $4; $5; $6)
End if 

C_TEXT:C284($sel)
$sel:=$1  // mandatory

Case of 
	: ($sel="open")
		C_LONGINT:C283($left; $top; $right; $bottom)
		GET WINDOW RECT:C443($left; $top; $right; $bottom)
		PP_PaletteProcess:=New process:C317("PPro_Palette"; 512*1024; "PagePro Palette"; "init"; $2; $3; PP_ExtArea; $left; $top; PP_DefaultMenu; *)
		
	: ($sel="load")
		Compiler_PProVariables
		
		If ($2#0)
			PP_HierList:=$2
		Else 
			// simple way how to implement palette - by putting predefined file into 
			// file PP_VariableList.xml need to be in Resources folder and will be loaded automatically
			// the root name
			C_TEXT:C284($path)
			$path:=Get 4D folder:C485(Current resources folder:K5:16; *)+"PP_VariableList.xml"
			If (Test path name:C476($path)#Is a document:K24:1)
				$path:=Get 4D folder:C485(Current resources folder:K5:16)+"PP_VariableList.xml"
			End if 
			
			If (Test path name:C476($path)=Is a document:K24:1)
				C_TEXT:C284($root; $content)
				$root:=DOM Parse XML source:C719($path)
				DOM EXPORT TO VAR:C863($root; $content)
				If ($content#"")
					$content:=PPro_Util_Fix4DNode($content)
					C_LONGINT:C283($list)
					PP_HierList:=PPro_LoadVarHList($content)
				End if 
				DOM CLOSE XML:C722($root)
			End if 
			
			
		End if 
		
	: ($sel="init")
		// run inside a process
		
		PP_PaletteProcess:=Current process:C322
		C_TEXT:C284($newMenuReference)
		$newMenuReference:=PPro_LoadMenuBar($7; "PageProMenus.xml")
		If ($newMenuReference#"")
			SET MENU BAR:C67($newMenuReference)
			C_LONGINT:C283($i)
			For ($i; 1; Count menus:C404)
				DISABLE MENU ITEM:C150($i; 0; Current process:C322)
			End for 
		End if 
		
		PPro_Palette("load"; $2; $3)
		C_LONGINT:C283($oleft; $otop; $oright; $obottom)
		RW_GetEditorRect($4; $oleft; $otop; $oright; $obottom)
		C_LONGINT:C283($win)
		$win:=Open window:C153($5+$oleft; $6+$obottom-200; $5+$oleft+300; $6+$obottom; -Palette window:K34:3; Get localized string:C991("PP_PaletteWindow"); "PPro_CloseBox")
		DIALOG:C40("Palette")
		CLOSE WINDOW:C154($win)
		PP_PaletteProcess:=0
		RELEASE MENU:C978($newMenuReference)
		
		
	: ($sel="quit")
		
		<>PP_PaletteMessage:="quit"
		POST OUTSIDE CALL:C329(PP_PaletteProcess)
		
	: ($sel="deactivate")
		
		// If (Frontmost process#PP_PaletteProcess)
		<>PP_PaletteMessage:="quit"
		POST OUTSIDE CALL:C329(PP_PaletteProcess)
		//End if 
		
End case 
