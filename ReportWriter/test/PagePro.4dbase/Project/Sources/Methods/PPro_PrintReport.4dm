//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //report
C_LONGINT:C283($2)  //flags
C_TEXT:C284($3)  //destination path for PDF generation

If (False:C215)
	C_LONGINT:C283(PPro_PrintReport; $1)
	C_LONGINT:C283(PPro_PrintReport; $2)
	C_TEXT:C284(PPro_PrintReport; $3)  //
End if 


C_LONGINT:C283($rwSrcFlags; $rwDstFlags)

If ($2=-1)
	$rwDstFlags:=PP_DestinationPrinter:K7907:1 | PP_AskPageSetup:K7907:12 | PP_AskJobSetup:K7907:13
Else 
	$rwDstFlags:=$2  // destination processing
End if 

If (Count parameters:C259<3)
	$destination:=""
Else 
	$destination:=$3
End if 
$session:=0

C_TEXT:C284($path; $destination; $document)
C_LONGINT:C283($err; $session)
If ($err=0x0000)
	
	If (Shift down:C543)
		// special case - process RW file, then print RW file
		C_TEXT:C284($temp)
		$path:=Temporary folder:C486+"_PProCurrent_.xml"
		$err:=RW_SaveReport($1; $path; 0x0001)  // save it into file
		$temp:=Temporary folder:C486+"_PProCurrent_.rw"
		$rwSrcFlags:=0x0001  // source processing - 1 means it is document
		$err:=RW_Process_RW($path; $rwSrcFlags; $temp)
		$err:=RW_Print_RW($temp; 1; $rwDstFlags; $destination; $session)
	Else 
		$err:=RW_SaveReport($1; $document; 0x0000)  // save it into text variable
		$err:=RW_OpenSession($session; $rwDstFlags; $destination; $document; ""; "")
		If ($err=0)
			$rwSrcFlags:=0x0000  // 0 means it is text variable
			$err:=RW_Print($document; $rwSrcFlags; $rwDstFlags; $destination; $session)
			$err:=RW_CloseSession($session)
		End if 
	End if 
	
End if 