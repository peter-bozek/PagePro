C_BLOB:C604($o)  //empty blob
C_LONGINT:C283($err)

$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_PageFormat:K7906:10; ->$o)
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_PrintSettings:K7906:11; ->$o)
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_DevMode:K7906:12; ->$o)
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_DeviceNames:K7906:13; ->$o)
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_PageSetupDlg:K7906:14; ->$o)
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; PProp_PrintDlg:K7906:15; ->$o)
