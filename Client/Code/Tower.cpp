#include "stdafx.h"
#include "Tower.h"

#include "Export_Function.h"

#include "Bullet.h"
#include "HpBar_B.h"
#include "Damage.h"
#include "FireBall.h"
#include "ElecBall.h"

CTower::CTower(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_fSpeed(3.f)
{
	m_bActivate = false;
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CTower::~CTower(void)
{
}

HRESULT CTower::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;
	
	return S_OK;
}

void CTower::LateReady_Object(void)
{
	if (m_fShaderType != 1.f && m_fShaderType != 2.f)
	{
		auto pHpBar = CHpBar_B::Create(m_pGraphicDev, this);
		Engine::Add_Object(L"GameLogic", L"HpBar_B", pHpBar);
	}
	else
		m_eCollType = Engine::COLLTYPE::COL_SELECT;
	CMeshObj::LateReady_Object();

	if (m_pMeshAdminCom->Get_MeshKey() == L"MageBlockade")
		m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 2400.f;
	else if (m_pMeshAdminCom->Get_MeshKey() == L"MissleTower")
	{
		m_pStatusCom->m_fAtt = 360.f;
		m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 1000.f;
	}
	else
	{
		m_pStatusCom->m_fAtt = 820.f;
		m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 1400.f;
	}

}

_int CTower::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	HRESULT hr = E_FAIL;
	if(m_bActivate)
		hr = Behavior(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);

	if (EVENT_DEAD == m_iEvent)
	{
		if (SUCCEEDED(hr))
			return EVENT_DEFAULT;
		else
			return m_iEvent;
	}
	return m_iEvent;
}

void CTower::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CTower::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CTower::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	if (m_iEvent == EVENT_DEAD)
		return;
	bool bDead = false;
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_SRC)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			bDead = true;
			Engine::Get_SoundMgr()->StartSound(L"DeadBuild", Engine::CSoundMgr::SE, false);
			
		}
		else
		{
			Engine::Get_SoundMgr()->StartSound(L"HitBuild", Engine::CSoundMgr::SE, false);
		}
		CMeshObj::Effect_BeAttacked();
		auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
		Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
	}

	if(true == bDead)
		m_iEvent = EVENT_DEAD;
}

HRESULT CTower::Behavior(const _float & fTimeDelta)
{
	if (m_iEvent == EVENT_DEAD)
	{
		m_pMeshAdminCom->Set_AnimationSet("Idle");
		if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
			return E_FAIL;
		else
			return S_OK;
	}
	if (m_pMeshAdminCom->Get_MeshKey() == L"MageBlockade")
	{
		return S_OK;
	}
	Set_Target(Engine::Find_NearTarget(L"GameLogic", Engine::TEAMTYPE_TEAM2, this, 15));
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
	{
		m_pMeshAdminCom->Set_AnimationSet("Idle");
		return E_FAIL;
	}

	Engine::RANGEFLOAT range;
	if (m_pMeshAdminCom->Get_MeshKey() == L"MissleTower")
	{
		range.fMin = 0.4f;
		range.fMax = 0.45f;
	}
	else
	{
		range.fMin = 0.7f;
		range.fMax = 0.75f;
	}
	
	if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&range))
	{
		if (false == m_abAniCount[ANICOUNT_ATTACK])
		{
			//if (m_pMeshAdminCom->Get_MeshKey() == L"MissleTower")
			//	Engine::Get_SoundMgr()->StartSound(L"Fire_ElectBall", Engine::CSoundMgr::SE, false);
			//else
			//	Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
			m_abAniCount[ANICOUNT_ATTACK] = true;
			Attack(fTimeDelta, pTarget);
		}
	}
	m_pMeshAdminCom->Set_AnimationSet("Fire");
	Set_Target(nullptr);
	return S_OK;
}

HRESULT CTower::Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget)
{
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	
	////auto pBullet = CBullet::Create(m_pGraphicDev, 0);
	//CBullet* pBullet = nullptr;
	//if (m_pMeshAdminCom->Get_MeshKey() == L"MissleTower")
	//	pBullet = CFireBall::Create(m_pGraphicDev, 0);
	//else
	//	pBullet = CElecBall::Create(m_pGraphicDev, 0);
	//auto pTrans = (Engine::CTransform*)pBullet->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);

	_vec3 vBulletPos = m_pTransCom->Get_Pos();
	vBulletPos.y += 2.f;
	_vec3 vRadian;
	_vec3 vTarget_Pos = pTrans_Target->Get_Pos();
	vTarget_Pos.y += pCollider_Target->Get_Range()->Get_Vertical().y*  pTrans_Target->Get_Scale().y;
	Engine::DirToRadian(&vRadian, &(vTarget_Pos - vBulletPos));
	////pTrans->Set_Radian(vRadian);
	//pBullet->InitSetting(&m_eTeam, &vBulletPos,
	//	nullptr, &vRadian,
	//	&m_pStatusCom->m_fAtt);

	Engine::DirToRadian(&vRadian, &(vTarget_Pos - vBulletPos));
	m_pTransCom->Set_Radian(_vec3(0.f, vRadian.y, 0.f));

	CBullet::BULLET_DESC tBullet;
	D3DXVec3Normalize(&tBullet.vDir, &(vTarget_Pos - vBulletPos));
	tBullet.eTeam = m_eTeam;
	tBullet.fAtt = m_pStatusCom->m_fAtt;
	tBullet.vPos = vBulletPos;
	tBullet.vRadian = vRadian;

	CBullet* pBullet = nullptr;
	if (m_pMeshAdminCom->Get_MeshKey() == L"MissleTower")
	{
		tBullet.iBulletType = 1;
		tBullet.vScale = _vec3(0.004f, 0.004f, 0.004f);
		pBullet = CFireBall::Create(m_pGraphicDev, &tBullet);
		Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
	}
	else
	{
		tBullet.iBulletType = 2;
		tBullet.vScale = _vec3(0.002f, 0.002f, 0.002f);
		pBullet = CElecBall::Create(m_pGraphicDev, &tBullet);
		Engine::Get_SoundMgr()->StartSound(L"Fire_ElectBall", Engine::CSoundMgr::SE, false);
	}

	Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
	
	return S_OK;
}

HRESULT CTower::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"OrcBruiser");
	//m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_DST);
	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	m_pTransCom->m_vInterval = _vec3(0.f, 100.f, 0.f);

	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_SRC);
	return S_OK;
}

CTower * CTower::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CTower *	pInstance = new CTower(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CTower::Free(void)
{
	CMeshObj::Free();
}
