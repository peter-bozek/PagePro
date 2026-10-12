//%attributes = {"invisible":true}
C_TEXT:C284($0)
C_TEXT:C284($1)
C_TEXT:C284($2)

If (False:C215)
	C_TEXT:C284(PPro_LoadMenuBar; $0)
	C_TEXT:C284(PPro_LoadMenuBar; $1)  //root (menu bar)
	C_TEXT:C284(PPro_LoadMenuBar; $2)  //file name
End if 

C_TEXT:C284($menuBar; $root; $menuRoot; $ID; $path)

$menuBar:=Create menu:C408
$ID:=$1

If (Count parameters:C259>=2)
	$path:=Get 4D folder:C485(Current resources folder:K5:16)+$2
Else 
	$path:=Get 4D folder:C485(Current resources folder:K5:16)+"menus.xml"
End if 

If (Test path name:C476($path)=Is a document:K24:1)
	$root:=DOM Parse XML source:C719($path)
	If (OK=1)
		
		$menuRoot:=DOM Find XML element by ID:C1010($root; $ID)
		
		If (OK=1)
			PPro_LoadMenu($menuBar; $menuRoot)
		Else 
			TRACE:C157
		End if 
		
		DOM CLOSE XML:C722($root)
		
	Else 
		TRACE:C157
	End if 
	
Else 
	TRACE:C157
End if 

$0:=$menuBar
