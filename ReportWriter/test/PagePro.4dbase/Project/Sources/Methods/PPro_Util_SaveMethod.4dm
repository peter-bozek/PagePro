//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_Util_SaveMethod; $1)
End if 

C_TEXT:C284($file)
$file:=Replace string:C233($1; ":"; "_")

C_TEXT:C284($method)
GET MACRO PARAMETER:C997(Full method text:K5:17; $method)
C_BLOB:C604($output)
TEXT TO BLOB:C554($method; $output; UTF8 C string:K22:15; *)

C_TEXT:C284($path)
$path:=Get 4D folder:C485(Database folder:K5:14)+"PagePro_Methods"+Folder separator:K24:12
If (Test path name:C476($path)=Is a folder:K24:2)
	$path:=$path+$file+".txt"
	
	BLOB TO DOCUMENT:C526($path; $output)
	_O_SET DOCUMENT CREATOR:C531($path; "????")
	_O_SET DOCUMENT TYPE:C530($path; "text")
End if 
