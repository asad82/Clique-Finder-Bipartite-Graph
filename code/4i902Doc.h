// 4i902Doc.h : interface of the Ci902Doc class
//


#pragma once

class Ci902Doc : public CDocument
{
protected: // create from serialization only
	Ci902Doc();
	DECLARE_DYNCREATE(Ci902Doc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~Ci902Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


