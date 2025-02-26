#include "stdafx.h"
#include "MainScene.h"

#include "Terrain.h"
#include "ToolCamera.h"

#include "MeshObj.h"

CMainScene::CMainScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
	: CToolScene(pGraphicDev, hWnd)
	, m_pLoading(nullptr)
{

}

CMainScene::~CMainScene(void)
{

}

HRESULT CMainScene::Ready_Scene(void)
{
	static _bool bReady_Loading = false;
	FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);
	FAILED_CHECK_RETURN(CToolScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);

	if (!bReady_Loading)
	{
		bReady_Loading = true;
		m_pLoading = CLoading::Create(m_pGraphicDev, LOADING_STAGE);
		NULL_CHECK_RETURN(m_pLoading, E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);
	}

	return S_OK;
}

_int CMainScene::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent = 0;
	if (m_pLoading)
	{
		if (true == m_pLoading->Get_Finish())
		{
			if (Engine::Get_DIKeyState(DIK_RETURN) & 0x80)
			{
				FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);
				Engine::Safe_Release(m_pLoading);
			}
		}
	}
	else
		iEvent = CToolScene::Update_Scene(fTimeDelta);

	return EVENT_DEFAULT;
}

void CMainScene::LateUpdate_Scene(void)
{
	CScene::LateUpdate_Scene();
	Engine::LateUpdate_CollisionMgr();
}

void CMainScene::Render_Scene(void)
{
	if (m_pLoading)
	{
		Engine::Render_Font(L"Font_Default", m_pLoading->Get_String(), &_vec2(50.f, 50.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CMainScene::Ready_GameLogicLayer(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	// DynamicCamera
	pGameObject = m_pToolCamera = CToolCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 10.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(45.f),
		float(WINCX) / WINCY,
		1.f,
		1000.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"DynamicCamera", pGameObject), E_FAIL);

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"Terrain", pGameObject), E_FAIL);

	//int i = 0;
	//pGameObject = CMeshObj::Create(m_pGraphicDev, i);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"MeshObj", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;	
}


void CMainScene::Free(void)
{
	Engine::Safe_Release(m_pLoading);
	CToolScene::Free();
}

CMainScene* CMainScene::Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
{
	CMainScene*	pInstance = new CMainScene(pGraphicDev, hWnd);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

HRESULT CMainScene::Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eMax)
{
	//pGraphicDev->AddRef();

	//Engine::Reserve_ContainerSize(eMax);

	//FAILED_CHECK_RETURN(Engine::Ready_Buffers(pGraphicDev, RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFER_RCTEX), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Logo/Logo.jpg"), E_FAIL);
	//
	//Engine::Safe_Release(pGraphicDev);

	return S_OK;
}

HRESULT CMainScene::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CMainScene::Ready_Camera(void)
{
	return m_pToolCamera->Ready_Object(&_vec3(0.f, 10.f, -5.f),
									&_vec3(0.f, 0.f, 0.f),
									&_vec3(0.f, 1.f, 0.f),
									D3DXToRadian(45.f),
									float(WINCX) / WINCY,
									1.f,
									1000.f);
}

