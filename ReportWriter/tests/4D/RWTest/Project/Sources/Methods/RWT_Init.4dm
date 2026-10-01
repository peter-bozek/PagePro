//%attributes = {"invisible":true}
// sets the output folder (system path) from --user-param and starts the log

var $param : Text
var $r : Real

$r:=Get database parameter(User param value; $param)
vRWT_Out:=Convert path POSIX to system($param)
USE CHARACTER SET("UTF-8"; 0)
TEXT TO DOCUMENT(vRWT_Out+"log.txt"; ""; "UTF-8")
ON ERR CALL("RWT_OnError")
vRWT_Failures:=0
vRWT_Checks:=0
