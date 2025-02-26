
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "NaviTool.h"
#include "CameraTool.h"
#include "DirectionTool.h"

#include "ToolScene.h"

#include "EditFormView.h"
#include "MapTool.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
	:m_pEditMgr(CEditMgr::GetInstance())
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_pEditMgr->m_pToolView = this;
}


void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnRButtonDown(nFlags, point);

	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->Event_RButton();
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->Event_RButton();
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->Event_RButton();
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->Event_RButton();
		break;
	case FOCUSID_END:
		break;
	default:
		break;
	}
}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonDown(nFlags, point);

	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		m_pEditMgr->m_pEditFormView->m_pMapTool->Event_LButton();
		break;
	case FOCUSID_NAVITOOL:
		m_pEditMgr->m_pEditFormView->m_pNaviTool->Event_LButton();
		break;
	case FOCUSID_CAMERATOOL:
		m_pEditMgr->m_pEditFormView->m_pCameraTool->Event_LButton();
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->Event_LButton();
		break;
	case FOCUSID_END:
		break;
	default:
		break;
	}	
}


void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnKeyDown(nChar, nRepCnt, nFlags);

	switch (m_pEditMgr->m_eFocus)
	{
	case FOCUSID_NONE:
		break;
	case FOCUSID_MAPTOOL:
		if (nChar == 0x51) //Q
			m_pEditMgr->m_pEditFormView->m_pMapTool->Event_QButton();
		break;
	case FOCUSID_NAVITOOL:
		if(nChar == 0x51) //Q
			m_pEditMgr->m_pEditFormView->m_pNaviTool->Event_QButton();
		break;
	case FOCUSID_CAMERATOOL:
		if (nChar == 0x51) //Q
			m_pEditMgr->m_pEditFormView->m_pCameraTool->Event_QButton();
		break;
	case FOCUSID_LISTTOOL:
		break;
	case FOCUSID_DIRECTIONTOOL:
		m_pEditMgr->m_pEditFormView->m_pDirectionTool->Event_QButton();
		break;
	case FOCUSID_END:
		break;
	default:
		break;
	}
}


void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnMouseMove(nFlags, point);
//	std::cout << point.x << " : " << point.y << std::endl;
}
