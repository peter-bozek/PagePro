C_REAL:C285($scale)
C_LONGINT:C283($err)
$scale:=(Self:C308->)/100
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; "scal"; ->$scale)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "scal"; ->$scale)
Self:C308->:=Round:C94($scale*100; 0x0002)
