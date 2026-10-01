//%attributes = {"invisible":true}
// RWT_SetText (report; object; property; text) -> error
#DECLARE($ref : Integer; $obj : Integer; $prop : Text; $value : Text)->$err : Integer
$err:=RW_SetProperty($ref; $obj; $prop; ->$value)
