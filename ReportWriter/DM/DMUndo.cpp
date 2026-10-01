/*
 *  DMUndo.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 04.12.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

# include	"DMUndo.h"
# include	"DMArea.h"
# include	"PSObjProps.h"
# include	"RWXml.h"
# include	<memory>

#if	TARGET_DEBUG
# include	<XStringTools.h>

extern "C" void			DebugShowDMUndo (long inThis, long inElement);
#if	WINVER
		inline	void  Debugger (void)	
		{
			__asm int 3;
		}
#endif

#endif


DMUndo::DMUndoAction::DMUndoAction (SInt32 inObjectID)
	:	fObjectID (inObjectID)
{
}


DMUndo::DMSelectionAction::DMSelectionAction (SInt32 inObjectID, size_t inCount)
	:	DMUndoAction (inObjectID)
{
	fSelectedObjects.reserve (inCount);
}

void
DMUndo::DMSelectionAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	inArea->DeselectAll();
	RWList<SInt32>::iterator	iter;
	for (iter = fSelectedObjects.begin(); iter != fSelectedObjects.end(); iter++)
	{
		DMBase	*obj = inUndo->GetObjectByID (*iter);
		if (obj != NULL)
			obj->SetSelected (true);
	}
}

void
DMUndo::DMSelectionAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	Undo (inArea, inUndo);
}

void
DMUndo::DMSelectionAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	{
		RWList<SInt32>::iterator	iter;
		for (iter = fSelectedObjects.begin(); iter != fSelectedObjects.end(); iter++)
			inUndo->ReleaseObjectID (*iter);
	}
	delete this;
}

void
DMUndo::DMSelectionAction::AddObject (SInt32 inObjectID)
{
	fSelectedObjects.push_back (inObjectID);
}

#if 0
void
DMUndo::DMSelectionAction::DebugShow (long inEntry)
{
	printf ("\t\tAction %ld: objectid=%ld, Selection:\n", inEntry, fObjectID);

	RWList<SInt32>::const_iterator	it;
	long	i;
	for (i = 0, it = fSelectedObjects.begin(); it != fSelectedObjects.end(); i++, it++)
	{
		printf ("\t\t\t%ld: objectid=%ld\n", i, *it);
	}
}
#endif


DMUndo::DMCreateAction::DMCreateAction (SInt32 inObjectID, DMBase *inObject, SInt32 inParentObjectID)
	:	DMUndoAction (inObjectID),
		fParentObjectID (inParentObjectID)
{
	// save current state
	RWXmlDocument	xml;
	inObject->WriteXML (xml.Node());
	fData = xml.SaveString (false);
}

DMUndo::DMCreateAction::~DMCreateAction (void)
{
}

void
DMUndo::DMCreateAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	if (obj)
	{
		inArea->RemoveObject (obj);
		inUndo->ModifyObjectByID (fObjectID, NULL);
		inArea->Modified();
	}
}

void
DMUndo::DMCreateAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*parent = inUndo->GetObjectByID (fParentObjectID);
	RWXmlDocument	xml;
	xml.LoadString (fData);
	DMBase	*obj = inArea->CreateObject (xml.Root(), (long) parent);
	inUndo->ModifyObjectByID (fObjectID, obj);
	inArea->Modified();
}

void
DMUndo::DMCreateAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	inUndo->ReleaseObjectID (fParentObjectID);
	delete this;
}

#if 0
void
DMUndo::DMCreateAction::DebugShow (long inEntry)
{
	printf ("\t\tAction %ld: objectid=%ld, Create: parentid=%ld\n",
			inEntry, fObjectID, fParentObjectID);
	printf ("\t\t\t%s\n", RWStr::ToUTF8 (fData).c_str());
}
#endif


DMUndo::DMDeleteAction::DMDeleteAction (SInt32 inObjectID, DMBase *inObject, SInt32 inParentObjectID)
	:	DMUndoAction (inObjectID),
		fParentObjectID (inParentObjectID)
{
	// save current state
	RWXmlDocument	xml;
	inObject->WriteXML (xml.Node());
	fData = xml.SaveString (false);
}

DMUndo::DMDeleteAction::~DMDeleteAction (void)
{
}

void
DMUndo::DMDeleteAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*parent = inUndo->GetObjectByID (fParentObjectID);
	RWXmlDocument	xml;
	xml.LoadString (fData);
	DMBase	*obj = inArea->CreateObject (xml.Root(), (long) parent);
	inUndo->ModifyObjectByID (fObjectID, obj);
	inArea->Modified();
}

void
DMUndo::DMDeleteAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	if (obj)
	{
		inArea->RemoveObject (obj);
		inUndo->ModifyObjectByID (fObjectID, NULL);
		inArea->Modified();
	}
}

void
DMUndo::DMDeleteAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	inUndo->ReleaseObjectID (fParentObjectID);
	delete this;
}

#if TARGET_DEBUG
void
DMUndo::DMDeleteAction::DebugShow (long inEntry)
{
	printf ("\t\tAction %ld: objectid=%ld, Delete: parentid=%ld\n",
			inEntry, fObjectID, fParentObjectID);
	printf ("\t\t\t%s\n", RWStr::ToUTF8 (fData).c_str());
}
#endif


DMUndo::DMChangeParentAction::DMChangeParentAction (SInt32 inObjectID, DMBase *inObject, SInt32 inOldParentObjectID, SInt32 inNewParentObjectID)
	:	DMUndoAction (inObjectID),
		fOldParentObjectID (inOldParentObjectID),
		fNewParentObjectID (inNewParentObjectID)
{
}

void
DMUndo::DMChangeParentAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*oldParent = inUndo->GetObjectByID (fOldParentObjectID);
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	inArea->ChangeObjectParent (obj, (long) oldParent);
	inArea->Modified();
}

void
DMUndo::DMChangeParentAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*newParent = inUndo->GetObjectByID (fNewParentObjectID);
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	inArea->ChangeObjectParent (obj, (long) newParent);
	inArea->Modified();
}

void
DMUndo::DMChangeParentAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	inUndo->ReleaseObjectID (fOldParentObjectID);
	inUndo->ReleaseObjectID (fNewParentObjectID);
	delete this;
}

#if 0
void
DMUndo::DMChangeParentAction::DebugShow (long inEntry)
{
	printf ("\t\tAction %ld: objectid=%ld, ChangeParent: oldparentid=%ld, new=%ld\n",
			inEntry, fObjectID, fOldParentObjectID, fNewParentObjectID);
}
#endif


DMUndo::DMPropertyAction::DMPropertyAction (SInt32 inObjectID, DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue)
	:	DMUndoAction (inObjectID),
		fID (inID),
		fOldValue (inOldValue),
		fNewValue (inNewValue)
{
}

void
DMUndo::DMPropertyAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	obj->SetProperty (fID, fOldValue);
	if (obj->GetKind() == PSObject::eObject_Style)	//mbs 28122009	notify renderer
		inArea->GetPageComposer()->StyleChanged (static_cast <DMStyle*> (obj));
	inArea->Modified();
}

void
DMUndo::DMPropertyAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	DMBase	*obj = inUndo->GetObjectByID (fObjectID);
	obj->SetProperty (fID, fNewValue);
	if (obj->GetKind() == PSObject::eObject_Style)	//mbs 28122009	notify renderer
		inArea->GetPageComposer()->StyleChanged (static_cast <DMStyle*> (obj));
	inArea->Modified();
}

void
DMUndo::DMPropertyAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	delete this;
}

#if 0
void
DMUndo::DMPropertyAction::DebugShow (long inEntry)
{
	char	cID [16];
	XStringTools::LongToFourChar (fID, cID);
	printf ("\t\tAction %ld: objectid=%ld, Property: id=%s\n", 
			inEntry, fObjectID, cID);
	RWTextValue	tv;
	fOldValue.GetTextValue (tv, NULL);
	printf ("\t\t\told: %d, %s\n", fOldValue.GetKind(), RWStr::ToUTF8 (tv).c_str());
	fNewValue.GetTextValue (tv, NULL);
	printf ("\t\t\tnew: %d, %s\n", fNewValue.GetKind(), RWStr::ToUTF8 (tv).c_str());
}
#endif


DMUndo::DMSnapshotAction::DMSnapshotAction (SInt32 inObjectID, DMBase *inObject, bool inOnUndo)
	:	DMUndoAction (inObjectID),
		fOnUndo (inOnUndo)
{
	// save current state
	RWXmlDocument	xml;
	inObject->WriteXML (xml.Node());
	fData = xml.SaveString (false);
}

DMUndo::DMSnapshotAction::~DMSnapshotAction (void)
{
}

void
DMUndo::DMSnapshotAction::Undo (DMArea *inArea, DMUndo *inUndo)
{
	if (fOnUndo)
	{
		RWValue	xml;
		xml.SetText (fData);
		DMBase	*obj = inUndo->GetObjectByID (fObjectID);
		obj->SetProperty (PSObjPropXML, xml);
		if (obj->GetKind() == PSObject::eObject_Style)	//mbs 28122009	notify renderer
			inArea->GetPageComposer()->StyleChanged (static_cast <DMStyle*> (obj));
		inArea->Modified();
	}
}

void
DMUndo::DMSnapshotAction::Redo (DMArea *inArea, DMUndo *inUndo)
{
	if (not fOnUndo)
	{
		RWValue	xml;
		xml.SetText (fData);
		DMBase	*obj = inUndo->GetObjectByID (fObjectID);
		obj->SetProperty (PSObjPropXML, xml);
		if (obj->GetKind() == PSObject::eObject_Style)	//mbs 28122009	notify renderer
			inArea->GetPageComposer()->StyleChanged (static_cast <DMStyle*> (obj));
		inArea->Modified();
	}
}

void
DMUndo::DMSnapshotAction::Destroy (DMUndo *inUndo)
{
	inUndo->ReleaseObjectID (fObjectID);
	delete this;
}

#if	TARGET_DEBUG
void
DMUndo::DMSnapshotAction::DebugShow (long inEntry)
{
	printf ("\t\tAction %ld: objectid=%ld, Snapshot: oldstate=%d\n",
			inEntry, fObjectID, int (fOnUndo));
	printf ("\t\t\t%s\n", RWStr::ToUTF8 (fData).c_str());
}
#endif


DMUndo::DMUndoElement::DMUndoElement (SInt32 inOperation)
	:	fOperation (inOperation)
{
}

void
DMUndo::DMUndoElement::Add (DMUndoAction *inAction)
{
	fActions.push_back (inAction);
}

void
DMUndo::DMUndoElement::Undo (DMArea *inArea, DMUndo *inUndo)
{
	DMUndoActionList::reverse_iterator	iter;
	for (iter = fActions.rbegin(); iter != fActions.rend(); iter++)
		(*iter)->Undo (inArea, inUndo);
}

void
DMUndo::DMUndoElement::Redo (DMArea *inArea, DMUndo *inUndo)
{
	DMUndoActionList::iterator	iter;
	for (iter = fActions.begin(); iter != fActions.end(); iter++)
		(*iter)->Redo (inArea, inUndo);
}

void
DMUndo::DMUndoElement::Destroy (DMUndo *inUndo)
{
	{
		DMUndoActionList::reverse_iterator	iter;
		for (iter = fActions.rbegin(); iter != fActions.rend(); iter++)
			(*iter)->Destroy (inUndo);
		fActions.clear();
	}
	delete this;
}

#if	0
void
DMUndo::DMUndoElement::DebugShow (long inEntry)
{
	printf ("\tElement %ld: operation=%ld, numActions=%lu\n",
			inEntry, fOperation, fActions.size());

	DMUndoElement::DMUndoActionList::iterator	it;
	long	i;
	for (i = 0, it = fActions.begin(); it != fActions.end(); i++, it++)
	{
		(*it)->DebugShow (i);
	}
}
#endif


DMUndo::DMUndo (void)
	:	fObjSeqID (0),
		fCurrent (0),
		fRecording (0),
		fPerformingUndoRedo (0),
		fMaxCount (kDefMaxUndoSize)
{
}


DMUndo::~DMUndo (void)
{
	{
		DMActionList::reverse_iterator	iter;
		for (iter = fActions.rbegin(); iter != fActions.rend(); iter++)
			(*iter)->Destroy (this);
		fObjects.clear();
		fActions.clear();
	}
}


SInt32
DMUndo::GetUndoRedoState (SInt32& canUndo, SInt32& canRedo)
const
{
	canUndo = fCurrent > 0? fActions [fCurrent - 1]->GetOperation(): 0;
	canRedo = fCurrent < (SInt32) fActions.size()? fActions [fCurrent]->GetOperation(): 0;
	return fActions.size();
}


SInt32
DMUndo::StartRecording (DMArea *inArea, SInt32 inOperation, bool inSelection)
{
	if (fMaxCount < 1)
		return noErr;

	if (fRecording)
	{
		StopRecording();
	}
	
	// clear redo buffer
	if (fCurrent < (SInt32) fActions.size())
		ClearFrom (fCurrent);

	DMUndoElement	*elem = new DMUndoElement (inOperation);
	fActions.push_back (elem);
	fCurrent = fActions.size();
	fRecording = 1;
	if (inSelection)
		AddSelection (inArea);

	// limit the undo buffer size
	while ((SInt32) fActions.size() > fMaxCount)
	{
		elem = fActions.front();
		elem->Destroy (this);
		fActions.erase (fActions.begin());
	}
	
	return noErr;
}


void
DMUndo::PauseRecording (bool inPause)
{
	if (inPause && IsRecording())
		fRecording |= 4;
	else if (not inPause && fRecording > 4)
		fRecording &= ~4;
	return;
}


void
DMUndo::StopRecording (void)
{
	PauseRecording (false);
	if (not IsRecording())
		return;

	DMUndoElement	*elem = fActions.back();
	if (fRecording < 3)
	{
		elem->Destroy (this);
		fActions.pop_back();	// fActions.erase (fActions.end() - 1);
	}
	fCurrent = fActions.size();
	fRecording = 0;
#if	TARGET_DEBUG
	if (DebugCheckObjects (NULL))
	{
		printf ("DMUndo::StopRecording: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
}


void
DMUndo::AddSelection (DMArea *inArea)
{
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddSelection: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
	fRecording = 2;
	SInt32	thisObj = GetIDForObject (inArea);
	std::unique_ptr<PSObjList>	l (inArea->GetObjects (PSObjPropSelected));
	PSObjList	*selection = l.get();
	DMSelectionAction	*action = new DMSelectionAction (thisObj, selection->size());
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	PSObjList::iterator	iter;
	for (iter = selection->begin(); iter != selection->end(); iter++)
	{
		DMBase	*obj = static_cast <DMBase*> (*iter);
		switch (obj->GetKind())
		{
				//mbs 22122009	Header & Column can be re-created every time - whole Table is stored as XML
			case PSObject::eObject_TblHdr:
			case PSObject::eObject_TblCol:
//				//mbs 22122009	selection of a DataSource or Style is a non-sense
//			case PSObject::eObject_DataSource:
//			case PSObject::eObject_Style:
				break;
			default:
				thisObj = GetIDForObject (obj);
				action->AddObject (thisObj);
				break;
		}
	}
}


SInt32
DMUndo::AddCreate (DMArea *inArea, DMBase *inObject)
{
	if (not IsRecording())
		return paramErr;
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddCreate: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
//	if (fRecording == 1)
//		AddSelection (inArea, NULL);

	fRecording = 3;
	SInt32	thisObj = GetIDForObject (inObject);
	SInt32	parentObj = GetIDForObject (inObject->GetParent());
	DMCreateAction	*action = new DMCreateAction (thisObj, inObject, parentObj);
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	return noErr;
}


SInt32
DMUndo::AddDelete (DMArea *inArea, DMBase *inObject)
{
	if (not IsRecording())
		return paramErr;
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddDelete: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
//	if (fRecording == 1)
//		AddSelection (inArea, NULL);

	fRecording = 3;
	SInt32	thisObj = GetIDForObject (inObject);
	SInt32	parentObj = GetIDForObject (inObject->GetParent());
	DMDeleteAction	*action = new DMDeleteAction (thisObj, inObject, parentObj);
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	ModifyObjectByID (thisObj, NULL);	//mbs 21122009	it will be deleted really soon...
	return noErr;
}


SInt32
DMUndo::AddParentChange (DMArea *inArea, DMBase *inObject, DMBase *inOldParent)
{
	if (not IsRecording())
		return paramErr;
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddParentChange: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
//	if (fRecording == 1)
//		AddSelection (inArea, NULL);

	fRecording = 3;
	SInt32	thisObj = GetIDForObject (inObject);
	SInt32	newParentObj = GetIDForObject (inObject->GetParent());
	SInt32	oldParentObj = GetIDForObject (inOldParent);
	DMChangeParentAction	*action = new DMChangeParentAction (thisObj, inObject, oldParentObj, newParentObj);
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	return noErr;
}


SInt32
DMUndo::AddProperty (DMArea *inArea, DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue)
{
	if (not IsRecording())
		return paramErr;
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddProperty: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
//	if (fRecording == 1)
//		AddSelection (inArea, NULL);

	fRecording = 3;
	SInt32	thisObj = GetIDForObject (inObject);
	DMPropertyAction	*action = new DMPropertyAction (thisObj, inObject, inID, inOldValue, inNewValue);
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	return noErr;
}


SInt32
DMUndo::AddSnapshot (DMArea *inArea, DMBase *inObject, bool inOldState)
{
	if (not IsRecording())
		return paramErr;
#if	TARGET_DEBUG
	if (DebugCheckObjects (inArea))
	{
		printf ("DMUndo::AddSnapshot: Object map damaged (some object was deleted) - clearing undo buffer\n");
		DebugShow (fCurrent);
		Debugger();
		ClearFrom (0);
	}
#endif
//	if (fRecording == 1)
//		AddSelection (inArea, NULL);
	
	fRecording = 3;
	SInt32	thisObj = GetIDForObject (inObject);
	DMSnapshotAction	*action = new DMSnapshotAction (thisObj, inObject, inOldState);
	DMUndoElement	*elem = fActions.back();
	elem->Add (action);
	return noErr;
}


SInt32
DMUndo::Undo (DMArea *inArea, bool inClear)
{
	StopRecording();
	if (fCurrent > 0)
	{
		fPerformingUndoRedo = 1;
		fCurrent--;
		DMUndoElement	*elem = fActions [fCurrent];
		elem->Undo (inArea, this);
		if (inClear)	//mbs 02082010
			ClearFrom (fCurrent);
		fPerformingUndoRedo = 0;
		return noErr;
	}
	return paramErr;
}


SInt32
DMUndo::Redo (DMArea *inArea)
{
	StopRecording();
	if (fCurrent < (SInt32) fActions.size())
	{
		fPerformingUndoRedo = 2;
		DMUndoElement	*elem = fActions [fCurrent];
		elem->Redo (inArea, this);
		fCurrent++;
		fPerformingUndoRedo = 0;
		return noErr;
	}
	return paramErr;
}


void
DMUndo::ClearFrom (SInt32 inFrom)
{
	StopRecording();
	while ((SInt32) fActions.size() > inFrom)
	{
		DMUndoElement	*elem = fActions.back();
		elem->Destroy (this);
		fActions.pop_back();
	}
	fCurrent = inFrom;
	if (inFrom == 0)
		fObjSeqID = 0;
}


void
DMUndo::Clear (SInt32 inMaxCount)
{
	StopRecording();
	ClearFrom (0);
	if (inMaxCount > 0)
		fMaxCount = inMaxCount + 1;	// +1 for last Redo
	else
		fMaxCount = 0;
}


SInt32
DMUndo::GetIDForObject (DMBase *inObject)
{
	DMObjectMap::iterator	it;
	for (it = fObjects.begin(); it != fObjects.end(); it++)
		if (it->second.fObject == inObject)
		{
			it->second.fRetainCount++;	// == RetainObjectID (it->first);
			return it->first;
		}

	DMObjectMapEntry		e;
	e.fObject = inObject;
	e.fRetainCount = 1;
	DMObjectMap::value_type	value (++fObjSeqID, e);
	fObjects.insert (value);
//	printf ("DMUndo::GetIDForObject (%lx) -> %ld\n", (long) inObject, fObjSeqID);
	return fObjSeqID;
}


DMBase*
DMUndo::GetObjectByID (SInt32 inObjectID)
{
//	DMObjectMap::key_type	key (inObjectID);
	DMObjectMap::iterator	it = fObjects.find (inObjectID);
	assert (it != fObjects.end());
#if	TARGET_DEBUG
	DMObjectMapEntry	&e = it->second;
	if (e.fObject == NULL)
	{
		printf ("DMUndo::GetObjectByID (%ld) -> NULL object! (retainCount=%ld)\n", inObjectID, e.fRetainCount);
		DebugShow (fCurrent);
	}
	else if (DMReport::GetReportOfObject ((long) e.fObject) == NULL)
	{
		printf ("DMUndo::GetObjectByID (%ld) -> object %lx does not exist!!! (retainCount=%ld)\n", inObjectID, (long) e.fObject, e.fRetainCount);
		DebugShow (fCurrent);
		e.fObject = NULL;
	}
#endif
	return it->second.fObject;
}


void
DMUndo::RetainObjectID (SInt32 inObjectID)
{
	DMObjectMap::iterator	it = fObjects.find (inObjectID);
	assert (it != fObjects.end());
	it->second.fRetainCount++;
}


void
DMUndo::ModifyObjectByID (SInt32 inObjectID, DMBase *inObject)
{
	DMObjectMap::iterator	it = fObjects.find (inObjectID);
	assert (it != fObjects.end());
	it->second.fObject = inObject;
}


void
DMUndo::ReleaseObjectID (SInt32 inObjectID)
{
	DMObjectMap::iterator	it = fObjects.find (inObjectID);
	assert (it != fObjects.end());
#if	TARGET_DEBUG
	DMObjectMapEntry	&e = it->second;
	if (e.fRetainCount < 1)
	{
		printf ("DMUndo::ReleaseObjectID (%ld) -> invalid retainCount=%ld\n", inObjectID, e.fRetainCount);
		DebugShow (fCurrent);
	}
#endif
	it->second.fRetainCount--;
	if (it->second.fRetainCount == 0)
		fObjects.erase (it);
}


#if	TARGET_DEBUG
bool
DMUndo::DebugCheckObjects (DMArea *inArea)
{
	bool	isBad = false;
	DMObjectMap::iterator	it;
	long	i;
	for (i = 0, it = fObjects.begin(); it != fObjects.end(); i++, it++)
	{
		DMObjectMapEntry		&e = it->second;
		if (e.fObject != NULL && (inArea != NULL? inArea->GetObject ((long) e.fObject): DMReport::GetReportOfObject ((long) e.fObject)) == NULL)
		{
			isBad = true;
			break;
		}
	}
	return isBad;
}

void
DMUndo::DebugShow (long inElement)
{
	void	*a = (void*) &DebugShowDMUndo;
	a = a;

	printf ("DMUndo: %#lx, count=%lu, current=%ld, recording=%d\n",
			(long) this, fActions.size(), fCurrent, fRecording);
	printf ("\tObjectMap: count=%lu, current=%ld\n", fObjects.size(), fObjSeqID);
	DMObjectMap::iterator	it;
	long	i;
	for (i = 0, it = fObjects.begin(); it != fObjects.end(); i++, it++)
	{
		DMObjectMap::value_type	&v = *it;
		DMObjectMapEntry		&e = it->second;
		printf ("\t\t%ld: id=%ld, object=%#lx, refcount=%ld",
				i, v.first, (long) e.fObject, e.fRetainCount);
		if (e.fObject == NULL)
			printf ("\tNULL object!\n");
		else if (DMReport::GetReportOfObject ((long) e.fObject) == NULL)
			printf ("\tinvalid object - does not exist!!!\n");
		else
			printf ("\tDM%s*\n", e.fObject->sKind [e.fObject->GetKind()]);
	}

	if (inElement >= 0 && inElement < (long) fActions.size())
	{
		fActions [inElement]->DebugShow (inElement);
	}
	else
	{
		RWList<DMUndoElement*>::const_iterator	it;
		for (i = 0, it = fActions.begin(); it != fActions.end(); i++, it++)
		{
			(*it)->DebugShow (i);
		}
	}
}

extern "C" void			DebugShowDMUndo (long inThis, long inElement)
{
	DMUndo	*o = reinterpret_cast <DMUndo*> (inThis);
	o->DebugShow (inElement);
}
#endif
