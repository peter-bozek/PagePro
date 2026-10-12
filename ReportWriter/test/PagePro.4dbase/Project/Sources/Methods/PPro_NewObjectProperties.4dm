//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)

// method is called when new object is created
// it sets default properties of new object

If (False:C215)
	C_LONGINT:C283(PPro_NewObjectProperties; $1; $2)
End if 

C_LONGINT:C283($area; $object; $value; $err)
$area:=$1
$object:=$2

If ($object#0)
	// new object created-handle initialisation of default properties
	C_TEXT:C284($text)
	$err:=RW_GetProperty($area; $object; "kind"; ->$text)
	
	// objects that contain texts are by default
	// attributed and wrapped
	If (($text="Text") | ($text="Field") | ($text="Var"))
		$value:=1
		$err:=RW_SetProperty($area; $object; PProp_TextWrap:K7906:75; ->$value)
		$value:=0
		$err:=RW_SetProperty($area; $object; PProp_Frame:K7906:42; ->$value)
		
		// set minimal size to something meaningful
		C_REAL:C285($font; $dim)
		$err:=RW_GetProperty($area; $object; PProp_FontSize:K7906:67; ->$font)
		$err:=RW_GetProperty($area; $object; PProp_ObjectWidth:K7906:27; ->$dim)
		If ($dim<$font)
			$err:=RW_SetProperty($area; $object; PProp_ObjectWidth:K7906:27; ->$font)
		End if 
		$font:=Round:C94($font*1.2+0.49; 0)
		$err:=RW_GetProperty($area; $object; PProp_ObjectHeight:K7906:28; ->$dim)
		$dim:=$font*Round:C94($dim/$font+0.49; 0)
		$err:=RW_SetProperty($area; $object; PProp_ObjectHeight:K7906:28; ->$dim)
	Else 
		If ($text#"Line")
			// set minimal size to 6 x 6
			// thisis to avoid situation when user creates new object with very small dimension(like 1 x 1)
			C_REAL:C285($dim)
			$err:=RW_GetProperty($area; $object; PProp_ObjectWidth:K7906:27; ->$dim)
			If ($dim<6)
				$dim:=6
				$err:=RW_SetProperty($area; $object; PProp_ObjectWidth:K7906:27; ->$dim)
			End if 
			$err:=RW_GetProperty($area; $object; PProp_ObjectHeight:K7906:28; ->$dim)
			If ($dim<6)
				$dim:=6
				$err:=RW_SetProperty($area; $object; PProp_ObjectHeight:K7906:28; ->$dim)
			End if 
		End if 
	End if 
	
	// specific properties for each object type
	Case of 
		: ($text="Text")
			$value:=1
			$err:=RW_SetProperty($area; $object; PProp_DynamicText:K7906:47; ->$value)
			$value:=0
			$err:=RW_SetProperty($area; $object; PProp_AttributedText:K7906:48; ->$value)
			
		: ($text="Field")  // no additional processing for field and variable
			$value:=0
			$err:=RW_SetProperty($area; $object; PProp_DynamicText:K7906:47; ->$value)
			$err:=RW_SetProperty($area; $object; PProp_AttributedText:K7906:48; ->$value)
			
		: ($text="Var")
			$value:=0
			$err:=RW_SetProperty($area; $object; PProp_DynamicText:K7906:47; ->$value)
			$err:=RW_SetProperty($area; $object; PProp_AttributedText:K7906:48; ->$value)
			
	End case 
End if 