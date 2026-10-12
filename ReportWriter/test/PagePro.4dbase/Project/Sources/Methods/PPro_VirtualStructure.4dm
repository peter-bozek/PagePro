//%attributes = {"shared":true}
// PPro_VirtualStructure (   allows to create or override content of hierarchical lists
//      selector;    one of 'field', 'var' or 'commands'
//      action;      one of 'init', 'default' or 'set'
//      group;       in case of set action, name of group (top level hierarchy item)
//      items;       pointer to string array with names of list nodes
//      varOrTable;  pointer to string array of variable names or longint array of table numbers
//      fieldNo)     pointer to longint array with field numbers
//    -> error

C_TEXT:C284($1)  // selector - field, var or commands
C_TEXT:C284($2)  //action selector: init default, set
C_TEXT:C284($3)  //item group name
C_POINTER:C301($4)  //pointer to array if items
C_POINTER:C301($5; $6)  // pointers to longint arrays of table no / field no
// or pointer to text array with names of variables

If (False:C215)
	C_TEXT:C284(PPro_VirtualStructure; $1)  // selector - field or var?
	C_TEXT:C284(PPro_VirtualStructure; $2)  //action selector
	C_TEXT:C284(PPro_VirtualStructure; $3)  //item name
	C_POINTER:C301(PPro_VirtualStructure; $4)  //pointer to array if items
	C_POINTER:C301(PPro_VirtualStructure; $5; $6)  // pointers to longint arrays of table no / field no
	C_LONGINT:C283(PPro_VirtualStructure; $0)  //error number
	
End if 

// virtual structure is kept in two hierarchical lists
// PP_TableList
// PP_VariableList

C_TEXT:C284($type)
$type:=$1
C_TEXT:C284($sel)
$sel:=$2


C_LONGINT:C283($list)
Case of 
	: ($type="field")
		$list:=PP_TableList
	: ($type="var")
		$list:=PP_VariableList
	: ($type="commands")
		$list:=PP_CommandList
		
End case 

Case of 
	: ($sel="init")
		If ($list#0)
			CLEAR LIST:C377($list; *)
			$list:=0
		End if 
		
	: ($sel="default")
		PPro_VirtualStructure($type; "init")  //clear list first
		Case of 
			: ($type="field")
				$list:=PPro_VirtStructCreateFieldList
			: ($type="var")
				$list:=PPro_VirtStructCreateVarList
			: ($type="Commands")
				$list:=PPro_VirtStructCreateCommList
		End case 
		
	: ($sel="set")
		If ($list=0)
			$list:=New list:C375
		End if 
		
		Case of 
			: ($type="field")
				If (Count parameters:C259>5)
					C_LONGINT:C283($hSubList)
					$hSubList:=New list:C375
					If (Size of array:C274($5->)>0)
						C_LONGINT:C283($i)
						For ($i; 1; Size of array:C274($4->))
							If (($i<=Size of array:C274($5->)) & ($i<=Size of array:C274($6->)))
								APPEND TO LIST:C376($hSubList; $4->{$i}; $5->{$i} << 0x0010+$6->{$i})
								SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "["+Table name:C256($5->{$i})+"]"+Field name:C257($5->{$i}; $6->{$i}))
							End if 
						End for 
						APPEND TO LIST:C376($list; $3; $5->{1}; $hSublist; False:C215)
					End if 
				End if 
			: ($type="var")
				If (Count parameters:C259>3)
					C_LONGINT:C283($hSubList)
					$hSubList:=New list:C375
					
					C_LONGINT:C283($offset)
					$offset:=Count list items:C380($list; *)
					
					C_LONGINT:C283($i)
					For ($i; 1; Size of array:C274($4->))
						If ($i<=Size of array:C274($5->))
							APPEND TO LIST:C376($hSubList; $4->{$i}; -128-$offset-$i)
							SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; $5->{$i})
						End if 
					End for 
					
					APPEND TO LIST:C376($list; $3; -128-$offset-$i; $hSublist; False:C215)
					
				End if 
			: ($type="Commands")
				For ($i; 1; Size of array:C274($4->))
					APPEND TO LIST:C376($list; $4->{$i}; $i)
				End for 
		End case 
		
	Else 
		//unknown command
End case 


Case of 
	: ($type="field")
		PP_TableList:=$list
	: ($type="var")
		PP_VariableList:=$list
	: ($type="commands")
		PP_CommandList:=$list
		
End case 
