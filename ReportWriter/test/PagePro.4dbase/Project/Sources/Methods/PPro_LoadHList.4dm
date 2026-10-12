//%attributes = {"invisible":true}
C_LONGINT:C283($0)
C_TEXT:C284($1)
C_TEXT:C284($2)

If (False:C215)
	C_LONGINT:C283(PPro_LoadHList; $0)
	C_TEXT:C284(PPro_LoadHList; $1)  //root (menu bar)
	C_TEXT:C284(PPro_LoadHList; $2)  //file name
End if 

C_LONGINT:C283($list)
C_TEXT:C284($root; $ID; $path)

$list:=New list:C375
$ID:=$1

If (Count parameters:C259>=2)
	$path:=Get 4D folder:C485(Current resources folder:K5:16)+$2
Else 
	$path:=Get 4D folder:C485(Current resources folder:K5:16)+"menus.xml"
End if 

If (Test path name:C476($path)=Is a document:K24:1)
	$root:=DOM Parse XML source:C719($path)
	If (OK=1)
		$root:=DOM Find XML element by ID:C1010($root; $ID)
		If (OK=1)
			C_LONGINT:C283($last)
			$last:=PPro_LoadList($list; $root; 0)
		End if 
	End if 
End if 

$0:=$list