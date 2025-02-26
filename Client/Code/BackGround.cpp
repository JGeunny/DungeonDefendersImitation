#include "stdafx.h"
#include "BackGround.h"

#include "Export_Function.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurrentImageCount(0)
	, m_fTimeDelta(0)
{
}

CBackGround::~CBackGround(void)
{
}

HRESULT CBackGround::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vScale.x = 2.f;
	m_pTransCom->m_vScale.y = 2.f;


	return S_OK;
}

_int CBackGround::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_fTimeDelta += fTimeDelta * 0.75f;
	m_iCurrentImageCount = (int)m_fTimeDelta % m_pTextureCom->Get_MaxCount() - 1;
	++m_iCurrentImageCount;
	if (m_pTextureCom->Get_MaxCount() == m_iCurrentImageCount + 1)
		m_fTimeDelta = 0.f;
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);
	return 0;
}

void CBackGround::LateUpdate_Object(void)
{
}

void CBackGround::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	m_pTextureCom->Render_Texture(m_iCurrentImageCount);
	m_pBufferCom->Render_Buffer();
}

HRESULT CBackGround::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Logo"));
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

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround *	pInstance = new CBackGround(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CBackGround::Free(void)
{
	Engine::CGameObject::Free();
}

