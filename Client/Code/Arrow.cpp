#include "stdafx.h"
#include "Arrow.h"

#include "Export_Function.h"

CArrow::CArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
{
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CArrow::~CArrow(void)
{
}

HRESULT CArrow::Ready_Object(BULLET_DESC* pArg)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(0), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_iFlag = iFlag;
	m_fShaderType = 3.f;
	m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	
	if (nullptr != pArg)
		CBullet::Synchronization_BulletData(*pArg);
	return S_OK;
}

void CArrow::LateReady_Object(void)
{
	CBullet::LateReady_Object();
}

_int CArrow::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();

	//Movement(fTimeDelta);
	CBullet::Movement(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);

	auto fDist = D3DXVec3Length(&(m_vBeginPos - m_pTransCom->Get_Pos()));
	if (fDist > 100.f)
		m_iEvent = EVENT_DEAD;
	return m_iEvent;
}

void CArrow::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CArrow::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CArrow::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	if (m_iEvent == EVENT_DEAD) return;
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_DST)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			m_iEvent = EVENT_DEAD;
		}
	}
}

void CArrow::Movement(const _float& fTimeDelta)
{
	_vec3 vLook;
	m_pTransCom->Get_Info(Engine::INFO::INFO_LOOK, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);
	m_pTransCom->Move_Pos(&(-vLook * m_fSpeed * fTimeDelta));
}

HRESULT CArrow::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Arrow");
	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	pComponent = m_pStatusCom = Engine::CStatus::Create(10.f, 1.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	m_pTransCom->Set_Scale(_vec3(0.03f, 0.03f, 0.03f));

	return S_OK;
}

CArrow * CArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg)
{
	CArrow *	pInstance = new CArrow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CArrow::Free(void)
{
	CMeshObj::Free();
}
