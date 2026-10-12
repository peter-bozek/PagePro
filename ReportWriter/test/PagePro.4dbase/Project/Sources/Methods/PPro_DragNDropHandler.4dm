//%attributes = {"shared":true}
C_REAL:C285($posX; $posY)
C_LONGINT:C283($obj; $err; $locked)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "drpX"; ->$posX)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "drpY"; ->$posY)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "drgo"; ->$obj)

ARRAY TEXT:C222($atSignature; 0)
ARRAY TEXT:C222($atType; 0)
ARRAY TEXT:C222($atName; 0)
C_LONGINT:C283($platform)
_O_PLATFORM PROPERTIES:C365($platform)
If ($platform=Windows:K25:3)
	GET PASTEBOARD DATA TYPE:C958($atSignature; $atType; $atName)
	COPY ARRAY:C226($atName; $atType)
Else 
	GET PASTEBOARD DATA TYPE:C958($atSignature; $atType)
End if 

If (Find in array:C230($atType; "eu.sdsbestatter.drag")>0)
	PPro_BlueHandler($posX; $posY; $obj)
Else 
	If (Find in array:C230($atType; "eu.pagepro4D.drag")>0)
		PPro_NativeDataDrop($posX; $posY; $obj)
	Else 
		If (Find in array:C230($atSignature; "com.4d.private.text.@")>0)
			PPro_TextDataDrop($posX; $posY; $obj)
		Else 
			If (Find in array:C230($atSignature; "com.4d.private.picture.@")>0)
				PPro_PictDataDrop($posX; $posY; $obj)
			Else 
				If (Find in array:C230($atSignature; "com.4d.private.file.@")>0)
					PPro_PictFileDrop($posX; $posY; $obj)
				End if 
			End if 
		End if 
	End if 
End if 