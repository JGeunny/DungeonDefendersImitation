#include "stdafx.h"
#include "Stage.h"
#include "CameraObj.h"
#include "DirectionObj.h"
#include "Monster.h"
#include "MonsterGate.h"
#include "UI.h"
#include "Export_Function.h"
#include "Stage2.h"
#include "HpBar_Player.h"
#include "TresureChest.h"
#include "MarioHouse.h"
#include "Effect.h"

#include "MiniGame.h"

#include "WaveInfo.h"
#include "MouseUI.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pShoulderCamera(nullptr)
	, m_pCamera(nullptr)
{
}

CStage::~CStage(void)
{
}

HRESULT CStage::Ready_Scene(void)
{
	Engine::CLightMgr::GetInstance()->Clear_Light();
	//FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Layer(SRC_PATH(L"../Bin/Resource/Data/MapInfo_Stage1.txt")), E_FAIL);
	FAILED_CHECK_RETURN(Load_Camera(SRC_PATH(L"../Bin/Resource/Data/CameraInfo_Stage1.txt")), E_FAIL);
	FAILED_CHECK_RETURN(Load_Direction(SRC_PATH(L"../Bin/Resource/Data/DirectionInfo_Stage1.txt")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	Engine::Get_SoundMgr()->StopAll();
	Engine::Get_SoundMgr()->StartSound(L"BuildUp", Engine::CSoundMgr::BGM, true);
	Engine::Get_SoundMgr()->StartSound(L"Intro_Stage1", Engine::CSoundMgr::ENV, false);
	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	//if(Engine::KeyDown(KEY_UP))
	//{
	//	if (m_pCamera)
	//		m_pCamera->Play_Camera();
	//}
	//if (Engine::KeyDown(KEY_DOWN))
	//{
	//	if (m_pCamera)
	//		m_pCamera->Stop_Camera();
	//}
	_int iEenvt = Engine::CScene::Update_Scene(fTimeDelta);

	return EVENT_DEFAULT;
}

void CStage::LateUpdate_Scene(void)
{
	CScene::LateUpdate_Scene();
	Engine::LateUpdate_CollisionMgr();


}

void CStage::Render_Scene(void)
{
	if (Engine::KeyDown(KEY_F12))
	{
		Engine::CScene*		pScene = nullptr;

		pScene = CStage2::Create(m_pGraphicDev);
		Engine::SetUp_Scene(pScene);
	}

	auto pair_MarioHouse = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MarioHouse");
	if (pair_MarioHouse.first != pair_MarioHouse.second)
	{
		auto pMarioHouse = (CMarioHouse*)pair_MarioHouse.first->second;
		if (true == pMarioHouse->IsEndGame())
		{
			Engine::CScene*		pScene = nullptr;
			pScene = CStage2::Create(m_pGraphicDev);
			Engine::SetUp_Scene(pScene);
		}
	}

}

void * CStage::Get_NaviMesh()
{
	auto pNaviMesh = Engine::Clone(RESOURCE_STAGE, L"Navi_Stage1");
	if (nullptr == pNaviMesh)
		return nullptr;
	return (void*)pNaviMesh;
}

HRESULT CStage::Ready_GameLogicLayer(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.emplace(pLayerTag, pLayer);

	Engine::CGameObject*		pGameObject = nullptr;
	// ShoulderCamera
	pGameObject = m_pShoulderCamera = CShoulderCamera::Create(m_pGraphicDev,
	//pGameObject = CDynamicCamera::Create(m_pGraphicDev,
										&_vec3(0.f,10.f, -5.f), 
										&_vec3(0.f, 0.f, 0.f), 
										&_vec3(0.f, 1.f, 0.f), 
										D3DXToRadian(45.f), 
										float(WINCX) / WINCY, 
										0.2f, 
										1000.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"ShoulderCamera", pGameObject), E_FAIL);

	//pGameObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"Terrain", pGameObject), E_FAIL);

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"SkyBox", pGameObject), E_FAIL);

	pGameObject = CUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"UI", pGameObject), E_FAIL);

	pGameObject = CHpBar_Player::Create(m_pGraphicDev, nullptr);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"HpBar_Player", pGameObject), E_FAIL);

	pGameObject = CMiniGame::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"MiniGame", pGameObject), E_FAIL);

	//pGameObject = CMeshObj::Create(m_pGraphicDev, 0);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"MeshObj", pGameObject), E_FAIL);

	//pGameObject = CPlayer::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"Player", pGameObject), E_FAIL);
	//m_pShoulderCamera->Set_Target(pGameObject);

	//for (_uint i = 0; i < 2; ++i)
	//{
	//	pGameObject = CStone::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pLayer, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_Object(L"Stone", pGameObject), E_FAIL);
	//}	

	pGameObject = CSword::Create(m_pGraphicDev, 1);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"Sword", pGameObject), E_FAIL);


	CWaveInfo::WAVEINFO_DESC tWaveInfo;
	tWaveInfo.vBeginPos.x = WINCX >> 1;
	tWaveInfo.vBeginPos.y = (WINCY >> 1) - 230.f;
	pGameObject = CWaveInfo::Create(m_pGraphicDev, &tWaveInfo);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"WaveInfo", pGameObject), E_FAIL);


	CMouseUI::MOUSEUI_DESC tMouseInfo;
	tMouseInfo.eCurType = CMouseUI::MOUSE_TYPE::MOUSE_AIM;
	pGameObject = CMouseUI::Create(m_pGraphicDev, &tMouseInfo);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"Mouse", pGameObject), E_FAIL);

	return S_OK;	
	
}


void CStage::Free(void)
{
	Engine::ReleaseData_CollisionMgr();
	Engine::CScene::Free();
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

HRESULT CStage::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);
	
	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);


	//D3DLIGHT9			LightDesc;
	//ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	//LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	//LightDesc.Direction = _vec3(1.f, -1.f, 1.f);
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	//LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	//FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &LightDesc, 0), E_FAIL);

	//ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	//LightDesc.Type = D3DLIGHT_POINT;
	//LightDesc.Position = _vec3(59.f, 12.f, 70.f);
	//LightDesc.Range = 10.0f;
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 0.0f, 0.0f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	//LightDesc.Specular = LightDesc.Diffuse;

	//FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &LightDesc, 0), E_FAIL);

	//ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	//LightDesc.Type = D3DLIGHT_POINT;
	//LightDesc.Position = _vec3(40.f, 12.f, 70.f);
	//LightDesc.Range = 10.0f;
	//LightDesc.Diffuse = D3DXCOLOR(0.0f, 1.f, 0.0f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	//LightDesc.Specular = LightDesc.Diffuse;

	//FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &LightDesc, 0), E_FAIL);

	return S_OK;
}

HRESULT CStage::Load_Layer(const _tchar * pStagePath)
{
	HANDLE hFile = CreateFile(pStagePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERR_MSG(L"오브젝트 불러오기 실패");
		return S_FALSE;
	}

	DWORD dwBytes = 0;
	Engine::OBJECT_SAVEDATA tObjectData = {};
	while (true)
	{
		//데이터 가져오기
		ReadFile(hFile, &tObjectData, sizeof(Engine::OBJECT_SAVEDATA), &dwBytes, nullptr);
		if (0 == dwBytes)
			break;

		Engine::CGameObject* pGameObject = nullptr;
		if (!lstrcmp(tObjectData.szObjectKey, L"Camera"))
		{

		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"Terrain"))
		{

		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"Player"))
		{
			pGameObject = CPlayer::Create(m_pGraphicDev);
			m_pShoulderCamera->Set_Target(pGameObject);
		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"Monster"))
		{
			pGameObject = CMonster::Create(m_pGraphicDev, 0);
		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"MonsterGate"))
		{
			pGameObject = CMonsterGate::Create(m_pGraphicDev, 0);
		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"TresureChest"))
		{
			pGameObject = CTresureChest::Create(m_pGraphicDev, 0);
		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"MarioHouse"))
		{
			pGameObject = CMarioHouse::Create(m_pGraphicDev, 0);
		}
		else if (!lstrcmp(tObjectData.szObjectKey, L"Fire"))
		{
			CEffect::EFFECT_DESC tEffect_Info;
			tEffect_Info.eType = CEffect::EFFECT_REPEAT;
			tEffect_Info.strImageKey = L"Texture_Fire";
			pGameObject = CEffect::Create(m_pGraphicDev, &tEffect_Info);

			D3DLIGHT9 LightDesc;
			ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

			LightDesc.Type = D3DLIGHT_POINT;
			LightDesc.Position = tObjectData.vPos;
			//LightDesc.Range = 10.f;
			LightDesc.Range = tObjectData.vScale.x * 2;
			LightDesc.Diffuse = D3DXCOLOR(1.f, 0.0f, 0.0f, 1.f);
			LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
			LightDesc.Specular = LightDesc.Diffuse;

			Engine::Ready_Light(m_pGraphicDev, &LightDesc, 0);
		}
		else
		{
			pGameObject = CMeshObj::Create(m_pGraphicDev, 0);
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

		Add_Object(tObjectData.szLayerTag, tObjectData.szObjectKey, pGameObject);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage::Load_Camera(const _tchar * pCameraPath)
{
	HANDLE hFile = CreateFile(pCameraPath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERR_MSG(L"오브젝트 불러오기 실패");
		return S_FALSE;
	}

	DWORD dwBytes = 0;

	while (true)
	{
		Engine::CAMERA_SAVEDATA tCameraData = Engine::CAMERA_SAVEDATA();
		//데이터 가져오기
		ReadFile(hFile, &tCameraData, sizeof(Engine::CAMERA_SAVEDATA), &dwBytes, nullptr);
		if (0 == dwBytes)
			break;

		CCameraObj* pGameObject = CCameraObj::Create(m_pGraphicDev, 0);
		m_pCamera = pGameObject;
		if (!pGameObject) continue;
		Engine::CComponent* pComponent = nullptr;
		//오브젝트 정보 저장
		pComponent = pGameObject->Get_Component(L"Com_ObjectInfo", Engine::COMPONENTID::ID_STATIC);
		if (pComponent)
		{
			auto pObjectInfoCom = (Engine::CObjectInfo*)pComponent;
			pObjectInfoCom->Set_ObjectKey(tCameraData.szObjectKey);
			pObjectInfoCom->Set_Option(tCameraData.iOption);
			if (tCameraData.iOption == 1)
				pGameObject->Play_Camera();
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
		Add_Object(L"GameLogic_Camera", tCameraData.szObjectKey, pGameObject);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CStage::Load_Direction(const _tchar * pDirectionPath)
{
	HANDLE hFile = CreateFile(pDirectionPath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERR_MSG(L"오브젝트 불러오기 실패");
		return S_FALSE;
	}

	DWORD dwBytes = 0;

	while (true)
	{
		Engine::CAMERA_SAVEDATA tCameraData = Engine::CAMERA_SAVEDATA();
		//데이터 가져오기
		ReadFile(hFile, &tCameraData, sizeof(Engine::CAMERA_SAVEDATA), &dwBytes, nullptr);
		if (0 == dwBytes)
			break;

		CDirectionObj* pGameObject = CDirectionObj::Create(m_pGraphicDev, 0);
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
			pGameObject->Add_Node( &pNode, tNodeData.vPoint);
		}
		Add_Object(L"GameLogic_Direction", tCameraData.szObjectKey, pGameObject);
	}
	CloseHandle(hFile);

	return S_OK;
}

