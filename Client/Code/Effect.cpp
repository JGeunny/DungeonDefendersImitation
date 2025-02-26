#include "stdafx.h"
#include "Effect.h"

#include "Export_Function.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CEffect::~CEffect(void)
{
}

HRESULT CEffect::Ready_Object(EFFECT_DESC* pArg)
{
	if (nullptr != pArg)
	{
		m_eType = pArg->eType;
		m_fAnimationSpeed = pArg->fAnimationSpeed;
		m_strImageKey = pArg->strImageKey;
		m_bNoTargetDead = pArg->bNoTargetDead;
		m_vIntervalPos = pArg->vIntervalPos;
		m_dwColor = pArg->dwColor;

		m_bParabora = pArg->bParabora;
		m_vDir = pArg->vDir;
		m_fJumpPower = pArg->fJumpPower;
	}
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	
	return S_OK;
}

void CEffect::LateReady_Object(void)
{
	m_vBeginPos = m_pTransCom->Get_Pos();
	//m_pTransCom->Set_Scale(_vec3(10.f, 10.f, 10.f));

	//auto pair = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
	//if (pair.first != pair.second)
	//{
	//	Set_Target(pair.first->second);
	//}
}

_int CEffect::Update_Object(const _float& fTimeDelta)
{
	auto pTarget = Get_Target();
	if (nullptr != pTarget)
	{
		auto pTrans = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		m_pTransCom->Set_Pos(pTrans->Get_Pos() + m_vIntervalPos);
	}
	else
	{
		if (true == m_bNoTargetDead)
			return EVENT_DEAD;
	}
	Engine::CGameObject::LateReady_Object();
	
	Update_Parabola(fTimeDelta);
	Update_Effect(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);

	Compare_ViewZ(&m_pTransCom->Get_Pos());
 	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CEffect::LateUpdate_Object(void)
{
}

void CEffect::Render_Object(void)
{
	BillBoard();
	m_pShaderCom->Begin_Shader(nullptr);
	if (SUCCEEDED(Ready_Shader(m_pShaderCom)))
	{
		m_pShaderCom->Begin_Pass(0);
		m_pBufferCom->Render_Buffer();
		m_pShaderCom->End_Pass();
	}
	m_pShaderCom->End_Shader();
}

bool CEffect::Update_AnimationFrame(float fTimeDelta)
{
	auto iMaxImageIndex = m_pTextureCom->Get_MaxCount();
	m_fCurrentImageIndex += fTimeDelta * m_fAnimationSpeed;
	if (iMaxImageIndex <= (int)m_fCurrentImageIndex)
	{
		m_fCurrentImageIndex = 0;
		return true;
	}
	return false;
}

bool CEffect::Update_Effect(float fTimeDelta)
{
	switch (m_eType)
	{
	case CEffect::EFFECT_NONE:
		break;
	case CEffect::EFFECT_ONCE:
		if (true == Update_AnimationFrame(fTimeDelta))
			m_iEvent = EVENT_DEAD;
		break;
	case CEffect::EFFECT_REPEAT:
		Update_AnimationFrame(fTimeDelta);
		break;
	default:
		break;
	}
	return false;
}

HRESULT CEffect::Ready_Shader(Engine::CShader* pShader)
{
	if(nullptr == pShader)
		return E_FAIL;
	_matrix matView, matProj;
	
	_vec4 vColor = D3DXCOLOR(m_dwColor);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	pShader->Set_Value("g_matWorld", &m_pTransCom->m_matWorld, sizeof(_matrix));
	pShader->Set_Value("g_matView", &matView, sizeof(_matrix));
	pShader->Set_Value("g_matProj", &matProj, sizeof(_matrix));
	pShader->Set_Value("g_vColor", &vColor, sizeof(_vec4));
	pShader->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture((int)m_fCurrentImageIndex));
	pShader->Commit_Changes();

	return S_OK;
}

HRESULT CEffect::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureAdminCom = Engine::CTextureAdmin::Create(&m_pTextureCom, RESOURCE_STAGE, m_strImageKey.c_str());
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
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Texture_Effect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

void CEffect::BillBoard(void)
{
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);
	matBill = matView;
	*((_vec4*)&matBill.m[3]) = _vec4(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}

HRESULT CEffect::Update_Parabola(float fTimeDelta)
{
	if (false == m_bParabora) return E_FAIL;
	if (m_vMovePos.y < -3.f)
	{
		m_iEvent = EVENT_DEAD;
		return m_iEvent;
	}
	
	_vec3 vDir = m_vDir;
	vDir *= 4.f * fTimeDelta;
	m_fMoveY = (m_fJumpPower - 4.5f * m_fJumpAccel) * fTimeDelta*0.8f;
	vDir.y += m_fMoveY;
	m_vMovePos += vDir;
	m_pTransCom->Set_Pos(m_vBeginPos + m_vMovePos);
	m_fJumpAccel += 2.f*fTimeDelta;

	return S_OK;
}

CEffect * CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_DESC* pArg)
{
	CEffect *	pInstance = new CEffect(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CEffect::Free(void)
{
	Engine::CGameObject::Free();
}
