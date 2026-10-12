//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area reference containing report
C_LONGINT:C283($2)  //flag - 0 create new file, 1 - use reference stored in Document variable
C_BOOLEAN:C305($0)  //success
If (False:C215)
	C_LONGINT:C283(PPro_SaveReport; $1)
	C_LONGINT:C283(PPro_SaveReport; $2)
	C_BOOLEAN:C305(PPro_SaveReport; $0)
End if 

C_BOOLEAN:C305($b)

$0:=False:C215
$b:=True:C214

If ($2=0)  //Save As
	C_TIME:C306($docRef)
	$docRef:=Create document:C266(""; ".xml")
	$b:=(OK=0x0001)
	CLOSE DOCUMENT:C267($docRef)
Else 
	If (Test path name:C476(PP_DocumentPath)=Is a document:K24:1)
		Document:=PP_DocumentPath  // we already have a path
	Else 
		$docRef:=Create document:C266(""; ".xml")
		$b:=(OK=0x0001)
		CLOSE DOCUMENT:C267($docRef)
	End if 
End if 
If ($b)
	C_TEXT:C284($path)
	$path:=Document
	C_LONGINT:C283($err)
	If (RW_SaveReport($1; $path; 0x0001)=0)  // third arg means it is a path
		PP_DocumentPath:=$path
		_O_SET DOCUMENT CREATOR:C531($path; "????")
		
		C_TEXT:C284($report)
		$err:=RW_SaveReport(PP_ExtArea; $report; 0x0000)  // third arg means it is a variable
		PP_Report:=$report
	End if 
End if 