//%attributes = {"invisible":true}
C_LONGINT:C283($idx)
C_LONGINT:C283($level)
C_LONGINT:C283($next)

For ($idx; 1; Size of array:C274(IN4_Line))
	$level:=IN4_LineIndent{$idx}
	Case of 
		: (IN4_Token{$idx}="if")
			IN4_LineIfTrue{$idx}:=$idx+1
			$next:=Find in array:C230(IN4_LineIndent; $level; $idx+1)
			If ($next>0)
				IN4_LineIfFalse{$idx}:=$next+1
			Else 
				IN4_LineIfFalse{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="else")
			$next:=Find in array:C230(IN4_LineIndent; $level; $idx+1)
			If ($next>0)
				IN4_LineIfTrue{$idx}:=$next+1
			Else 
				IN4_LineIfTrue{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="until")
			IN4_LineIfFalse{$idx}:=$idx+1
			$next:=PPro_ExecFindBackInArray($level; $idx-1)
			If ($next>0)
				IN4_LineIfTrue{$idx}:=$next+1
			Else 
				IN4_LineIfTrue{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="while")
			IN4_LineIfTrue{$idx}:=$idx+1
			$next:=Find in array:C230(IN4_LineIndent; $level; $idx+1)
			If ($next>0)
				IN4_LineIfFalse{$idx}:=$next+1
			Else 
				IN4_LineIfFalse{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="endwhile")
			$next:=PPro_ExecFindBackInArray($level; $idx-1)
			If ($next>0)
				IN4_LineIfTrue{$idx}:=$next+1
			Else 
				IN4_LineIfTrue{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="for")
			IN4_LineIfTrue{$idx}:=$idx+1
			$next:=Find in array:C230(IN4_LineIndent; $level; $idx+1)
			If ($next>0)
				IN4_LineIfFalse{$idx}:=$next+1
			Else 
				IN4_LineIfFalse{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="endfor")
			IN4_LineIfFalse{$idx}:=$idx+1
			$next:=PPro_ExecFindBackInArray($level; $idx-1)
			If ($next>0)
				IN4_LineIfTrue{$idx}:=$next+1
			Else 
				IN4_LineIfTrue{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="case")
			$next:=Find in array:C230(IN4_LineIndent; $level+1; $idx+1)  //next ":"
			If ($next>0)
				IN4_LineIfTrue{$idx}:=$next
			Else 
				IN4_LineIfTrue{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
			
		: (IN4_Token{$idx}="item")
			IN4_LineIfTrue{$idx}:=$idx+1
			$next:=Find in array:C230(IN4_LineIndent; $level; $idx+1)  //next item
			C_LONGINT:C283($next1)
			$next1:=Find in array:C230(IN4_LineIndent; $level-1; $idx+1)+1  //end case
			If ((($next1>0) & ($next>$next1)) | ($next<1))
				$next:=$next1
			End if 
			
			If ($next>0)
				IN4_LineIfFalse{$idx}:=$next
			Else 
				IN4_LineIfFalse{$idx}:=Size of array:C274(IN4_Line)+1
			End if 
	End case 
	
End for 