//%attributes = {"invisible":true}
// RWT_Save (file; text): writes a result file (UTF-8, no BOM)
#DECLARE($file : Text; $text : Text)
TEXT TO DOCUMENT(vRWT_Out+$file; $text; "UTF-8-no-bom")
