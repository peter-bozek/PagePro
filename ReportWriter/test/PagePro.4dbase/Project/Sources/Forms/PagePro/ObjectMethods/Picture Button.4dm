If (Self:C308->=1)  //unlocked status - lock
	PPro_ObjectLock(PP_ExtArea; "lock"; "")  //lock 
Else 
	PPro_ObjectLock(PP_ExtArea; "unlock"; "sel")  //unlock selection only
End if 