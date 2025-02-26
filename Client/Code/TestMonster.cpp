#include "stdafx.h"
#include "TestMonster.h"

#include "Export_Function.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CTestMonster::~CTestMonster(void)
{
}

HRESULT CTestMonster::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	
	return S_OK;
}

_int CTestMonster::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Engine::CTransform*		pPlayerTransform = dynamic_cast<Engine::CTransform*>
												(Engine::Get_Component(L"GameLogic", 
																		L"TestPlayer", 
																		L"Com_Transform", 
																		Engine::ID_DYNAMIC));

	NULL_CHECK_RETURN(pPlayerTransform, -1);

	_vec3	vPlayerPos;
	pPlayerTransform->Get_Info(Engine::INFO_POS, &vPlayerPos);

	m_pTransCom->Chase_Target(&vPlayerPos, m_fSpeed * fTimeDelta);

	return 0;
}

void CTestMonster::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);

	m_pBufferCom->Render_Buffer();
}

HRESULT CTestMonster::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTriCol*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_TriCol"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Transform", pComponent);


	return S_OK;
}

CTestMonster * CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestMonster *	pInstance = new CTestMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CTestMonster::Free(void)
{
	Engine::CGameObject::Free();
}



