// DirectionTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "DirectionTool.h"
#include "afxdialogex.h"

#include "EditMgr.h"
#include "DirectionObj.h"
#include "MainScene.h"

// CDirectionTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDirectionTool, CDialog)

CDirectionTool::CDirectionTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIRECTIONTOOL, pParent)
	, m_pEditMgr(CEditMgr::GetInstance())
	, m_vPos_Direction(0.f, 0.f, 0.f)
	, m_strName_Direction(_T("Direction"))
	, m_dwOption_Direction(0)
	, m_vPos_Node(0.f, 0.f, 0.f)
	, m_strName_Node(_T("Node"))
	, m_dwOption_Node(0)
	, m_vPos_Load(0.f, 0.f, 0.f)
{

}

CDirectionTool::~CDirectionTool()
{
}

void CDirectionTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_vPos_Direction.x);
	DDX_Text(pDX, IDC_EDIT3, m_vPos_Direction.y);
	DDX_Text(pDX, IDC_EDIT4, m_vPos_Direction.z);
	DDX_Text(pDX, IDC_EDIT11, m_strName_Direction);
	DDX_Text(pDX, IDC_EDIT29, m_dwOption_Direction);
	DDX_Text(pDX, IDC_EDIT7, m_vPos_Node.x);
	DDX_Text(pDX, IDC_EDIT8, m_vPos_Node.y);
	DDX_Text(pDX, IDC_EDIT9, m_vPos_Node.z);
	DDX_Text(pDX, IDC_EDIT13, m_strName_Node);
	DDX_Text(pDX, IDC_EDIT12, m_dwOption_Node);
	DDX_Text(pDX, IDC_EDIT22, m_vPos_Load.x);
	DDX_Text(pDX, IDC_EDIT23, m_vPos_Load.y);
	DDX_Text(pDX, IDC_EDIT24, m_vPos_Load.z);
	DDX_Control(pDX, IDC_TREE1, m_DirectionTree);
}

HRESULT CDirectionTool::Ready_DirectionTool()
{
	return E_NOTIMPL;
}

void CDirectionTool::Update_DirectionTool()
{
}

void CDirectionTool::Free()
{
}

void CDirectionTool::BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (auto pNode : m_lstSelectedNode)
	{
		auto vPos = pNode->Get_Point();
		*((_float*)&vPos + ePos) += fControlSpeed;
		pNode->Set_Point(vPos);
	}
}

void CDirectionTool::Event_LButton()
{
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		Put_Vertex();
	else
		Select_Vertex();
}

void CDirectionTool::Event_MouseMove()
{
}

void CDirectionTool::Event_RButton()
{
}

void CDirectionTool::Event_QButton()
{
}

bool CDirectionTool::Put_Vertex()
{
	bool bCrash = false;
	auto m_pmapLayer = m_pEditMgr->m_pMainScene->Get_LayerMap();
	auto pair_Layer = m_pmapLayer->find(L"GameLogic");
	if (pair_Layer == m_pmapLayer->end()) return false;
	auto& pLayer = pair_Layer->second;

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
			UpdateData(TRUE);
			vCollisionPos.y += 1.f;
			m_vPos_Node = vCollisionPos;
			UpdateData(FALSE);
			OnBnClicked_Add_Node();
			bCrash = true;
			break;
		}
	}

	return bCrash;
}

bool CDirectionTool::Select_Vertex()
{
	auto hItem = m_DirectionTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_DirectionTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 길찾기 객체가 없습니다.");
		return false;
	}
	auto pCameraObj = (CDirectionObj*)m_DirectionTree.GetItemData(hItem_Parents);
	for (int iType = 0; iType < Engine::CAMERATYPE::CAMERATYPE_END; ++iType)
	{
		auto plstNode = pCameraObj->Get_NodeList();
		for (auto pNode : *plstNode)
		{
			auto pMesh = pNode->Get_Collider()->Get_ColliderMesh();
			auto pmatWorld = pNode->Get_Collider()->Get_ColliderWorld();
			if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, pmatWorld, nullptr))
			{
				Select_Node(pNode);
			}
		}
	}

	UpdateData(FALSE);

	return true;
}

bool CDirectionTool::Select_Node(Engine::CCurve * pNode)
{
	if (!pNode) return false;

	auto hItem = m_DirectionTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_DirectionTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 길찾기 객체가 없습니다.");
		return false;
	}
	HTREEITEM hItem_Node = find_if_tree(&m_DirectionTree, hItem_Parents, [&](auto _pNode) {
		return (DWORD)pNode == _pNode;
	});
	if (nullptr != hItem_Node)
	{
		m_DirectionTree.SelectItem(hItem_Node);
		Add_SelectedNode(pNode);
	}

	return true;
}

void CDirectionTool::Add_SelectedNode(Engine::CCurve * pNode)
{
	if (nullptr == pNode) return;
	auto iter = find_if(m_lstSelectedNode.begin(), m_lstSelectedNode.end(), [&](auto _pNode) {
		return _pNode == pNode;
	});
	if (iter != m_lstSelectedNode.end()) return;
	pNode->Get_Collider()->Set_CollType(Engine::COLLTYPE::COL_SELECT);
	m_lstSelectedNode.emplace_back(pNode);
}

void CDirectionTool::Clear_SelectedNode()
{
	for (auto& pNode : m_lstSelectedNode)
	{
		pNode->Get_Collider()->Set_CollType(Engine::COLLTYPE::COL_FALSE);
		pNode = nullptr;
	}
	m_lstSelectedNode.clear();
}

HTREEITEM CDirectionTool::Get_RootItem(_int * pOut_Level, CTreeCtrl * pTreeCtr, HTREEITEM hItem)
{
	HTREEITEM hNextItem = pTreeCtr->GetParentItem(hItem);
	if (nullptr == hNextItem)
		return hItem;
	++*pOut_Level;
	return Get_RootItem(pOut_Level, pTreeCtr, hNextItem);
}

HRESULT CDirectionTool::Add_Node(HTREEITEM hItem_Parents, CDirectionObj * pDirectionObj, _vec3 vPos_Node)
{
	Engine::CCurve* pNode = nullptr;
	pDirectionObj->Add_Node(&pNode, vPos_Node);
	NULL_CHECK_RETURN(pNode, E_FAIL);
	Set_NodeData(pNode, false);
	CString strType = L"Node";
	auto hItem_Node = m_DirectionTree.InsertItem(strType, hItem_Parents, TVI_LAST);
	m_DirectionTree.SetItemData(hItem_Node, (DWORD)pNode);
	return S_OK;
}

void CDirectionTool::Set_ObjectData(CDirectionObj * pGameObject)
{
	if (!pGameObject) return;

	//오브젝트 정보 저장
	Engine::CComponent* pComponent = nullptr;
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		pObjectInfoCom->Set_ObjectKey(m_strName_Direction.GetString());
		pObjectInfoCom->Set_Option(m_dwOption_Direction);
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		pTransCom->Set_Pos(m_vPos_Direction);
	}
}

void CDirectionTool::Get_ObjectData(CDirectionObj * pGameObject)
{
	if (!pGameObject) return;
	Engine::CComponent* pComponent = nullptr;

	//오브젝트 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		m_strName_Direction = pObjectInfoCom->Get_ObjectKey().c_str();
		m_dwOption_Direction = pObjectInfoCom->Get_Option();
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		m_vPos_Direction = pTransCom->Get_Pos();
	}
}

void CDirectionTool::Set_NodeData(Engine::CCurve * pNode, _bool bChange_Pos)
{
	if (nullptr == pNode) return;
	if (bChange_Pos)
		pNode->Set_Point(m_vPos_Node);
}

void CDirectionTool::Get_NodeData(Engine::CCurve * pNode)
{
	if (nullptr == pNode) return;
	m_vPos_Node = pNode->Get_Point();
}


BEGIN_MESSAGE_MAP(CDirectionTool, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON32, &CDirectionTool::OnBnClicked_New)
	ON_BN_CLICKED(IDC_BUTTON23, &CDirectionTool::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON24, &CDirectionTool::OnBnClicked_Load)
	ON_BN_CLICKED(IDC_BUTTON1, &CDirectionTool::OnBnClicked_Add_Direction)
	ON_BN_CLICKED(IDC_BUTTON2, &CDirectionTool::OnBnClicked_Adjust_Direction)
	ON_BN_CLICKED(IDC_BUTTON14, &CDirectionTool::OnBnClicked_Del_Direction)
	ON_BN_CLICKED(IDC_BUTTON8, &CDirectionTool::OnBnClicked_Add_Node)
	ON_BN_CLICKED(IDC_BUTTON15, &CDirectionTool::OnBnClicked_Adjust_Node)
	ON_BN_CLICKED(IDC_BUTTON9, &CDirectionTool::OnBnClicked_Del_Node)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CDirectionTool::OnTvnSelchanged_DirectionTree)
END_MESSAGE_MAP()


// CDirectionTool 메시지 처리기입니다.


void CDirectionTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pEditMgr->m_eFocus = FOCUSID_DIRECTIONTOOL;
}


void CDirectionTool::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;

	CDialog::OnClose();
}


void CDirectionTool::OnBnClicked_New()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	for (auto hItem = m_DirectionTree.GetRootItem();
		hItem != nullptr;
		hItem = m_DirectionTree.GetNextSiblingItem(hItem))
	{
		auto pCameraObj = (CDirectionObj*)m_DirectionTree.GetItemData(hItem);
		pCameraObj->Set_Event(EVENT_DEAD);
	}
	m_lstSelectedNode.clear();
	m_DirectionTree.DeleteAllItems();
	UpdateData(FALSE);
}


void CDirectionTool::OnBnClicked_Save()
{
	UpdateData(TRUE);

	int iMax = m_DirectionTree.GetCount();
	if (iMax <= 0)
	{
		ERR_MSG(L"카메라 오브젝트가 없습니다.");
		return;// S_FALSE;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		FALSE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"DirectionInfo.txt", /* 디폴트 파일이름 */
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

		for (auto hItem = m_DirectionTree.GetRootItem();
			hItem != nullptr;
			hItem = m_DirectionTree.GetNextSiblingItem(hItem))
		{
			//Camera Save
			Engine::CAMERA_SAVEDATA tCameraData = Engine::CAMERA_SAVEDATA();
			CDirectionObj* pGameObject = nullptr;
			pGameObject = (CDirectionObj*)m_DirectionTree.GetItemData(hItem);
			if (!pGameObject) continue;

			Engine::CComponent* pComponent = nullptr;

			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				lstrcpy(tCameraData.szObjectKey, pObjectInfoCom->Get_ObjectKey().c_str());
				tCameraData.iOption = pObjectInfoCom->Get_Option();
			}
			//오브젝트키가 없다면 저장하지 않는다.
			if (!lstrcmp(tCameraData.szObjectKey, L""))
				continue;

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				tCameraData.vPos = pTransCom->Get_Pos();
			}

			std::vector<Engine::CCurve*> *pvecNode;
			pvecNode = pGameObject->Get_NodeList();
			tCameraData.iCount_Node = pvecNode->size();

			//카메라 데이터 저장
			WriteFile(hFile, &tCameraData, sizeof(Engine::CAMERA_SAVEDATA), &dwBytes, nullptr);

			//Node Save
			Engine::NODE_SAVEDATA tNodeData = Engine::NODE_SAVEDATA();
			for (auto pNode : *pvecNode)
			{
				tNodeData.vPoint = pNode->Get_Point();
				//노드 데이터 저장
				WriteFile(hFile, &tNodeData, sizeof(Engine::NODE_SAVEDATA), &dwBytes, nullptr);
			}
		}
		CloseHandle(hFile);
	}
	UpdateData(FALSE);
}


void CDirectionTool::OnBnClicked_Load()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		TRUE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"DirectionInfo.txt", /* 디폴트 파일이름 */
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
			Engine::CAMERA_SAVEDATA tCameraData = Engine::CAMERA_SAVEDATA();
			//데이터 가져오기
			ReadFile(hFile, &tCameraData, sizeof(Engine::CAMERA_SAVEDATA), &dwBytes, nullptr);
			if (0 == dwBytes)
				break;
			tCameraData.vPos += m_vPos_Load;

			CDirectionObj* pGameObject = CDirectionObj::Create(m_pEditMgr->m_pGraphicDev, 0);
			if (!pGameObject) continue;
			Engine::CComponent* pComponent = nullptr;
			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				pObjectInfoCom->Set_ObjectKey(tCameraData.szObjectKey);
				pObjectInfoCom->Set_Option(tCameraData.iOption);
			}

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				pTransCom->Set_Pos(tCameraData.vPos);
			}

			//Node Save/////////////////////////////////////////////////////////			
			for (int idx_Node = 0; idx_Node < tCameraData.iCount_Node; ++idx_Node)
			{
				Engine::NODE_SAVEDATA tNodeData = Engine::NODE_SAVEDATA();
				ReadFile(hFile, &tNodeData, sizeof(Engine::NODE_SAVEDATA), &dwBytes, nullptr);
				Engine::CCurve* pNode = nullptr;
				pGameObject->Add_Node(&pNode, tNodeData.vPoint);
			}

			pScene->Add_Object(L"GameLogic_Direction", tCameraData.szObjectKey, pGameObject);

			auto hRootItem = m_DirectionTree.InsertItem(tCameraData.szObjectKey, TVI_ROOT, TVI_LAST);
			m_DirectionTree.SetItemData(hRootItem, (DWORD)pGameObject);
			std::vector<Engine::CCurve*> *pvecNode;
			pvecNode = pGameObject->Get_NodeList();
			auto iMax = (int)pvecNode->size();

			for (int i = 0; i < iMax; ++i)
			{
				HTREEITEM hNodeItem = nullptr;
				hNodeItem = m_DirectionTree.InsertItem(L"Node", hRootItem, TVI_LAST);
				m_DirectionTree.SetItemData(hNodeItem, (DWORD)((*pvecNode)[i]));
			}
		}
		CloseHandle(hFile);
	}
}


void CDirectionTool::OnBnClicked_Add_Direction()
{
	UpdateData(TRUE);
	if (m_strName_Direction == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	CDirectionObj*		pGameObject = nullptr;
	pGameObject = CDirectionObj::Create(m_pEditMgr->m_pGraphicDev, 0);
	NULL_CHECK(pGameObject);
	Set_ObjectData(pGameObject);

	HTREEITEM hTreeItem = m_DirectionTree.InsertItem(m_strName_Direction, TVI_ROOT, TVI_LAST);
	m_DirectionTree.SetItemData(hTreeItem, (DWORD)pGameObject);
	m_DirectionTree.SelectItem(hTreeItem);

	m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic_Direction", m_strName_Direction, pGameObject);

	UpdateData(FALSE);
}


void CDirectionTool::OnBnClicked_Adjust_Direction()
{
	UpdateData(TRUE);
	if (m_strName_Direction == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}

	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_DirectionTree, m_DirectionTree.GetSelectedItem());
	if (nullptr == hItem_Root)
		return;

	CDirectionObj*		pGameObject = nullptr;
	pGameObject = (CDirectionObj*)m_DirectionTree.GetItemData(hItem_Root);
	NULL_CHECK(pGameObject);
	Set_ObjectData(pGameObject);
}


void CDirectionTool::OnBnClicked_Del_Direction()
{
	Clear_SelectedNode();
	auto hItem = m_DirectionTree.GetSelectedItem();
	if (nullptr == hItem) return;
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_DirectionTree, hItem);
	if (nullptr == hItem_Root) return;
	auto pGameObject = (CDirectionObj*)m_DirectionTree.GetItemData(hItem_Root);
	pGameObject->Set_Event(EVENT_DEAD);
	m_DirectionTree.DeleteItem(hItem_Root);
}


void CDirectionTool::OnBnClicked_Add_Node()
{
	UpdateData(TRUE);
	if (m_strName_Node == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	auto hItem = m_DirectionTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_DirectionTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 카메라가 없습니다.");
		return;
	}
	auto pDirectionObj = (CDirectionObj*)m_DirectionTree.GetItemData(hItem_Parents);
	Add_Node(hItem_Parents, pDirectionObj, m_vPos_Node);

	UpdateData(FALSE);
}


void CDirectionTool::OnBnClicked_Adjust_Node()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_lstSelectedNode.size() == 1)
	{
		m_lstSelectedNode.front()->Set_Point(m_vPos_Node);
	}
	UpdateData(FALSE);
}


void CDirectionTool::OnBnClicked_Del_Node()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDirectionTool::OnTvnSelchanged_DirectionTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	Clear_SelectedNode();
	UpdateData(TRUE);
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	if (nullptr == hTreeItem) return;
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_DirectionTree, hTreeItem);
	auto pDirectionObj = (CDirectionObj*)m_DirectionTree.GetItemData(hItem_Root);

	switch (iLevel)
	{
	case 0:	// 최상위
		break;
	case 1: // 노드
	{
		auto pNode = (Engine::CCurve*)m_DirectionTree.GetItemData(hTreeItem);
		Get_NodeData(pNode);
		Add_SelectedNode(pNode);
		break;
	}
	default:
		break;
	}
	UpdateData(FALSE);
}
