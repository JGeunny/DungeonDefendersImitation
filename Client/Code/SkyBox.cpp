#include "stdafx.h"
#include "SkyBox.h"

#include "Export_Function.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CSkyBox::~CSkyBox(void)
{
}

HRESULT CSkyBox::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	
	m_pTransCom->Set_Scale(40.f, 40.f, 40.f);
	

	return S_OK;
}

_int CSkyBox::Update_Object(const _float& fTimeDelta)
{
	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	m_pTransCom->Set_Pos(matView._41, matView._42 + 3.f, matView._43);


	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);
	return 0;
}

void CSkyBox::LateUpdate_Object(void)
{
}

void CSkyBox::Render_Object(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CSkyBox::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_SkyBox"));
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

CSkyBox * CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox *	pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CSkyBox::Free(void)
{
	Engine::CGameObject::Free();
}

