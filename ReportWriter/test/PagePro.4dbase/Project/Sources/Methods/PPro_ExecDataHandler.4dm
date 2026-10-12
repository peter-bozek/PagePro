//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($2; $0)

If (False:C215)
	C_TEXT:C284(PPro_ExecDataHandler; $1)
	C_LONGINT:C283(PPro_ExecDataHandler; $2; $0)
End if 

C_TEXT:C284($selector)
$selector:=$1
C_LONGINT:C283($pos)
If (Count parameters:C259>1)
	$pos:=$2
End if 

Case of 
	: ($selector="Init")
		
		ARRAY TEXT:C222(IN4_Line; 0)
		ARRAY TEXT:C222(IN4_Token; 0)
		
		ARRAY LONGINT:C221(IN4_LineIndent; 0)
		ARRAY LONGINT:C221(IN4_CommandOffset; 0)
		ARRAY LONGINT:C221(IN4_ConditionOffset; 0)
		ARRAY LONGINT:C221(IN4_LineIfTrue; 0)
		ARRAY LONGINT:C221(IN4_LineIfFalse; 0)
		
		ARRAY LONGINT:C221(IN4_Position; 0)
		
		C_LONGINT:C283(IN4_NextLevel)
		IN4_NextLevel:=0
		
		C_BLOB:C604(IN4_ObjectBlob)
		SET BLOB SIZE:C606(IN4_ObjectBlob; 0)
		LONGINT TO BLOB:C550(0xDEADBE01; IN4_ObjectBlob; Native byte ordering:K22:1)
		
		ARRAY LONGINT:C221(IN4_ForStack; 256)
		IN4_ForStack:=0
		
	: ($selector="Append")
		C_LONGINT:C283($j)
		$j:=Size of array:C274(IN4_Line)+1
		PPro_ExecDataHandler("Insert"; $j)
		$0:=Size of array:C274(IN4_Line)
		
	: ($selector="Insert")
		
		INSERT IN ARRAY:C227(IN4_Line; $pos; 1)
		INSERT IN ARRAY:C227(IN4_Token; $pos; 1)
		
		INSERT IN ARRAY:C227(IN4_LineIndent; $pos; 1)
		INSERT IN ARRAY:C227(IN4_CommandOffset; $pos; 1)
		INSERT IN ARRAY:C227(IN4_ConditionOffset; $pos; 1)
		INSERT IN ARRAY:C227(IN4_LineIfTrue; $pos; 1)
		INSERT IN ARRAY:C227(IN4_LineIfFalse; $pos; 1)
		
		INSERT IN ARRAY:C227(IN4_Position; $pos; 1)
		
End case 