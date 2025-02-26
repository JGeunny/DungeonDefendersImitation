// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "afxdialogex.h"

#include "MainScene.h"
#include "EditMgr.h"
#include "ToolView.h"
#include "ToolScene.h"
#include "MainScene.h"
#include "ConfirmFormView.h"
//#include "FileInfo.h"

#include "Layer.h"
#include "GameObject.h"
#include "Terrain.h"
#include "CollisionMgr.h"
#include "MeshObj.h"

#include <functional>
#include <algorithm>
#include <iostream>
// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: m_pEditMgr(CEditMgr::GetInstance())
	, CDialog(IDD_MAPTOOL, pParent)
	, m_vPos_Object(0.f, 0.f, 0.f)
	, m_vAngle_Object(0.f, 0.f, 0.f)
	, m_vScale_Object(1.f, 1.f, 1.f)
	, m_vMultiAdd_Object(1.f, 1.f, 1.f)
	, m_vPos_Load(0.f, 0.f, 0.f)
	, m_strPreviewScale(L"(0,0,0)")
	, m_dwOption(0)
	, m_strName(_T("Mesh"))
	, m_iRndPercentage(100)
	, m_bChkRot(TRUE)
	, m_bChkScale(TRUE)
{
}

CMapTool::~CMapTool()
{
	Free();
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_vPos_Object.x);
	DDX_Text(pDX, IDC_EDIT2, m_vPos_Object.y);
	DDX_Text(pDX, IDC_EDIT3, m_vPos_Object.z);
	DDX_Text(pDX, IDC_EDIT4, m_vAngle_Object.x);
	DDX_Text(pDX, IDC_EDIT5, m_vAngle_Object.y);
	DDX_Text(pDX, IDC_EDIT6, m_vAngle_Object.z);
	DDX_Text(pDX, IDC_EDIT7, m_vScale_Object.x);
	DDX_Text(pDX, IDC_EDIT8, m_vScale_Object.y);
	DDX_Text(pDX, IDC_EDIT9, m_vScale_Object.z);
	DDX_Text(pDX, IDC_EDIT10, m_dwOption);
	DDX_Text(pDX, IDC_EDIT11, m_strName);
	DDX_Control(pDX, IDC_LIST1, m_CubeListBox);
	DDX_Control(pDX, IDC_LIST2, m_TextureListBox);
	DDX_Control(pDX, IDC_STATIC_MAPTOOL_PREVIEW_TEXTURE, m_PictureTexture);
	DDX_Text(pDX, IDC_EDIT17, m_vMultiAdd_Object.x);
	DDX_Text(pDX, IDC_EDIT18, m_vMultiAdd_Object.y);
	DDX_Text(pDX, IDC_EDIT19, m_vMultiAdd_Object.z);
	DDX_Control(pDX, IDC_COMBO2, m_TextureCbo);
	DDX_Text(pDX, IDC_EDIT20, m_iRndPercentage);
	DDX_Check(pDX, IDC_CHECK5, m_bChkRot);
	DDX_Check(pDX, IDC_CHECK6, m_bChkScale);
	DDX_Text(pDX, IDC_EDIT22, m_vPos_Load.x);
	DDX_Text(pDX, IDC_EDIT23, m_vPos_Load.y);
	DDX_Text(pDX, IDC_EDIT24, m_vPos_Load.z);
	DDX_Text(pDX, IDC_TEXTSCALE, m_strPreviewScale);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON2, &CMapTool::OnBnClickedAdjust)
	ON_BN_CLICKED(IDC_BUTTON4, &CMapTool::OnBnClickedDelete)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnLbnSelchangeCubeList)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON21, &CMapTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON22, &CMapTool::OnBnClickedLoad)
	ON_LBN_SELCHANGE(IDC_LIST2, &CMapTool::OnLbnSelchangeTexture)
	ON_BN_CLICKED(IDC_BUTTON28, &CMapTool::OnBnClickedMultiAdd)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CMapTool::OnCbnSelchangeTextureCbo)
	ON_BN_CLICKED(IDC_BUTTON30, &CMapTool::OnBnClickedRndAdd)
	ON_BN_CLICKED(IDC_BUTTON31, &CMapTool::OnBnClickedNew)
	ON_BN_CLICKED(IDC_BUTTON37, &CMapTool::OnBnClickedDelName)
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATEAPP()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMapTool 메시지 처리기입니다.


BOOL CMapTool::OnInitDialog()
{
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CDialog::OnInitDialog();
	//srand((unsigned)time(NULL));

	//auto lstCubeTextureKey = m_pEditMgr->m_pResourcesMgr->GetTextureKeyList(Engine::TEXTURETYPE::Cube);

	//m_TextureListBox.ResetContent();
	//for (auto strKey : lstCubeTextureKey)
	//	m_TextureListBox.AddString(strKey.c_str());
	//
	//for (auto strImageKey : lstCubeTextureKey)
	//{
	//	m_mapTextureInfo.emplace(strImageKey, dynamic_cast<Engine::CTexture*>(m_pEditMgr->m_pResourcesMgr->
	//		Clone(Engine::RESOURCETYPE::Dynamic, strImageKey)));
	//}

	//m_pToolScene = CToolScene::Create(m_pEditMgr->m_pDevice, &m_PictureTexture);
	//m_pEditMgr->AddScene(L"MapToolTexture", m_pToolScene);

	//Engine::CGameObject* pGameObject = CFreeCamera::Create(m_pEditMgr->m_pDevice, Engine::CameraName::Player);
	//NULL_CHECK_RETURN(pGameObject, FALSE);
	//auto pCamera = (CFreeCamera*)pGameObject;
	//pCamera->SetUseControl(false);
	//pCamera->SetViewMatrix(&D3DXVECTOR3(0.f, 0.f, -40.f), &D3DXVECTOR3(0.f, 0.f, 1.f), &D3DXVECTOR3(0.f, 1.f, 0.f));
	//pCamera->SetProjectionMatrix(D3DXToRadian(45.f), 4.f / 3.f, 1.f, 1000.f);
	////pCamera->SetAngle();

	//m_pToolScene->AddGameObject(Engine::LAYERID::GameObject, L"StaticCamera", pGameObject);

	Ready_TextureCboList();
	UpdateData(TRUE);
	return TRUE;  
}

void CMapTool::OnBnClickedAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_strName == L"")
	{
		ERR_MSG(L"이름을 입력 해주세요.");
		return;
	}
	Engine::CGameObject*		pGameObject = nullptr;

	int idxTexture = m_TextureListBox.GetCurSel();
	if (idxTexture == -1) return;

	//선택된 놈들이 없다면 그냥 생산 있다면 여러가지 한꺼번에 생산
	if (m_lstSelectedObject.size() == 0)
	{
		pGameObject = CMeshObj::Create(m_pEditMgr->m_pGraphicDev, 0);
		NULL_CHECK(pGameObject);
		Set_ObjectData(pGameObject);

		int idx = m_CubeListBox.AddString(m_strName);
		m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
		m_CubeListBox.SetCurSel(idx);

		m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic", m_strName, pGameObject);
	}
	else
	{
		for (auto& pObj : m_lstSelectedObject)
		{
			Get_ObjectData(pObj);
			UpdateData(FALSE);
			UpdateData(TRUE);
			pGameObject = CMeshObj::Create(m_pEditMgr->m_pGraphicDev, 0);
			Set_ObjectData(pGameObject);

			int idx = m_CubeListBox.AddString(m_strName);
			m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
			m_CubeListBox.SetCurSel(idx);

			m_pEditMgr->m_pMainScene->Add_Object(L"GameLogic", m_strName, pGameObject);
		}
	}
	UpdateData(FALSE);
	Change_PreView();
}


void CMapTool::OnBnClickedAdjust()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int idx = m_CubeListBox.GetCurSel();
	if (idx == -1) return;

	int iSize = m_lstSelectedObject.size();
	for (auto pObj : m_lstSelectedObject)
	{
		Set_ObjectData(pObj, iSize == 1);
		int iMax = m_CubeListBox.GetCount();
		for (int i = 0; i < iMax; ++i)
		{
			if (pObj == (Engine::CGameObject*)m_CubeListBox.GetItemData(i))
			{
				m_CubeListBox.DeleteString(i);
				idx = m_CubeListBox.AddString(m_strName);
				m_CubeListBox.SetItemData(idx, (DWORD)pObj);
				break;
			}
		}
	}
	UpdateData(FALSE);
	Change_PreView();
}


void CMapTool::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int idx = m_CubeListBox.GetCurSel();
	if (idx == -1) return;

	for (auto pObj : m_lstSelectedObject)
	{
		int iMax = m_CubeListBox.GetCount();
		for (int i = 0; i < iMax; ++i)
		{
			if (pObj == (Engine::CGameObject*)m_CubeListBox.GetItemData(i))
			{
				m_CubeListBox.DeleteString(i);
				pObj->Set_Event(EVENT_DEAD);
				break;
			}
		}
	}
	m_lstSelectedObject.clear();
}


void CMapTool::OnLbnSelchangeCubeList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int idx = m_CubeListBox.GetCurSel();
	if (idx == -1) return;

	m_CubeListBox.GetText(idx, m_strName);
	auto pGameObject = (Engine::CGameObject*)m_CubeListBox.GetItemData(idx);

	Get_ObjectData(pGameObject);

	Add_SelectedObject(pGameObject);
	pGameObject->Set_Selected(true);

	UpdateData(FALSE);
	Change_PreView();
}

HRESULT CMapTool::Ready_MapTool()
{
	m_pPreViewScene = CPreViewScene::Create(m_pEditMgr->m_pGraphicDev, m_PictureTexture.GetSafeHwnd());
	m_pEditMgr->Add_Scene(L"MapTool_PreView", m_pPreViewScene);
	return S_OK;
}

void CMapTool::Free()
{
	for (auto pair : m_mapTextureInfo)
		Engine::Safe_Release(pair.second);
	m_mapTextureInfo.clear();
	m_lstSelectedObject.clear();
}

void CMapTool::Ready_HorizontalScroll()
{
	CString strName = L"";
	CSize size;
	int iCX = 0;

	CDC* pDC = m_TextureListBox.GetDC();

	// CListBox::GetCount함수(리스트 박스 목록의 전체 개수)
	for (int i = 0; i < m_TextureListBox.GetCount(); ++i)
	{
		m_TextureListBox.GetText(i, strName);

		// CDC::GetTextExtent함수(현재 텍스트의 픽셀 단위 크기를 얻어오는 함수)
		size = pDC->GetTextExtent(strName);

		if (iCX < size.cx)
			iCX = size.cx;
	}

	// 현재 리스트박스의 수평 스크롤 최대 범위를 얻어오는 함수.
	if (iCX > m_TextureListBox.GetHorizontalExtent())
		m_TextureListBox.SetHorizontalExtent(iCX); // 수평 스크롤 최대범위 설정.

	m_TextureListBox.ReleaseDC(pDC);
}

void CMapTool::Change_PreView()
{
	int idxTexture = m_TextureListBox.GetCurSel();
	if (idxTexture == -1) return;
	int iResourceType = m_TextureCbo.GetCurSel();
	if (iResourceType == -1) return;
	
	static Engine::CGameObject* pGameObject = nullptr;
	if (!pGameObject)
	{
		pGameObject = CMeshObj::Create(m_pEditMgr->m_pGraphicDev, 0);
		m_pPreViewScene->Add_Object(L"GameLogic", L"MeshObj", pGameObject);
	}

	UpdateData(TRUE);
	Set_ObjectData(pGameObject, false);
	//트렌스폼 정보 저장
	Engine::CComponent *pComponent = nullptr, *pComponent2 = nullptr;
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		pTransCom->Set_Pos(0.f, 0.f, 0.f);
		pComponent2 = pGameObject->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
		if (pComponent2)
		{
			auto pColliderCom = (Engine::CCollider*)pComponent2;
			auto tRange = pColliderCom->Get_Range();
			auto fLen = D3DXVec3Length(&_vec3(tRange->vMax - tRange->vMin));
			auto fScale = 8.f / fLen;
			
			//pTransCom->Set_Pos(-tRange->Get_Center());
			pTransCom->Set_Scale(fScale, fScale, fScale);
			
		}
		auto vScale = pTransCom->Get_Scale();

		TCHAR szScale[MIN_STR];
		swprintf_s(szScale, L"(%f,%f,%f)", vScale.x, vScale.y, vScale.z);
		m_strPreviewScale = szScale;
	}
	UpdateData(FALSE);
}

void CMapTool::Event_LButton()
{
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		Put_Object();
	else
		Pick_Object();
}

void CMapTool::Event_MouseMove()
{
}

void CMapTool::Event_RButton()
{
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		Clear_SelectedObject();
}

void CMapTool::Event_QButton()
{
}

void CMapTool::Set_ObjectData(Engine::CGameObject * pGameObject, bool bTransPos)
{
	if (!pGameObject) return;
	int idxTexture = m_TextureListBox.GetCurSel();
	if (idxTexture == -1) return;
	int iResourceType = m_TextureCbo.GetCurSel();
	if (iResourceType == -1) return;


	CString strTextureKey = L"";
	m_TextureListBox.GetText(idxTexture, strTextureKey);

	//오브젝트 정보 저장
	Engine::CComponent* pComponent = nullptr;
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		pObjectInfoCom->Set_ObjectKey(m_strName.GetString());
		pObjectInfoCom->Set_Option(m_dwOption);
	}

	//메쉬 변경
	pComponent = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pMeshAdminCom = (Engine::CMeshAdmin*)pComponent;
		pMeshAdminCom->Change_Mesh((RESOURCEID)iResourceType, strTextureKey.GetString());
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;
		pTransCom->Set_Pos(m_vPos_Object);
		pTransCom->Set_Scale(m_vScale_Object);
		pTransCom->Set_Radian(D3DXToRadian(m_vAngle_Object));
	}
}

void CMapTool::Get_ObjectData(Engine::CGameObject * pGameObject)
{
	if (!pGameObject) return;
	Engine::CComponent* pComponent = nullptr;

	//오브젝트 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		m_strName = pObjectInfoCom->Get_ObjectKey().c_str();
		m_dwOption = pObjectInfoCom->Get_Option();
	}

	//메쉬 변경
	pComponent = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
	if (pComponent)
	{
		auto pMeshAdminCom = (Engine::CMeshAdmin*)pComponent;
		auto strMeshKey = pMeshAdminCom->Get_MeshKey();
		auto eResourceType = pMeshAdminCom->Get_ResourceType();

		m_TextureCbo.SetCurSel((_int)eResourceType);
		Change_TextureList(eResourceType);
		auto iMax = m_TextureListBox.GetCount();

		CString strCmp;
		for (int i = 0; i < iMax; ++i)
		{
			m_TextureListBox.GetText(i, strCmp);
			if (strMeshKey == strCmp.GetString())
			{
				m_TextureListBox.SetCurSel(i);
				break;
			}
		}
	}

	//트렌스폼 정보 저장
	pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (pComponent)
	{
		auto pTransCom = (Engine::CTransform*)pComponent;

		m_vPos_Object = pTransCom->Get_Pos();
		m_vScale_Object = pTransCom->Get_Scale();
		m_vAngle_Object = D3DXToDegree(pTransCom->Get_Radian());
	}

	//Set_TextureCboList();
	//int iImageIndex = 0;
	//if (pTextureAdminCom)
	//{
	//	iImageIndex = (int)(*pTextureAdminCom->m_ppTextureCom)->m_fFrameCount;
	//}
	//m_TextureCbo.SetCurSel(iImageIndex);
}

bool CMapTool::Pick_Object()
{	
	auto m_pmapLayer = m_pEditMgr->m_pMainScene->Get_LayerMap();
	auto pair_Layer = m_pmapLayer->find(L"GameLogic");
	if (pair_Layer == m_pmapLayer->end()) return false;
	auto& pLayer = pair_Layer->second;

	std::list<Engine::CGameObject*> lstGameObject;
	pLayer->Get_ObjectList(&lstGameObject);
	_matrix matView;
	m_pEditMgr->m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW, &matView);
	if (lstGameObject.size() > 1)
	{
		lstGameObject.sort([&](Engine::CGameObject*& pObject1, Engine::CGameObject*& pObject2)
		{
			auto pComponent1 = pObject1->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			auto pComponent2 = pObject2->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent1 && pComponent2)
			{
				auto pTrans1 = (Engine::CTransform*)pComponent1;
				auto pTrans2 = (Engine::CTransform*)pComponent2;
				auto vPos1 = pTrans1->Get_Pos();
				auto vPos2 = pTrans2->Get_Pos();

				D3DXVec3TransformCoord(&vPos1, &vPos1, &matView);
				D3DXVec3TransformCoord(&vPos2, &vPos2, &matView);
				return vPos1.z < vPos2.z;
			}
			return false;
		});
	}

	for (auto& pGameObject : lstGameObject)
	{
		auto pComponent_Collider = pGameObject->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
		if (!pComponent_Collider) continue;

		auto pCollider = (Engine::CCollider*)pComponent_Collider;

		auto pMesh = pCollider->Get_ColliderMesh();
		auto pmatWorld = pCollider->Get_ColliderWorld();
		if (m_pEditMgr->m_pCalculatorCom->Collision_Mouse(g_hWnd, pMesh, pmatWorld))
		{
			Select_Object(pGameObject);
			break;
		}
	}
	return false;
}

bool CMapTool::Put_Object()
{
	bool bCrash =false;
	UpdateData(TRUE);
	//2 레이어에서 찾기
	std::list<Engine::CGameObject*> lstGameObject;

	auto m_pmapLayer = m_pEditMgr->m_pMainScene->Get_LayerMap();
	auto pair_Layer = m_pmapLayer->find(L"GameLogic");
	if (pair_Layer == m_pmapLayer->end()) return false;
	auto& pLayer = pair_Layer->second;
	pLayer->Get_ObjectList(&lstGameObject);
	for (auto& pGameObject : lstGameObject)
	{
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
			Engine::CGameObject* pGameObject = CMeshObj::Create(m_pEditMgr->m_pGraphicDev, 0);
			m_vPos_Object = vCollisionPos;
			Set_ObjectData(pGameObject);
			int idx = m_CubeListBox.AddString(m_strName);
			m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
			m_CubeListBox.SetCurSel(idx);
			pLayer->Add_Object(L"GameLogic", pGameObject);
			bCrash = true;
			break;
		}
	}
	UpdateData(FALSE);
	return bCrash;
}

bool CMapTool::Select_Object(Engine::CGameObject * pGameObject)
{
	if(!pGameObject) return false;
	for (auto i = 0; i < m_CubeListBox.GetCount(); ++i)
	{
		if (pGameObject == (Engine::CGameObject*)m_CubeListBox.GetItemData(i))
		{
			m_CubeListBox.SetCurSel(i);
			Add_SelectedObject(pGameObject);
			pGameObject->Set_Selected(true);
			UpdateData(TRUE);
			Get_ObjectData(pGameObject);
			UpdateData(FALSE);

			return true;
		}
	}
	return false;
}

void CMapTool::Add_SelectedObject(Engine::CGameObject * pGameObject)
{
	auto iter = find_if(m_lstSelectedObject.begin(), m_lstSelectedObject.end(), [&](auto _pObj){
					return pGameObject == _pObj;
				});
	if(iter == m_lstSelectedObject.end())
		m_lstSelectedObject.emplace_back(pGameObject);
}

void CMapTool::Clear_SelectedObject()
{
	for (auto pGameObject : m_lstSelectedObject)
	{
		pGameObject->Set_Selected(false);
		pGameObject = nullptr;
	}
	m_lstSelectedObject.clear();
}

void CMapTool::Clear_Object()
{
	UpdateData(TRUE);
	int iMax = m_CubeListBox.GetCount();
	for (int i = 0; i < iMax; ++i)
		((Engine::CGameObject*)m_CubeListBox.GetItemData(i))->Set_Event(EVENT_DEAD);
	m_CubeListBox.ResetContent();
	m_lstSelectedObject.clear();
	UpdateData(FALSE);
}

void CMapTool::Remove_Object(CString strName)
{
	int iMax = m_CubeListBox.GetCount();
	CString strObjName = L"";
	bool bContinue = true;

	while (bContinue)
	{
		bContinue = false;
		for (int i = 0; i < iMax; ++i)
		{
			m_CubeListBox.GetText(i, strObjName);
			if (strObjName == strName)
			{
				((Engine::CGameObject*)m_CubeListBox.GetItemData(i))->Set_Event(EVENT_DEAD);
				m_CubeListBox.DeleteString(i);
				bContinue = true;
				break;
			}
		}
	}
	m_lstSelectedObject.clear();
}

void CMapTool::Ready_Camera()
{
	m_pEditMgr->m_pMainScene->Ready_Camera();
}

void CMapTool::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		FALSE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"MapInfo.txt", /* 디폴트 파일이름 */
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

		//Engine::OBJECT_SAVEDATA tObjectData;
		//Engine::CGameObject* pGameObject = nullptr;
		//auto pLayerMap = m_pEditMgr->m_pMainScene->Get_LayerMap();
		//for (auto& pair_Layer : *pLayerMap)
		//{
		//	auto pObjectMap = pair_Layer.second->Get_ObjectMap();
		//	for (auto& pair_Obejct : *pObjectMap)
		//	{
		//		pGameObject = pair_Obejct.second;
		//		if (!pGameObject) continue;
		//		Engine::CComponent* pComponent = nullptr;
		//		_vec3	vTemp;
		//		tObjectData = Engine::OBJECT_SAVEDATA();
		//		lstrcpy(tObjectData.szLayerTag, pair_Layer.first.c_str());

		//		//오브젝트 정보 저장
		//		pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
		//		if (pComponent)
		//		{
		//			auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
		//			lstrcpy(tObjectData.szObjectKey, pObjectInfoCom->Get_ObjectKey().c_str());
		//			tObjectData.iOption = pObjectInfoCom->Get_Option();
		//		}
		//		//오브젝트키가 없다면 저장하지 않는다.
		//		if (!lstrcmp(tObjectData.szObjectKey, L""))
		//			continue;

		//		//메쉬 변경
		//		pComponent = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
		//		if (pComponent)
		//		{
		//			auto pMeshAdminCom = (Engine::CMeshAdmin*)pComponent;
		//			auto strMeshKey = pMeshAdminCom->Get_MeshKey();
		//			auto eResourceType = pMeshAdminCom->Get_ResourceType();
		//			lstrcpy(tObjectData.szMeshKey, strMeshKey.c_str());
		//			tObjectData.iResorceType = (int)eResourceType;
		//		}

		//		//트렌스폼 정보 저장
		//		pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		//		if (pComponent)
		//		{
		//			auto pTransCom = (Engine::CTransform*)pComponent;

		//			tObjectData.vPos = pTransCom->Get_Pos();
		//			tObjectData.vScale = pTransCom->Get_Scale();
		//			tObjectData.vAngle = pTransCom->Get_Radian();
		//		}

		//		//데이터 저장
		//		WriteFile(hFile, &tObjectData, sizeof(Engine::OBJECT_SAVEDATA), &dwBytes, nullptr);
		//	}
		//}
		auto iMax = m_CubeListBox.GetCount();
		for (int i = 0; i < iMax ; ++i)
		{
			Engine::OBJECT_SAVEDATA tObjectData;
			Engine::CGameObject* pGameObject = (Engine::CGameObject*)m_CubeListBox.GetItemData(i);
			if (!pGameObject) continue;
			Engine::CComponent* pComponent = nullptr;
			_vec3	vTemp;
			tObjectData = Engine::OBJECT_SAVEDATA();
			lstrcpy(tObjectData.szLayerTag, L"GameLogic");

			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				lstrcpy(tObjectData.szObjectKey, pObjectInfoCom->Get_ObjectKey().c_str());
				tObjectData.iOption = pObjectInfoCom->Get_Option();
			}
			//오브젝트키가 없다면 저장하지 않는다.
			if (!lstrcmp(tObjectData.szObjectKey, L""))
				continue;

			//메쉬 변경
			pComponent = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pMeshAdminCom = (Engine::CMeshAdmin*)pComponent;
				auto strMeshKey = pMeshAdminCom->Get_MeshKey();
				auto eResourceType = pMeshAdminCom->Get_ResourceType();
				lstrcpy(tObjectData.szMeshKey, strMeshKey.c_str());
				tObjectData.iResorceType = (int)eResourceType;
			}

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				tObjectData.vPos = pTransCom->Get_Pos();
				tObjectData.vScale = pTransCom->Get_Scale();
				tObjectData.vAngle = pTransCom->Get_Radian();
			}

			//데이터 저장
			WriteFile(hFile, &tObjectData, sizeof(Engine::OBJECT_SAVEDATA), &dwBytes, nullptr);
		}
		CloseHandle(hFile);
	}
}


void CMapTool::OnBnClickedLoad()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		TRUE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"MapInfo.txt", /* 디폴트 파일이름 */
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
		Engine::OBJECT_SAVEDATA tObjectData = {};
		while (true)
		{
			//데이터 가져오기
			ReadFile(hFile, &tObjectData, sizeof(Engine::OBJECT_SAVEDATA), &dwBytes, nullptr);
			if (0 == dwBytes)
				break;
			tObjectData.vPos += m_vPos_Load;

			Engine::CGameObject* pGameObject = nullptr;
			if (!lstrcmp(tObjectData.szObjectKey, L"Camera"))
			{

			}
			else if (!lstrcmp(tObjectData.szObjectKey, L"Terrain"))
			{

			}
			else if (!lstrcmp(tObjectData.szObjectKey, L"Navi_Stage1"))
			{
				break;
			}
			else
			{
				pGameObject = CMeshObj::Create(m_pEditMgr->m_pGraphicDev, 0);
			}

			if (!pGameObject) continue;
			Engine::CComponent* pComponent = nullptr;
			//오브젝트 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
				pObjectInfoCom->Set_ObjectKey(tObjectData.szObjectKey);
				pObjectInfoCom->Set_Option(tObjectData.iOption);
			}

			//메쉬 변경
			pComponent = pGameObject->Get_Component(L"Com_MeshAdmin", Engine::COMPONENTID::ID_STATIC);
			if (pComponent)
			{
				auto pMeshAdminCom = (Engine::CMeshAdmin*)pComponent;
				pMeshAdminCom->Change_Mesh((RESOURCEID)tObjectData.iResorceType, tObjectData.szMeshKey);
			}

			//트렌스폼 정보 저장
			pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			if (pComponent)
			{
				auto pTransCom = (Engine::CTransform*)pComponent;

				pTransCom->Set_Pos(tObjectData.vPos);
				pTransCom->Set_Scale(tObjectData.vScale);
				pTransCom->Set_Radian(tObjectData.vAngle);
			}
			
			pScene->Add_Object(tObjectData.szLayerTag, tObjectData.szObjectKey, pGameObject);

			int idx = m_CubeListBox.AddString(tObjectData.szObjectKey);
			m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
		}
		CloseHandle(hFile);

		//InitObjList();
	}
}


void CMapTool::OnLbnSelchangeTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	Change_PreView();
}



void CMapTool::OnBnClickedMultiAdd()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateData(TRUE);
	//if (m_strName == L"")
	//{
	//	ERR_MSG(L"이름을 입력 해주세요.");
	//	return;
	//}

	//int idxTexture = m_TextureListBox.GetCurSel();
	//if (idxTexture == -1) return;

	//CString strTextureKey = L"";
	//m_TextureListBox.GetText(idxTexture, strTextureKey);

	//D3DXVECTOR3 vIterPos = D3DXVECTOR3(m_fX, m_fY, m_fZ);
	//D3DXVECTOR3 vSize = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
	//D3DXVECTOR3 vCount = D3DXVECTOR3(m_fMultiAddX, m_fMultiAddY, m_fMultiAddZ);

	//Engine::CGameObject*		pGameObject = nullptr;

	//vIterPos.y = m_fY;
	//for (int iY = 0; iY < vCount.y; ++iY)
	//{
	//	vIterPos.x = m_fX;
	//	for (int iX = 0; iX < vCount.x; ++iX)
	//	{
	//		vIterPos.z = m_fZ;
	//		for (int iZ = 0; iZ < vCount.z; ++iZ)
	//		{
	//			if ((iY != 0 && iY != vCount.y - 1) && (iX != 0 && iX != vCount.x - 1) && (iZ != 0 && iZ != vCount.z - 1))
	//			{
	//				vIterPos.z += m_fScaleZ * 2;
	//				continue;
	//			}
	//			pGameObject = CCubeObj::Create(m_pEditMgr->m_pDevice);
	//			NULL_CHECK(pGameObject);

	//			auto pTextureCom = (Engine::CTextureAdmin*)pGameObject->Get_Component(L"TextureAdmin");
	//			pTextureCom->SetImageKey(Engine::RESOURCETYPE::Dynamic, strTextureKey.GetString());
	//			int iImageIndex = m_TextureCbo.GetCurSel();
	//			if (iImageIndex > 0)
	//				(*pTextureCom->m_ppTextureCom)->m_fFrameCount = (float)iImageIndex;

	//			auto pObjectInfoCom = (Engine::CObjectInfo*)pGameObject->Get_Component(L"ObjectInfo");
	//			pObjectInfoCom->m_eObjectType = Engine::OBJECTTYPE::Cube;
	//			pObjectInfoCom->m_wstrObjectKey = m_strName.GetString();

	//			auto pOptionCom = (Engine::COption*)pGameObject->Get_Component(L"Option");
	//			pOptionCom->m_dwOption = m_dwOption;

	//			auto pInfoCom = (Engine::CTransform*)pGameObject->Get_Component(L"Transform");
	//			pInfoCom->m_vPos = D3DXVECTOR3(vIterPos.x, vIterPos.y, vIterPos.z);
	//			pInfoCom->m_vSize = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::X] = D3DXToRadian(m_fAngleX);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::Y] = D3DXToRadian(m_fAngleY);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::Z] = D3DXToRadian(m_fAngleZ);
	//			pInfoCom->Update();

	//			m_pEditMgr->m_pToolView->m_pToolScene->AddGameObject(Engine::LAYERID::GameObject, L"CubeObj", pGameObject);
	//			int idx = m_CubeListBox.AddString(m_strName);
	//			m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
	//			m_CubeListBox.SetCurSel(idx);
	//			
	//			vIterPos.z += m_fScaleZ * 2;
	//		}
	//		vIterPos.x += m_fScaleX * 2;
	//	}
	//	vIterPos.y += m_fScaleY * 2;
	//}

	//UpdateData(FALSE);
	//Change_PreView();
}

void CMapTool::OnCbnSelchangeTextureCbo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	auto iSel = m_TextureCbo.GetCurSel();
	if (iSel == -1) return;
	Change_TextureList((RESOURCEID)iSel);
	Change_PreView();
}


void CMapTool::OnBnClickedRndAdd()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateData(TRUE);
	//if (m_strName == L"")
	//{
	//	ERR_MSG(L"이름을 입력 해주세요.");
	//	return;
	//}

	//int idxTexture = m_TextureListBox.GetCurSel();
	//if (idxTexture == -1) return;

	//CString strTextureKey = L"";
	//m_TextureListBox.GetText(idxTexture, strTextureKey);

	//D3DXVECTOR3 vIterPos = D3DXVECTOR3(m_fX, m_fY, m_fZ);
	//D3DXVECTOR3 vSize = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
	//D3DXVECTOR3 vCount = D3DXVECTOR3(m_fMultiAddX, m_fMultiAddY, m_fMultiAddZ);

	//Engine::CGameObject*		pGameObject = nullptr;

	//vIterPos.y = m_fY;
	//for (int iY = 0; iY < vCount.y; ++iY)
	//{
	//	vIterPos.x = m_fX;
	//	for (int iX = 0; iX < vCount.x; ++iX)
	//	{
	//		vIterPos.z = m_fZ;
	//		for (int iZ = 0; iZ < vCount.z; ++iZ)
	//		{
	//			//if ((iY != 0 && iY != vCount.y - 1) && (iX != 0 && iX != vCount.x - 1) && (iZ != 0 && iZ != vCount.z - 1))
	//			//{
	//			//	vIterPos.z += m_fScaleZ * 2;
	//			//	continue;
	//			//}
	//			
	//			int iRand = rand() % m_iRndPercentage;
	//			if (iRand != 0)
	//			{
	//				vIterPos.z += m_fScaleZ * 2;
	//				continue;
	//			}
	//			
	//			D3DXVECTOR3 vRandScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
	//			if (m_bChkScale)
	//			{
	//				vRandScale = D3DXVECTOR3(
	//					(rand() % ((int)(m_fScaleX * 100 * 1.5)) + m_fScaleX * 100) * 0.01f
	//					, (rand() % ((int)(m_fScaleY * 100 * 1.5)) + m_fScaleY * 100) * 0.01f
	//					, (rand() % ((int)(m_fScaleZ * 100 * 1.5)) + m_fScaleZ * 100) * 0.01f
	//				);
	//			}
	//			D3DXVECTOR3 vRandAngle = D3DXVECTOR3(m_fAngleX, m_fAngleY, m_fAngleZ);
	//			if (m_bChkRot)
	//			{
	//				vRandAngle = D3DXVECTOR3(
	//					float(rand() % 360)
	//					, float(rand() % 360)
	//					, float(rand() % 360)
	//				);
	//			}

	//			pGameObject = CCubeObj::Create(m_pEditMgr->m_pDevice);
	//			NULL_CHECK(pGameObject);

	//			auto pTextureCom = (Engine::CTextureAdmin*)pGameObject->Get_Component(L"TextureAdmin");
	//			pTextureCom->SetImageKey(Engine::RESOURCETYPE::Dynamic, strTextureKey.GetString());
	//			int iImageIndex = m_TextureCbo.GetCurSel();
	//			if (iImageIndex > 0)
	//				(*pTextureCom->m_ppTextureCom)->m_fFrameCount = (float)iImageIndex;

	//			auto pObjectInfoCom = (Engine::CObjectInfo*)pGameObject->Get_Component(L"ObjectInfo");
	//			pObjectInfoCom->m_eObjectType = Engine::OBJECTTYPE::Cube;
	//			pObjectInfoCom->m_wstrObjectKey = m_strName.GetString();

	//			auto pOptionCom = (Engine::COption*)pGameObject->Get_Component(L"Option");
	//			pOptionCom->m_dwOption = m_dwOption;

	//			auto pInfoCom = (Engine::CTransform*)pGameObject->Get_Component(L"Transform");
	//			pInfoCom->m_vPos = D3DXVECTOR3(vIterPos.x, vIterPos.y, vIterPos.z);
	//			pInfoCom->m_vSize = D3DXVECTOR3(vRandScale.x, vRandScale.y, vRandScale.z);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::X] = D3DXToRadian(vRandAngle.x);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::Y] = D3DXToRadian(vRandAngle.y);
	//			pInfoCom->m_fAngle[(int)Engine::ANGLETYPE::Z] = D3DXToRadian(vRandAngle.z);
	//			pInfoCom->Update();

	//			m_pEditMgr->m_pToolView->m_pToolScene->AddGameObject(Engine::LAYERID::GameObject, L"CubeObj", pGameObject);
	//			int idx = m_CubeListBox.AddString(m_strName);
	//			m_CubeListBox.SetItemData(idx, (DWORD)pGameObject);
	//			m_CubeListBox.SetCurSel(idx);

	//			vIterPos.z += m_fScaleZ * 2;
	//		}
	//		vIterPos.x += m_fScaleX * 2;
	//	}
	//	vIterPos.y += m_fScaleY * 2;
	//}

	//UpdateData(FALSE);
	//Change_PreView();
}

void CMapTool::OnBnClickedNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Clear_Object();
	Ready_Camera();
}

void CMapTool::OnBnClickedDelName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	Remove_Object(m_strName);
}


void CMapTool::Ready_TextureCboList()
{
	m_TextureCbo.ResetContent();
	for (_int i = 0; i < RESOURCE_END; ++i)
	{
		switch (i)
		{
		case RESOURCE_STATIC:
			m_TextureCbo.AddString(L"STATIC");
			break;
		case RESOURCE_LOGO:
			m_TextureCbo.AddString(L"LOGO");
			break;
		case RESOURCE_STAGE:
			m_TextureCbo.AddString(L"STAGE");
			break;
		default:
			m_TextureCbo.AddString(L"ERROR");
			break;
		}
	}
	m_TextureCbo.SetCurSel(0);

	Change_TextureList((RESOURCEID)0);
}

void CMapTool::Change_TextureList(RESOURCEID eType)
{
	auto& lstMeshTag = Engine::Get_MeshTagList(eType);

	m_TextureListBox.ResetContent();
	for (auto& strMeshTag : lstMeshTag)
	{
		m_TextureListBox.AddString(strMeshTag.c_str());
	}
	m_TextureListBox.SetCurSel(0);
}

void CMapTool::BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (auto pGameObject : m_lstSelectedObject)
	{
		//트렌스폼 정보 저장
		Engine::CComponent* pComponent = pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		if (pComponent)
		{
			auto pTransCom = (Engine::CTransform*)pComponent;
			*((_float*)&pTransCom->m_vInfo[Engine::INFO::INFO_POS] + ePos) += fControlSpeed;
		}
	}
}

void CMapTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pEditMgr->m_eFocus = FOCUSID_MAPTOOL;
}


void CMapTool::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;
	CDialog::OnClose();
}
