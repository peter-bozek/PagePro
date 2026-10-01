# include	"SR4DData.h"
# include	"RWString4D.h"
# include	"4DPluginAPI.h"
// using namespace    FourDAPIEx;


SR4DData::SR4DData (void)
	:	mNeedsFetch (true),
		mChanged (true),
		mSize (1)
{
	return;
}


SR4DData::~SR4DData (void)
{
}


void
SR4DData::GetOldValue (RWValue &outValue)
const
{
	outValue.Attach (mOld);
	return;
}


void
SR4DData::GetValue (RWValue &outValue)
const
{
	outValue.Clone (mCurrent); // pB changed from  	outValue.Attach (mCurrent);

	return;
}


void
SR4DData::DetachValue (RWValue &outValue)
{
	outValue.Detach (mCurrent);
	return;
}


bool
SR4DData::IsChanged (void)
const
{
	return mChanged;
}


void
SR4DData::Invalidate (void)
{
	mNeedsFetch = true;
	return;
}


void
SR4DData::Shunt (void)
{
	mOld.Detach (mCurrent);
	return;
}


long
SR4DData::GetSize (void)
const
{
	return mSize;
}


void
SR4DData::GetPictureAsBlobFrom4D (void *ph, RWValue &outValue)
{
	outValue.SetPicture  (RWValue::eValue_BLOB, 0, 0, true);
	if (ph != NULL)
	{
		PA_Unistring	u = PA_GetPictureData ((PA_Picture) ph, 1, NULL);
		if (PA_GetLastError() == 0)	// at least one format is available
		{
			PA_Variable	v2[2];
			memset (&v2, 0, sizeof (v2));
			v2[0].fType = eVK_Picture;
			v2[0].uValue.fPicture = ph;
			v2[1] = PA_CreateVariable (eVK_Blob);
			PA_Variable	result = PA_ExecuteCommandByID (532, v2, 2);	// VARIABLE TO BLOB
			long	len = PA_GetBlobVariable (v2[1], NULL);
			if (len > 0)
			{
				void	*blob = malloc (len);
				if (blob)
				{
					len = PA_GetBlobVariable (v2[1], blob);
					outValue.SetPicture (RWValue::eValue_BLOB, blob, len, true);
				}
			}
			PA_ClearVariable (&result);
//			PA_ClearVariable (&v2[0]);	this belongs to 4D var/field
			PA_ClearVariable (&v2[1]);
		}
		PA_DisposeUnistring (&u);
	}
}


void
SR4DData::GetPictureFrom4D (void *ph, RWValue &outValue, bool inForPDF)
{
	outValue.SetPicture  (RWValue::eValue_BLOB, 0, 0, true);
	if (ph != NULL)
	{
		PA_Unistring			u;
		long					index = 1, best = 0;
		RWValue::EValue_Kind	bestKind = RWValue::eValue_Undefined;

		while (true)
		{
			u = PA_GetPictureData (ph, index, NULL);
			if (PA_GetLastError() != 0)
				break;
			RWString	s = RWStr::FromPA (&u);

			if (s.find (u".pict", 1) != RWString::npos)
			{
				best = index;
				bestKind = RWValue::eValue_PicturePICT;
			}
			else if (s.find (u".pdf;", 1) != RWString::npos)
			{
				best = index;
				bestKind = RWValue::eValue_PicturePDF;
			}
			else if (s.find (u".jpg;", 1) != RWString::npos)
			{
				best = index;
				bestKind = RWValue::eValue_PictureJPG;
			}
			else if (s.find (u".png;", 1) != RWString::npos)
			{
				best = index;
				bestKind = RWValue::eValue_PicturePNG;
			}
			else if (s.find (u".tif;", 1) != RWString::npos)
			{
				best = index;
				bestKind = RWValue::eValue_PictureTIFF;
			}
			PA_DisposeUnistring (&u);
			// PDF (PoDoFo 1.0): JPEG is embedded unchanged, PNG decoded and Flate compressed (alpha becomes a soft mask)
			if (bestKind > RWValue::eValue_PicturePICT && (not inForPDF || bestKind == RWValue::eValue_PictureJPG || bestKind == RWValue::eValue_PicturePNG))
				break;
			index++;
		}
		if (bestKind != RWValue::eValue_Undefined)
		{
			if (inForPDF && bestKind != RWValue::eValue_PictureJPG && bestKind != RWValue::eValue_PicturePNG)
			{
				// lossless and keeps transparency (was .jpg)
				PA_Unichar		fmt [] = { '.', 'p', 'n', 'g', 0 };
				PA_Unistring	uni = PA_CreateUnistring (fmt);
/*
 switch (inKind)
				{
					case RWValue::eValue_PictureJPG:
					{
						PA_Unichar	jpeg [] = { '.', 'j', 'p', 'g', 0 };
						uni = PA_CreateUnistring (jpeg);
						break;
					}
					case RWValue::eValue_PicturePNG:
					{
						PA_Unichar	png [] = { '.', 'p', 'n', 'g', 0 };
						uni = PA_CreateUnistring (png);
						break;
					}
					case RWValue::eValue_PictureTIFF:
					{
						PA_Unichar	tiff [] = { '.', 't', 'i', 'f', 0 };
						uni = PA_CreateUnistring (tiff);
						break;
					}
					default:
						return;
				}
*/
				PA_Variable	pv[2];
				PA_SetPictureVariable (&pv[0], PA_DuplicatePicture (ph, true));	//••••••••• TODO ••• check this
				PA_SetStringVariable (&pv[1], &uni);
				PA_Variable	v = PA_ExecuteCommandByID (1002, pv, 2);	// CONVERT PICTURE (pict:P, codec:S)	- does it use QuickTime?!?
				PA_ClearVariable (&v);
				PA_ClearVariable (&pv[1]);
				GetPictureFrom4D (PA_GetPictureVariable (pv[0]), outValue, false);
				PA_ClearVariable (&pv[0]);
			}
			else
			{
				PA_Handle		data = PA_NewHandle (0);
				u = PA_GetPictureData ((PA_Picture) ph, best, data);
				if (PA_GetLastError() == 0)
				{
					long	size = PA_GetHandleSize (data);
					if (size > 0)
					{
						void	*pd = PA_LockHandle (data);
						RWValue	temp (bestKind, pd, size);
						outValue.Detach (temp);
						PA_UnlockHandle (data);
					}
				}
				if (data)
					PA_DisposeHandle (data);
				PA_DisposeUnistring (&u);
			}
		}
	}
}


void*
SR4DData::Get4DPicture (const RWValue &inValue)
{
	PA_Picture	pict = NULL;
	if (inValue.GetBlobSize() > 0)
	{
		if (inValue.GetKind() == RWValue::eValue_BLOB)
		{
			PA_Variable	v2[2];
			memset (&v2, 0, sizeof (v2));
			v2[0] = PA_CreateVariable (eVK_Blob);
			v2[1] = PA_CreateVariable (eVK_Picture);
			PA_SetBlobVariable (&v2[0], inValue.GetBlobData(), inValue.GetBlobSize());
			PA_Variable	result = PA_ExecuteCommandByID (533, v2, 2);	// BLOB TO VARIABLE
			PA_ClearVariable (&result);
			PA_ClearVariable (&v2[0]);
			if (PA_GetVariableKind (v2[1]) == eVK_Picture)
				pict = PA_GetPictureVariable (v2[1]);
			else
				PA_ClearVariable (&v2[1]);
		}
		else
			pict = PA_CreatePicture (inValue.GetBlobData(), inValue.GetBlobSize());
	}
	return pict;
}


void
SR4DData::GetCurrentPicture (void *ph)
{
	GetPictureAsBlobFrom4D (ph, mCurrent);
    PA_DisposePicture((PA_Picture) ph);
//	GetPictureFrom4D (ph, mCurrent, false);
}



SR4DVariable::SR4DVariable (const RWString inName, long inIndex)
	:	SR4DData(),
		mIndex (inIndex)
{
	if (!inName.empty() && inName[0] == u'=')	//mbs 17062010
	{
		mIndex = SR4DVariable_Expression;
		mName = inName.substr (1);
	}
	else
		mName = inName;
	return;
}



SR4DVariable::~SR4DVariable (void)
{
	return;
}


void
SR4DVariable::Fetch (long inIteration, bool inSeek)
{
	if (mNeedsFetch)
	{
		mNeedsFetch = false;
		if (mIndex >= 0)
			inIteration = mIndex;
		mCurrent.Free();

		PA_Variable	v;
		if (mIndex == SR4DVariable_Expression)	//mbs 17062010
		{
			PA_Unistring	ustr = RWStr::CreatePA (mName);	
			v = PA_ExecuteFunction (&ustr);
			PA_DisposeUnistring (&ustr);
		}
		else
		{
			v = PA_GetVariable (RWStr::ToPA (mName));
			if (PA_GetVariableKind (v) == eVK_Undefined)
			{
				PA_Unistring	ustr = RWStr::CreatePA (mName);
				v = PA_ExecuteFunction (&ustr);
				PA_DisposeUnistring (&ustr);
			}
		}
		
		mSize = 1;

		short			day, month, year;

		switch (PA_GetVariableKind (v))
		{
			case eVK_Undefined:		// undefined
			case eVK_Pointer:		// unsupported
			case eVK_Blob:			// unsupported
			case eVK_ArrayOfArray:	// unsupported
			case eVK_ArrayPointer:	// unsupported
				break;

			case eVK_Real:
				mCurrent.SetReal (PA_GetRealVariable (v));
				break;

			case eVK_Date:
			{
				PA_GetDateVariable (v, &day, &month, &year);
				// day: 0 - 31 ==> 5 bits
				// month: 0 - 12 ==> 4 bits
				// day | (month << 5) | (year << 9)
				mCurrent.SetInteger ((day & 0x1F) | ((month & 0xF) << 5) | (((long)year) << 9), RWValue::eValue_Date);
				break;
			}

			case eVK_Boolean:
				mCurrent.SetInteger (long (PA_GetBooleanVariable (v) != 0), RWValue::eValue_Boolean);
				break;

			case eVK_Integer:
			case eVK_Longint:
				mCurrent.SetInteger (PA_GetLongintVariable (v));
				break;

			case eVK_Picture:
				GetCurrentPicture (PA_GetPictureVariable (v));
				break;

			case eVK_Time:
				mCurrent.SetInteger (PA_GetTimeVariable (v), RWValue::eValue_Time);
				break;

			case eVK_Unistring:
			{
				PA_Unistring	u4d = PA_GetStringVariable (v);
					mCurrent.SetText (RWStr::FromPA (&u4d));
				PA_DisposeUnistring (&u4d);
				break;
			}

			case eVK_ArrayReal:
			case eVK_ArrayInteger:
			case eVK_ArrayLongint:
			case eVK_ArrayDate:
			case eVK_ArrayPicture:
			case eVK_ArrayBoolean:
			case eVK_ArrayUnicode:
				mSize = PA_GetArrayNbElements (v);
				if (inIteration != SR4DVariable_Variable)
				{
					switch (PA_GetVariableKind (v))
					{
						case eVK_ArrayReal:
							mCurrent.SetReal (PA_GetRealInArray (v, inIteration));
							break;

						case eVK_ArrayInteger:
							mCurrent.SetInteger ((long) PA_GetIntegerInArray (v, inIteration));
							break;

						case eVK_ArrayLongint:
							mCurrent.SetInteger (PA_GetLongintInArray (v, inIteration));
							break;

						case eVK_ArrayDate:
						{
							PA_GetDateInArray (v, inIteration, &day, &month, &year);
							// day: 0 - 31 ==> 5 bits
							// month: 0 - 12 ==> 4 bits
							// day | (month << 5) | (year << 9)
							mCurrent.SetInteger ((day & 0x1F) | ((month & 0xF) << 5) | (((long)year) << 9), RWValue::eValue_Date);
							break;
						}


						case eVK_ArrayPicture:
							GetCurrentPicture (PA_GetPictureInArray (v, inIteration));
							break;

						case eVK_ArrayBoolean:
							mCurrent.SetInteger (long (PA_GetBooleanInArray (v, inIteration) != 0), RWValue::eValue_Boolean);
							break;

						case eVK_ArrayUnicode:
						{
							PA_Unistring	u4d = PA_GetStringInArray (v, inIteration);
								mCurrent.SetText (RWStr::FromPA (&u4d));
							break;
						}

						default:	// to shut up compiler
							break;
					}
				}
				else	// we should use a Variable, but it is an array -> get current
				{
					mCurrent.SetInteger (PA_GetArrayCurrent (v));
				}
				break;
		}

		mChanged = (mOld != mCurrent);
	}

	return;
}



SR4DField::SR4DField (long inTable, long inField)
	:	SR4DData(),
		mTable (inTable),
		mField (inField),
		mFieldType (-2)
{
	return;
}


void
SR4DField::Fetch (long inIteration, bool inSeek)
{
	if (mNeedsFetch)
	{
		mNeedsFetch = false;
//		mOld = mCurrent;
		mCurrent.Free();

		if (mFieldType == -2)
		{
			PA_FieldKind	kind;
			short			stringlength;
			char			indexed;
			PA_long32		attributes;

			PA_GetFieldProperties (mTable, mField, &kind, &stringlength, &indexed, &attributes);
			mFieldType = kind;
		}

		mSize = PA_RecordsInSelection (mTable);
		if (inSeek && inIteration > 0 && inIteration <= mSize)
		{
			PA_UseAutomaticRelations (false, false);
			PA_GotoSelectedRecord (mTable, inIteration);
		}

		if ((inIteration > 0 && inIteration <= mSize) || not inSeek)
		{
			switch (mFieldType)
			{
				case eFK_InvalidFieldKind:	// undefined
				case eFK_SubfileField:		// unsupported
				case eFK_BlobField:			// unsupported
					break;

				case eFK_AlphaField:
				case eFK_TextField:
				{
					PA_Unistring	u4d = PA_GetStringField (mTable, mField);
						mCurrent.SetText (RWStr::FromPA (&u4d));
					PA_DisposeUnistring (&u4d);
					break;
				}

				case eFK_RealField:
					mCurrent.SetReal (PA_GetRealField (mTable, mField));
					break;

				case eFK_PictureField:
					GetCurrentPicture (PA_GetPictureField (mTable, mField));
					break;

				case eFK_DateField:
				{
					short	day, month, year;
					PA_GetDateField (mTable, mField, &day, &month, &year);
					// day: 0 - 31 ==> 5 bits
					// month: 0 - 12 ==> 4 bits
					// day | (month << 5) | (year << 9)
					mCurrent.SetInteger ((day & 0x1F) | ((month & 0xF) << 5) | (((long)year) << 9), RWValue::eValue_Date);
					break;
				}

				case eFK_BooleanField:
					mCurrent.SetInteger (long (PA_GetBooleanField (mTable, mField) != 0), RWValue::eValue_Boolean);
					break;

				case eFK_IntegerField:
					mCurrent.SetInteger ((long) PA_GetIntegerField (mTable, mField));
					break;

				case eFK_LongintField:
					mCurrent.SetInteger (PA_GetLongintField (mTable, mField));
					break;

				case eFK_TimeField:
					mCurrent.SetInteger (PA_GetTimeField (mTable, mField), RWValue::eValue_Time);
					break;
			}
		}

		mChanged = (mOld != mCurrent);
	}

	return;
}
