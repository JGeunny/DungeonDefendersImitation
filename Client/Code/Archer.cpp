#include "stdafx.h"
#include "Archer.h"
#include "Arrow.h"
#include "HpBar_B.h"
#include "DirectionObj.h"
#include "Bullet.h"
#include "Export_Function.h"
#include "Gold.h"
#include "Damage.h"
#include "Player.h"

CArcher::CArcher(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
	m_eTeam = Engine::TEAMTYPE_TEAM2;
}

CArcher::~CArcher(void)
{
}

HRESULT CArcher::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	//m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	m_pTransCom->m_vInterval = _vec3(0.f, 8.f, 0.f);
	m_iCountGold = 6;

	m_pStatusCom->m_fAtt = 320.f;
	m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 1200.f;
	m_pStatusCom->m_iGold = 9;
	m_pStatusCom->m_fMaxExp = 18;
	m_iCountGold = m_pStatusCom->m_iGold;
	return S_OK;
}

void CArcher::LateReady_Object(void)
{
	CMonster::LateReady_Object();
}

_int CArcher::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	/*if (Engine::KeyDown(KEY_LEFT))
		m_iEvent = EVENT_DEAD;
	*/
	HRESULT hr = Behavior(fTimeDelta);
	m_pMeshAdminCom->Set_AnimationSet(m_strAniName);
	if (false == m_abAniCount[ANICOUNT_NOIDLE])
		m_strAniName = "Idle";

	CMeshObj::Update_Object(fTimeDelta);

	if (EVENT_DEAD == m_iEvent)
	{
		if (SUCCEEDED(hr))
			return EVENT_DEFAULT;
		else
			return m_iEvent;
	}

	if ("Attack" == m_pMeshAdminCom->Get_AnimationSet())
		m_fTimeDelta_AniPlay = fTimeDelta* 1.5f;
	return m_iEvent;
}

void CArcher::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CArcher::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CArcher::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
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
			TCHAR szSoundName[MIN_STR];
			ZeroMemory(szSoundName, sizeof(szSoundName));
			swprintf_s(szSoundName, L"Dead%d_DarkElf", rand() % 2 + 1);
			Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
			bDead = true;
		
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
			/*if (m_fHurtTime >= 1.f)
			{
				m_fHurtTime = 0.f;*/
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Hurt";

				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"Hurt%d_DarkElf", rand() % 3 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
			//}

			auto pPlayer = dynamic_cast<CPlayer*>(pGameObject);
			if (nullptr != pPlayer)
			{
				auto pTrans_Player = (Engine::CTransform*)pPlayer->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
				float fPower = 1.f;
				if (true == pPlayer->IsAura())
					fPower = 10.f;
				Effect_KnockBack(pTrans_Player->Get_Pos(), fPower);
			}
		}
		CMeshObj::Effect_BeAttacked();
		auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
		Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
	}
	if(true == bDead)
		m_iEvent = EVENT_DEAD;
}

HRESULT CArcher::Behavior(const _float & fTimeDelta)
{
	if (m_iEvent == EVENT_DEAD)
	{
		m_strAniName = "Death";
		if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
			return E_FAIL;
		else
			return S_OK;
	}

	auto eEnemy = (m_eTeam == Engine::TEAMTYPE_TEAM1) ? Engine::TEAMTYPE_TEAM2 : Engine::TEAMTYPE_TEAM1;
	Set_Target(Engine::Find_NearTarget(L"GameLogic", eEnemy, this, 10));
	auto pTarget = Get_Target();
	if (nullptr == pTarget && m_eTeam == Engine::TEAMTYPE_TEAM1)
	{
		Set_Target(Engine::Find_NearTarget(L"GameLogic", L"Player", this, 10));
		pTarget = Get_Target();
	}

	if (nullptr == pTarget)
	{	
		if (SUCCEEDED(Movement_GuidLine(fTimeDelta)))
		{
			m_strAniName = "Move_forward";
			return S_OK;
		}
		else
			return E_FAIL;
	}
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto fLength = D3DXVec3Length(&(m_pTransCom->Get_Pos() - pTrans_Target->Get_Pos()));
	
	if (fLength > 8 || (m_eTeam == pTarget->Get_Team() && fLength > 3))
	{
		Movement(fTimeDelta, pTarget);
		m_strAniName = "Move_forward";
	}
	else
	{
		if (pTarget->Get_Team() != m_eTeam && m_strAniName == "Idle")
		{
			Attack(fTimeDelta, pTarget);

			if (true == m_bPreShotSound)
			{
				m_bPreShotSound = false;
				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"AttReady%d_DarkElf", rand() % 3 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
			}

			m_strAniName = "Attack";
			Engine::RANGEFLOAT tRangeFloat;
			tRangeFloat.fMax = 0.8f;
			tRangeFloat.fMin = 0.6f;
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
			{
				if (false == m_abAniCount[ANICOUNT_ATTACK])
				{
					m_bPreShotSound = true;
					TCHAR szSoundName[MIN_STR];
					ZeroMemory(szSoundName, sizeof(szSoundName));
					swprintf_s(szSoundName, L"Att%d_DarkElf", rand() % 3 + 1);
					Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);

					m_abAniCount[ANICOUNT_ATTACK] = true;
					_vec3 vRadian, vDir;
					auto vPos_This = m_pTransCom->Get_Pos();
					auto vPos_Target = ((Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
					D3DXVec3Normalize(&vDir, &(((CMeshObj*)pTarget)->Get_StartBulletPos() - Get_StartBulletPos()));
					Engine::DirToRadian(&vRadian, &vDir);
					vRadian.y += D3DX_PI;
					vRadian.x *= -1;

					CBullet::BULLET_DESC tBullet;
					tBullet.vDir = vDir;
					tBullet.eTeam = m_eTeam;
					tBullet.fAtt = m_pStatusCom->m_fAtt;
					tBullet.vPos = Get_StartBulletPos();
					tBullet.vRadian = vRadian;

					CBullet* pBullet = nullptr;
					tBullet.iBulletType = 3;
					tBullet.vScale = _vec3(0.03f, 0.03f, 0.03f);
					pBullet = CArrow::Create(m_pGraphicDev, &tBullet);
					
					Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
				}
			}
		}
	}
	Set_Target(nullptr);
	return S_OK;
}

HRESULT CArcher::Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget)
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

HRESULT CArcher::Movement_GuidLine(const _float & fTimeDelta)
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

HRESULT CArcher::Attack(const _float & fTimeDelta, Engine::CGameObject * pTarget)
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

HRESULT CArcher::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	m_pColliderCom->Set_UsingCollision(COLLISION_BASE_SRC);
	m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_SRC | COLLISION_PUSH_DST);

	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"DarkElfArcher");
	m_pTransCom->Set_Scale(_vec3(0.08f, 0.08f, 0.08f));
	//pComponent = m_pNaviMeshCom = (Engine::CNaviMesh*)Engine::Get_NaviMesh();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	return S_OK;
}

_vec3 CArcher::Get_StartBulletPos()
{
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	_vec3 vInterval = { 0.f, 13.f, 0.f };
	D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	return (_vec3)matWorld.m[3] + vInterval;
}

CArcher * CArcher::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CArcher *	pInstance = new CArcher(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CArcher::Free(void)
{
	CMeshObj::Free();
}
