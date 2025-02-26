#include "stdafx.h"
#include "ToolScene.h"

CToolScene::CToolScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
	: Engine::CScene(pGraphicDev)
{
	Add_Hwnd(hWnd);
}

CToolScene::~CToolScene(void)
{
}

HRESULT CToolScene::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

_int CToolScene::Update_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CToolScene::LateUpdate_Scene(void)
{
}

void CToolScene::Render_Scene(void)
{
}

HRESULT CToolScene::Ready_LightInfo(void)
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

void CToolScene::Add_Hwnd(HWND hWnd)
{
	m_vecHwnd.emplace_back(hWnd);
}

const std::vector<HWND> CToolScene::Get_HwndList() const
{
	return m_vecHwnd;
}

void CToolScene::Free(void)
{
	m_vecHwnd.clear();	
	m_vecHwnd.shrink_to_fit();
	Engine::CScene::Free();
}