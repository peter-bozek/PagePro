//%attributes = {"invisible":true}
// RWT_SetBool (report; object; property; value) -> error
#DECLARE($ref : Integer; $obj : Integer; $prop : Text; $value : Boolean)->$err : Integer
$err:=RW_SetProperty($ref; $obj; $prop; ->$value)
