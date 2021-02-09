
// PMSHJDoc.h : interface of the CPMSHJDoc class
//


#pragma once
#include "PMSHJSet.h"


class CPMSHJDoc : public CDocument
{
protected: // create from serialization only
	CPMSHJDoc();
	DECLARE_DYNCREATE(CPMSHJDoc)

// Attributes
public:
	CPMSHJSet m_PMSHJSet;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CPMSHJDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
