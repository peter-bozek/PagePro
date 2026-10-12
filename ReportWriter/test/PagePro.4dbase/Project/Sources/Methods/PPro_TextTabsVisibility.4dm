//%attributes = {"invisible":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_TextTabsVisibility; $1)
End if 

C_LONGINT:C283($sel)

$sel:=$1

Case of 
	: ($sel=1)
		OBJECT SET VISIBLE:C603(*; "page1@"; True:C214)
		OBJECT SET VISIBLE:C603(*; "page2@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page3@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page4@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page5@"; False:C215)
		
	: ($sel=2)
		OBJECT SET VISIBLE:C603(*; "page1@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page2@"; True:C214)
		OBJECT SET VISIBLE:C603(*; "page3@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page4@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page5@"; False:C215)
		
	: ($sel=3)
		OBJECT SET VISIBLE:C603(*; "page1@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page2@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page3@"; True:C214)
		OBJECT SET VISIBLE:C603(*; "page4@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page5@"; False:C215)
		
	: ($sel=4)
		OBJECT SET VISIBLE:C603(*; "page1@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page2@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page3@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page4@"; True:C214)
		OBJECT SET VISIBLE:C603(*; "page5@"; False:C215)
		
	: ($sel=5)
		OBJECT SET VISIBLE:C603(*; "page1@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page2@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page3@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page4@"; False:C215)
		OBJECT SET VISIBLE:C603(*; "page5@"; True:C214)
End case 