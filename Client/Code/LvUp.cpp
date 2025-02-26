#include "stdafx.h"
#include "LvUp.h"

#include "Export_Function.h"

CLvUp::CLvUp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurrentImageCount(0)
	, m_fTimeDelta(0)
	, m_fCurHp(0.f)
	, m_vColorHp(1.f, 1.f, 1.f, 1.f)
	, m_fCurImage(0.f)
	, m_fMoveY(0.f)
{
}

CLvUp::~CLvUp(void)
{
}

HRESULT CLvUp::Ready_Object(Engine::CGameObject* pTarget)
{
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	Set_Target(pTarget);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

void CLvUp::LateReady_Object(void)
{
}

_int CLvUp::Update_Object(const _float& fTimeDelta)
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
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);

	if (m_fMoveY < 13.f)
		m_fMoveY += fTimeDelta * 15.f;
	else
		m_fMoveY = 13.f;
	m_fCurImage += fTimeDelta * 15.f;
	if(m_fCurImage >= 30.f)
		m_iEvent = EVENT_DEAD;
	return m_iEvent;
}

void CLvUp::LateUpdate_Object(void)
{
}

void CLvUp::Render_Object(void)
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
}

HRESULT CLvUp::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_LvUp"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	m_pTransCom->Set_Scale(_vec3(1.f, 1.2f, 1.f));

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

HRESULT CLvUp::FollowTarget()
{
	auto pTarget = Get_Target();
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	auto vPos = pTrans_Target->Get_Pos();
	auto vVertical = pCollider_Target->Get_Range()->Get_Vertical() * 2.f + pTrans_Target->m_vInterval;
	vPos.y += (vVertical.y + m_fMoveY) * pTrans_Target->Get_Scale().y;
	m_pTransCom->Set_Pos(vPos);

	m_vColorHp = _vec4(1.f, 1.f, 1.f, 1.f);
	
	return S_OK;
}

HRESULT CLvUp::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;	
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fCurHp", 1);
	pEffect->SetFloatArray("g_vColorHp", (float*)&m_vColorHp, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", true);
	pEffect->SetInt("g_iType", 0);
	m_pTextureCom->Render_Texture(pEffect, "g_BaseTexture", (int(m_fCurImage) >= 30 ? 29 : int(m_fCurImage)));

	Safe_Release(pEffect);
	return S_OK;
}

CLvUp * CLvUp::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget)
{
	CLvUp *	pInstance = new CLvUp(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTarget)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CLvUp::Free(void)
{
	Engine::CGameObject::Free();
}

void CLvUp::BillBoard(void)
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
