// 4i902Doc.cpp : implementation of the Ci902Doc class
//

#include "stdafx.h"
#include "4i902.h"

#include "4i902Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci902Doc

IMPLEMENT_DYNCREATE(Ci902Doc, CDocument)

BEGIN_MESSAGE_MAP(Ci902Doc, CDocument)
END_MESSAGE_MAP()


// Ci902Doc construction/destruction

Ci902Doc::Ci902Doc()
{
	// TODO: add one-time construction code here

}

Ci902Doc::~Ci902Doc()
{
}

BOOL Ci902Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// Ci902Doc serialization

void Ci902Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// Ci902Doc diagnostics

#ifdef _DEBUG
void Ci902Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci902Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci902Doc commands
