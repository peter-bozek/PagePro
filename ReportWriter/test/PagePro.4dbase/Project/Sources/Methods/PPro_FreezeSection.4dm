//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)

If (False:C215)
	C_LONGINT:C283(PPro_FreezeSection; $1; $2)
End if 

C_LONGINT:C283($area; ; $section; $i; $err)
$area:=$1
$section:=$2
C_TEXT:C284($type; $value; $text)

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($area; $section; "objs"; $alObjects)
For ($i; 1; Size of array:C274($alObjects))
	$err:=RW_GetProperty($area; $alObjects{$i}; PProp_Kind:K7906:2; ->$type)
	Case of 
		: ($type=PPro_Group:K7905:1)
			PPro_FreezeSection($area; $alObjects{$i})
		: ($type=PPro_Text:K7905:6)
			$err:=RW_GetProperty($area; $alObjects{$i}; "text"; ->$value)
			$err:=RW_SetProperty($area; $alObjects{$i}; PProp_Data:K7906:53; ->$value)
		: (($type="var") | ($type="field"))
			$err:=RW_GetProperty($area; $alObjects{$i}; "text"; ->$value)
			$err:=RW_GetObjectXML($area; $alObjects{$i}; $text)
			$err:=RW_DeleteObject($area; $alObjects{$i})
			$text:=Replace string:C233($text; "<Var"; "<Text")
			$text:=Replace string:C233($text; "Var>"; "Text>")
			$text:=Replace string:C233($text; "<Field"; "<Text")
			$text:=Replace string:C233($text; "Field>"; "Text>")
			$err:=RW_NewObjectFromXML($area; $alObjects{$i}; $text; $section)
			$err:=RW_SetProperty($area; $alObjects{$i}; PProp_Data:K7906:53; ->$value)
			
		Else 
			// other objects are left as they are
	End case 
End for 