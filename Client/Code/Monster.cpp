#include "stdafx.h"
#include "Monster.h"
#include "HpBar_B.h"
#include "DirectionObj.h"
#include "Bullet.h"
#include "Export_Function.h"
#include "Gold.h"
#include "Damage.h"
#include "Player.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_fSpeed(3.f)
{
	m_eTeam = Engine::TEAMTYPE_TEAM2;
}

CMonster::~CMonster(void)
{
}

HRESULT CMonster::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	//m_eRenderType = Engine::RENDERID::RENDER_ALPHA;

	if (m_iFlag == 1)
	{
		m_pStatusCom->m_fAtt = 90.f;
		m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 720.f;
		m_pStatusCom->m_iGold = 3;
		m_pStatusCom->m_fMaxExp = 10;
	}
	else
	{
		m_pStatusCom->m_fAtt = 560.f;
		m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 2800.f;
		m_pStatusCom->m_iGold = 12;
		m_pStatusCom->m_fMaxExp = 24;
	}
	m_iCountGold = m_pStatusCom->m_iGold;
	return S_OK;
}

void CMonster::LateReady_Object(void)
{
	m_pNaviMeshCom = (Engine::CNaviMesh*)(Engine::Get_NaviMesh());
	NULL_CHECK(m_pNaviMeshCom);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	CMeshObj::LateReady_Object();
	CMeshObj::Seek_NaviIndex();

	if (m_iFlag == 1)
		m_pTransCom->m_vInterval = _vec3(0.f, 10.f, 0.f);

	/////////////체력 바 생성
	auto pHpBar = CHpBar_B::Create(m_pGraphicDev, this);
	Engine::Add_Object(L"GameLogic", L"HpBar_B", pHpBar);

	auto pair = Engine::Get_ObjectMultimap(L"GameLogic_Direction")->equal_range(m_strDirection);
	if (pair.first != pair.second)
		m_pDirectionObj = (CDirectionObj*)(pair.first->second);
}

_int CMonster::Update_Object(const _float& fTimeDelta)
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

	return m_iEvent;
}

void CMonster::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CMonster::Render_Object(void)
{
	CMeshObj::Render_Object();
}

void CMonster::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	auto vSoundPos = m_pTransCom->Get_Pos();

	if (m_iEvent == EVENT_DEAD)
		return;
	bool bDead = false;
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_SRC)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			if (m_iFlag == 0)
			{
				Engine::Get_SoundMgr()->StartSound(L"Dead1_Ogre", Engine::CSoundMgr::SE, false, &vSoundPos);
			}
			else
			{
				TCHAR szSoundName[MIN_STR];
				ZeroMemory(szSoundName, sizeof(szSoundName));
				swprintf_s(szSoundName, L"Dead%d_Goblin", rand() % 6 + 1);
				Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false, &vSoundPos);
			}
			auto pmapPlayer = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
			if (pmapPlayer.first != pmapPlayer.second)
			{
				auto pPlayer = pmapPlayer.first->second;
				auto pStatus_Player = (Engine::CStatus*)pPlayer->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
				if (nullptr != pStatus_Player)
					pStatus_Player->m_fExp += m_pStatusCom->m_fMaxExp;
				if(m_eTeam != pPlayer->Get_Team())
					Create_Gold(pPlayer);
			}
			bDead = true;
		}
		else
		{
			//if (m_fHurtTime >= 1.f)
			//{
			//	m_fHurtTime = 0.f;
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				if (m_iFlag == 0)
				{
					m_strAniName = "Shock";
					Engine::Get_SoundMgr()->StartSound(L"Hurt1_Ogre", Engine::CSoundMgr::SE, false, &vSoundPos);
				}
				else
				{
					m_strAniName = "Hurt";
					TCHAR szSoundName[MIN_STR];
					ZeroMemory(szSoundName, sizeof(szSoundName));
					swprintf_s(szSoundName, L"Hurt%d_Goblin", rand()%3 + 1);
					Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false, &vSoundPos);
				}
			//}
			auto pPlayer = dynamic_cast<CPlayer*>(pGameObject);
			if (nullptr != pPlayer)
			{
				auto pTrans_Player = (Engine::CTransform*)pPlayer->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
				float fPower = 1.f;
				if(true == pPlayer->IsAura())
					fPower = 10.f;
				Effect_KnockBack(pTrans_Player->Get_Pos(), fPower);
			}
		}
		CMeshObj::Effect_BeAttacked();
		auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
		Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
	}
	if (true == bDead)
		m_iEvent = EVENT_DEAD;
}

HRESULT CMonster::Behavior(const _float & fTimeDelta)
{
	auto vSoundPos = m_pTransCom->Get_Pos();

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
	auto fVolumeX = m_pColliderCom->Get_Range()->Get_Volume().x * m_pTransCom->Get_Scale().x;
	if (fLength > fVolumeX + 1 || (m_eTeam == pTarget->Get_Team() && fLength > 3))
	{
		Movement(fTimeDelta, pTarget);
		m_strAniName = "Move_forward";
	}
	else
	{
		if (pTarget->Get_Team() != m_eTeam && m_strAniName == "Idle")
		{
			Attack(fTimeDelta, pTarget);

			if (m_iFlag == 1)
			{
				if (m_iAniCount % 2 == 0)
					m_strAniName = "Attack";
				else
					m_strAniName = "Attack_1";
			}
			else
			{
				if (m_iAniCount % 2 == 0)
					m_strAniName = "Attack1";
				else
					m_strAniName = "Attack2";
			}

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
				if (m_iFlag == 0)
				{
					Engine::Get_SoundMgr()->StartSound(L"Att1_Ogre", Engine::CSoundMgr::SE, false, &vSoundPos);
				}
				else
				{
					TCHAR szSoundName[MIN_STR];
					ZeroMemory(szSoundName, sizeof(szSoundName));
					swprintf_s(szSoundName, L"Att%d_Goblin", rand() % 5 + 1);
					Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::SE, false, &vSoundPos);
				}
			}
		}
	}
	Set_Target(nullptr);
	return S_OK;
}

HRESULT CMonster::Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget)
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

HRESULT CMonster::Movement_GuidLine(const _float & fTimeDelta)
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

HRESULT CMonster::Attack(const _float & fTimeDelta, Engine::CGameObject * pTarget)
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

HRESULT CMonster::Create_Gold(Engine::CGameObject* pTarget)
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
		D3DXMatrixRotationAxis(&matRotate, &vLook, D3DXToRadian(360.f/ m_iCountGold * i));
		_vec3 vRadian;
		Engine::MatrixToRadian(&vRadian, &matRotate);
		pTans_Gold->Set_Radian(vRadian);
		Engine::Add_Object(L"GameLogic", L"Gold", pGold);
	}
	return E_NOTIMPL;
}

HRESULT CMonster::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	m_pColliderCom->Set_UsingCollision(COLLISION_BASE_SRC);
	m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_SRC | COLLISION_PUSH_DST);

	if (m_iFlag == 1)
	{
		m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Goblin");
		m_pTransCom->Set_Scale(_vec3(0.07f, 0.07f, 0.07f));
	}
	else
	{
		m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"OrcBruiser");
		m_pTransCom->Set_Scale(_vec3(0.1f, 0.1f, 0.1f));
	}
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"DarkElfArcher");
	
	//pComponent = m_pNaviMeshCom = (Engine::CNaviMesh*)Engine::Get_NaviMesh();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	return S_OK;
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CMonster *	pInstance = new CMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CMonster::Free(void)
{
	CMeshObj::Free();
}
