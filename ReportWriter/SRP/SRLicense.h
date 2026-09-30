/*
 *  SRLicense.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 10.09.2010.
 *  Copyright 2010 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

#include "RWBaseTypes.h"

enum	ELicense
{
	eLicense_Invalid = -1,
	eLicense_Beta,
	eLicense_Demo,
	eLicense_Valid,	//2
	eLicense_ValidOEM,
	eLicense_ExpiredBeta,
	eLicense_ExpiredDemo,
	eLicense_Expired,
	eLicense_ExpiredOEM,
	eLicense_Environment,	//8
	eLicense_UserCount
};

class RWPageComposer;
struct SRect;

ELicense	RW_SetLicense (const RWString &inLicense);
ELicense	RW_GetLicense (void);
void		RW_CheckLicense (RWPageComposer *inComposer, const SRect *inRect, bool inForPrinting);


void		RW_CreateLicense (const CText &inCustomer, long in4DNumber, long inFlags, CText &outLicense);
