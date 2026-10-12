
Case of 
	: (Form event code:C388=On Bound Variable Change:K2:52)
		PPro_GroupProperties("get")
		
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Clicked:K2:4))
		PPro_GroupProperties("set")
		
End case 