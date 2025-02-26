#include "stdafx.h"
#include "Bullet.h"

#include "Export_Function.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_vBeginPos(0.f, 0.f, 0.f)
{
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CBullet::~CBullet(void)
{
}

HRESULT CBullet::Ready_Object(BULLET_DESC* pArg)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(0), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fShaderType = 3.f;
	m_eRenderType = Engine::RENDERID::RENDER_ALPHA;

	if(nullptr != pArg)
		Synchronization_BulletData(*pArg);
	return S_OK;
}

void CBullet::LateReady_Object(void)
{
	CMeshObj::LateReady_Object();
}

_int CBullet::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	Update_TimeDelta_Target(fTimeDelta);
	Movement(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);

	auto fDist = D3DXVec3Length(&(m_vBeginPos - m_pTransCom->Get_Pos()));
	if (fDist > 100.f)
		m_iEvent = EVENT_DEAD;
	return m_iEvent;
}

void CBullet::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CBullet::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CBullet::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
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

void CBullet::Movement(const _float& fTimeDelta)
{
	if (false == m_tBullet.bRotateBullet)
	{
		m_tBullet.vPos += m_tBullet.vDir *  m_tBullet.fSpeed * fTimeDelta;
		m_pTransCom->Set_Pos(m_tBullet.vPos);
	}
	else
	{
		_matrix matRotate;
		D3DXMatrixRotationAxis(&matRotate, &m_tBullet.vRotateDir, m_tBullet.fSpeed);
		D3DXVec3TransformCoord(&m_tBullet.vRotateDir, &m_tBullet.vRotateDir, &matRotate);
		D3DXVec3Normalize(&m_tBullet.vRotateDir, &m_tBullet.vRotateDir);

		m_tBullet.vPos += m_tBullet.vDir *  m_tBullet.fSpeed * fTimeDelta;
		m_pTransCom->Set_Pos(m_tBullet.vPos + m_tBullet.vRotateDir * m_tBullet.vRotateDist);
	}
}

void CBullet::InitSetting(Engine::TEAMTYPE * peTeam, _vec3 * pvPos, _vec3 * pvScale, _vec3 * pvRadian, float * fAtt)
{
	if (nullptr != peTeam)
	{
		m_tBullet.eTeam = *peTeam;
		m_eTeam = *peTeam;
	}
	if (nullptr != pvPos)
	{
		m_tBullet.vPos = *pvPos;
		m_pTransCom->Set_Pos(*pvPos);
	}
	if (nullptr != pvScale)
	{
		m_tBullet.vScale = *pvScale;
		m_pTransCom->Set_Scale(*pvScale);
	}
	if (nullptr != pvRadian)
	{
		m_tBullet.vRadian = *pvRadian;
		m_pTransCom->Set_Radian(*pvRadian);
	}
	if (nullptr != fAtt)
		m_pStatusCom->m_fAtt = *fAtt;
}

void CBullet::Synchronization_BulletData(BULLET_DESC & tBullet)
{
	m_tBullet = tBullet;
	m_pStatusCom->m_fAtt = tBullet.fAtt;
	m_eTeam = tBullet.eTeam;
	m_vBeginPos = tBullet.vPos;
	m_pTransCom->Set_Pos(tBullet.vPos);
	m_pTransCom->Set_Scale(tBullet.vScale);
	m_pTransCom->Set_Radian(tBullet.vRadian);
}

void CBullet::Update_TimeDelta_Target(float fTimeDelta)
{
	if (true == m_tBullet.bTimeDelta_Target)
	{
		m_tBullet.fTimeDelta_Target -= fTimeDelta;
		if (m_tBullet.fTimeDelta_Target <= 0.f)
		{
			m_tBullet.fTimeDelta_Target = 0.f;
			m_tBullet.bTimeDelta_Target = false;
			auto pMeshObj = (CMeshObj*)Get_Target();
			if(nullptr != pMeshObj)
				D3DXVec3Normalize(&m_tBullet.vDir, &(pMeshObj->Get_StartBulletPos() - m_pTransCom->Get_Pos()));
		}
	}
}

HRESULT CBullet::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"EnergyBall");
	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	pComponent = m_pStatusCom = Engine::CStatus::Create(10.f, 1.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	m_pTransCom->Set_Scale(_vec3(0.001f, 0.001f, 0.001f));
	return S_OK;
}

CBullet * CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg)
{
	CBullet *	pInstance = new CBullet(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CBullet::Free(void)
{
	CMeshObj::Free();
}
