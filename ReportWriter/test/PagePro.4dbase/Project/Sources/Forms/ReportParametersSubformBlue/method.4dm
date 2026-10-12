
Case of 
	: (Form event code:C388=On Bound Variable Change:K2:52)
		PPro_ReportProperties("get")
		
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Clicked:K2:4))
		PPro_ReportProperties("set")
		
End case 