/*
 *  ExtendedExecute.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 30/10/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

# include	"ExtendedExecute.h"
# include	"RWString4D.h"

FILE * fileref;

/* 
 ExecLine		constructor
 */

ExecLine::ExecLine (const char16_t* line, int len) :
mIndent (0)		,
mCondition (ex_plain),
mTrueLine (-1)		,
mFalseLine (-1)		,
mLine (line, len)	,
mCommandSize (0)	,
mConditionSize (0)
{
}

/* 
 ExecLine		destructor
 */

ExecLine::~ExecLine (void) 
{
	if (mCommandSize > 0) {
		delete [] mCommandBlock;
	}
	if (mConditionSize > 0) {
		delete [] mConditionBlock;
	}
}

/* 
 InsertCommand		public
*/

void
ExecLine::InsertCommand (char * block, int size)
{
	mCommandBlock = block;
	mCommandSize = size; 
}

/* 
 InsertCondition	public
*/

void
ExecLine::InsertCondition (char * block, int size)
{
	mConditionBlock = block;
	mConditionSize = size;
}

/* 
 SetTrueJump		public
*/

void
ExecLine::SetTrueJump (int line)  {mTrueLine = line;}

/* 
 SetFalseJump		public
*/

void
ExecLine::SetFalseJump (int line)  {mFalseLine = line;}

/* 
 SetIndent			public
 */

void
ExecLine::SetIndent (int indent)  {mIndent = indent;}

void
ExecLine::ExecuteCommand (void)
{
	if (mCommandSize > 0) {
		PA_ExecuteTokens (mCommandBlock, mCommandSize);
	}
}

bool
ExecLine::HasCondition (void)
{
	return mConditionSize > 0;
}

bool
ExecLine::ExecuteCondition (void)
{
	if (mConditionSize > 0) {
		PA_Variable execValue = PA_ExecuteTokensAsFunction (mConditionBlock, mConditionSize);
		// TODO:	check the result for eVK_Boolean
		if (execValue.fType == eVK_Boolean) {
			return	execValue.uValue.fBoolean;	
		} else {
			return	false;

		}

	}
	return false;
}

/*
 ExtendedExecute constructor
*/

ExtendedExecute::ExtendedExecute (const PA_Unichar* method, long len) : mMethod (RWStr::FromPA (method, size_t (len))) 
{
	// Init(); init need to be done before execution - coditions are evaluaded here pB 2010-12
}

ExtendedExecute::ExtendedExecute () 
: mMethod () 
{
	// Init();
}


ExtendedExecute::ExtendedExecute (const ExtendedExecute &inOther) : mMethod (inOther.mMethod) //mbs 05112010
{
	// Init(); init need to be done before execution - coditions are evaluaded here pB 2010-12
}

/*
 ExtendedExecute destructor
 */

ExtendedExecute::~ExtendedExecute (void) 
{
	Free();
//	mLines.clear();
};

void
ExtendedExecute::Free (void)
{
	while (!mForStack.empty())
	{
		//mbs 08112010	needs clean-up!
		Tokens	*tokens = mForStack.top();
		delete tokens;
		mForStack.pop();
	};
	if (mLines.size() > 0)		//mbs 08112010	needs clean-up!
	{
		ExecList::reverse_iterator	it;
		for (it = mLines.rbegin(); it != mLines.rend(); it++)
		{
			ExecLine	*line = *it;
			delete line;
		}
		mLines.clear();
	}
}


ExtendedExecute&
ExtendedExecute::operator = (const RWString inText)
{
	Free();
	mMethod = inText;
	// Init(); init need to be done before execution - coditions are evaluaded here pB 2010-12
	return *this;
}

/*
 Init ()	public
*/

void
ExtendedExecute::Init (void)
{
	// Parse mMethod by lines without modifying c_str() buffer
#if 0 && TARGET_DEBUG
	fileref = fopen("/private/tmp/scriptLog.txt", "a");
#endif
	Free();
	if (!mMethod.empty())
	{
		std::size_t pos = 0;
		int indent = 0;
		while (pos <= mMethod.length()) {
			// find end of line (\r or \n)
			std::size_t lineEnd = pos;
			while (lineEnd < mMethod.length() && mMethod[lineEnd] != u'\r' && mMethod[lineEnd] != u'\n') {
				lineEnd++;
			}

			if (lineEnd > pos) {
				RWString temp = mMethod.substr(pos, lineEnd - pos);
				// Append a comment marker to make sure tokenization succeeds consistently
				temp += RWString (u" //");

				PA_Unistring ustr = RWStr::CreatePA (temp);
				char * tokens = 0;
				int tokenLen = PA_Tokenize (&ustr, tokens);
				tokens = new char [tokenLen];
				tokenLen = PA_Tokenize (&ustr, tokens);
				PA_DisposeUnistring (&ustr);

				PA_Unistring normLine = PA_Detokenize (tokens, tokenLen);
				ExecLine* newline =  ExecLineFactory (reinterpret_cast <const char16_t*> (PA_GetUnistring (&normLine)), PA_GetUnistringLength (&normLine), tokens, tokenLen, indent);
				PA_DisposeUnistring (&normLine);

				mLines.push_back (newline);
			}

			// advance past line ending (handle \r\n)
			if (lineEnd >= mMethod.length()) break;
			std::size_t nextPos = lineEnd + 1;
			if (nextPos < mMethod.length()) {
				char16_t c = mMethod[nextPos - 1];
				char16_t c2 = mMethod[nextPos];
				if ((c == u'\r' && c2 == u'\n') || (c == u'\n' && c2 == u'\r')) {
					nextPos++;
				}
			}
			pos = nextPos;	// past "\r\n" too
		}

		// Set up jumps after lines are created
		SetJumps();
#if 0 && TARGET_DEBUG
		LogParsing();
		(void) fclose (fileref);
#endif
	}
	
}

double
ExtendedExecute::EvaluateExpression (RWString &expression)
{
	PA_Unistring	ustr = RWStr::CreatePA (expression);
	char *			tokens = 0;
	int				tokenLen = PA_Tokenize (&ustr, tokens);
	tokens = new char [tokenLen];
	tokenLen = PA_Tokenize (&ustr, tokens);
	PA_DisposeUnistring (&ustr);
	
	PA_Variable execValue = PA_ExecuteTokensAsFunction (tokens, tokenLen);
	delete[] tokens;
	
	switch (execValue.fType) {
		case eVK_Longint:
			return (double) execValue.uValue.fLongint;
			break;
		case eVK_Real:
			return  execValue.uValue.fReal;
			break;
		default:
			return	0;
			break;			
	}
	
	return 0;
}

void
ExtendedExecute::Execute (void)
{
	Init();

	int			index = 0;
	int			arrSize = mLines.size ();
	ExecLine *	line;
	
	while (index < arrSize) 
	{
		line = mLines[index];
		line->ExecuteCommand();
		
		if (line->mTrueLine > -1)
		{
			if (line->HasCondition ()) 
			{
				if (line->ExecuteCondition()) {
					index = line->mTrueLine;
				}
				else {
					index = line->mFalseLine;
				}
			}
			else 
				index = line->mTrueLine;

		}
		else 
			index++;
	}
}

int
ExtendedExecute::FindIndent (int from, int indent)
{
	int		arrSize = mLines.size ();
	for (int i = from; i < arrSize; i++) {
		if (mLines[i]->mIndent == indent) {
			return i;
		}
	}
	return arrSize;  //exit processing
}

int
ExtendedExecute::RFindIndent (int from, int indent)
{
	int		arrSize = mLines.size ();
	for (int i = from; i >= 0; i--) {
		if (mLines[i]->mIndent == indent) {
			return i;
		}
	}
	return arrSize;	//exit processing
}

bool		
ExtendedExecute::IsEmpty (void) const 
{
	return mMethod.empty();
}

void
ExtendedExecute::SetJumps (void)
{
	int		arrSize = mLines.size ();
	
	ExecLine *			currLine;
	for (int i = 0; i < arrSize; i++)
	{
		currLine = mLines[i];
		
		switch (currLine->mCondition) {
			case ex_if:
				currLine->SetTrueJump(i + 1);
				currLine->SetFalseJump(FindIndent (i + 1, currLine->mIndent) + 1);
				break;
				
			case ex_else:
			{
				int nextIf = FindIndent (i + 1, currLine->mIndent);
				int nextCase = FindIndent (i + 1, currLine->mIndent - 1);
				currLine->SetTrueJump(nextIf < nextCase ? nextIf : nextCase);
				break;
			}
				
			case ex_until:
				currLine->SetTrueJump(i + 1);
				currLine->SetFalseJump(RFindIndent (i - 1, currLine->mIndent));
				break;
				
			case ex_while:
				currLine->SetTrueJump(i + 1);
				currLine->SetFalseJump(FindIndent (i + 1, currLine->mIndent) + 1);
				break;

			case ex_endwhile:
				currLine->SetFalseJump(i + 1);
				currLine->SetTrueJump(RFindIndent (i - 1, currLine->mIndent));
				break;
				
			case ex_for:
				currLine->SetTrueJump(i + 1);
				currLine->SetFalseJump(FindIndent (i + 1, currLine->mIndent) + 1);
				break;
				
			case ex_endfor:
				currLine->SetFalseJump(i + 1);
				currLine->SetTrueJump(RFindIndent (i - 1, currLine->mIndent) + 1);
				break;
			
			case ex_case:
				currLine->SetTrueJump(FindIndent (i + 1, currLine->mIndent + 1));
				break;

			case ex_item:
			{
				currLine->SetTrueJump(i + 1);
				
				int	nextItem = FindIndent (i + 1, currLine->mIndent);
				int	endcase= FindIndent (i + 1, currLine->mIndent - 1);
				
				nextItem = (nextItem < endcase) ? nextItem : endcase;
				
				/*if (nextItem >= arrSize) {
					nextItem = FindIndent (i + 1, currLine->mIndent - 1);
				}*/   // pB 2013-7-2
				currLine->SetFalseJump(nextItem);
				if (nextItem >= 0)
				{
					ExecLine * nextLine = mLines[nextItem];
					if (nextLine->mCondition == ex_else) 
					{
						currLine->SetFalseJump(nextItem + 1);
					}
				}
				if (i > 0)
				{
					ExecLine * prevLine = mLines[i - 1];
					if ((prevLine->mTrueLine < 0) || ((prevLine->mTrueLine == i) && (prevLine->mCondition != ex_case)))
					{
						prevLine->SetTrueJump(FindIndent (i + 1, currLine->mIndent - 1));
					}
				}
				
				break;
			}
			default:

				break;
		}
	}
}

int	
ExtendedExecute::ParseStatement (const char16_t* method, char * &outTokens, int &outLen)
{
	RWString		line (method);

	// Find '(' and ')', using standard string ops
	size_t fromPos = line.find(u'(');
	size_t toPos = line.rfind(u')');

	if (toPos == RWString::npos) {
		if (!line.empty()) toPos = line.length() - 1;
	}
	if (fromPos != RWString::npos) {
		fromPos += 1;
		if (toPos >= fromPos) {
			RWString			cond = line.substr(fromPos, toPos - fromPos + 1);

			PA_Unistring	ustr = RWStr::CreatePA (cond);
			char *			tokens = 0;
			int				tokenLen = PA_Tokenize (&ustr, tokens);

			tokens = new char [tokenLen];
			tokenLen = PA_Tokenize (&ustr, tokens);
			PA_DisposeUnistring (&ustr);

			outTokens = tokens;
			outLen = tokenLen;

			return 1;
		}
	}

	return 0;
}

int	
ExtendedExecute::ParseForStatement (const char16_t* method, char * &outTokensInit, int &outLenInit, char * &outTokensAdd, int &outLenAdd, char * &outTokensCond, int &outLenCond)
{
	RWString		line (method);

	size_t fromPos = line.find(u'(');
	size_t toPos = line.rfind(u')');
	if (toPos == RWString::npos) {
		toPos = line.length();
	}
	if (fromPos != RWString::npos) {
		fromPos += 1;
		RWString			condition = line.substr(fromPos, toPos - fromPos);
		RWString			part1;
		RWString			part2;
		RWString			part3;
		RWString			part4;
		double			step;

		size_t delim1 = condition.find(u';');
		if (delim1 != RWString::npos) {
			part1 = condition.substr(0, delim1);
		}

		size_t delim2 = condition.find(u';', (delim1 == RWString::npos ? 0 : delim1 + 1));
		if (delim2 != RWString::npos) {
			part2 = condition.substr(delim1 + 1, delim2 - delim1 - 1);
			if (part2.length() == 0) {
				part2 = RWString (u"0x01");
			}
		}

		size_t delim3 = condition.find(u';', delim2 == RWString::npos ? 0 : delim2 + 1);
		if (delim3 != RWString::npos) {
			part3 = condition.substr(delim2 + 1, delim3 - delim2 - 1);
			part4 = condition.substr(delim3 + 1);
			if (part4.length() == 0) {
				part4 = RWString (u"0x01");
			}
		} else {
			part3 = condition.substr(delim2 + 1);
			part4 = RWString (u"0x01");
		}

		RWString			init = part1 + RWString (u":=") + part2;
		RWString			cond;

		step = EvaluateExpression(part4);
		if (fabs(step) < 1e-5) {
			step = 1;

			part4 = RWString (u"0x01");
		}
		if (step >= 0)
			cond = part1 + RWString (u"<=") + part3;
		else
			cond = part1 + RWString (u">=") + part3;

		RWString			incr;
		if (step >= 0)
			incr = part1 + RWString (u":=") + part1 + RWString (u"+") + part4;
		else
			incr = part1 + RWString (u":=") + part1 + RWString (u"-") + part4;


		PA_Unistring	ustr;
		char *			tokens = 0;
		int				tokenLen;

		ustr = RWStr::CreatePA (init);
		tokenLen = PA_Tokenize (&ustr, tokens);
		tokens = new char [tokenLen];
		tokenLen = PA_Tokenize (&ustr, tokens);
		PA_DisposeUnistring (&ustr);
		outTokensInit = tokens;
		outLenInit = tokenLen;

		tokens = 0;
		ustr = RWStr::CreatePA (incr);
		tokenLen = PA_Tokenize (&ustr, tokens);
		tokens = new char [tokenLen];
		tokenLen = PA_Tokenize (&ustr, tokens);
		PA_DisposeUnistring (&ustr);
		outTokensAdd = tokens;
		outLenAdd = tokenLen;

		tokens = 0;
		ustr = RWStr::CreatePA (cond);
		tokenLen = PA_Tokenize (&ustr, tokens);
		tokens = new char [tokenLen];
		tokenLen = PA_Tokenize (&ustr, tokens);
		PA_DisposeUnistring (&ustr);
		outTokensCond = tokens;
		outLenCond = tokenLen;

		return 1;
	}

	return 0;
}

ExecLine *
ExtendedExecute::ExecLineFactory (const char16_t* methodLine, long len, char * tokens, int tokensLen, int &ioIndent) 
{
	ExecLine *	line = new ExecLine (methodLine, len);
	char *	condition;
	int		condLen;
	int		indent = ioIndent;
	
	EToken	firstToken;
	Tokens *	tokenAdd;
	Tokens *	tokenCond;
	
	if (tokensLen > 0x30)
		firstToken = EToken (tokens [0x30]);
	else {
		firstToken = token_Operator; // probably empty line
	}

	switch (firstToken) {
		case token_If:
			line->mCondition = ex_if;
			line->mIndent = indent;
			indent++;
			
			delete [] tokens;
			if (ParseStatement(methodLine, condition, condLen)) {
				line->InsertCondition	(condition, condLen);
			}
			break;		
			
		case token_Else:
			line->mCondition = ex_else;
			line->mIndent = indent - 1;
			delete [] tokens;
			break;		
			
		case token_EndIf:
			line->mCondition = ex_endif;
			indent--;
			line->mIndent = indent;
			delete [] tokens;

			break;		
			
		case token_Repeat:
			line->mCondition = ex_repeat;
			line->mIndent = indent;
			 indent++;
			delete [] tokens;
			break;		
			
		case token_Until:
			line->mCondition = ex_until;
			indent--;
			line->mIndent = indent;
			delete [] tokens;
			if (ParseStatement(methodLine, condition, condLen)) {
				line->InsertCondition	(condition, condLen);
			}
			break;		
			
		case token_While:
			line->mCondition = ex_while;
			line->mIndent = indent;
			indent++;
			delete [] tokens;
			if (ParseStatement(methodLine, condition, condLen)) {
				line->InsertCondition	(condition, condLen);
			}
			break;		
			
		case token_EndWhile:
			line->mCondition = ex_endwhile;
			indent--;
			line->mIndent = indent;
			delete [] tokens;
			break;		
			
		case token_For:
			line->mCondition = ex_for;
			line->mIndent = indent;
			indent++;
			
			delete [] tokens;
			
			char *	tokensInit;
			char *	tokensAdd;
			char *	tokensCond;
			int		tokInitLen;
			int		tokAddLen;
			int		tokCondLen;
			
			if (ParseForStatement (methodLine, tokensInit, tokInitLen, tokensAdd, tokAddLen, tokensCond, tokCondLen))
			{
				line->InsertCommand (tokensInit, tokInitLen);
				line->InsertCondition (tokensCond, tokCondLen);
						
				char *	cond2 = new char [tokCondLen];
				memcpy(cond2, tokensCond, tokCondLen);	//mbs 08112010	Peter: destination, source, length!!!
				Tokens *	token1 = new Tokens (cond2, tokCondLen);	//mbs 08112010	Peter: cond2 instead of tokensCond!!!
				Tokens *	token2 = new Tokens (tokensAdd, tokAddLen);
				mForStack.push(token1);
				mForStack.push(token2);
			}
			break;		
			
		case token_EndFor:
			line->mCondition = ex_endfor;
			indent--;
			line->mIndent = indent;
			delete [] tokens;
			tokenAdd =  mForStack.top();
			line->InsertCommand (tokenAdd->mTokens, tokenAdd->mLength);
			tokenAdd->mTokens = NULL;
			mForStack.pop();
			delete tokenAdd;
			
			tokenCond = mForStack.top();
			line->InsertCondition (tokenCond->mTokens, tokenCond->mLength);
			tokenCond->mTokens = NULL;
			mForStack.pop();
			delete tokenCond;
			break;
			
		case token_CaseOf:
			line->mCondition = ex_case;
			line->mIndent = indent;
			indent += 2;
			delete [] tokens;
			break;		
			
		case token_EndCase:
			line->mCondition = ex_endcase;
			indent -= 2;
			line->mIndent = indent;
			delete [] tokens;
			break;		
			
		case token_Colon:
			line->mCondition = ex_item;
			line->mIndent = indent - 1;
			delete [] tokens;
			if (ParseStatement(methodLine, condition, condLen)) {
				line->InsertCondition	(condition, condLen);
			}
			break;		
			
		case token_Comment:
			line->mCondition = ex_comment;
			line->mIndent = indent + 2;
			delete [] tokens;
			break;		

		default:
			line->mIndent = indent;
			line->InsertCommand (tokens, tokensLen);
			break;
	}

#if 0 && TARGET_DEBUG
	fprintf(fileref, "entry indent -> %d ident %d, line condition: %d indent: %d tokenSize: %d\n", ioIndent, indent, firstToken, line->mIndent, tokensLen);
#endif
	
	ioIndent = indent;
	return line;
};

void
ExtendedExecute::LogParsing ()
{
	int		arrSize = mLines.size ();
	
	ExecLine *			currLine;
	for (int i = 0; i < arrSize; i++)
	{
		currLine = mLines[i];
		
		switch (currLine->mCondition) {
			case ex_if:
				fprintf(fileref, "%d If: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_else:
				fprintf(fileref, "%d Else: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_until:
				fprintf(fileref, "%d Until: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_while:
				fprintf(fileref, "%d While: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_endwhile:
				fprintf(fileref, "%d End while: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_for:
				fprintf(fileref, "%d For: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_endfor:
				fprintf(fileref, "%d End for: true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_case:
				fprintf(fileref, "%d Case: true -> %d false -> %d ident %d\n", i , currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
				
			case ex_item:
				fprintf(fileref, "%d ':' true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);
				break;
			default:
				fprintf(fileref, "%d plain true -> %d false -> %d ident %d\n", i, currLine->mTrueLine, currLine->mFalseLine, currLine->mIndent);

				break;
		}
	}
};


