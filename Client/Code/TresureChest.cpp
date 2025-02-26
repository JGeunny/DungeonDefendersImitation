#include "stdafx.h"
#include "TresureChest.h"
#include "Arrow.h"
#include "HpBar_B.h"
#include "DirectionObj.h"
#include "Bullet.h"
#include "Export_Function.h"
#include "Gold.h"
#include "Damage.h"

CTresureChest::CTresureChest(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_fDeathTime(0.f)
{
	m_eTeam = Engine::TEAMTYPE_TEAM2;
}

CTresureChest::~CTresureChest(void)
{
}

HRESULT CTresureChest::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	//m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	m_pTransCom->m_vInterval = _vec3(0.f, 20.f, 0.f);
	m_iCountGold = 36;

	m_strAniName = "Open";
	return S_OK;
}

void CTresureChest::LateReady_Object(void)
{
	CMonster::LateReady_Object();

	m_pStatusCom->m_fAtt = 0.f;
	m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 450.f;
	m_pStatusCom->m_iGold = 36;
	m_pStatusCom->m_fMaxExp = 0;
	m_iCountGold = m_pStatusCom->m_iGold;
}

_int CTresureChest::Update_Object(const _float& fTimeDelta)
{
	if (m_fDeathTime >= 1)
		return EVENT_DEAD;
	CGameObject::LateReady_Object();

	HRESULT hr = Behavior(fTimeDelta);
	m_pMeshAdminCom->Set_AnimationSet(m_strAniName);
	CMeshObj::Update_Object(fTimeDelta);
	auto bEndTime = m_pMeshAdminCom->Is_AnimationSetEnd();
	if(m_iEvent != EVENT_DEAD || bEndTime)
		m_fTimeDelta_AniPlay = 0.f;
	if (bEndTime)
		m_fDeathTime += fTimeDelta;
	return EVENT_DEFAULT;
}

void CTresureChest::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CTresureChest::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CTresureChest::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	if (m_iEvent == EVENT_DEAD)
		return;
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	bool bDead = false;

	if (dwUsingCollision & COLLISION_BASE_SRC)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			bDead = true;
			Engine::Get_SoundMgr()->StartSound(L"DeadBuild", Engine::CSoundMgr::SE, false);
			
			auto pmapPlayer = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
			if (pmapPlayer.first != pmapPlayer.second)
			{
				auto pPlayer = pmapPlayer.first->second;
				auto pStatus_Player = (Engine::CStatus*)pPlayer->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
				if (nullptr != pStatus_Player)
					pStatus_Player->m_fExp += m_pStatusCom->m_fMaxExp;
				if (m_eTeam != pPlayer->Get_Team())
					Create_Gold(pPlayer);
			}
		}
		else
		{
			Engine::Get_SoundMgr()->StartSound(L"HitBuild", Engine::CSoundMgr::SE, false);
			m_abAniCount[ANICOUNT_NOIDLE] = true;
		}
		CMeshObj::Effect_BeAttacked();
		auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
		Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
		Effect_KnockBack(m_pTransCom->Get_Pos());
	}

	if (true == bDead)
		m_iEvent = EVENT_DEAD;
}

HRESULT CTresureChest::Behavior(const _float & fTimeDelta)
{
	//if (m_iEvent == EVENT_DEAD)
	//{
	//	m_strAniName = "Death";
	//	if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
	//		return E_FAIL;
	//	else
	//		return S_OK;
	//}

	Set_Target(nullptr);
	return S_OK;
}

HRESULT CTresureChest::Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget)
{
	_vec3 vPos, vPos_Target, vDir;
	auto pTrans = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	vPos = m_pTransCom->Get_Pos();
	vPos_Target = pTrans->Get_Pos();
	D3DXVec3Normalize(&vDir, &(vPos_Target - vPos));
	_vec3 vRadian;
	Engine::DirToRadian(&vRadian, &vDir);
	_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&vPos, &(vDir * m_fSpeed * fTimeDelta));
	m_pTransCom->Set_Pos(vTemp);
	vRadian.x = 0.f;
	vRadian.z = 0.f;
	m_pTransCom->Set_Radian(vRadian);
	return S_OK;
}

HRESULT CTresureChest::Movement_GuidLine(const _float & fTimeDelta)
{
	if (nullptr == m_pDirectionObj)
		return E_FAIL;
	if (false == m_pDirectionObj->Is_DirectionIdx(m_iDirectIdx))
		return E_FAIL;
	_vec3 vPos, vPos_Target, vDir;
	vPos = m_pTransCom->Get_Pos();
	vPos_Target = m_pDirectionObj->Get_DirectPos(m_iDirectIdx);
	vPos_Target.y -= 1.f;
	D3DXVec3Normalize(&vDir, &(vPos_Target - vPos));
	_vec3 vRadian;
	Engine::DirToRadian(&vRadian, &vDir);
	_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&vPos, &(vDir * m_fSpeed * fTimeDelta));
	m_pTransCom->Set_Pos(vTemp);
	m_pTransCom->Set_Radian(vRadian);

	auto fDist = D3DXVec3Length(&(vPos_Target - vPos));
	if (fDist < 2.f)
		++m_iDirectIdx;
	return S_OK;
}

HRESULT CTresureChest::Attack(const _float & fTimeDelta, Engine::CGameObject * pTarget)
{
	_vec3 vPos, vPos_Target, vDir;
	auto pTrans = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	vPos = m_pTransCom->Get_Pos();
	vPos_Target = pTrans->Get_Pos();
	D3DXVec3Normalize(&vDir, &(vPos_Target - vPos));
	_vec3 vRadian;
	Engine::DirToRadian(&vRadian, &vDir);
	//m_pTransCom->Set_Pos(vPos + vDir * m_fSpeed * fTimeDelta);
	m_pTransCom->Set_Radian(vRadian);

	return S_OK;
}

HRESULT CTresureChest::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	m_pColliderCom->Set_UsingCollision(COLLISION_BASE_SRC);
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"TresureChest");
	m_pTransCom->Set_Scale(_vec3(0.08f, 0.08f, 0.08f));
	//pComponent = m_pNaviMeshCom = (Engine::CNaviMesh*)Engine::Get_NaviMesh();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	return S_OK;
}

_vec3 CTresureChest::Get_StartBulletPos()
{
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	_vec3 vInterval = { 0.f, 15.f, 0.f };
	D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	return (_vec3)matWorld.m[3] + vInterval;
}

CTresureChest * CTresureChest::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CTresureChest *	pInstance = new CTresureChest(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CTresureChest::Free(void)
{
	CMeshObj::Free();
}
