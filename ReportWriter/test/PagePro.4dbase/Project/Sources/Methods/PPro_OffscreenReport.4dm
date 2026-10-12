//%attributes = {"invisible":true}
// wrapper around RW_NewReport call
//      

C_LONGINT:C283($2; $0)
C_TEXT:C284($1)

If (False:C215)
	C_LONGINT:C283(PPro_OffscreenReport; $2; $0)
	C_TEXT:C284(PPro_OffscreenReport; $1)
End if 

C_LONGINT:C283($area; $err)
$area:=0
$err:=RW_NewReport($area; $1; $2)  // 

$0:=$area