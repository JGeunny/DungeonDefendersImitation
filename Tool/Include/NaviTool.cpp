// NaviTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NaviTool.h"
#include "afxdialogex.h"
#include "EditMgr.h"
#include "ConfirmFormView.h"
#include "MainScene.h"

#include "NaviObj.h"
#include "MeshObj.h"
#include "PickPoint.h"
#include <iostream>

// CNaviTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviTool, CDialog)

CNaviTool::CNaviTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAVITOOL, pParent)
	, m_pEditMgr(CEditMgr::GetInstance())
	, m_vPos_Navi(0.f, 0.f, 0.f)
	, m_dwOption_Navi(0)
	, m_strName_Navi(_T("Navi"))
	, m_vPos_Vertex1(0.f, 0.f, 0.f)
	, m_vPos_Vertex2(0.f, 0.f, 0.f)
	, m_vPos_Vertex3(0.f, 0.f, 0.f)
	, m_strName_Cell(_T("Cell"))
	, m_dwOption_Cell(0)
	, m_bNoUnit(FALSE)
	, m_bNoBuild(FALSE)
	, m_bNoCam(FALSE)
	, m_bYesFish(FALSE)
	, m_vPos_Load(0.f, 0.f, 0.f)
	, m_iSelectType(0)
	, m_bPutVertex(TRUE)
{

}

CNaviTool::~CNaviTool()
{
	Free();
}

void CNaviTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_vPos_Navi.x);
	DDX_Text(pDX, IDC_EDIT2, m_vPos_Navi.y);
	DDX_Text(pDX, IDC_EDIT3, m_vPos_Navi.z);
	DDX_Text(pDX, IDC_EDIT10, m_dwOption_Navi);
	DDX_Text(pDX, IDC_EDIT11, m_strName_Navi);

	DDX_Text(pDX, IDC_EDIT7, m_vPos_Vertex1.x);
	DDX_Text(pDX, IDC_EDIT8, m_vPos_Vertex1.y);
	DDX_Text(pDX, IDC_EDIT9, m_vPos_Vertex1.z);
	DDX_Text(pDX, IDC_EDIT17, m_vPos_Vertex2.x);
	DDX_Text(pDX, IDC_EDIT18, m_vPos_Vertex2.y);
	DDX_Text(pDX, IDC_EDIT19, m_vPos_Vertex2.z);
	DDX_Text(pDX, IDC_EDIT20, m_vPos_Vertex3.x);
	DDX_Text(pDX, IDC_EDIT25, m_vPos_Vertex3.y);
	DDX_Text(pDX, IDC_EDIT26, m_vPos_Vertex3.z);
	DDX_Text(pDX, IDC_EDIT28, m_strName_Cell);
	DDX_Text(pDX, IDC_EDIT27, m_dwOption_Cell);
	DDX_Check(pDX, IDC_CHECK1, m_bNoUnit);
	DDX_Check(pDX, IDC_CHECK2, m_bNoBuild);
	DDX_Check(pDX, IDC_CHECK3, m_bNoCam);
	DDX_Control(pDX, IDC_LIST1, m_NaviListBox);
	DDX_Control(pDX, IDC_LIST6, m_CellListBox);
	DDX_Text(pDX, IDC_EDIT22, m_vPos_Load.x);
	DDX_Text(pDX, IDC_EDIT23, m_vPos_Load.y);
	DDX_Text(pDX, IDC_EDIT24, m_vPos_Load.z);
	DDX_Radio(pDX, IDC_RADIO3, m_iSelectType);
	DDX_Check(pDX, IDC_CHECK4, m_bPutVertex);
	DDX_Check(pDX, IDC_CHECK7, m_bYesFish);
}


BEGIN_MESSAGE_MAP(CNaviTool, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK1, &CNaviTool::OnBnClickedChkNoUnit)
	ON_BN_CLICKED(IDC_CHECK2, &CNaviTool::OnBnClickedChkNoBuild)
	ON_BN_CLICKED(IDC_CHECK3, &CNaviTool::OnBnClickedChkNoCam)
	ON_BN_CLICKED(IDC_BUTTON1, &CNaviTool::OnBnClickedAdd_Navi)
	ON_BN_CLICKED(IDC_BUTTON2, &CNaviTool::OnBnClickedAdjust_Navi)
	ON_BN_CLICKED(IDC_BUTTON4, &CNaviTool::OnBnClickedDel_Navi)
	ON_LBN_SELCHANGE(IDC_LIST1, &CNaviTool::OnLbnSelchangeNaviList)
	ON_LBN_SELCHANGE(IDC_LIST6, &CNaviTool::OnLbnSelchangeCellList)
	ON_BN_CLICKED(IDC_BUTTON8, &CNaviTool::OnBnClickedAdd_Cell)
	ON_BN_CLICKED(IDC_BUTTON9, &CNaviTool::OnBnClickedAdjust_Cell)
	ON_BN_CLICKED(IDC_BUTTON13, &CNaviTool::OnBnClickedDel_Cell)
	ON_BN_CLICKED(IDC_BUTTON31, &CNaviTool::OnBnClicked_New)
	ON_BN_CLICKED(IDC_BUTTON21, &CNaviTool::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON22, &CNaviTool::OnBnClicked_Load)
	ON_BN_CLICKED(IDC_CHECK7, &CNaviTool::OnBnClickedChkYesFish)
END_MESSAGE_MAP()


// CNaviTool 메시지 처리기입니다.

void CNaviTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NAVITOOL;
}


void CNaviTool::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;
	CDialog::OnClose();	
}

HRESULT CNaviTool::Ready_NaviTool()
{
	return S_OK;
}

void CNaviTool::Free()
{
	m_lstPickPoint.clear();
	m_lstSelectedCell.clear();
}

void CNaviTool::Event_LButton()
{
	Put_Vertex();
}

void CNaviTool::Event_MouseMove()
{

}

void CNaviTool::Event_RButton()
{
	
}

void CNaviTool::Event_QButton()
{
	Clear_PickPoint();
	Clear_SelectedCell();
}

bool CNaviTool::Put_Vertex()
{
	UpdateData(TRUE);
	
	if (FALSE == m_bPutVertex)
	{
		Select_Vertex();
		return false;
	}

	bool bCrash = false;
	auto m_pmapLayer = m_pEditMgr->m_pMainScene->Get_LayerMap();
	auto pair_Layer = m_pmapLayer->find(L"GameLogic");
	if (pair_Layer == m_pmapLayer->end()) return false;
	auto& pLayer = pair_Layer->second;

	////1 셀에서 찾기
	auto iMax = m_CellListBox.GetCount();
	for (int idx = 0; (idx < iMax) && !bCrash; ++idx)
	{
		auto pCell = (Engine::CCell*)m_CellListBox.GetItemData(idx);
		for (int idx_Point = 0; (idx_Point < Engine::CCell::POINT_END) && !bCrash; ++idx_Point)
		{
			auto pMesh = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderMesh();
			auto pmatWorld = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderWorld();
			if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, pmatWorld, nullptr))
			{
				Engine::CGameObject* pGameObject = nullptr;
				pGameObject = CPickPoint::Create(m_pEditMgr->m_pGraphicDev, m_lstPickPoint.size());
				Engine::CComponent* pComponent = nullptr;
				pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
				if (pComponent)
				{
					auto pTrans = (Engine::CTransform*)pComponent;
					pTrans->Set_Pos(pCell->m_vPoint[idx_Point] + m_vPos_Navi);
				}
				pLayer->Add_Object(L"GameLogic", pGameObject);
				Add_PickPoint((CPickPoint*)pGameObject);
				CheckAndClear_PickPoint();
				bCrash = true;
			}
		}
	}
	UpdateData(FALSE);

	if (bCrash) return bCrash;
	//2 레이어에서 찾기
	std::list<Engine::CGameObject*> lstGameObject;
	pLayer->Get_ObjectList(&lstGameObject);
	for (auto& pGameObject : lstGameObject)
	{
		//auto pComponent_Collider = pGameObject->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
		auto pComponent_Transform = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		auto pComponent_MeshAdmin = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
		if (!pComponent_MeshAdmin || !pComponent_Transform) continue;

		auto pTrans = (Engine::CTransform*)pComponent_Transform;
		auto pMeshAdmin = (Engine::CMeshAdmin*)pComponent_MeshAdmin;
		auto pMesh = pMeshAdmin->Get_Mesh()->Get_Mesh();
		if (!pMesh) continue;
		_matrix matWorld;
		pTrans->Get_WorldMatrix(&matWorld);
		_vec3 vCollisionPos = _vec3(0.f, 0.f, 0.f);
		if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, &matWorld, &vCollisionPos))
		{
			Engine::CGameObject* pGameObject = nullptr;
			pGameObject = CPickPoint::Create(m_pEditMgr->m_pGraphicDev, m_lstPickPoint.size());
			Engine::CComponent* pComponent = nullptr;
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTrans = (Engine::CTransform*)pComponent;
				pTrans->Set_Pos(vCollisionPos);
			}
			
			pLayer->Add_Object(L"GameLogic", pGameObject);
			Add_PickPoint((CPickPoint*)pGameObject);

			CheckAndClear_PickPoint();
			//Select_Object(pGameObject);
			bCrash = true;
			break;
		}
	}
	return bCrash;
}

bool CNaviTool::Select_Vertex()
{
	UpdateData(TRUE);

	bool bCrash = false;
	bool bSelectCell = false;

	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);
	////1 셀에서 찾기
	auto iMax = m_CellListBox.GetCount();
	//std::cout << m_iSelectType << std::endl;
	for (int idx = 0; (idx < iMax) && !bCrash; ++idx)
	{
		bSelectCell = false;
		auto pCell = (Engine::CCell*)m_CellListBox.GetItemData(idx);
		
		if (m_iSelectType == 2)
		{
			if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse_Tri(g_hWnd, pCell->m_vPoint, &matTemp, nullptr))
				bSelectCell = true;
		}
		else
		{
			for (int idx_Point = 0; (idx_Point < Engine::CCell::POINT_END) && !bCrash; ++idx_Point)
			{
				auto pMesh = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderMesh();
				auto pmatWorld = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderWorld();
				if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, pmatWorld, nullptr))
				{
					bSelectCell = true;
					pCell->m_pColliderCom_Point[idx_Point]->Set_CollType(Engine::COLLTYPE::COL_SELECT);
					if (1 == m_iSelectType) //Single
						bCrash = true;
				}
			}
		}
		if (bSelectCell)
			Select_Cell(pCell);
	}
	UpdateData(FALSE);

	return false;
}

void CNaviTool::Update_Option_Cell()
{
	UpdateData(TRUE);	
	m_dwOption_Cell = 0;
	m_dwOption_Cell |= m_bNoUnit ? CELL_NOUNIT : 0;
	m_dwOption_Cell |= m_bNoBuild ? CELL_NOBUILD : 0;
	m_dwOption_Cell |= m_bNoCam ? CELL_NOCAM : 0;
	m_dwOption_Cell |= m_bYesFish ? CELL_YESFISH : 0;
	UpdateData(FALSE);
}

void CNaviTool::Set_Option_Cell_CheckBox()
{
	//UpdateData(TRUE);
	m_bNoUnit = (m_dwOption_Cell & CELL_NOUNIT) ? TRUE : FALSE;
	m_bNoBuild = (m_dwOption_Cell & CELL_NOBUILD) ? TRUE : FALSE;
	m_bNoCam = (m_dwOption_Cell & CELL_NOCAM) ? TRUE : FALSE;
	m_bYesFish = (m_dwOption_Cell & CELL_YESFISH) ? TRUE : FALSE;
	//UpdateData(FALSE);
}

void CNaviTool::Change_PreView()
{
}

bool CNaviTool::Select_Cell(Engine::CCell * pCell)
{
	if (!pCell) return false;
	auto iMax = m_CellListBox.GetCount();
	for (auto i = 0; i < iMax; ++i)
	{
		if (pCell == (Engine::CCell*)m_CellListBox.GetItemData(i))
		{
			m_CellListBox.SetCurSel(i);
			Add_SelectedCell(pCell);
			if (2 == m_iSelectType) //Cell
				pCell->Set_CollType(Engine::COLLTYPE::COL_SELECT);
			UpdateData(TRUE);
			Get_CellData(pCell);
			UpdateData(FALSE);

			return true;
		}
	}
	return false;
}

void CNaviTool::Add_SelectedCell(Engine::CCell* pCell)
{
	auto iter = find_if(m_lstSelectedCell.begin(), m_lstSelectedCell.end(), [&](auto _pCell) {
		return pCell == _pCell;
	});
	if (iter == m_lstSelectedCell.end())
		m_lstSelectedCell.emplace_back(pCell);
}

void CNaviTool::Clear_SelectedCell()
{
	for (auto pCell : m_lstSelectedCell)
	{
		pCell->Set_CollType(Engine::COLLTYPE::COL_FALSE);
		for (int idx_Point = 0; idx_Point < Engine::CCell::POINT_END; ++idx_Point)
		{
			pCell->m_pColliderCom_Point[idx_Point]->Set_CollType(Engine::COLLTYPE::COL_FALSE);
		}
	}
	m_lstSelectedCell.clear();
}

void CNaviTool::Set_ObjectData(Engine::CGameObject * pGameObject)
{
	if (!pGameObject) return;

	//오브젝트 정보 저장
	Engine::CComponent* pComponent = nullptr;
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		pObjectInfoCom->Set_ObjectKey(m_strName_Navi.GetString());
		pObjectInfoCom->Set_Option(m_dwOption_Navi);
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		pTransCom->Set_Pos(m_vPos_Navi);
	}
}

void CNaviTool::Get_ObjectData(Engine::CGameObject * pGameObject)
{
	if (!pGameObject) return;
	Engine::CComponent* pComponent = nullptr;

	//오브젝트 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		m_strName_Navi = pObjectInfoCom->Get_ObjectKey().c_str();
		m_dwOption_Navi = pObjectInfoCom->Get_Option();
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;

		m_vPos_Navi = pTransCom->Get_Pos();
	}

}

void CNaviTool::Set_CellData(Engine::CCell * pCell)
{
	if (!pCell) return;
	
	//오브젝트 정보 저장
	auto pObjectInfoCom = pCell->m_pObjectInfo;
	if (pObjectInfoCom)
	{
		pObjectInfoCom->Set_ObjectKey(m_strName_Cell.GetString());
		pObjectInfoCom->Set_Option(m_dwOption_Cell);
	}

	//트렌스폼 정보 저장
	pCell->m_vPoint[Engine::CCell::POINT_A] = m_vPos_Vertex1 - m_vPos_Navi;
	pCell->m_vPoint[Engine::CCell::POINT_B] = m_vPos_Vertex2 - m_vPos_Navi;
	pCell->m_vPoint[Engine::CCell::POINT_C] = m_vPos_Vertex3 - m_vPos_Navi;
}

void CNaviTool::Get_CellData(Engine::CCell * pCell)
{
	if (!pCell) return;
	Engine::CComponent* pComponent = nullptr;

	//오브젝트 정보 저장
	auto pObjectInfoCom = pCell->m_pObjectInfo;
	if (pObjectInfoCom)
	{
		m_strName_Cell = pObjectInfoCom->Get_ObjectKey().c_str();
		m_dwOption_Cell = pObjectInfoCom->Get_Option();
	}

	//트렌스폼 정보 저장
	m_vPos_Vertex1 = pCell->m_vPoint[Engine::CCell::POINT_A] + m_vPos_Navi;
	m_vPos_Vertex2 = pCell->m_vPoint[Engine::CCell::POINT_B] + m_vPos_Navi;
	m_vPos_Vertex3 = pCell->m_vPoint[Engine::CCell::POINT_C] + m_vPos_Navi;

	Set_Option_Cell_CheckBox();
}

HRESULT CNaviTool::Ready_CellList(CNaviObj * pNaviObj)
{
	if (!pNaviObj) return E_FAIL;
	auto pCellList = pNaviObj->Get_CellList();
	m_CellListBox.ResetContent();
	for (auto pCell : *pCellList)
	{
		CString strName = pCell->m_pObjectInfo->Get_ObjectKey().c_str();
		int idx = m_CellListBox.AddString(strName);
		m_CellListBox.SetItemData(idx, (DWORD)pCell);
	}
	m_CellListBox.SetCurSel(0);
	return S_OK;
}

void CNaviTool::Add_PickPoint(CPickPoint * pPickPoint)
{	
	bool bCreateCell = false;
	m_lstPickPoint.emplace_back(pPickPoint);

	UpdateData(TRUE);
	auto pComponent = pPickPoint->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (!pComponent) return;
	auto pTrans = (Engine::CTransform*)pComponent;
	
	switch (m_lstPickPoint.size())
	{
	case 1:
		m_vPos_Vertex1 = pTrans->Get_Pos();
		break;
	case 2:
		m_vPos_Vertex2 = pTrans->Get_Pos();
		break;
	case 3:
		m_vPos_Vertex3 = pTrans->Get_Pos();
		bCreateCell = true;
		break;
	}
	UpdateData(FALSE);
	if(bCreateCell)
		OnBnClickedAdd_Cell();
}

BOOL CNaviTool::CheckAndClear_PickPoint()
{
	if ((int)m_lstPickPoint.size() < m_iMaxCount_PickPoint)
		return FALSE;

	Clear_PickPoint();
	return TRUE;
}

void CNaviTool::Clear_PickPoint()
{
	for (auto pPickPoint : m_lstPickPoint)
	{
		pPickPoint->Set_Event(EVENT_DEAD);
	}
	m_lstPickPoint.clear();
}

void CNaviTool::BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed)
{
	UpdateData(TRUE);
	for (auto& pCell : m_lstSelectedCell)
	{
		if (m_iSelectType == 2 && pCell->Get_CollType() != Engine::COLLTYPE::COL_SELECT) //셀 선택 일경우에 셀이 선택 안되어있으면 넘어감
			continue;

		for (int idx_Point = 0; idx_Point < Engine::CCell::POINT_END; ++idx_Point)
		{
			if (m_iSelectType != 2 &&
				(pCell->m_pColliderCom_Point[idx_Point]->Get_CollType()
					!= Engine::COLLTYPE::COL_SELECT)) //셀 이 아닐경우에는 선택된 버택스만 이동
				continue;

			*((_float*)&(pCell->m_vPoint[idx_Point]) + ePos) += fControlSpeed;
		}
	}
}

void CNaviTool::OnBnClickedChkNoUnit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Update_Option_Cell();
}


void CNaviTool::OnBnClickedChkNoBuild()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Update_Option_Cell();
}


void CNaviTool::OnBnClickedChkNoCam()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Update_Option_Cell();
}


void CNaviTool::OnBnClickedAdd_Navi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_strName_Navi == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CNaviObj::Create(m_pEditMgr->m_pGraphicDev, 0);
	NULL_CHECK(pGameObject);
	Set_ObjectData(pGameObject);

	int idx = m_NaviListBox.AddString(m_strName_Navi);
	m_NaviListBox.SetItemData(idx, (DWORD)pGameObject);
	m_NaviListBox.SetCurSel(idx);

	m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic", m_strName_Navi, pGameObject);

	UpdateData(FALSE);
	Change_PreView();
	
	OnLbnSelchangeNaviList();
}


void CNaviTool::OnBnClickedAdjust_Navi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int idx = m_NaviListBox.GetCurSel();
	if (idx == -1) return;

	auto pGameObject = (Engine::CGameObject*)m_NaviListBox.GetItemData(idx);
	Set_ObjectData(pGameObject);
	
	m_NaviListBox.DeleteString(idx);
	idx = m_NaviListBox.AddString(m_strName_Navi);
	m_NaviListBox.SetItemData(idx, (DWORD)pGameObject);

	UpdateData(FALSE);
	Change_PreView();
}


void CNaviTool::OnBnClickedDel_Navi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int idx = m_NaviListBox.GetCurSel();
	if (idx == -1) return;

	auto pGameObject = (Engine::CGameObject*)m_NaviListBox.GetItemData(idx);
	m_NaviListBox.DeleteString(idx);
	if (pGameObject)
		pGameObject->Set_Event(EVENT_DEAD);
	
	m_CellListBox.ResetContent();

	Clear_SelectedCell();
}


void CNaviTool::OnLbnSelchangeNaviList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int idx = m_NaviListBox.GetCurSel();
	if (idx == -1) return;

	auto pNaviObj = (CNaviObj*)m_NaviListBox.GetItemData(idx);
	Get_ObjectData(pNaviObj);
	Ready_CellList(pNaviObj);
	UpdateData(FALSE);

	OnLbnSelchangeCellList();
}


void CNaviTool::OnLbnSelchangeCellList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int idx = m_CellListBox.GetCurSel();
	if (idx == -1) return;

	auto pCell = (Engine::CCell*)m_CellListBox.GetItemData(idx);
	Get_CellData(pCell);

	UpdateData(FALSE);
}


void CNaviTool::OnBnClickedAdd_Cell()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int idx = m_NaviListBox.GetCurSel();
	if (idx == -1) return;

	auto pNaviObj = (CNaviObj*)m_NaviListBox.GetItemData(idx);
	Engine::CCell* pCell = nullptr;
	HRESULT hMsg = pNaviObj->Add_Cell(&pCell, m_vPos_Vertex1 - m_vPos_Navi, m_vPos_Vertex2 - m_vPos_Navi, m_vPos_Vertex3 - m_vPos_Navi);
	if (0 == hMsg) //  세점이 일직선상에 있음// 추가 안함
	{

	}

	if (pCell)
	{
		pCell->m_pObjectInfo->Set_ObjectKey(m_strName_Cell);
		pCell->m_pObjectInfo->Set_Option(m_dwOption_Cell);
		int idx_AddData = m_CellListBox.AddString(m_strName_Cell);
		m_CellListBox.SetItemData(idx_AddData, (DWORD)pCell);
	}

	UpdateData(FALSE);
}


void CNaviTool::OnBnClickedAdjust_Cell()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int idx = m_CellListBox.GetCurSel();
	if (idx == -1) return;

	auto pCell = (Engine::CCell*)m_CellListBox.GetItemData(idx);
	Set_CellData(pCell);
	m_CellListBox.DeleteString(idx);
	idx = m_CellListBox.AddString(m_strName_Cell);
	m_CellListBox.SetItemData(idx, (DWORD)pCell);
	UpdateData(FALSE);
}


void CNaviTool::OnBnClickedDel_Cell()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int idx = m_NaviListBox.GetCurSel();
	if (idx == -1) return;
	int idx_cell = m_CellListBox.GetCurSel();
	if (idx_cell == -1) return;

	auto pNaviObj = (CNaviObj*)m_NaviListBox.GetItemData(idx);
	//auto pCell = (Engine::CCell*)m_CellListBox.GetItemData(idx_cell);
	//m_CellListBox.DeleteString(idx_cell);
	//pNaviObj->Del_Cell(pCell);
	//UpdateData(FALSE);

	Engine::CCell* pCell = nullptr;
	int iMax = m_CellListBox.GetCount();
	for (int i = 0; i < iMax; ++i)
	{
		pCell = (Engine::CCell*)m_CellListBox.GetItemData(i);
		auto& iter = find_if(m_lstSelectedCell.begin(), m_lstSelectedCell.end(), 
								[&](auto _pCell) {
								return (_pCell == pCell);
							});
		if (iter != m_lstSelectedCell.end())
		{
			if (Engine::COLLTYPE::COL_SELECT == (*iter)->Get_CollType())
				pNaviObj->Del_Cell(*iter);
			m_CellListBox.DeleteString(i);
		}
	}
	m_lstSelectedCell.clear();
	UpdateData(FALSE);
}


void CNaviTool::OnBnClicked_New()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int iMax = m_NaviListBox.GetCount();
	for (int idx = 0; idx < iMax; ++idx)
	{
		auto pNaviObj = (CNaviObj*)m_NaviListBox.GetItemData(idx);
		pNaviObj->Set_Event(EVENT_DEAD);
	}
	m_NaviListBox.ResetContent();
	m_CellListBox.ResetContent();
	Clear_PickPoint();
	UpdateData(FALSE);
}

void CNaviTool::OnBnClicked_Save()
{
	UpdateData(TRUE);
	int iMax = m_NaviListBox.GetCount();
	if (iMax <= 0) 
	{
		ERR_MSG(L"네비게이션 오브젝트가 없습니다.");
		return;// S_FALSE;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		FALSE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"NaviInfo.txt", /* 디폴트 파일이름 */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* 파일 형식 */
		this /* 부모 윈도우 */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // 절대경로

	if (IDOK == Dlg.DoModal())
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName().GetString(), GENERIC_WRITE, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERR_MSG(L"오브젝트 저장 실패");
			return;// S_FALSE;
		}

		DWORD dwBytes = 0;

		for (int idx = 0; idx < iMax; ++idx)
		{
			//Navi Save
			Engine::NAVI_SAVEDATA tNaviData = Engine::NAVI_SAVEDATA();
			CNaviObj* pGameObject = nullptr;
			pGameObject = (CNaviObj*)m_NaviListBox.GetItemData(idx);
			if (!pGameObject) continue;

			Engine::CComponent* pComponent = nullptr;

			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				lstrcpy(tNaviData.szObjectKey, pObjectInfoCom->Get_ObjectKey().c_str());
				tNaviData.iOption = pObjectInfoCom->Get_Option();
			}
			//오브젝트키가 없다면 저장하지 않는다.
			if (!lstrcmp(tNaviData.szObjectKey, L""))
				continue;

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				tNaviData.vPos = pTransCom->Get_Pos();
			}

			auto pCellList = pGameObject->Get_CellList();
			tNaviData.iCount_Cell = pCellList->size();

			//네비게이션 데이터 저장
			WriteFile(hFile, &tNaviData, sizeof(Engine::NAVI_SAVEDATA), &dwBytes, nullptr);

			//셀 데이터 저장
			Engine::CELL_SAVEDATA tCellData = Engine::CELL_SAVEDATA();
			
			for (auto pCell : *pCellList)
			{
				for (int idx_Point = 0; idx_Point < POINT_END_CELL; ++idx_Point)
				{
					tCellData.vPoint[idx_Point] = pCell->m_vPoint[idx_Point];
				}
				
				//오브젝트 정보 저장
				if (pCell->m_pObjectInfo)
				{
					auto pObjectInfoCom = pCell->m_pObjectInfo;
					lstrcpy(tCellData.szObjectKey, pObjectInfoCom->Get_ObjectKey().c_str());
					tCellData.iOption = pObjectInfoCom->Get_Option();
				}
				//네비게이션 데이터 저장
				WriteFile(hFile, &tCellData, sizeof(Engine::CELL_SAVEDATA), &dwBytes, nullptr);
			}			
		}

		CloseHandle(hFile);
	}
	UpdateData(FALSE);
}


void CNaviTool::OnBnClicked_Load()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		TRUE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"NaviInfo.txt", /* 디폴트 파일이름 */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* 파일 형식 */
		this /* 부모 윈도우 */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // 절대경로

	if (IDOK == Dlg.DoModal())
	{
		//m_CubeListBox.ResetContent();

		HANDLE hFile = CreateFile(Dlg.GetPathName().GetString(), GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERR_MSG(L"오브젝트 불러오기 실패");
			return;// S_FALSE;
		}

		CToolScene* pScene = m_pEditMgr->m_pMainScene;
		DWORD dwBytes = 0;
		
		while (true)
		{
			Engine::NAVI_SAVEDATA tNaviData = Engine::NAVI_SAVEDATA();
			//데이터 가져오기
			ReadFile(hFile, &tNaviData, sizeof(Engine::NAVI_SAVEDATA), &dwBytes, nullptr);
			if (0 == dwBytes)
				break;
			tNaviData.vPos += m_vPos_Load;

			CNaviObj* pGameObject = CNaviObj::Create(m_pEditMgr->m_pGraphicDev, 0);
			if (!pGameObject) continue;
			Engine::CComponent* pComponent = nullptr;
			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				pObjectInfoCom->Set_ObjectKey(tNaviData.szObjectKey);
				pObjectInfoCom->Set_Option(tNaviData.iOption);
			}

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				pTransCom->Set_Pos(tNaviData.vPos);
			}

			//Cell Save/////////////////////////////////////////////////////////
			for (int idx_Cell = 0; idx_Cell < tNaviData.iCount_Cell; ++idx_Cell)
			{
				Engine::CELL_SAVEDATA tCellData = Engine::CELL_SAVEDATA();
				ReadFile(hFile, &tCellData, sizeof(Engine::CELL_SAVEDATA), &dwBytes, nullptr);
				Engine::CCell* pCell = nullptr;

				HRESULT hSuccess = pGameObject->Add_Cell(&pCell, 
					tCellData.vPoint[Engine::CCell::POINT_A],
					tCellData.vPoint[Engine::CCell::POINT_B],
					tCellData.vPoint[Engine::CCell::POINT_C]);

				//if (0 == hSuccess) continue;

				//오브젝트 정보 저장
				if (pCell->m_pObjectInfo)
				{
					auto pObjectInfoCom = pCell->m_pObjectInfo;
					pObjectInfoCom->Set_ObjectKey(tCellData.szObjectKey);
					pObjectInfoCom->Set_Option(tCellData.iOption);
				}
			}

			pScene->Add_Object(L"GameLogic", tNaviData.szObjectKey, pGameObject);

			int idx = m_NaviListBox.AddString(tNaviData.szObjectKey);
			m_NaviListBox.SetItemData(idx, (DWORD)pGameObject);
		}
		CloseHandle(hFile);

		//InitObjList();
	}
}


void CNaviTool::OnBnClickedChkYesFish()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Update_Option_Cell();
}
