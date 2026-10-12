C_REAL:C285($scale)
C_LONGINT:C283($err; $l)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "scal"; ->$scale)
$l:=(4*$scale-1)
$scale:=$l/4
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; "scal"; ->$scale)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "scal"; ->$scale)
PP_Scaling:=Round:C94($scale*100; 0x0002)