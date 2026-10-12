//%attributes = {"invisible":true}
// Pictures from 4D picture variables and a picture array, printed to PDF:
// tests/4D/run_4d_tests.sh <bundle> <arch> <out> RWT_Pict
// The plugin writes "RW pict: ..." diagnostics to stdout (4d_stdout.txt).

var $ref; $body; $obj; $err; $flags; $i : Integer
var $xml; $src; $rect; $folder; $name : Text
var $value : Real
var vRWPng; vRWJpg; vRWSvg; vRWGif : Picture
ARRAY PICTURE(aRWPict; 2)
ARRAY TEXT($srcs; 0)

RWT_Declare
RWT_Init
TEXT TO DOCUMENT(vRWT_Out+"results.txt"; ""; "UTF-8-no-bom")
RWT_Result("version"; RW_GetVersion)
$err:=RW_Register("TEST")

$folder:=Get 4D folder(Current resources folder)+"pictures"+Folder separator
READ PICTURE FILE($folder+"test.png"; vRWPng)
READ PICTURE FILE($folder+"test.jpg"; vRWJpg)
READ PICTURE FILE($folder+"test.svg"; vRWSvg)
READ PICTURE FILE($folder+"test.gif"; vRWGif)
aRWPict{1}:=vRWSvg
aRWPict{2}:=vRWPng
RWT_Result("pict.read"; String(Picture size(vRWPng))+" "+String(Picture size(vRWJpg))+" "+String(Picture size(vRWSvg))+" "+String(Picture size(vRWGif)))

$err:=RW_NewReport($ref; ""; 0)
$err:=RW_NewObject($ref; $body; "body"; $ref)
RWT_Result("pict.body"; String($err)+" "+String(RWT_SetReal($ref; $body; "high"; 400)))

APPEND TO ARRAY($srcs; "vRWPng")
APPEND TO ARRAY($srcs; "vRWJpg")
APPEND TO ARRAY($srcs; "vRWSvg")
APPEND TO ARRAY($srcs; "vRWGif")
APPEND TO ARRAY($srcs; "aRWPict")  // element 1 (SVG)
APPEND TO ARRAY($srcs; "aRWPict")  // element 2 (PNG)

For ($i; 1; Size of array($srcs))
	$err:=RW_NewObject($ref; $obj; "VARI"; $body)
	// 4D has no operator precedence (left to right): parenthesize the products
	$rect:=String(20+((($i-1)%2)*200))+";"+String(10+((($i-1)\2)*130))+";"+String(200+((($i-1)%2)*200))+";"+String(140+((($i-1)\2)*130))
	$name:="pict."+String($i)+" "+$srcs{$i}
	If ($i>=5)
		RWT_Result($name; String($err)+" "+String(RWT_SetText($ref; $obj; " src"; $srcs{$i}))+" "+String(RWT_SetText($ref; $obj; "rect"; $rect))+" "+String(RWT_SetReal($ref; $obj; "elem"; $i-4)))
	Else
		RWT_Result($name; String($err)+" "+String(RWT_SetText($ref; $obj; " src"; $srcs{$i}))+" "+String(RWT_SetText($ref; $obj; "rect"; $rect)))
	End if
End for

$err:=RW_SaveReport($ref; $xml; 0)
RWT_Result("pict.save"; String($err))
RWT_Save("pict_report.xml"; $xml)

// PDF (4) + default page / job setup, no progress; 0x10000000 keeps the processed report (.rwxml)
$flags:=4+0x0020+0x0200+0x4000+0x10000000
RWT_Log("printing pict.pdf")
$err:=RW_Print($xml; 0; $flags; vRWT_Out+"pict.pdf"; 0; "")
RWT_Result("pict.pdf"; String($err)+" "+String(Num(Test path name(vRWT_Out+"pict.pdf")=Is a document)))

// second run: pictures fetched again after the first report released them
RWT_Log("printing pict2.pdf")
$err:=RW_Print($xml; 0; $flags; vRWT_Out+"pict2.pdf"; 0; "")
RWT_Result("pict2.pdf"; String($err)+" "+String(Num(Test path name(vRWT_Out+"pict2.pdf")=Is a document)))
RWT_Result("pict.after"; String(Picture size(vRWPng))+" "+String(Picture size(aRWPict{1}))+" "+String(Picture size(aRWPict{2})))

$err:=RW_DeleteReport($ref)
RWT_Result("4D errors"; String(vRWT_Errors))
RWT_Log("done")
QUIT 4D
