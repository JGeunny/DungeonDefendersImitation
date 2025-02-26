
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

protected://Virtual Funtion
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	CSplitterWnd m_RightWnd;	// 분리된 오른쪽
	CSplitterWnd m_LeftWnd;		// 분리된 왼쪽

public:
	CEditMgr*	m_pEditMgr;
};


