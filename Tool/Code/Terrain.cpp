#include "stdafx.h"
#include "Terrain.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CTerrain::~CTerrain(void)
{
}

HRESULT CTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTerrain::Update_Object(const _float& _fTimeDelta)
{
	Engine::CGameObject::Update_Object(_fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);
	return 0;
}

void CTerrain::LateUpdate_Object(void)
{

}

void CTerrain::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);

	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

HRESULT CTerrain::Add_Component()
{
	Engine::CComponent*		pComponent = nullptr;
	
	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_TerrainTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STATIC, L"Texture_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain *	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CTerrain::Free()
{
	Engine::CGameObject::Free();
}

