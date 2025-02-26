#pragma once
#include "afxwin.h"

// CConfirmFormView �� ���Դϴ�.
class CEditMgr;
class CConfirmFormView : public CFormView
{
	DECLARE_DYNCREATE(CConfirmFormView)

protected:
	CConfirmFormView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CConfirmFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIRMFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate();

public://Message Function
	DECLARE_MESSAGE_MAP()

public:
	HRESULT		Ready_ConfirmFormView();
	void		Change_FocusText();

public:
	CEditMgr*	m_pEditMgr;
	CStatic		m_txtFocus;
	afx_msg void OnBnClickedX1();
	afx_msg void OnBnClickedX2();
	afx_msg void OnBnClickedY1();
	afx_msg void OnBnClickedY2();
	afx_msg void OnBnClickedZ1();
	afx_msg void OnBnClickedZ2();
	afx_msg void OnBnClickedReadyCamera();
	float m_fConstrolSpeed;
	float m_fCameraSpeed;
};


