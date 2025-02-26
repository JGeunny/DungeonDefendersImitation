// CameraTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "CameraTool.h"
#include "afxdialogex.h"
#include "EditMgr.h"
#include "CameraObj.h"
#include "MainScene.h"
// CCameraTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraTool, CDialog)

CCameraTool::CCameraTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CAMERATOOL, pParent)
	, m_pEditMgr(CEditMgr::GetInstance())
	, m_vPos_Camera(0.f, 0.f, 0.f)
	, m_strName_Camera(_T("Camera"))
	, m_dwOption_Camera(0)
	, m_vPos_Node(0.f, 0.f, 0.f)
	, m_vPos_Node_Interval(5.f, 0.f, 0.f)
	, m_fSpeed_Node(0.5f)
	, m_strName_Node(_T("Section"))
	, m_dwOption_Node(0)
	, m_vPos_Load(0.f, 0.f, 0.f)
	, m_iPlay(0)
	, m_bPlay(FALSE)
{

}

CCameraTool::~CCameraTool()
{
	Free();
}

void CCameraTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON3, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON5, m_btnPause);
	DDX_Control(pDX, IDC_BUTTON6, m_btnStop);
	DDX_Text(pDX, IDC_EDIT1, m_vPos_Camera.x);
	DDX_Text(pDX, IDC_EDIT3, m_vPos_Camera.y);
	DDX_Text(pDX, IDC_EDIT4, m_vPos_Camera.z);
	DDX_Text(pDX, IDC_EDIT11, m_strName_Camera);
	DDX_Text(pDX, IDC_EDIT29, m_dwOption_Camera);
	DDX_Text(pDX, IDC_EDIT7, m_vPos_Node.x);
	DDX_Text(pDX, IDC_EDIT8, m_vPos_Node.y);
	DDX_Text(pDX, IDC_EDIT9, m_vPos_Node.z);
	DDX_Text(pDX, IDC_EDIT18, m_vPos_Node_Interval.x);
	DDX_Text(pDX, IDC_EDIT20, m_vPos_Node_Interval.y);
	DDX_Text(pDX, IDC_EDIT30, m_vPos_Node_Interval.z);
	DDX_Text(pDX, IDC_EDIT21, m_fSpeed_Node);
	DDX_Text(pDX, IDC_EDIT13, m_strName_Node);
	DDX_Text(pDX, IDC_EDIT12, m_dwOption_Node);
	DDX_Text(pDX, IDC_EDIT22, m_vPos_Load.x);
	DDX_Text(pDX, IDC_EDIT23, m_vPos_Load.y);
	DDX_Text(pDX, IDC_EDIT24, m_vPos_Load.z);
	DDX_Control(pDX, IDC_SLIDER1, m_PlayBar);
	DDX_Slider(pDX, IDC_SLIDER1, m_iPlay);
	DDX_Control(pDX, IDC_TREE1, m_CameraTree);
}

HRESULT CCameraTool::Ready_CameraTool()
{
	m_btnPlay.SetFont(&m_pEditMgr->m_Font_Default, TRUE);
	m_btnPause.SetFont(&m_pEditMgr->m_Font_Default, TRUE);
	m_btnStop.SetFont(&m_pEditMgr->m_Font_Default, TRUE);

	m_PlayBar.SetRange(0, 20000);
	
	return S_OK;
}

void CCameraTool::Update_CameraTool()
{
	UpdateData(TRUE);
	if(TRUE == m_bPlay)
		++m_iPlay;
	if (m_iPlay >= m_PlayBar.GetRangeMax())
		m_bPlay = FALSE;
	UpdateData(FALSE);
}

void CCameraTool::Free()
{
}

void CCameraTool::BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (auto pNode : m_lstSelectedNode)
	{
		auto vPos = pNode->Get_Point();
		*((_float*)&vPos + ePos) += fControlSpeed;
		pNode->Set_Point(vPos);
	}
}

void CCameraTool::Event_LButton()
{
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		Put_Vertex();
	else
		Select_Vertex();
}

void CCameraTool::Event_MouseMove()
{
}

void CCameraTool::Event_RButton()
{
}

void CCameraTool::Event_QButton()
{
}

bool CCameraTool::Put_Vertex()
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
			OnBnClickedAdd_Node();
			bCrash = true;
			break;
		}
	}

	return bCrash;
}

bool CCameraTool::Select_Vertex()
{
	//UpdateData(TRUE);

	//bool bCrash = false;
	//bool bSelectCell = false;

	//_matrix matTemp;
	//D3DXMatrixIdentity(&matTemp);
	//////1 셀에서 찾기
	//auto iMax = m_CellListBox.GetCount();
	//bSelectCell = false;
	//for (int idx = 0; (idx < iMax) && !bSelectCell; ++idx)
	//{
	//	
	//	auto pCell = (Engine::CCurve*)m_CellListBox.GetItemData(idx);

	//	for (int idx_Point = 0; (idx_Point < Engine::CCell::POINT_END) && !bSelectCell; ++idx_Point)
	//	{
	//		auto pMesh = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderMesh();
	//		auto pmatWorld = pCell->m_pColliderCom_Point[idx_Point]->Get_ColliderWorld();
	//		if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, pmatWorld, nullptr))
	//		{
	//			bSelectCell = true;
	//		}
	//	}
	//	if (bSelectCell)
	//		Select_Node(pNode);
	//}
	//UpdateData(FALSE);

	auto hItem = m_CameraTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_CameraTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 카메라가 없습니다.");
		return false;
	}
	auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem_Parents);
	for (int iType = 0; iType < Engine::CAMERATYPE::CAMERATYPE_END; ++iType)
	{
		auto plstNode = pCameraObj->Get_NodeList((Engine::CAMERATYPE)iType);
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

bool CCameraTool::Select_Node(Engine::CCurve * pNode)
{
	if (!pNode) return false;

	auto hItem = m_CameraTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_CameraTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 카메라가 없습니다.");
		return false;
	}
	HTREEITEM hItem_Node = find_if_tree(&m_CameraTree, hItem_Parents, [&](auto _pNode) {
											return (DWORD)pNode == _pNode; 
										});
	if (nullptr != hItem_Node)
	{
		m_CameraTree.SelectItem(hItem_Node);
		Add_SelectedNode(pNode);
	}
	
	return true;
}

void CCameraTool::Add_SelectedNode(Engine::CCurve * pNode)
{
	if (nullptr == pNode) return;
	auto iter = find_if(m_lstSelectedNode.begin(), m_lstSelectedNode.end(), [&](auto _pNode) {
							return _pNode == pNode;
						});
	if (iter != m_lstSelectedNode.end()) return;
	pNode->Get_Collider()->Set_CollType(Engine::COLLTYPE::COL_SELECT);
	m_lstSelectedNode.emplace_back(pNode);
}

void CCameraTool::Clear_SelectedNode()
{
	for (auto& pNode : m_lstSelectedNode)
	{
		pNode->Get_Collider()->Set_CollType(Engine::COLLTYPE::COL_FALSE);
		pNode = nullptr;
	}
	m_lstSelectedNode.clear();
}

HTREEITEM CCameraTool::Get_RootItem(_int* pOut_Level, CTreeCtrl* pTreeCtr, HTREEITEM hItem)
{
	HTREEITEM hNextItem = pTreeCtr->GetParentItem(hItem);
	if (nullptr == hNextItem)
		return hItem;
	++*pOut_Level;
	return Get_RootItem(pOut_Level, pTreeCtr, hNextItem);
}

HRESULT CCameraTool::Add_Node(HTREEITEM hItem_Parents, CCameraObj* pCameraObj, Engine::CAMERATYPE eType, _vec3 vPos_Node)
{
	Engine::CCurve* pNode = nullptr;
	pCameraObj->Add_Node(eType, &pNode, vPos_Node);
	NULL_CHECK_RETURN(pNode, E_FAIL);
	Set_NodeData(pNode, false);
	CString strType = L"";
	switch (eType)
	{
	case Engine::CAMERATYPE_EYE:
		strType = L"Eye";
		break;
	case Engine::CAMERATYPE_AT:
		strType = L"At";
		break;
	case Engine::CAMERATYPE_END:
		break;
	default:
		break;
	}

	auto hItem_Node = m_CameraTree.InsertItem(strType, hItem_Parents, TVI_LAST);
	m_CameraTree.SetItemData(hItem_Node, (DWORD)pNode);
	return S_OK;
}

void CCameraTool::Set_ObjectData(CCameraObj * pGameObject)
{
	if (!pGameObject) return;

	//오브젝트 정보 저장
	Engine::CComponent* pComponent = nullptr;
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		pObjectInfoCom->Set_ObjectKey(m_strName_Camera.GetString());
		pObjectInfoCom->Set_Option(m_dwOption_Camera);
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		pTransCom->Set_Pos(m_vPos_Camera);
	}
}

void CCameraTool::Get_ObjectData(CCameraObj * pGameObject)
{
	if (!pGameObject) return;
	Engine::CComponent* pComponent = nullptr;

	//오브젝트 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		m_strName_Camera = pObjectInfoCom->Get_ObjectKey().c_str();
		m_dwOption_Camera = pObjectInfoCom->Get_Option();
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		m_vPos_Camera = pTransCom->Get_Pos();
	}
}

void CCameraTool::Set_NodeData(Engine::CCurve * pNode, _bool bChange_Pos)
{
	if (nullptr == pNode) return;
	if(bChange_Pos)
		pNode->Set_Point(m_vPos_Node);
	pNode->Set_Speed(m_fSpeed_Node);
}

void CCameraTool::Get_NodeData(Engine::CCurve * pNode)
{
	if (nullptr == pNode) return;
	m_vPos_Node = pNode->Get_Point();
	m_fSpeed_Node = pNode->Get_Speed();
}



BEGIN_MESSAGE_MAP(CCameraTool, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON32, &CCameraTool::OnBnClicked_New)
	ON_BN_CLICKED(IDC_BUTTON23, &CCameraTool::OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BUTTON24, &CCameraTool::OnBnClicked_Load)
	ON_BN_CLICKED(IDC_BUTTON3, &CCameraTool::OnBnClicked_Play)
	ON_BN_CLICKED(IDC_BUTTON5, &CCameraTool::OnBnClicked_Pause)
	ON_BN_CLICKED(IDC_BUTTON6, &CCameraTool::OnBnClicked_Stop)
	ON_BN_CLICKED(IDC_BUTTON1, &CCameraTool::OnBnClickedAdd_Camera)
	ON_BN_CLICKED(IDC_BUTTON2, &CCameraTool::OnBnClickedAdjust_Camera)
	ON_BN_CLICKED(IDC_BUTTON14, &CCameraTool::OnBnClickedDel_Camera)
	ON_BN_CLICKED(IDC_BUTTON8, &CCameraTool::OnBnClickedAdd_Node)
	ON_BN_CLICKED(IDC_BUTTON15, &CCameraTool::OnBnClickedAdjust_Node)
	ON_BN_CLICKED(IDC_BUTTON9, &CCameraTool::OnBnClickedDel_Node)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CCameraTool::OnTvnSelchangedCameraTree)
END_MESSAGE_MAP()


// CCameraTool 메시지 처리기입니다.

void CCameraTool::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;

	CDialog::OnClose();
}

void CCameraTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pEditMgr->m_eFocus = FOCUSID_CAMERATOOL;
}


void CCameraTool::OnBnClicked_New()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	for (auto hItem = m_CameraTree.GetRootItem();
		hItem != nullptr;
		hItem = m_CameraTree.GetNextSiblingItem(hItem))
	{
		auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem);
		pCameraObj->Set_Event(EVENT_DEAD);
	}
	m_lstSelectedNode.clear();
	m_CameraTree.DeleteAllItems();
	UpdateData(FALSE);
}


void CCameraTool::OnBnClicked_Save()
{
	UpdateData(TRUE);
	
	int iMax = m_CameraTree.GetCount();
	if (iMax <= 0)
	{
		ERR_MSG(L"카메라 오브젝트가 없습니다.");
		return;// S_FALSE;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		FALSE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"CameraInfo.txt", /* 디폴트 파일이름 */
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

		for (auto hItem = m_CameraTree.GetRootItem();
			hItem != nullptr;
			hItem = m_CameraTree.GetNextSiblingItem(hItem))
		{
			//Camera Save
			Engine::CAMERA_SAVEDATA tCameraData = Engine::CAMERA_SAVEDATA();
			CCameraObj* pGameObject = nullptr;
			pGameObject = (CCameraObj*)m_CameraTree.GetItemData(hItem);
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

			std::vector<Engine::CCurve*> *pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_END];
			pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_EYE] = pGameObject->Get_NodeList(Engine::CAMERATYPE::CAMERATYPE_EYE);
			pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_AT] = pGameObject->Get_NodeList(Engine::CAMERATYPE::CAMERATYPE_AT);
			tCameraData.iCount_Node = pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_EYE]->size();

			//카메라 데이터 저장
			WriteFile(hFile, &tCameraData, sizeof(Engine::CAMERA_SAVEDATA), &dwBytes, nullptr);

			//Node Save
			Engine::NODE_SAVEDATA tNodeData = Engine::NODE_SAVEDATA();
			for (auto pvecNode : pvecNodeArr)
			{
				for (auto pNode : *pvecNode)
				{
					tNodeData.vPoint = pNode->Get_Point();
					tNodeData.fSpeed = pNode->Get_Speed();
					//노드 데이터 저장
					WriteFile(hFile, &tNodeData, sizeof(Engine::NODE_SAVEDATA), &dwBytes, nullptr);
				}
			}
		}
		CloseHandle(hFile);
	}
	UpdateData(FALSE);
}


void CCameraTool::OnBnClicked_Load()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		TRUE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"CameraInfo.txt", /* 디폴트 파일이름 */
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

			CCameraObj* pGameObject = CCameraObj::Create(m_pEditMgr->m_pGraphicDev, 0);
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
			for (int iType_Camera = 0; iType_Camera < Engine::CAMERATYPE::CAMERATYPE_END; ++iType_Camera)
			{
				for (int idx_Node = 0; idx_Node < tCameraData.iCount_Node; ++idx_Node)
				{
					Engine::NODE_SAVEDATA tNodeData = Engine::NODE_SAVEDATA();
					ReadFile(hFile, &tNodeData, sizeof(Engine::NODE_SAVEDATA), &dwBytes, nullptr);
					Engine::CCurve* pNode = nullptr;
					pGameObject->Add_Node((Engine::CAMERATYPE)iType_Camera, &pNode, tNodeData.vPoint);

					//오브젝트 정보 저장
					if (nullptr != pNode)
					{
						pNode->Set_Speed(tNodeData.fSpeed);
					}
				}
			}

			pScene->Add_Object(L"GameLogic_Camera", tCameraData.szObjectKey, pGameObject);

			auto hRootItem = m_CameraTree.InsertItem(tCameraData.szObjectKey, TVI_ROOT, TVI_LAST);
			m_CameraTree.SetItemData(hRootItem, (DWORD)pGameObject);
			std::vector<Engine::CCurve*> *pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_END];
			pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_EYE] = pGameObject->Get_NodeList(Engine::CAMERATYPE::CAMERATYPE_EYE);
			pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_AT] = pGameObject->Get_NodeList(Engine::CAMERATYPE::CAMERATYPE_AT);
			auto iMax = (int)pvecNodeArr[Engine::CAMERATYPE::CAMERATYPE_EYE]->size();

			for (int i = 0; i < iMax; ++i)
			{
				auto hSectionItem = m_CameraTree.InsertItem(L"Section", hRootItem, TVI_LAST);
				for (int iType_Camera = 0; iType_Camera < Engine::CAMERATYPE::CAMERATYPE_END ; ++iType_Camera)
				{
					HTREEITEM hNodeItem = nullptr;
					switch (iType_Camera)
					{
					case Engine::CAMERATYPE::CAMERATYPE_EYE:
						hNodeItem = m_CameraTree.InsertItem(L"Eye", hSectionItem, TVI_LAST);
							break;
					case Engine::CAMERATYPE::CAMERATYPE_AT:
						hNodeItem = m_CameraTree.InsertItem(L"At", hSectionItem, TVI_LAST);
							break;
					}
					if(nullptr != hNodeItem)
						m_CameraTree.SetItemData(hNodeItem, (DWORD)((*pvecNodeArr[iType_Camera])[i]));
				}
			}			

			//int idx = m_NaviListBox.AddString(tCameraData.szObjectKey);
			//m_NaviListBox.SetItemData(idx, (DWORD)pGameObject);
		}
		CloseHandle(hFile);

		//InitObjList();
	}
}


void CCameraTool::OnBnClicked_Play()
{
	auto hItem = m_CameraTree.GetSelectedItem();
	if (nullptr == hItem) return;
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_CameraTree, hItem);
	if (nullptr == hItem_Root) return;
	auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem_Root);

	UpdateData(TRUE);
	if (m_iPlay >= m_PlayBar.GetRangeMax())
		m_iPlay = 0;
	pCameraObj->Play_Camera();
	m_bPlay = TRUE;
	UpdateData(FALSE);
}


void CCameraTool::OnBnClicked_Pause()
{
	UpdateData(TRUE);
	m_bPlay = FALSE;
	UpdateData(FALSE);
}


void CCameraTool::OnBnClicked_Stop()
{
	UpdateData(TRUE);
	m_bPlay = FALSE;
	m_iPlay = 0;
	UpdateData(FALSE);
}


void CCameraTool::OnBnClickedAdd_Camera()
{
	UpdateData(TRUE);
	if (m_strName_Camera == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	CCameraObj*		pGameObject = nullptr;
	pGameObject = CCameraObj::Create(m_pEditMgr->m_pGraphicDev, 0);
	NULL_CHECK(pGameObject);
	Set_ObjectData(pGameObject);

	HTREEITEM hTreeItem = m_CameraTree.InsertItem(m_strName_Camera, TVI_ROOT, TVI_LAST);
	m_CameraTree.SetItemData(hTreeItem, (DWORD)pGameObject);
	m_CameraTree.SelectItem(hTreeItem);

	m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic_Camera", m_strName_Camera, pGameObject);

	UpdateData(FALSE);
}


void CCameraTool::OnBnClickedAdjust_Camera()
{
	UpdateData(TRUE);
	if (m_strName_Camera == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_CameraTree, m_CameraTree.GetSelectedItem());
	if (nullptr == hItem_Root)
		return;

	CCameraObj*		pGameObject = nullptr;
	pGameObject = (CCameraObj*)m_CameraTree.GetItemData(hItem_Root);
	NULL_CHECK(pGameObject);
	Set_ObjectData(pGameObject);

	//HTREEITEM hTreeItem = m_CameraTree.InsertItem(m_strName_Camera, TVI_ROOT, TVI_LAST);
	//m_CameraTree.SetItemData(hTreeItem, (DWORD)pGameObject);
	//m_CameraTree.SelectItem(hItem_Root);

	//m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic_Camera", m_strName_Camera, pGameObject);

	//UpdateData(FALSE);
}


void CCameraTool::OnBnClickedDel_Camera()
{
	Clear_SelectedNode();
	auto hItem = m_CameraTree.GetSelectedItem();
	if (nullptr == hItem) return;
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_CameraTree, hItem);
	if (nullptr == hItem_Root) return;
	auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem_Root);
	pCameraObj->Set_Event(EVENT_DEAD);
	m_CameraTree.DeleteItem(hItem_Root);
}


void CCameraTool::OnBnClickedAdd_Node()
{
	UpdateData(TRUE);
	if (m_strName_Node == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	auto hItem = m_CameraTree.GetSelectedItem();
	int iLevel = 0;
	auto hItem_Parents = Get_RootItem(&iLevel, &m_CameraTree, hItem);

	if (nullptr == hItem_Parents)
	{
		ERR_MSG(L"선택된 카메라가 없습니다.");
		return;
	}
	auto hItem_Section = m_CameraTree.InsertItem(m_strName_Node, hItem_Parents, TVI_LAST);
	auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem_Parents);

	_vec3 vInterval = { 5.f, 0.f, 0.f };

	auto vEyePos = m_vPos_Node;
	auto vAtPos = m_vPos_Node + m_vPos_Node_Interval;

	Add_Node(hItem_Section, pCameraObj, Engine::CAMERATYPE::CAMERATYPE_EYE, vEyePos);
	Add_Node(hItem_Section, pCameraObj, Engine::CAMERATYPE::CAMERATYPE_AT, vAtPos);
	m_CameraTree.SelectItem(hItem_Section);

	UpdateData(FALSE);
}


void CCameraTool::OnBnClickedAdjust_Node()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_lstSelectedNode.size() == 1)
	{
		m_lstSelectedNode.front()->Set_Point(m_vPos_Node);
		m_lstSelectedNode.front()->Set_Speed(m_fSpeed_Node);
	}
	UpdateData(FALSE);
}


void CCameraTool::OnBnClickedDel_Node()
{
	//int idx = m_CameraListBox.GetCurSel();
	//if (idx == -1) return;
	//auto pCameraObj = (CCameraObj*)m_CameraListBox.GetItemData(idx);
	//m_CameraListBox.DeleteString(idx);

	//int idx_Eye = m_EyeListBox.GetCurSel();
	//if (idx_Eye == -1) return;

	//auto pGameObject = (CCameraObj*)m_EyeListBox.GetItemData(idx);
	//m_CameraListBox.DeleteString(idx);
	//if (pGameObject)
	//	pGameObject->Set_Event(EVENT_DEAD);
}


void CCameraTool::OnTvnSelchangedCameraTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	Clear_SelectedNode();
	UpdateData(TRUE);
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	if (nullptr == hTreeItem) return;
	int iLevel = 0;
	auto hItem_Root = Get_RootItem(&iLevel, &m_CameraTree, hTreeItem);
	auto pCameraObj = (CCameraObj*)m_CameraTree.GetItemData(hItem_Root);

	switch (iLevel)
	{
	case 0:	// 최상위
		break;
	case 1: // 섹션
	{
		break;
	}
	case 2: // 노드
	{
		auto pNode = (Engine::CCurve*)m_CameraTree.GetItemData(hTreeItem);
		Get_NodeData(pNode);
		Add_SelectedNode(pNode);
		break;
	}
	default:
		break;
	}
	UpdateData(FALSE);
}