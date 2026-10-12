//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_ExecInsertLine; $1)
End if 

C_TEXT:C284($line)
$line:=$1

C_LONGINT:C283($idx)
$idx:=PPro_ExecDataHandler("Append")

C_LONGINT:C283($level)
$level:=IN4_NextLevel

C_LONGINT:C283($from)
$from:=Length:C16($line)
C_LONGINT:C283($j)
For ($j; 1; Length:C16($line))
	If (Character code:C91($line[[$j]])>32)  //not a space
		$from:=$j
		$j:=Length:C16($line)
	End if 
End for 
$line:=Substring:C12($line; $from)

C_BLOB:C604($o)
C_LONGINT:C283($err)
$err:=RW_Tokenize($line; $o)
$line:=RW_Detokenize($o)
IN4_Line{$idx}:=$line
IN4_CommandOffset{$idx}:=-1

C_LONGINT:C283($comment)
$comment:=Position:C15("`"; $line)
If ($comment<1)
	$comment:=Length:C16($line)
End if 

Case of 
	: (Position:C15("If"; $line)=1)
		IN4_Token{$idx}:="if"
		IN4_LineIndent{$idx}:=$level
		IN4_NextLevel:=$level+1
		IN4_ConditionOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		$o:=PPro_ExecTokenizeCondition(Substring:C12($line; 3))
		VARIABLE TO BLOB:C532($o; IN4_ObjectBlob; *)
		
	: (Position:C15("Else"; $line)=1)
		IN4_Token{$idx}:="else"
		IN4_LineIndent{$idx}:=$level-1
		//IN4_NextLevel:=$level
		
	: (Position:C15("End if"; $line)=1)
		IN4_Token{$idx}:="endif"
		IN4_LineIndent{$idx}:=$level-1
		IN4_NextLevel:=$level-1
		
	: (Position:C15("Repeat"; $line)=1)
		IN4_Token{$idx}:="repeat"
		IN4_LineIndent{$idx}:=$level
		IN4_NextLevel:=$level+1
		
	: (Position:C15("Until"; $line)=1)
		IN4_Token{$idx}:="until"
		IN4_LineIndent{$idx}:=$level-1
		IN4_NextLevel:=$level-1
		IN4_ConditionOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		$o:=PPro_ExecTokenizeCondition(Substring:C12($line; 3))
		VARIABLE TO BLOB:C532($o; IN4_ObjectBlob; *)
		
	: (Position:C15("While"; $line)=1)
		IN4_Token{$idx}:="while"
		IN4_LineIndent{$idx}:=$level
		IN4_NextLevel:=$level+1
		IN4_ConditionOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		$o:=PPro_ExecTokenizeCondition(Substring:C12($line; 3))
		VARIABLE TO BLOB:C532($o; IN4_ObjectBlob; *)
		
	: (Position:C15("End while"; $line)=1)
		IN4_Token{$idx}:="endwhile"
		IN4_LineIndent{$idx}:=$level-1
		IN4_NextLevel:=$level-1
		
	: (Position:C15("For"; $line)=1)
		IN4_Token{$idx}:="for"
		IN4_LineIndent{$idx}:=$level
		IN4_NextLevel:=$level+1
		C_BLOB:C604($o1; $o2; $o3)
		PPro_ExecTokenizeForCondition(Substring:C12($line; 4); ->$o1; ->$o2; ->$o3)
		IN4_CommandOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		VARIABLE TO BLOB:C532($o1; IN4_ObjectBlob; *)
		
		//push increment and condition on stack
		IN4_ForStack:=IN4_ForStack+1
		IN4_ForStack{IN4_ForStack}:=BLOB size:C605(IN4_ObjectBlob)
		VARIABLE TO BLOB:C532($o2; IN4_ObjectBlob; *)
		
		IN4_ConditionOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		IN4_ForStack:=IN4_ForStack+1
		IN4_ForStack{IN4_ForStack}:=BLOB size:C605(IN4_ObjectBlob)
		VARIABLE TO BLOB:C532($o3; IN4_ObjectBlob; *)
		
		
	: (Position:C15("End for"; $line)=1)
		IN4_Token{$idx}:="endfor"
		IN4_LineIndent{$idx}:=$level-1
		IN4_NextLevel:=$level-1
		
		//pop increment and condition out of stack
		If (IN4_ForStack>0)
			IN4_ConditionOffset{$idx}:=IN4_ForStack{IN4_ForStack}
			IN4_ForStack:=IN4_ForStack-1
			IN4_CommandOffset{$idx}:=IN4_ForStack{IN4_ForStack}
			IN4_ForStack:=IN4_ForStack-1
		End if 
		
	: (Position:C15("Case"; $line)=1)
		IN4_Token{$idx}:="case"
		IN4_LineIndent{$idx}:=$level
		IN4_NextLevel:=$level+2
		
	: (Position:C15(":"; $line)=1)
		IN4_Token{$idx}:="item"
		IN4_LineIndent{$idx}:=$level-1
		//IN4_NextLevel:=$level
		IN4_ConditionOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		$o:=PPro_ExecTokenizeCondition(Substring:C12($line; 3))
		VARIABLE TO BLOB:C532($o; IN4_ObjectBlob; *)
		
	: (Position:C15("End case"; $line)=1)
		IN4_Token{$idx}:="endcase"
		IN4_LineIndent{$idx}:=$level-2
		IN4_NextLevel:=$level-2
		
	: (Position:C15("`"; $line)=1)
		IN4_Token{$idx}:="comment"
		IN4_LineIndent{$idx}:=$level+2
		//IN4_NextLevel:=$level
		
	Else 
		IN4_LineIndent{$idx}:=$level
		IN4_CommandOffset{$idx}:=BLOB size:C605(IN4_ObjectBlob)
		VARIABLE TO BLOB:C532($o; IN4_ObjectBlob; *)
End case 
