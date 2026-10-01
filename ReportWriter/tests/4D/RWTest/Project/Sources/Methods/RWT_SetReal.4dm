//%attributes = {"invisible":true}
// RWT_SetReal (report; object; property; value) -> error
#DECLARE($ref : Integer; $obj : Integer; $prop : Text; $value : Real)->$err : Integer
$err:=RW_SetProperty($ref; $obj; $prop; ->$value)
