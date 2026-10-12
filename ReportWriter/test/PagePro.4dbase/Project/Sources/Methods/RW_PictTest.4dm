//%attributes = {}
// RW_PictTest: pictures from 4D variables and an array, to PDF and to preview
var $ref; $body; $obj; $err; $flags : Integer
var $xml; $src; $out : Text
var vRWPng; vRWJpg; vRWSvg; vRWGif : Picture
ARRAY PICTURE:C279(aRWPict; 2)
ARRAY TEXT:C222($srcs; 0)

READ PICTURE FILE:C678(""; vRWPng)  // choose a .png
READ PICTURE FILE:C678(""; vRWJpg)  // choose a .jpg
READ PICTURE FILE:C678(""; vRWSvg)  // choose an .svg
READ PICTURE FILE:C678(""; vRWGif)  // choose a .gif
aRWPict{1}:=vRWSvg
aRWPict{2}:=vRWPng

$err:=RW_NewReport($ref; ""; 0)
$err:=RW_NewObject($ref; $body; "body"; $ref)
$value:=700
$err:=RW_SetProperty($ref; $body; "high"; ->$value)

APPEND TO ARRAY:C911($srcs; "vRWPng")
APPEND TO ARRAY:C911($srcs; "vRWJpg")
APPEND TO ARRAY:C911($srcs; "vRWSvg")
APPEND TO ARRAY:C911($srcs; "vRWGif")
APPEND TO ARRAY:C911($srcs; "aRWPict")  // element 1
APPEND TO ARRAY:C911($srcs; "aRWPict")  // element 2

var $i : Integer
var $rect : Text
var $value : Real
For ($i; 1; Size of array:C274($srcs))
	$err:=RW_NewObject($ref; $obj; "VARI"; $body)
	$src:=$srcs{$i}
	$err:=RW_SetProperty($ref; $obj; " src"; ->$src)
	$rect:=String:C10(20+((($i-1)%2)*270))+";"+String:C10(10+(($i-1)\2)*230)+";"+String:C10(270+((($i-1)%2)*270))+";"+String:C10(230+(($i-1)\2)*230)
	$err:=RW_SetProperty($ref; $obj; "rect"; ->$rect)
	If ($i>=5)
		$value:=$i-4
		$err:=RW_SetProperty($ref; $obj; "elem"; ->$value)
	End if 
End for 

$err:=RW_SaveReport($ref; $xml; 0)
$out:=System folder:C487(Desktop:K41:16)+"rw_pict.pdf"
$flags:=4+0x0020+0x0200+0x4000  // PDF, default page/job setup, no progress
$err:=RW_Print($xml; 0; $flags; $out; 0; "")
ALERT:C41("PDF: "+String:C10($err)+" → "+$out)
$flags:=3+0x0020+0x0200+0x4000  // preview (Mac print path)
$err:=RW_Print($xml; 0; $flags; ""; 0; "")