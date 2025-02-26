#pragma once
#include "afxwin.h"
// CListTool 대화 상자입니다.

class CListTool : public CDialog
{
	DECLARE_DYNAMIC(CListTool)

public:
	CListTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CListTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LISTTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLbnSelchangeTextureList();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

public:
	void Release();
	void CreateHorizontalScroll();

public:
	CEditMgr* m_pEditMgr;
	CListBox m_TextureListBox;
	std::list<Engine::PATH_INFO*> m_lstPathInfo;
};
