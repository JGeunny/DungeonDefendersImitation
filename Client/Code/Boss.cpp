#include "stdafx.h"
#include "Boss.h"
#include "HpBar_B.h"
#include "DirectionObj.h"
#include "Bullet.h"
#include "Export_Function.h"
#include "Damage.h"
#include "ShoulderCamera.h"
#include "FireBall.h"
#include "ElecBall.h"

#include "HpBar.h"
#include "Gold.h"
#include "Player.h"
#include "Effect.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_fSpeed(5.f)
{
	m_eTeam = Engine::TEAMTYPE_TEAM2;
}

CBoss::~CBoss(void)
{
}

HRESULT CBoss::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	//m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	m_strAniName = "Cinematic_cutthroat";
	m_abAniCount[ANICOUNT_NOIDLE] = true;
	return S_OK;
}

void CBoss::LateReady_Object(void)
{
	m_pNaviMeshCom = (Engine::CNaviMesh*)(Engine::Get_NaviMesh());
	NULL_CHECK(m_pNaviMeshCom);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	CMeshObj::LateReady_Object();
	CMeshObj::Seek_NaviIndex();

	/////////////체력 바 생성
	auto pHpBar = CHpBar_B::Create(m_pGraphicDev, this);
	Engine::Add_Object(L"GameLogic", L"HpBar_B", pHpBar);

	auto pair = Engine::Get_ObjectMultimap(L"GameLogic_Direction")->equal_range(m_strDirection);
	if (pair.first != pair.second)
		m_pDirectionObj = (CDirectionObj*)(pair.first->second);

	m_pTransCom->Set_Scale(_vec3(0.3f, 0.3f, 0.3f));
	m_vBeginPos = m_pTransCom->Get_Pos();

	Create_HpBar();
}

_int CBoss::Update_Object(const _float& _fTimeDelta)
{
	Update_SpoilBullet(_fTimeDelta);
	if(m_eBossState != BOSSSTATE::BOSSSTATE_FLY)
		m_fFlyCount += _fTimeDelta;
	if (m_fFlyCount > 10.f)
	{
		m_fFlyCount = 0.f;
		m_eBossState = BOSSSTATE::BOSSSTATE_FLY;
		Add_Pattern();
	}
	CGameObject::LateReady_Object();
	float fTimeDelta = _fTimeDelta;
	fTimeDelta += (m_iRage - 1)/6 * fTimeDelta;
	HRESULT hr = Behavior(fTimeDelta);
	m_pMeshAdminCom->Set_AnimationSet(m_strAniName);
	if (false == m_abAniCount[ANICOUNT_NOIDLE])
	{
		if (m_strAniName == "Fly_pound_end")
			m_strAniName = "Fly_pound_loop";
		if (m_strAniName != "Fly_pound_loop")
		{
			if (m_eBossState == BOSSSTATE::BOSSSTATE_FLY)
				m_strAniName = "Fly_hover";
			else
				m_strAniName = "Idle";
		}
	}
	

	CMeshObj::Update_Object(fTimeDelta);

	if (EVENT_DEAD == m_iEvent)
	{
		if (m_pHpBar != nullptr)
		{
			m_pHpBar->Set_Event(EVENT_DEAD);
			m_pHpBar = nullptr;
		}

		if (SUCCEEDED(hr))
			return EVENT_DEFAULT;
		else
			return m_iEvent;
	}

	return m_iEvent;
}

void CBoss::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CBoss::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CBoss::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
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
			Engine::Get_SoundMgr()->StartSound(L"Dead1_Boss", Engine::CSoundMgr::SE, false);

			auto pmapPlayer = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
			if (pmapPlayer.first != pmapPlayer.second)
			{
				auto pPlayer = pmapPlayer.first->second;
				auto pStatus_Player = (Engine::CStatus*)pPlayer->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
				if (nullptr != pStatus_Player)
					pStatus_Player->m_fExp += m_pStatusCom->m_fMaxExp;
				if (m_eTeam != pPlayer->Get_Team())
					Create_Gold(pPlayer);

				if (pGameObject == pPlayer)
					m_pStatusCom->Recovery(pStatus_Enemy->m_fAtt * 0.5f);
			}
		}
		else
		{
			if (m_fHurtTime >= 1.f)
			{
				m_fHurtTime = 0.f;
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Hurt";

				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"Hurt%d_Boss", rand() % 2 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
			}

			auto pPlayer = dynamic_cast<CPlayer*>(pGameObject);
			
			if (nullptr != pPlayer)
			{
				if (true == pPlayer->IsAura())
				{
					auto pTrans_Player = (Engine::CTransform*)pPlayer->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
					auto fPower = 5.f;
					Effect_KnockBack(pTrans_Player->Get_Pos(), fPower);
				}
			}
		}
		CMeshObj::Effect_BeAttacked();
		auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
		Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
	}

	if(true == bDead)
		m_iEvent = EVENT_DEAD;
}

HRESULT CBoss::Behavior(const _float & fTimeDelta)
{
	if (m_iEvent == EVENT_DEAD)
	{
		m_strAniName = "Death";
		if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
			return E_FAIL;
		else
			return S_OK;
	}
	auto fHp = m_pStatusCom->m_fHp / m_pStatusCom->m_fMaxHp;
	static float fHp_Before;

	if (nullptr != m_pEffect_Aura)
	{
		auto pTrans = (Engine::CTransform*)m_pEffect_Aura->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		auto vScale = pTrans->Get_Scale();

		if (fHp < 0.25f)
		{
			if (fHp_Before == 0.50f)
			{
				m_iRage = 4;
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Attack_summonflame";
				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"Att%d_Boss", rand() % 2 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
				fHp_Before = 0.25f;
				vScale *= 1.2;
				pTrans->Set_Scale(vScale);
			}
		}

		if (fHp < 0.50f)
		{
			if (fHp_Before == 0.75f)
			{
				m_iRage = 3;
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Attack_summonflame";
				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"Att%d_Boss", rand() % 2 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
				fHp_Before = 0.50f;
				vScale *= 1.2;
				pTrans->Set_Scale(vScale);
			}
		}
	}
	if (fHp < 0.75f)
	{
		if (nullptr == m_pEffect_Aura)
		{
			m_iRage = 2;
			m_abAniCount[ANICOUNT_NOIDLE] = true;
			m_strAniName = "Attack_summonflame";
			TCHAR szSoundName[MIN_STR];
			ZeroMemory(szSoundName, sizeof(szSoundName));
			swprintf_s(szSoundName, L"Att%d_Boss", rand() % 2 + 1);
			Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
			fHp_Before = 0.75f;
			m_pEffect_Aura = Create_Effect_Aura();
		}
	}

	auto eEnemy = (m_eTeam == Engine::TEAMTYPE_TEAM1) ? Engine::TEAMTYPE_TEAM2 : Engine::TEAMTYPE_TEAM1;
	if (m_eBossState == BOSSSTATE::BOSSSTATE_FLY)
		//Set_Target(Engine::Find_NearTarget(L"GameLogic", L"Player", this, 9999));
		Set_Target(Engine::Find_NearTarget(L"GameLogic", eEnemy, this, 9999));
	else
		Set_Target(Engine::Find_NearTarget(L"GameLogic", eEnemy, this, 10));
	auto pTarget = Get_Target();
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
	if (m_eBossState == BOSSSTATE::BOSSSTATE_FLY)
	{
		if (m_bAttack && m_strAniName == "Fly_hover")
		{
			Attack(fTimeDelta, pTarget);
			m_strAniName = "Fly_fireball";
			Engine::RANGEFLOAT tRangeFloat;
			tRangeFloat.fMax = 0.55f;
			tRangeFloat.fMin = 0.4f;
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
			{
				if (false == m_abAniCount[ANICOUNT_ATTACK])
				{
					TCHAR szSoundName[MIN_STR];
					ZeroMemory(szSoundName, sizeof(szSoundName));
					swprintf_s(szSoundName, L"Att%d_Boss", rand() % 2 + 1);
					Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false);
					//Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
					m_abAniCount[ANICOUNT_ATTACK] = true;
					/*_vec3 vRadian, vDir;
					auto vPos_This = m_pTransCom->Get_Pos();
					auto vPos_Target = ((Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
					D3DXVec3Normalize(&vDir, &(vPos_Target - vPos_This));
					Engine::DirToRadian(&vRadian, &vDir);
										
					CBullet::BULLET_DESC tBullet;
					tBullet.vDir = vDir;
					tBullet.eTeam = m_eTeam;
					tBullet.fAtt = m_pStatusCom->m_fAtt;
					tBullet.vPos = Get_StartBulletPos();
					tBullet.vRadian = vRadian;

					CBullet* pBullet = nullptr;
					if (true)
					{
						tBullet.iBulletType = 1;
						tBullet.vScale = _vec3(0.05f, 0.05f, 0.05f);
						pBullet = CFireBall::Create(m_pGraphicDev, &tBullet);
						Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
					}
					else
					{
						tBullet.iBulletType = 2;
						tBullet.vScale = _vec3(0.02f, 0.02f, 0.02f);
						pBullet = CElecBall::Create(m_pGraphicDev, &tBullet);
						Engine::Get_SoundMgr()->StartSound(L"Fire_ElectBall", Engine::CSoundMgr::SE, false);
					}
					Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);*/

					if (m_lstSpoilBullet.size() == 0)
					{
						if(fHp < 0.25f)
						{
							Ready_SpoilBullet(21);
							Ready_SpoilBullet(21, 0.5f);
							Ready_SpoilBullet(21, 1.f);
							Ready_SpoilBullet(21, 1.5f);
							Ready_SpoilBullet(20);
							Ready_SpoilBullet(20, 0.5f);
							Ready_SpoilBullet(20, 1.f);
							Engine::Get_SoundMgr()->StartSound(L"Fire_ElectBall", Engine::CSoundMgr::SE, false);
						}
						else if (fHp < 0.5f)
						{
							Ready_SpoilBullet(21);
							Ready_SpoilBullet(21, 0.5f);
							Ready_SpoilBullet(21, 1.f);
							Ready_SpoilBullet(20);
							Ready_SpoilBullet(20, 0.5f);
							Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
						}
						else if (fHp < 0.75f)
						{
							Ready_SpoilBullet(11);
							Ready_SpoilBullet(11, 0.5f);
							Ready_SpoilBullet(11, 1.f);
							Ready_SpoilBullet(10);
							Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
						}
						else
						{
							Ready_SpoilBullet(10);
							Engine::Get_SoundMgr()->StartSound(L"Fire_FireBall", Engine::CSoundMgr::SE, false);
						}
					}
				}
				m_bAttack = false;
				m_strAniName = "Fly_hover";
			}
			//if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
			//{
			//	m_bAttack = false;
			//	m_strAniName = "Fly_hover";
			//}
		}
		else if (m_bAttack && m_strAniName == "Fly_pound_loop")
		{
			Attack(fTimeDelta, pTarget);
			m_strAniName = "Fly_pound_end";
			Engine::RANGEFLOAT tRangeFloat;
			tRangeFloat.fMax = 0.7f;
			tRangeFloat.fMin = 0.6f;
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
			{
				if (false == m_abAniCount[ANICOUNT_ATTACK])
				{
					m_abAniCount[ANICOUNT_ATTACK] = true;

					auto vPos_Target = ((Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC))->Get_Pos();
					auto vPos_This = m_pTransCom->Get_Pos();
					auto fDist = D3DXVec3Length(&(vPos_Target - vPos_This));
					if (fDist < 6.f)
					{
						pTarget->BeCollision(COLLISION_BASE_SRC, this);
					}
					//카메라 쉐이킹
					auto pair_Camera = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"ShoulderCamera");
					if (pair_Camera.first != pair_Camera.second)
					{
						auto pCamera = (CShoulderCamera*)pair_Camera.first->second;
						if (nullptr != pCamera)
						{
							pCamera->DoShack(false);
						}
					}
				}
			}
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd())
			{
				m_bAttack = false;
				m_strAniName = "Fly_hover";
			}
		}
		else
		{
			Movement(fTimeDelta, pTarget);
		}
	}
	else
	{

		auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		auto fLength = D3DXVec3Length(&(m_pTransCom->Get_Pos() - pTrans_Target->Get_Pos()));
		if (fLength > 3)
		{
			Movement(fTimeDelta, pTarget);
			m_strAniName = "Move_forward";
		}
		else
		{
			if (pTarget->Get_Team() != m_eTeam && (m_strAniName == "Idle"))
			{
				Attack(fTimeDelta, pTarget);

				//if (m_iAniCount % 2 == 0)
				//	m_strAniName = "Attack_Left";
				//else
				m_strAniName = "Attack_Right";

				Engine::RANGEFLOAT tRangeFloat;
				tRangeFloat.fMax = 0.5f;
				tRangeFloat.fMin = 0.4f;
				if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
				{
					if (false == m_abAniCount[ANICOUNT_ATTACK])
					{
						m_abAniCount[ANICOUNT_ATTACK] = true;
						pTarget->BeCollision(COLLISION_BASE_SRC, this);
					}
				}
			}
		}
	}
	//Set_Target(nullptr);

	CBullet* pBullet = nullptr;
	auto iter = m_lstSpoilBullet.begin();
	while (iter != m_lstSpoilBullet.end())
	{
		if ((*iter).fDelayTime <= 0)
		{
			if ((*iter).tBullet.iBulletType == 1)
			{
				pBullet = CFireBall::Create(m_pGraphicDev, &(*iter).tBullet);
				Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
				if (true == (*iter).tBullet.bTimeDelta_Target)
					pBullet->Set_Target(pTarget);
			}
			else if ((*iter).tBullet.iBulletType == 2)
			{
				pBullet = CElecBall::Create(m_pGraphicDev, &(*iter).tBullet);
				Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
				if (true == (*iter).tBullet.bTimeDelta_Target)
					pBullet->Set_Target(pTarget);
			}
			iter = m_lstSpoilBullet.erase(iter);
		}
		else
			++iter;
	}

	return S_OK;
}

HRESULT CBoss::Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget)
{
	_vec3 vPos, vPos_Target, vDir;
	auto pTrans = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	vPos = m_pTransCom->Get_Pos();
	vPos_Target = pTrans->Get_Pos();
	D3DXVec3Normalize(&vDir, &(vPos_Target - vPos));
	_vec3 vRadian;                              
	Engine::DirToRadian(&vRadian, &vDir);

	if (m_eBossState == BOSSSTATE::BOSSSTATE_FLY)
	{
		auto iter = m_vecPos.begin();
		while (iter != m_vecPos.end())
		{
			auto fDist = D3DXVec3Length(&(*iter - vPos));
			if (fDist < 1.5f)
			{
				iter = m_vecPos.erase(iter);
				if (0 == rand() % 6)
				{
					m_strAniName = "Fly_pound_loop";
					iter = m_vecPos.insert(iter, vPos_Target);
				}
				else
					m_bAttack = true;
			}
			break;
		}
		
		if (iter != m_vecPos.end())
		{
			_vec3 vDir_Pos = (*iter) - vPos;
			D3DXVec3Normalize(&vDir_Pos, &vDir_Pos);
			if(m_strAniName == "Fly_pound_loop")
				m_pTransCom->Set_Pos(vPos + (vDir_Pos * m_fSpeed * 6.f * fTimeDelta));
			else
				m_pTransCom->Set_Pos(vPos + (vDir_Pos * m_fSpeed * 2.f * fTimeDelta));
			m_pTransCom->Set_Radian(vRadian);
		}
		else
		{
			m_eBossState = BOSSSTATE::BOSSSTATE_BASE;
			CMeshObj::Seek_NaviIndex();
		}
	}
	else
	{
		_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&vPos, &(vDir * m_fSpeed * fTimeDelta));
		m_pTransCom->Set_Pos(vTemp);
		vRadian.x = 0.f;
		vRadian.z = 0.f;
		m_pTransCom->Set_Radian(vRadian);
	}
	
	return S_OK;
}

HRESULT CBoss::Movement_GuidLine(const _float & fTimeDelta)
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

HRESULT CBoss::Attack(const _float & fTimeDelta, Engine::CGameObject * pTarget)
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

HRESULT CBoss::Ready_SpoilBullet(int iType, float fDelayTime)
{
	//m_lstSpoilBullet.clear();
	
	_vec3 vDir = _vec3(0.f, 0.f, 0.f);
	_vec3 vRight = _vec3(0.f, 0.f, 0.f);
	auto pMeshObj = (CMeshObj*)Get_Target();
	if (nullptr != pMeshObj)
	{
		D3DXVec3Normalize(&vDir, &(pMeshObj->Get_StartBulletPos() - Get_StartBulletPos()));
	}
	else
	{
		m_pTransCom->Get_Info(Engine::INFO_LOOK, &vDir);
		D3DXVec3Normalize(&vDir, &vDir);
	}
	m_pTransCom->Get_Info(Engine::INFO_RIGHT, &vRight);
	D3DXVec3Normalize(&vRight, &vRight);

	switch (iType)
	{
	case 10:
	{
		BOSS_BULLET_DESC tBullet;
		tBullet.tBullet.vDir = vDir;
		tBullet.tBullet.eTeam = m_eTeam;
		tBullet.tBullet.fAtt = m_pStatusCom->m_fAtt;
		tBullet.tBullet.vPos = Get_StartBulletPos();
		tBullet.tBullet.iBulletType = 1;
		tBullet.tBullet.vScale = _vec3(0.05f, 0.05f, 0.05f);
		tBullet.fDelayTime += fDelayTime;
		m_lstSpoilBullet.emplace_back(tBullet);
	}
	break;
	case 20:
	{
		BOSS_BULLET_DESC tBullet;
		tBullet.tBullet.vDir = vDir;
		tBullet.tBullet.eTeam = m_eTeam;
		tBullet.tBullet.fAtt = m_pStatusCom->m_fAtt;
		tBullet.tBullet.vPos = Get_StartBulletPos();
		tBullet.tBullet.iBulletType = 2;
		tBullet.tBullet.vScale = _vec3(0.02f, 0.02f, 0.02f);
		tBullet.fDelayTime += fDelayTime;
		m_lstSpoilBullet.emplace_back(tBullet);
	}
	break;
	case 11:
		for (int i = 0; i < 12; ++i)
		{
			_matrix matRotate;
			D3DXMatrixRotationAxis(&matRotate, &vDir, D3DXToRadian(360.f / 12 * i));
			D3DXVec3TransformCoord(&vRight, &vRight, &matRotate);
			D3DXVec3Normalize(&vRight, &vRight);
			
			
			BOSS_BULLET_DESC tBullet;
			tBullet.tBullet.vDir = vRight;
			tBullet.tBullet.eTeam = m_eTeam;
			tBullet.tBullet.fAtt = m_pStatusCom->m_fAtt * 0.5f;
			tBullet.tBullet.vPos = Get_StartBulletPos();
			//tBullet.tBullet.fTimeDelta_Target = 2.f;
			//tBullet.tBullet.bTimeDelta_Target = true;
			tBullet.tBullet.iBulletType = 1;
			tBullet.tBullet.vScale = _vec3(0.02f, 0.02f, 0.02f);
			tBullet.fDelayTime += fDelayTime;
			m_lstSpoilBullet.emplace_back(tBullet);
		}
		break;
	case 21:
		for (int i = 0; i < 12; ++i)
		{
			_matrix matRotate;
			D3DXMatrixRotationAxis(&matRotate, &vDir, D3DXToRadian(360.f / 12 * i));
			D3DXVec3TransformCoord(&vRight, &vRight, &matRotate);
			D3DXVec3Normalize(&vRight, &vRight);

			BOSS_BULLET_DESC tBullet;
			tBullet.tBullet.vDir = vRight;
			tBullet.tBullet.eTeam = m_eTeam;
			tBullet.tBullet.fAtt = m_pStatusCom->m_fAtt * 0.5f;
			tBullet.tBullet.vPos = Get_StartBulletPos();
			tBullet.tBullet.fTimeDelta_Target = 2.f;
			tBullet.tBullet.bTimeDelta_Target = true;
			tBullet.tBullet.iBulletType = 2;
			tBullet.tBullet.vScale = _vec3(0.008f, 0.008f, 0.008f);

			tBullet.fDelayTime += fDelayTime;
			m_lstSpoilBullet.emplace_back(tBullet);
		}
		break;
	}
	return S_OK;
}

HRESULT CBoss::Update_SpoilBullet(_float fTimeDelta)
{
	for (auto& tBullet : m_lstSpoilBullet)
		tBullet.fDelayTime -= fTimeDelta;
	return S_OK;;
}

HRESULT CBoss::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	m_pColliderCom->Set_UsingCollision(COLLISION_BASE_SRC);
	//m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_SRC | COLLISION_PUSH_DST);

	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Demon");
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"DarkElfArcher");
	//pComponent = m_pNaviMeshCom = (Engine::CNaviMesh*)Engine::Get_NaviMesh();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	pComponent = m_pStatusCom = Engine::CStatus::Create(1250.f, 11800.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	return S_OK;
}

HRESULT CBoss::Add_Pattern()
{
	_vec3 vPos = m_vBeginPos;
	_vec3 vDir = _vec3(1.f, 0.f, 0.f);
	//switch (rand() % 2)
	switch (rand() % 2)
	{
	case 0:
		vPos.y += 20.f;
		m_vecPos.emplace_back(vPos);
		vPos.x -= 30.f;
		m_vecPos.emplace_back(vPos);
		vPos.x += 60.f;
		m_vecPos.emplace_back(vPos);
		vPos.x -= 30.f;
		m_vecPos.emplace_back(vPos);
		m_vecPos.emplace_back(m_vBeginPos);
		break;
	case 1:
		{
		_matrix matRot;
		_vec3 vDir_Pos;
			vPos.y += 10.f;
			m_vecPos.emplace_back(vPos);
			for (int i = 0; i < 12; ++i)
			{
				D3DXMatrixRotationZ(&matRot, D3DXToRadian(i* 30.f));
				D3DXVec3TransformNormal(&vDir_Pos, &vDir, &matRot);
				m_vecPos.emplace_back(vPos + vDir_Pos* 10.f);
			}
			vPos.x -= 30;
			m_vecPos.emplace_back(vPos);
			for (int i = 0; i < 12; ++i)
			{
				D3DXMatrixRotationZ(&matRot, D3DXToRadian(i* 30.f));
				D3DXVec3TransformNormal(&vDir_Pos, &vDir, &matRot);
				m_vecPos.emplace_back(vPos + vDir_Pos* 10.f);
			}
			vPos.x += 60;
			m_vecPos.emplace_back(vPos);
			for (int i = 0; i < 12; ++i)
			{
				D3DXMatrixRotationZ(&matRot, D3DXToRadian(i* 30.f));
				D3DXVec3TransformNormal(&vDir_Pos, &vDir, &matRot);
				m_vecPos.emplace_back(vPos + vDir_Pos* 10.f);
			}
			vPos.x -= 30;
			m_vecPos.emplace_back(vPos);
			for (int i = 0; i < 12; ++i)
			{
				D3DXMatrixRotationZ(&matRot, D3DXToRadian(i* 30.f));
				D3DXVec3TransformNormal(&vDir_Pos, &vDir, &matRot);
				m_vecPos.emplace_back(vPos + vDir_Pos* 10.f);
			}
			m_vecPos.emplace_back(m_vBeginPos);
			break;
		}
	}
	return S_OK;
}

CEffect * CBoss::Create_Effect_Aura()
{
	m_pStatusCom->m_fAtt *= 1.2f;
	float fScale = m_pTransCom->Get_Scale().x * 25.f;
	CEffect::EFFECT_DESC tEffect_Info;
	tEffect_Info.eType = CEffect::EFFECT_REPEAT;
	tEffect_Info.strImageKey = L"Texture_Effect_Aura";
	tEffect_Info.bNoTargetDead = true;
	tEffect_Info.vIntervalPos = _vec3(0.f, 0.8f, 0.f) * fScale;
	auto pEffect = CEffect::Create(m_pGraphicDev, &tEffect_Info);
	pEffect->Set_Target(this);
	pEffect->Get_Transform()->Set_Scale(m_pTransCom->Get_Scale() * 40.f);
	//pEffect->Get_Transform()->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));
	Engine::Add_Object(L"GameLogic", L"Effect", pEffect);

	return pEffect;
}

_vec3 CBoss::Get_StartBulletPos()
{
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	_vec3 vInterval = { 0.f, 13.f, 0.f };
	D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	return (_vec3)matWorld.m[3] + vInterval;
}

HRESULT CBoss::Create_HpBar()
{
	Engine::CGameObject* pGameObject = nullptr;
	CHpBar::HPBAR_DESC tHpBar;
	tHpBar.vPos = _vec2(400 + 70, 90.f);
	tHpBar.vSize = _vec2(500.f, 60.f);
	tHpBar.dwColor = D3DCOLOR_ARGB(200, 255, 55, 55);
	tHpBar.strContent = L"Boss";

	tHpBar.pfCurHp = &m_pStatusCom->m_fHp;
	tHpBar.pfMaxHp = &m_pStatusCom->m_fMaxHp;
	pGameObject = m_pHpBar = CHpBar::Create(m_pGraphicDev, tHpBar);
	Engine::Add_Object(L"GameLogic", L"HpBar_Boss", pGameObject);

	return S_OK;
}

CBoss * CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CBoss *	pInstance = new CBoss(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CBoss::Free(void)
{
	CMeshObj::Free();
}

HRESULT CBoss::Create_Gold(Engine::CGameObject* pTarget)
{
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW, &matView);
	_vec3 vLook;
	D3DXVec3Normalize(&vLook, &(-(_vec3)matView.m[2]));
	for (int i = 0; i < m_iCountGold; ++i)
	{
		auto pGold = CGold::Create(m_pGraphicDev, pTarget);
		auto pTans_Gold = (Engine::CTransform*)pGold->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		pTans_Gold->Set_Pos(m_pTransCom->Get_Pos());

		_matrix matRotate;
		D3DXMatrixRotationAxis(&matRotate, &vLook, D3DXToRadian(360.f / m_iCountGold * i));
		_vec3 vRadian;
		Engine::MatrixToRadian(&vRadian, &matRotate);
		pTans_Gold->Set_Radian(vRadian);
		Engine::Add_Object(L"GameLogic", L"Gold", pGold);
	}
	return E_NOTIMPL;
}