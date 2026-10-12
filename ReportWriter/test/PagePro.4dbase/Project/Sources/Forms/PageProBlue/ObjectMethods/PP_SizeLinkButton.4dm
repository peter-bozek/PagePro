// PageProBlue.PP_SizeLinkButton
// 

// created 13.06.2012/13:47/SDS Software GmbH/Frank
// modifed

// —— Declaration ———————————————

// —— Initialisation ———————————————

// —— Main ———————————————

Case of 
	: (Form event code:C388=On Load:K2:1)
		
		If (PP_LockProportions=1)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; True:C214)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; False:C215)
		Else 
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; False:C215)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; True:C214)
		End if 
		
		
	: (Form event code:C388=On Clicked:K2:4)
		
		PP_LockProportions:=1-PP_LockProportions
		
		If (PP_LockProportions=1)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; True:C214)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; False:C215)
		Else 
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; False:C215)
			OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; True:C214)
		End if 
		
		
End case 
