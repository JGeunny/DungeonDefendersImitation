
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView()
	:m_pEditMgr(CEditMgr::GetInstance())
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_pEditMgr->m_pToolView = this;
}


void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);
//	std::cout << point.x << " : " << point.y << std::endl;
}
