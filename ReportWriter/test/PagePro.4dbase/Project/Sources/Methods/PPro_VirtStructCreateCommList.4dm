//%attributes = {"invisible":true}
C_LONGINT:C283($0)  //returns default list of PP variables

If (False:C215)
	C_LONGINT:C283(PPro_VirtStructCreateCommList; $0)
End if 

C_LONGINT:C283($hList; $hSubList)

$hList:=New list:C375

C_LONGINT:C283($command; $i)
C_TEXT:C284($name)

For ($i; 1; 1200)
	$command:=$i
	$name:=Command name:C538($command)
	Case of 
		: ($name="")
			
		: (Character code:C91($name[[1]])=Character code:C91("@"))
			
		: (Character code:C91($name[[1]])=Character code:C91("_"))
			
		Else 
			APPEND TO LIST:C376($hList; $name; $i)
	End case 
End for 
SORT LIST:C391($hList)
$0:=$hList
