/*
 *  SRLicense.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 10.09.2010.
 *  Copyright 2010 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

# include	"SRLicense.h"
# include	"theVersion.h"
# include	"RWStyle.h"
# include	"RWPageComposer.h"
//using namespace	XF;
# if	_4D_Package_
# include	"4DPluginAPI.h"
// using namespace	FourDAPIEx;
#endif
# include	"PSObjProps.h"

#if	_MSL_USING_MW_C_HEADERS && !__MACH__
# include	<time_api.h>
#else
# include	<time.h>
#endif

//#pragma export off
//#pragma	sym off

/*
 * Derived from CRC algorithm for JTAG ICE mkII, published in Atmel
 * Appnote AVR067.  Converted from C++ to C.
 */

/* CRC16 Definitions */
static const unsigned short crc_table[256] = {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/* CRC calculation macros */
#define CRC_INIT 0xFFFF
#define CRC(crcval,newchar) crcval = (crcval >> 8) ^ \
crc_table[(crcval ^ newchar) & 0x00ff]

static unsigned short
crcsum(const unsigned char* message, unsigned long length,
       unsigned short crc)
{
	unsigned long i;
	
	for(i = 0; i < length; i++)
    {
		CRC(crc, message[i]);
    }
	return crc;
}


enum	ELicenceBits
{
	eLB_Developer		= 0x00000001,	// disallow compiled
	eLB_StandaloneMac	= 0x00000002,	// 4D Mac
	eLB_StandaloneWin	= 0x00000004,	// 4D Win
//	eLB_Standalone		= 0x00000006,	// 4D
	eLB_EnginedMac		= 0x00000008,	// 4D Runtime Mac
	eLB_EnginedWin		= 0x00000010,	// 4D Runtime Win
//	eLB_Engined			= 0x00000018,	// 4D Runtime
	eLB_Server			= 0x00000020,	// 4D Client/Server
	eLB_4DNumber		= 0x00000040,
	eLB_OEM				= 0x00000080,
	eLB_Expiration		= 0x0000FF00,	// expiration in months since 1.1.2011
	eLB_UserCount		= 0x00FF0000,	// max number of users, 0 = unlimited

	eLB_Expiration_Shift	= 8,
	eLB_UserCount_Shift		= 16
};

//# define	TARGET_BOMB	1325375999UL	// using date -ur gives us "Sat Dec 31 23:59:59 UTC 2011"
# define	kDemoExpiry	1200UL			// seconds
# define	kBaseXOR	0xA55ABAAB
static const RWString  	kPPVersion = u"A010";
//static const std::string_view kPPVersion = "A011";

static	ELicense		sLicense = eLicense_Demo;
static	unsigned long	sLicenseFlags = 0;
static	unsigned long	sLicenseExpiry = 0;
static	RWStyle			sLicenseStyle (NULL, RWXmlNode());


ELicense	RW_GetLicense (void)
{
	if (sLicense < eLicense_ExpiredBeta)
	{
		if (sLicenseExpiry != 0)
		{
#if	_MSL_USING_MW_C_HEADERS && !__MACH__
			time_t	seconds = __get_time();
			__to_gm_time (&seconds);
#else
			time_t	seconds = time (NULL);
#endif
			if ((unsigned long) seconds > sLicenseExpiry)
			{
				if (sLicense == eLicense_Invalid)
					sLicense = eLicense_Expired;
				else
					sLicense = ELicense (sLicense + 4);
			}
		}
	}
	return sLicense;
}


#if	0
void		RW_CreateLicense (const RWString &inCustomer, long in4DNumber, long inFlags, RWString &outLicense)
{
	// 20-bitovy bitfield (zoberme rovno 32bitovy long)
	//	inFlags

	// XORnuty s niecim  (0xA55ABAAB)
	// skonvertovany na string A
	char	stringA [16];
	snprintf (stringA, sizeof (stringA), "%lu", inFlags & kBaseXOR);

	// string A skombinovany s menom zakaznika a cislom 4D (ak ma byt pouzite)
	RWString	tempB (inCustomer);
	tempB.AppendAscii (stringA);
	char	tempC [16];
	if (inFlags & eLB_4DNumber)
	{
		snprintf (tempC, sizeof (tempC), "%lu", in4DNumber);
		tempB.AppendAscii (tempC);
	}
	// z vysledneho stringu spocitany hash
	// z hashu vyrobeny string B
	char	stringB [16];
	unsigned char*	data = tempB.GetUTF8();
	snprintf (stringB, sizeof (stringB), "%u", crcsum (data, RWString::StrLength (data), CRC_INIT));

	// licencne cislo je kombinaciou mena zakaznika, stringu A a stringu B, ak sa pouzije 16-bitovy hash, kludne sa mozu cisla skonvertovat do dekadickej formy - A bude mat 9 znakov a B 5.
	outLicense = inCustomer;
	outLicense += '-';
	outLicense.AppendAscii (stringA);
	outLicense += '-';
	outLicense.AppendAscii (stringB);
}
#endif


ELicense	RW_SetLicense (const RWString &inLicense)
{
	if (sLicense == eLicense_Demo && sLicenseExpiry == 0 && inLicense.length() == 0)
	{
		RWValue	v;
		v.SetInteger (0x40000000);
		sLicenseStyle.SetProperty (PSObjPropTextColor, v);
		v.SetReal (45);
		sLicenseStyle.SetProperty (PSObjPropRotation, v);
		v.SetInteger (RWStyle::st_bold);
		sLicenseStyle.SetProperty (PSObjPropStyleF, v);
		v.SetReal (48);
		sLicenseStyle.SetProperty (PSObjPropSize, v);
		v.SetBoolean (true);
		sLicenseStyle.SetProperty (PSObjPropWrap, v);
		v.SetInteger (RWStyle::st_center);
		sLicenseStyle.SetProperty (PSObjPropAlign, v);
		sLicenseStyle.SetProperty (PSObjPropVertAlign, v);

#if	_MSL_USING_MW_C_HEADERS && !__MACH__
		time_t	seconds = __get_time();
		__to_gm_time (&seconds);
#else
		time_t	seconds = time (NULL);
#endif
#ifdef	TARGET_BOMB
		if ((unsigned long) seconds > TARGET_BOMB)
			sLicense = eLicense_ExpiredBeta;
		else
			sLicense = eLicense_Beta;
#else
		sLicenseExpiry = (unsigned long) seconds + kDemoExpiry;
#endif
		return sLicense;
	}

//	if (sLicense == eLicense_Beta || sLicense == eLicense_ExpiredBeta)
//		return sLicense;

	sLicense = eLicense_ValidOEM;

    /*
	long	pos1 = inLicense.rfind ('-', 0);
	if (pos1 != string::npos)
	{
		long	pos2 = inLicense.rfind ('-', inLicense.length() - pos1);
		if (pos2 != string::npos)
		{
			string	customer = inLicense.substr (0, pos2);
            string	stringA = inLicense.substr (pos2 + 1, pos1 - pos2 - 1);
            string	stringB = inLicense.substr (pos1 + 1);

			// string A skombinovany s menom zakaznika a cislom 4D (ak ma byt pouzite)
            string	tempB (kPPVersion);
			tempB += customer;
			tempB += stringA;
			char	tempC [16];
			unsigned long	flags = 0;
			stringA.ToAscii (tempC, sizeof (tempC));
			sscanf (tempC, "%lu", &flags);
			tempC [0] = '\0';
			// z vysledneho stringu spocitany hash
			// z hashu vyrobeny string B
			char	checkB [16];
			unsigned char*	data = tempB.GetUTF8();
			snprintf (checkB, sizeof (checkB), "%u", crcsum (data.c_str(),  data.length(), CRC_INIT));
			if (stringB.compare (checkB) == 0)
			{
				if (flags & eLB_4DNumber)
					flags = 0;
			}
			else 
			{
				snprintf (tempC, sizeof (tempC), "%lu", PA_GetSerialKey());
				tempB.append (tempC);
				unsigned char*	data = tempB.c_str();
				snprintf (checkB, sizeof (checkB), "%u", crcsum (data,  data.length(), CRC_INIT));
				if (stringB.compare (checkB) == 0)
				{
					if ((flags & eLB_4DNumber) == 0)
						flags = 0;
				}
				else
					flags = 0;
			}

			if (flags != 0)
			{
				flags ^= kBaseXOR;
				sLicense = eLicense_Valid;
				sLicenseFlags = flags;
# if	_4D_Package_
				long applicationType = 0;
				{
					const	long	k4D_APPLICATION_TYPE = 494;
					PA_Variable	result = PA_ExecuteCommandByID( k4D_APPLICATION_TYPE, 0, 0 );
					if( PA_GetVariableKind( result ) == eVK_Longint )
						applicationType = PA_GetLongintVariable( result );
					else
						applicationType = (long) PA_GetRealVariable( result );
				}

				if ((flags == eLB_Developer) && PA_IsCompiled (true) == 1)
					sLicense = eLicense_Environment;

				if (sLicense == eLicense_Valid && (flags & eLB_UserCount) != 0 && PA_Is4DClient())
				{
					if (((flags & eLB_UserCount) >> eLB_UserCount_Shift) < (unsigned long) PA_CountConnectedUsers())
						sLicense = eLicense_UserCount;
				}

				// enviroment check
				
				if (sLicense == eLicense_Valid)
				{
					if ((flags & eLB_OEM) == 0 )			//pB OEM is valid for all application types 
					{
						if ((applicationType == 0) || (applicationType == 2))	// 4D
						{
							if (flags & eLB_Developer)	// pB 
								;						// valid
							else if (MACVER && (flags & eLB_StandaloneMac))
								;
							else if (WINVER && (flags & eLB_StandaloneWin))
								;
							else
								sLicense = eLicense_Environment; // server and runtime
							
						}
						else if (applicationType > 0 && applicationType < 4)	// 4D Runtime
						{
							if (MACVER && (flags & eLB_EnginedMac))
								;
							else if (WINVER && (flags & eLB_EnginedWin))
								;
							else
								sLicense = eLicense_Environment; // server and developer
						}
						else if (applicationType >= 4)
							if (flags & eLB_Developer)	// pB 
								;						// valid
							else if ((flags & eLB_Server) == 0)	// 4D Client/Server
							sLicense = eLicense_Environment;
					}
					else {
						sLicense = eLicense_ValidOEM;
					}

				}

				if ((sLicense == eLicense_Valid) || (sLicense == eLicense_ValidOEM))
				{
					if ((flags & eLB_Expiration) != 0 && ((flags & eLB_OEM) == 0 || !PA_IsCompiled (true)))
					{
						int months = (flags & eLB_Expiration) >> eLB_Expiration_Shift;
						struct tm	tm;
						tm.tm_sec = 0;
						tm.tm_min = 0;
						tm.tm_hour = 0;
						tm.tm_mday = 1;
						tm.tm_mon = months % 12;
						tm.tm_year = 111 + months / 12;
						tm.tm_isdst = 0;
#if	!_MSL_USING_MW_C_HEADERS || __MACH__
						tm.tm_gmtoff = 0;
#endif
						sLicenseExpiry = mktime (&tm);
					}
					else
						sLicenseExpiry = 0;
				}
# endif
			}
		}
	}
*/
	return sLicense;
}


void	RW_CheckLicense (RWPageComposer *inComposer, const SRect *inRect, bool inForPrinting)
{
	const char16_t	*msg;
	switch (RW_GetLicense())
	{
//		case eLicense_Beta:				msg = "Beta version (" kVersionString ") of PagePro"; break;
		case eLicense_Beta:				inForPrinting? msg = u"BETA of PagePro": msg = u"BETA"; break;
		case eLicense_Demo:				inForPrinting? msg = u"DEMO of PagePro": msg = u"DEMO"; break;
		case eLicense_ExpiredDemo:		inForPrinting? msg = u"DEMO of PagePro": msg = u"DEMO"; break;  // pB there is no expired demo
		case eLicense_Expired:			inForPrinting? msg = u"EXPIRED PagePro LICENSE": msg = u"EXPIRED LICENSE"; break;
		case eLicense_ExpiredOEM:		msg = u"EXPIRED OEM LICENSE"; break;
		case eLicense_Invalid:
		case eLicense_Environment:
		case eLicense_UserCount:		inForPrinting? msg = u"INVALID PagePro LICENSE": msg = u"INVALID LICENSE"; break;
		default:						msg = NULL; break;
	}
	if (msg != 0)
	{
		RWString	demo;
		demo.assign (msg);
		inComposer->DrawTextBox (demo, &sLicenseStyle, *inRect, false, false, false, NULL);
	}
}

/*
 20-bitovy bitfield (zoberme rovno 32bitovy long)
 XORnuty s niecim  (0xA55ABAAB)
 skonvertovany na string A
 
 string A skombinovany s menom zakaznika a cislom 4D (ak ma byt pouzite)
 z vysledneho stringu spocitany hash
 z hashu vyrobeny string B
 licencne cislo je kombinaciou mena zakaznika, stringu A a stringu B, ak sa pouzije 16-bitovy hash, kludne sa mozu cisla skonvertovat do dekadickej formy - A bude mat 9 znakov a B 5.
 
 Pri dekodovani sa zobere meno zakaznika z licencneho kodu, string A z licencneho kodu a cislo 4D, spocita sa hash ( s aj bez cisla 4D) a musi sediet s B.
 
 Ak sedi, dekoduje sa A
 
 Mame nasledujuce moznosti:
 
 Developer : nebezi v Runtime a skompilovany
 SU aj Server
 both platforms
 
 Deployment SU - bezi len Runtime alebo 4D skompilovany (skompilovana main databaza)
 per platform
 DEployment Server 
 both platforms
 viazany na cislo ucto
 Deployment Server multi-pack
 both platforms
 neviazany na cislo 4D
 
 OEM
 bezi vsade 
 v deployment prostredi vybehne s "Expired" po expiracii
 
 Teraz ak cislo nei je platne:
 Ak nebolo zadane - zobrazuje a tlaci DEMO of PagePro
 ak bolo zadane, ale nie pre dane prostredie / verziu 4D, zobrazuje a tlaci DEMO of PagePro (alebo INVALID PagePro LICENSE)
 Ak je to OEM a je po expiracii, zobrazuje Expired PagePro License len v externej oblasti v deployment prostredi (t.j v Runtime a skompilovany vobec nekontroluje expiraciu) 
*/

#pragma	sym reset
#pragma export reset
