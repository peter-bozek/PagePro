//%attributes = {"invisible":true}
// ON ERR CALL handler: records 4D errors in the log instead of stopping the run
var $codes : Collection
var $i : Integer
ARRAY LONGINT($errCodes; 0)
ARRAY TEXT($components; 0)
ARRAY TEXT($texts; 0)
GET LAST ERROR STACK($errCodes; $components; $texts)
vRWT_Errors:=vRWT_Errors+1
For ($i; 1; Size of array($errCodes))
	RWT_Log("4D ERROR "+String($errCodes{$i})+" ("+$components{$i}+"): "+$texts{$i}+" in "+Error method+" line "+String(Error line))
End for
