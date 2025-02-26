// ConfirmFormView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "ConfirmFormView.h"
#include "EditFormView.h"
#include "EditMgr.h"

#include "MapTool.h"
#include "NaviTool.h"
#include "CameraTool.h"
#include "DirectionTool.h"

#include "MainScene.h"

// CConfirmFormView

IMPLEMENT_DYNCREATE(CConfirmFormView, CFormView)

CConfirmFormView::CConfirmFormView()
	: CFormView(IDD_CONFIRMFORMVIEW)
	, m_pEditMgr(CEditMgr::GetInstance())
	, m_fConstrolSpeed(1.f)
	, m_fCameraSpeed(1.f)
{

}

CConfirmFormView::~CConfirmFormView()
{
}

void CConfirmFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOCUS, m_txtFocus);
	DDX_Text(pDX, IDC_EDIT21, m_fConstrolSpeed);
	DDX_Text(pDX, IDC_EDIT22, m_fCameraSpeed);
}

BEGIN_MESSAGE_MAP(CConfirmFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON17, &CConfirmFormView::OnBnClickedX1)
	ON_BN_CLICKED(1070, &CConfirmFormView::OnBnClickedX2)
	ON_BN_CLICKED(1065, &CConfirmFormView::OnBnClickedY1)
	ON_BN_CLICKED(IDC_BUTTON35, &CConfirmFormView::OnBnClickedY2)
	ON_BN_CLICKED(IDC_BUTTON20, &CConfirmFormView::OnBnClickedZ1)
	ON_BN_CLICKED(1071, &CConfirmFormView::OnBnClickedZ2)
	ON_BN_CLICKED(IDC_BUTTON36, &CConfirmFormView::OnBnClickedReadyCamera)
END_MESSAGE_MAP()


// CConfirmFormView �����Դϴ�.

#ifdef _DEBUG
void CConfirmFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CConfirmFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CConfirmFormView �޽��� ó�����Դϴ�.


void CConfirmFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CScrollView::SetScrollSizes(MM_TEXT, CSize(0, 0));

	CEditMgr::GetInstance()->m_pConfirmView = this;
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

HRESULT CConfirmFormView::Ready_ConfirmFormView()
{
	m_txtFocus.SetFont(&m_pEditMgr->m_Font_Default, TRUE);
	return S_OK;
}

void CConfirmFormView::Change_FocusText()
{
	static FOCUSID eFocus_Before = FOCUSID_END;
	if (eFocus_Before == m_pEditMgr->m_eFocus) return;
	eFocus_Before = m_pEditMgr->m_eFocus;
	CString strFocus = L"��Ȱ��ȭ";
	switch (eFocus_Before)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		strFocus = L"����";
		break;
	case FOCUSID_NAVITOOL:
		strFocus = L"�׺���";
		break;
	case FOCUSID_CAMERATOOL:
		strFocus = L"ī�޶���";
		break;
	case FOCUSID_LISTTOOL:
		strFocus = L"����Ʈ��";
		break;
	case FOCUSID_DIRECTIONTOOL:
		strFocus = L"��ã����";
		break;
	default:
		break;
	}

	m_txtFocus.SetWindowTextW(strFocus);
}


void CConfirmFormView::OnBnClickedX1()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_X, m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_X, m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_X, m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_X, m_fConstrolSpeed);
		break;
	default:
		break;
	}
}

void CConfirmFormView::OnBnClickedX2()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_X, -m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_X, -m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_X, -m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_X, -m_fConstrolSpeed);
		break;
	default:
		break;
	}
}

void CConfirmFormView::OnBnClickedY1()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_Y, m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_Y, m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_Y, m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_Y, m_fConstrolSpeed);
		break;
	default:
		break;
	}
}

void CConfirmFormView::OnBnClickedY2()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_Y, -m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_Y, -m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_Y, -m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_Y, -m_fConstrolSpeed);
		break;
	default:
		break;
	}
}

void CConfirmFormView::OnBnClickedZ1()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_Z, m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_Z, m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_Z, m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_Z, m_fConstrolSpeed);
		break;
	default:
		break;
	}
}

void CConfirmFormView::OnBnClickedZ2()
{
	UpdateData(TRUE);
	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->BnClickedXYZ(Engine::POSITION::POS_Z, -m_fConstrolSpeed);
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->BnClickedXYZ(Engine::POSITION::POS_Z, -m_fConstrolSpeed);
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->BnClickedXYZ(Engine::POSITION::POS_Z, -m_fConstrolSpeed);
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->BnClickedXYZ(Engine::POSITION::POS_Z, -m_fConstrolSpeed);
		break;
	default:
		break;
	}
}


void CConfirmFormView::OnBnClickedReadyCamera()
{
	m_pEditMgr->m_pMainScene->Ready_Camera();
}
