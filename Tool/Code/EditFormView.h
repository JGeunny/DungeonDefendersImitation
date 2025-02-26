#pragma once

class CMapTool;
class CNaviTool;
class CCharTool;
class CCameraTool;
class CEventTool;
class CSoundTool;
class CListTool;
class CEffectTool;
class CDirectionTool;

// CEditFormView 폼 뷰입니다.

class CEditFormView : public CFormView
{
	DECLARE_DYNCREATE(CEditFormView)

protected:
	CEditFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEditFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();

public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked_MapTool();
	afx_msg void OnBnClicked_NaviTool();
	afx_msg void OnBnClicked_CameraTool();
	afx_msg void OnBnClicked_ListTool();
	//afx_msg void OnBnClicked_EffectTool();
	afx_msg void OnBnClicked_DirectionTool();

public:
	void			Release();

public:
	CMapTool*		m_pMapTool;
	CNaviTool*		m_pNaviTool;
	CCharTool*		m_pCharTool;
	CCameraTool*	m_pCameraTool;
	CEventTool*		m_pEventTool;
	CSoundTool*		m_pSoundTool;
	CListTool*		m_pListTool;
	CEffectTool*	m_pEffectTool;
	CDirectionTool* m_pDirectionTool;
};


