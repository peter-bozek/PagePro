//%attributes = {"shared":true}
// PPro_PreviewRW (
//      path;    path to preprint file 
//      flags)     see PagePro Printing flags constants
//    -> error: one of Printing Error Codes
//              or -1 - user cancelled

C_TEXT:C284($1)
C_LONGINT:C283($2; $0)

If (False:C215)
	C_TEXT:C284(PPro_PreviewRW; $1)
	C_LONGINT:C283(PPro_PreviewRW; $2; $0)
End if 

C_TEXT:C284($path)
If (Count parameters:C259>0)
	$path:=$1
End if 

$0:=0

OK:=1
If ($path="")
	C_TIME:C306($hRef)
	$hRef:=Open document:C264(""; "XML"; Get pathname:K24:6)
	If (OK=1)
		$path:=Document
	Else 
		$0:=-1
	End if 
End if 

If (OK=1)
	C_LONGINT:C283($rwDstFlags; $session)
	If (Count parameters:C259>1)
		$rwDstFlags:=$2
	Else 
		$rwDstFlags:=PP_Default:K7907:17
	End if 
	$session:=0
	C_LONGINT:C283($err)
	$err:=RW_Print_RW($path; 1; $rwDstFlags; ""; $session)
	$0:=$err
End if 
