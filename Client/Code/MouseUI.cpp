#include "stdafx.h"
#include "MouseUI.h"

#include "Export_Function.h"

CMouseUI::CMouseUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CMouseUI::~CMouseUI(void)
{
}

HRESULT CMouseUI::Ready_Object(MOUSEUI_DESC* pArg)
{
	if (nullptr != pArg)
	{
		m_tMouseUI = *pArg;
	}
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eRenderType = Engine::RENDERID::RENDER_UI;
	
	//Ready_UI(1, true);
	m_fViewZ = 9999.f;

	ShowCursor(false);
	return S_OK;
}

void CMouseUI::LateReady_Object(void)
{
}

_int CMouseUI::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::LateReady_Object();	
	Engine::CGameObject::Update_Object(fTimeDelta);

	Update_MouseUI(fTimeDelta, m_tMouseUI.eCurType);
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CMouseUI::LateUpdate_Object(void)
{
}

void CMouseUI::Render_Object(void)
{
	Ortho();
	m_pShaderCom->Begin_Shader(nullptr);
	if (SUCCEEDED(Ready_Shader(m_pShaderCom)))
	{
		m_pShaderCom->Begin_Pass(0);
		m_pBufferCom->Render_Buffer();
		m_pShaderCom->End_Pass();
	}
	m_pShaderCom->End_Shader();
}

void CMouseUI::Update_MouseUI(const _float& fTimeDelta, MOUSE_TYPE eType)
{
	switch (eType)
	{
	case CMouseUI::MOUSE_BASE:
	{
		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);
		m_vSize.x = 40.f;
		m_vSize.y = 40.f;
		m_vPos.x = (float)ptMouse.x + m_vSize.x * 0.5f;
		m_vPos.y = (float)ptMouse.y + m_vSize.y * 0.5f;
	}
		break;
	case CMouseUI::MOUSE_AIM:
	{
		m_vPos.x = WINCX >> 1;
		m_vPos.y = WINCY >> 1;
		m_vSize.x = 80.f;
		m_vSize.y = 80.f;
		m_fAimAngle += D3DXToRadian(720) * fTimeDelta;
		if (D3DXToRadian(m_fAimAngle) >= D3DX_PI * 2)
			m_fAimAngle = 0.f;
	}
		break;
	default:
		break;
	}
}

HRESULT CMouseUI::Ready_Shader(Engine::CShader* pShader)
{
	if(nullptr == pShader)
		return E_FAIL;
	_matrix matView;
	D3DXMatrixIdentity(&matView);
	pShader->Set_Value("g_matWorld", &m_pTransCom->m_matWorld, sizeof(_matrix));
	pShader->Set_Value("g_matView", &matView, sizeof(_matrix));
	pShader->Set_Value("g_matProj", &m_matProj, sizeof(_matrix));
	pShader->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture((int)m_tMouseUI.eCurType));
	pShader->Commit_Changes();

	return S_OK;
}

HRESULT CMouseUI::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureAdminCom = Engine::CTextureAdmin::Create(&m_pTextureCom, RESOURCE_STAGE, m_tMouseUI.strImageKey.c_str());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_TextureAdmin", pComponent);
	
	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Texture_UI"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

void CMouseUI::Ortho(void)
{
	_matrix		matWorld;
	D3DXMatrixIdentity(&matWorld);
	matWorld._11 = m_vSize.x;
	matWorld._22 = m_vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = m_vPos.x - (WINCX >> 1);
	matWorld._42 = -m_vPos.y + (WINCY >> 1);
	//matWorld._43 = 999;

	if (m_tMouseUI.eCurType == CMouseUI::MOUSE_AIM)
	{
		_matrix matRotZ;
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAimAngle));
		matWorld *= matRotZ;
	}

	m_pTransCom->m_matWorld = matWorld;
}

CMouseUI * CMouseUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, MOUSEUI_DESC* pArg)
{
	CMouseUI *	pInstance = new CMouseUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CMouseUI::Free(void)
{
	Engine::CGameObject::Free();
}
