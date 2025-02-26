#include "stdafx.h"
#include "HpBar_B.h"

#include "Export_Function.h"

CHpBar_B::CHpBar_B(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurrentImageCount(0)
	, m_fTimeDelta(0)
	, m_fCurHp(0.f)
	, m_vColorHp(1.f, 1.f, 1.f, 1.f)
{
}

CHpBar_B::~CHpBar_B(void)
{
}

HRESULT CHpBar_B::Ready_Object(Engine::CGameObject* pTarget)
{
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	Set_Target(pTarget);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

_int CHpBar_B::Update_Object(const _float& fTimeDelta)
{
	if (nullptr == Get_Target())
	{
		m_iEvent = EVENT_DEAD;
		return m_iEvent;
	}
	FollowTarget();

	Engine::CGameObject::Update_Object(fTimeDelta);
	//m_fTimeDelta += fTimeDelta * 0.75f;
	//m_iCurrentImageCount = (int)m_fTimeDelta % m_pTextureCom->Get_MaxCount() - 1;
	//++m_iCurrentImageCount;
	//if (m_pTextureCom->Get_MaxCount() == m_iCurrentImageCount + 1)
	//	m_fTimeDelta = 0.f;
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);
	return m_iEvent;
}

void CHpBar_B::LateUpdate_Object(void)
{
}

void CHpBar_B::Render_Object(void)
{
	BillBoard();
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	SetUp_ConstantTable(pEffect);
	pEffect->AddRef();

	_uint iPassCnt = 0;
	pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
	pEffect->BeginPass(0);	// 0패스의 쉐이더를 적용

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();
	Safe_Release(pEffect);

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();
}

HRESULT CHpBar_B::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_HpBar"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	m_pTransCom->Set_Scale(_vec3(2.f, 0.3f, 2.f));

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CHpBar_B::FollowTarget()
{
	auto pTarget = Get_Target();
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	auto vPos = pTrans_Target->Get_Pos();
	auto vVertical = pCollider_Target->Get_Range()->Get_Vertical() * 2.f + pTrans_Target->m_vInterval;
	vPos.y += vVertical.y * pTrans_Target->Get_Scale().y;
	m_pTransCom->Set_Pos(vPos);

	m_fCurHp = pStatus->m_fHp / pStatus->m_fMaxHp;
	Engine::TEAMTYPE eTeam = pTarget->Get_Team();
	m_vColorHp = _vec4(1.f, 1.f, 1.f, 1.f);
	switch (eTeam)
	{
	case Engine::TEAMTYPE_NONE:
		break;
	case Engine::TEAMTYPE_TEAM1:
		m_vColorHp = _vec4(0.f, 1.f, 0.f, 1.f);
		break;
	case Engine::TEAMTYPE_TEAM2:
		m_vColorHp = _vec4(1.f, 0.f, 0.f, 1.f);
		break;
	case Engine::TEAMTYPE_END:
		break;
	default:
		break;
	}
	
	return S_OK;
}

HRESULT CHpBar_B::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;	
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fCurHp", m_fCurHp);
	pEffect->SetInt("g_iType", 0);
	pEffect->SetFloatArray("g_vColorHp", (float*)&m_vColorHp, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);
	m_pTextureCom->Render_Texture(pEffect, "g_BaseTexture", 1);

	Safe_Release(pEffect);
	return S_OK;
}

CHpBar_B * CHpBar_B::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget)
{
	CHpBar_B *	pInstance = new CHpBar_B(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTarget)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CHpBar_B::Free(void)
{
	Engine::CGameObject::Free();
}

void CHpBar_B::BillBoard(void)
{
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}
