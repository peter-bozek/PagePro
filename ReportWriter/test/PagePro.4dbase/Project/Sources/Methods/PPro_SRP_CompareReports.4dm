//%attributes = {"shared":true}
// As the method contains call to SuperReportPro and the SRPro Plugin is not part of the PagePro package
// text of the method is included in method comment  
// copy the method - or its parts - from method comment to main database
//
// This methods allows to compare SRP and PP reports
// It accepts SRP report - either as SRP external area reference in $1 or SRP report as blob in $3
// $2 is a name of report - arbitrary string
// The method 
// 1. Converts SRP report to PagePro report
// 2. Creates folder with name "PageProConversionTest" next to a database structure
// 3. Saves PagePro report there
// 4  Saves a SRP report there
// 5. Creates PDF file with PagePro output 
// 6. On Mac, creates PDF file with SRP print output


// copy the method - or its parts - from method comment to main database

// lines (45,114,115) that call SuperReport Pro are commented out - if installed into 
// structure where SRP is installed, remove comments
// if this method is used to convert reports, SRP calls can stay commented out


C_LONGINT:C283($1)  //SuperReportPro external area reference
C_TEXT:C284($2)  // name of report
C_BLOB:C604($3)  // blob with SRP report - if $1 = 0

If (False:C215)
	C_LONGINT:C283(PPro_SRP_CompareReports; $1)  //SuperReportPro external area reference
	C_TEXT:C284(PPro_SRP_CompareReports; $2)  // name of report
	C_BLOB:C604(PPro_SRP_CompareReports; $3)  // blob with SRP report - if $1 = 0
End if 

C_LONGINT:C283($vSRPArea)
C_TEXT:C284($name)
C_BLOB:C604($oSRPReport)

$vSRPArea:=$1
$name:=$2

C_BOOLEAN:C305($reportOK)
$reportOK:=False:C215
If ($vSRPArea#0)
	// following line contain call to SuperReportPro
	//$reportOK:=(SR Get Area($vSRPArea;$oSRPReport)=0)  // <- uncomment
Else 
	If (Count parameters:C259>2)
		$oSRPReport:=$3
		$reportOK:=(BLOB size:C605($oSRPReport)>0)
	End if 
End if 

If ($reportOK)
	
	C_TEXT:C284($delimiter)
	C_LONGINT:C283($platform)
	_O_PLATFORM PROPERTIES:C365($platform)
	If ($platform=Windows:K25:3)
		$delimiter:="\\"
	Else 
		$delimiter:=":"
	End if 
	
	C_BLOB:C604($oPageProRep)
	//$err:=SR Convert Report ($oSRPReport;$oPageProRep;$name)
	C_LONGINT:C283($err)
	If ($err=0x0000)
		C_TEXT:C284($tPageProRep)
		$tPageProRep:=BLOB to text:C555($oPageProRep; UTF8 text without length:K22:17)
		
		C_TEXT:C284($path)
		$path:=Get 4D folder:C485(Database folder:K5:14)
		C_LONGINT:C283($charIdx)
		For ($charIdx; Length:C16($path)-1; 1; -1)
			If ($path[[$charIdx]]=$delimiter)
				$path:=Substring:C12($path; 1; $charIdx)
				$charIdx:=1
			End if 
		End for 
		
		// here is created fodler "PageProConversionTest"
		$path:=$path+"PageProConversionTest"+$delimiter
		If (Test path name:C476($path)#Is a folder:K24:2)
			CREATE FOLDER:C475($path)
		End if 
		
		// PagePro document is written
		BLOB TO DOCUMENT:C526($path+$name+"_PP.xml"; $oPageProRep)
		If ($platform#Windows:K25:3)
			_O_SET DOCUMENT TYPE:C530($path+$name+"_PP.xml"; "TEXT")
			_O_SET DOCUMENT CREATOR:C531($path+$name+"_PP.xml"; "????")
		End if 
		
		// PagePro preprint file is created and printed to PDF
		C_LONGINT:C283($rwSrcFlags; $rwDstFlags)
		$rwSrcFlags:=0x0001  // source processing - 1 means path
		$rwDstFlags:=PP_DestinationPDF:K7907:5  // destination processing - PDF file on both platforms
		$err:=RW_Process_RW($path+$name+"_PP.xml"; $rwSrcFlags; $path+$name+"_RW.XML")
		$err:=RW_Print_RW($path+$name+"_RW.xml"; $rwSrcFlags; $rwDstFlags; $path+$name+"_PP.PDF")
		
	End if 
	
	// SRP document is written
	BLOB TO DOCUMENT:C526($path+$name+".srp"; $oSRPReport)
	_O_SET DOCUMENT CREATOR:C531($path+$name+".srp"; "????")
	
	// on Mac SuperReportPro document is printed to PDF
	// SuperReport Pro cannot print to PDF on windows
	If ($platform#Windows:K25:3)
		// ***************   Read this     ****************
		// following lines contain call to SuperReportPro
		// they are commented out in component as component does not contain 
		
		//$err:=SR SetDestination(SR PrintDestination File;$path+$name+"_SRP.pdf")  // <- uncomment
		//$err:=SR Print Report($oSRPReport;SR Print Option No Progress;SR Print Defined Sections)  // <- uncomment
	End if 
	
	
End if 