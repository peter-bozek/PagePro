//%attributes = {"invisible":true}
C_LONGINT:C283($err; $i; $id)
C_TEXT:C284($name)
ARRAY LONGINT:C221($alStyles; 0)
$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; PPro_StyleSet:K7905:24; $alStyles)
ARRAY TEXT:C222(PP_Styles; Size of array:C274($alStyles))  //
ARRAY LONGINT:C221(PP_StyleID; Size of array:C274($alStyles))

For ($i; 1; Size of array:C274($alStyles))
	$err:=RW_GetProperty(PP_ExtArea; $alStyles{$i}; PProp_Name:K7906:3; ->$name)
	PP_Styles{$i}:=$name
	$err:=RW_GetProperty(PP_ExtArea; $alStyles{$i}; PProp_ID:K7906:16; ->$id)
	PP_StyleID{$i}:=$id
	
End for 
