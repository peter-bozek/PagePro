/*
 *  DMUndo.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 04.12.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

#ifndef	_DMUndo_h_
#define	_DMUndo_h_
# include	"RWBaseTypes.h"
class	DMBase;
class	DMArea;

class	DMUndo
{
public:
							DMUndo (void);
							~DMUndo (void);

	SInt32					GetUndoRedoState (SInt32& canUndo, SInt32& canRedo) const;
	SInt32					StartRecording (DMArea *inArea, SInt32 inOperation, bool inSelection);
	void					PauseRecording (bool inPause);
	void					StopRecording (void);
	inline	bool			IsRecording (void) const;
	inline	bool			IsRecordingPaused (void) const;
	inline	bool			IsPerformingUndoRedo (void) const;
	SInt32					AddCreate (DMArea *inArea, DMBase *inObject);
	SInt32					AddDelete (DMArea *inArea, DMBase *inObject);
	SInt32					AddParentChange (DMArea *inArea, DMBase *inObject, DMBase *inNewParent);
	SInt32					AddProperty (DMArea *inArea, DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue);
	SInt32					AddSnapshot (DMArea *inArea, DMBase *inObject, bool inOldState);
	SInt32					Undo (DMArea *inArea, bool inClear = false);
	SInt32					Redo (DMArea *inArea);
	void					Clear (void);
	void					Clear (SInt32 inMaxCount);

	SInt32					GetIDForObject (DMBase *inObject);
	DMBase				*	GetObjectByID (SInt32 inObjectID);
	void					RetainObjectID (SInt32 inObjectID);
	void					ModifyObjectByID (SInt32 inObjectID, DMBase *inObject);
	void					ReleaseObjectID (SInt32 inObjectID);

#if	TARGET_DEBUG
			bool			DebugCheckObjects (DMArea *inArea);
			void			DebugShow (long inElement);
#endif

protected:
	void					AddSelection (DMArea *inArea);
	void					ClearFrom (SInt32 inFrom);

	class	DMUndoAction
	{
	public:
		enum	EActionKind
		{
			eAction_Create,
			eAction_Delete,
			eAction_ChangeParent,
			eAction_Property,
			eAction_Snapshot
		};
								DMUndoAction (SInt32 inObjectID);
		virtual					~DMUndoAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo) = 0;
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo) = 0;
		virtual	void			Destroy (DMUndo *inUndo) = 0;
		inline	SInt32			GetObjectID (void) const;

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry) = 0;
#endif

	protected:
		SInt32					fObjectID;
	};

	class	DMSelectionAction	: public DMUndoAction
	{
	public:
								DMSelectionAction (SInt32 inObjectID, size_t inCount);
//		virtual					~DMSelectionAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);
				void			AddObject (SInt32 inObjectID);

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif

	protected:
		RWList<SInt32>			fSelectedObjects;
	};

	class	DMCreateAction	: public DMUndoAction
	{
	public:
								DMCreateAction (SInt32 inObjectID, DMBase *inObject, SInt32 inParentObjectID);
		virtual					~DMCreateAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif

	protected:
		// Object is the created object, NIL after Undo, re-created after Redo
		SInt32					fParentObjectID;
		char	*				fData;
	};
	
	class	DMDeleteAction	: public DMUndoAction
	{
	public:
								DMDeleteAction (SInt32 inObjectID, DMBase *inObject, SInt32 inParentObjectID);
		virtual					~DMDeleteAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif

	protected:
		// Object is NIL, re-created after Undo, NIL again after Redo
		SInt32					fParentObjectID;
		char	*				fData;
	};
	
	class	DMChangeParentAction	: public DMUndoAction
	{
	public:
								DMChangeParentAction (SInt32 inObjectID, DMBase *inObject, SInt32 inOldParentObjectID, SInt32 inNewParentObjectID);
//		virtual					~DMChangeParentAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif

	protected:
		// Object is NIL, re-created after Undo, NIL again after Redo
		SInt32					fOldParentObjectID;
		SInt32					fNewParentObjectID;
	};
	
	class	DMPropertyAction	: public DMUndoAction
	{
	public:
								DMPropertyAction (SInt32 inObjectID, DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue);
//		virtual					~DMPropertyAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);

#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif

	protected:
		OSType					fID;
		RWValue					fOldValue;
		RWValue					fNewValue;
	};
	
	class	DMSnapshotAction	: public DMUndoAction
	{
	public:
		DMSnapshotAction (SInt32 inObjectID, DMBase *inObject, bool inOnUndo);
		virtual					~DMSnapshotAction (void);
		virtual	void			Undo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Redo (DMArea *inArea, DMUndo *inUndo);
		virtual	void			Destroy (DMUndo *inUndo);
		
#if	TARGET_DEBUG
		virtual	void			DebugShow (SInt32 inEntry);
#endif
		
	protected:
		// Object is NIL, re-created after Undo, NIL again after Redo
		bool					fOnUndo;
		char	*				fData;
	};
	
	class	DMUndoElement
	{
	public:
								DMUndoElement (SInt32 inOperation);

		void					Add (DMUndoAction *inAction);
		void					Undo (DMArea *inArea, DMUndo *inUndo);
		void					Redo (DMArea *inArea, DMUndo *inUndo);
		void					Destroy (DMUndo *inUndo);
		inline	SInt32			GetOperation (void) const;
		inline	SInt32			GetCount (void) const;

#if	TARGET_DEBUG
		void					DebugShow (SInt32 inEntry);
#endif

	protected:
		typedef	RWList<DMUndoAction*>	DMUndoActionList;

		SInt32					fOperation;
		DMUndoActionList		fActions;
	};

	struct	DMObjectMapEntry
	{
		DMBase				*	fObject;
		SInt32					fRetainCount;
	};
	typedef	RWMap<SInt32, DMObjectMapEntry>	DMObjectMap;
	typedef	RWList<DMUndoElement*>			DMActionList;
		
	DMObjectMap				fObjects;
	DMActionList			fActions;
	SInt32					fObjSeqID;
	SInt32					fCurrent;
	int						fRecording;	// 0 = no, 1 = started, 2 = selection only, 3 = recorded, 4 = paused (OR-ed)
	int						fPerformingUndoRedo;
	enum	{	kDefMaxUndoSize = 512 };
	SInt32					fMaxCount;
};

inline					DMUndo::DMUndoAction::~DMUndoAction (void)			{}
inline	SInt32			DMUndo::DMUndoAction::GetObjectID (void) const		{ return fObjectID; }
inline	SInt32			DMUndo::DMUndoElement::GetOperation (void) const	{ return fOperation; }
inline	SInt32			DMUndo::DMUndoElement::GetCount (void) const		{ return fActions.size(); }
inline	bool			DMUndo::IsRecording (void) const					{ return fRecording > 0 && fRecording < 4; }
inline	bool			DMUndo::IsRecordingPaused (void) const				{ return fRecording > 4; }
inline	bool			DMUndo::IsPerformingUndoRedo (void) const			{ return fPerformingUndoRedo != 0; }
inline	void			DMUndo::Clear (void)								{ ClearFrom (0); }

#endif
