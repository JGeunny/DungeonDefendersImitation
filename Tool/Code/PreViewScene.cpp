#include "stdafx.h"
#include "PreViewScene.h"
#include "MeshObj.h"
#include "ToolCamera.h"

CPreViewScene::CPreViewScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
	: CToolScene(pGraphicDev, hWnd)
{

}

CPreViewScene::~CPreViewScene(void)
{

}

HRESULT CPreViewScene::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);
	FAILED_CHECK_RETURN(CToolScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);

	return S_OK;
}

_int CPreViewScene::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent = 0;
	iEvent = CToolScene::Update_Scene(fTimeDelta);

	return EVENT_DEFAULT;
}

void CPreViewScene::LateUpdate_Scene(void)
{
	CScene::LateUpdate_Scene();
	Engine::LateUpdate_CollisionMgr();
}

void CPreViewScene::Render_Scene(void)
{
}

HRESULT CPreViewScene::Ready_GameLogicLayer(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	// DynamicCamera
	pGameObject = CToolCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 10.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(45.f),
		float(WINCX) / WINCY,
		1.f,
		1000.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"DynamicCamera", pGameObject), E_FAIL);
	((CToolCamera*)pGameObject)->Set_EnableControl(false);
	//int i = 0;
	//pGameObject = CMeshObj::Create(m_pGraphicDev, i);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"MeshObj", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;	
}


void CPreViewScene::Free(void)
{
	CToolScene::Free();
}

CPreViewScene* CPreViewScene::Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
{
	CPreViewScene*	pInstance = new CPreViewScene(pGraphicDev, hWnd);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

HRESULT CPreViewScene::Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eMax)
{
	//pGraphicDev->AddRef();

	//Engine::Reserve_ContainerSize(eMax);

	//FAILED_CHECK_RETURN(Engine::Ready_Buffers(pGraphicDev, RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFER_RCTEX), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Logo/Logo.jpg"), E_FAIL);
	//
	//Engine::Safe_Release(pGraphicDev);

	return S_OK;
}

