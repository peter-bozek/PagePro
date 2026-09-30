/*
 *  ExtendedExecute.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 30/10/2010.
 *  Copyright 2010 INFORCE Bratislava sro. All rights reserved.
 *
 */

#ifndef _ExtendedExecute_h_  
# define _ExtendedExecute_h_ 

# include	"RWBaseTypes.h"
# include	"4DPluginAPI.h"

# include <vector>
# include <stack>
# include	<stdio.h>


enum	EToken	{
	token_Operator			= 0,	//	{operator}
	token_Command			= 1,	//	{4D Command}
	token_Constant			= 2,	//	{constant}
	token_TableField		= 3,	//	{table/field}
	token_LeftParenthesis	= 4,	// (
	token_RightParenthesis	= 5,	// )
	token_Semicolon			= 6,	// ;
	token_Assignment		= 7,	// :=
	token_NOOP				= 8,	//	{invalidtoken:8} == NOOP
	token_Variable			= 9,	//	{variable}
	token_10				= 10,	//	{invalidtoken:10}
	token_Method			= 11,	//	{method}
	token_12				= 12,	//	{invalidtoken:12}
	token_Parameter			= 13,	//	{$parameter}
	token_While				= 14,	// While
	token_EndWhile			= 15,	// End While
	token_If				= 16,	// If
	token_Else				= 17,	// Else
	token_EndIf				= 18,	// End If
	token_CaseOf			= 19,	// Case of
	token_Colon				= 20,	// :
	token_EndCase			= 21,	// End case
	token_Plugin			= 22,	//	{external}
	token_Comment			= 23,	//	{comment}
	token_LeftElement		= 24,	// {
	token_RightElement		= 25,	// }
	token_LocalVariable		= 26,	//	{$local var}
	token_Garbage			= 27,	//	{garbage}
	token_Pointer			= 28,	// ->
	token_For				= 29,	// For
	token_EndFor			= 30,	// End For
	token_Repeat			= 31,	// Repeat
	token_Until				= 32,	// Until
	token_LeftIndex			= 33,	// [[
	token_RightIndex		= 34,	// ]]
	token_IPVariable		= 35,	//	{<>interproc var}
	token_V6Constant		= 36,	//	{v6 constant}
	token__last__
};


enum exec_condition {
	ex_plain	= 0,
	ex_if		,
	ex_else		,
	ex_endif	,
	ex_repeat	,
	ex_until	,
	ex_while	,
	ex_endwhile	,
	ex_for		,
	ex_endfor	,
	ex_case		,
	ex_item		,
	ex_endcase	,
	ex_comment	,
	ex_last
	
};


class ExecLine   {
public:
					ExecLine (const char16_t* line, int len);
					~ExecLine (void);
	void			InsertCommand (char * block, int size);
	void			InsertCondition (char * block, int size);
	void			SetTrueJump (int line);
	void			SetFalseJump (int line);
	void			SetIndent (int indent);
	
	void			ExecuteCommand (void);
	bool			ExecuteCondition (void);
	bool			HasCondition (void);

	int				Execute (void);

	int				mIndent;
	exec_condition	mCondition;
	int				mTrueLine;
	int				mFalseLine;
	
private:
	CText			mLine;

	char *			mCommandBlock;
	int				mCommandSize;
	char *			mConditionBlock;
	int				mConditionSize;
	

};


class Tokens {
public:
	Tokens (char * inTokens, int inLength) : mTokens (inTokens), mLength (inLength) {}
	~Tokens () { if (mTokens) delete [] mTokens;}

	char *	mTokens;
	int		mLength;
};

typedef	std::vector<ExecLine*>	ExecList;
typedef std::stack<Tokens*>		ExecStack;

class ExtendedExecute
{
public:
				ExtendedExecute (const PA_Unichar* method, long len);
				ExtendedExecute (const ExtendedExecute &inOther);	//mbs 05112010
				ExtendedExecute (void);
				~ExtendedExecute (void);

	void		Init (void);
	void		Execute (void);
	ExecStack	mForStack;
	int			FindIndent (int from, int indent);
	int			RFindIndent (int from, int indent);
	void		SetJumps (void);
	double		EvaluateExpression (CText &expression);
	bool		IsEmpty (void) const ;
	void		Free (void);
	inline CText	Get ();
	ExtendedExecute &	operator = (const CText inText);

	//	operator	CText (void) const;
	inline operator	CText (void);
//	operator	RWValue (void);
//	operator	RWTextValue (void);

	
private:
	int			ParseStatement (const char16_t* method, char * &outTokens, int &outLen);
	int			ParseForStatement (const char16_t* method, char * &outTokensInit, int &outLenInit, char * &outTokensAdd, int &outLenAdd, char * &outTokensCond, int &outLenCond);
	ExecLine *	ExecLineFactory (const char16_t* methodLine, long len, char * tokens, int tokensLen, int &ioIndent);
	
	void		LogParsing (void);
protected:
    CText					mMethod;
	ExecList				mLines;
};

inline CText	ExtendedExecute::Get () {return mMethod; }
//ExtendedExecute::operator	CText (void) const {CText str (mMethod); return str;}
inline ExtendedExecute::operator	CText (void) { return mMethod.data();}
//ExtendedExecute::operator	RWValue (void) {RWValue str; str.SetText (mMethod.Get()); return str;}
//ExtendedExecute::operator	RWTextValue (void) {RWTextValue str (mMethod.Get()); return str;}


#endif
