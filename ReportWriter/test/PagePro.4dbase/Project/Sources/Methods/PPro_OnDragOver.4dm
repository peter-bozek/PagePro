//%attributes = {"shared":true}
C_LONGINT:C283($0)


If (False:C215)
	C_LONGINT:C283(PPro_OnDragOver; $0)
End if 

ARRAY TEXT:C222($atSignature; 0)
ARRAY TEXT:C222($atType; 0)
GET PASTEBOARD DATA TYPE:C958($atSignature; $atType)
Case of 
	: (Find in array:C230($atSignature; "eu.pagepro4D.drag")>0)
		$0:=0
	: (Find in array:C230($atSignature; "com.4d.private.text.@")>0)
		$0:=0
	: (Find in array:C230($atSignature; "com.4d.private.picture.@")>0)
		$0:=0
		
	Else 
		$0:=-1
End case 