#include "stdafx.h"
#include "Player.h"
#include "MeshObj.h"

#include "Export_Function.h"

#include "Bullet.h"
#include "Tower.h"
#include "Fishing.h"
#include "ShoulderCamera.h"
#include "HpBar_Player.h"
#include "LvUp.h"
#include "MiniGame.h"
#include "FireBall.h"
#include "ElecBall.h"
#include "MouseUI.h"

#include "Effect.h"
#include "MarioHouse.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_eMode(Engine::CONTROLTYPE_BASE)
	, m_eBuildMode(BUILDMODE_NONE)
	, m_eFishingMode(FISHINGMODE_NONE)
	, m_vFishingPickPoint(0.f, 0.f, 0.f)
{
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CPlayer::~CPlayer(void)
{
}

HRESULT CPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(0), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(5.f, 0.f, 2.f);
	m_fSpeed = 10.f;
	m_pTowerPreView = CreateTower(m_pTransCom->Get_Pos(), m_vTowerScale);
	m_pTowerPreView->Set_ShaderType(1.f);

	m_pFishingPreView = CreateFishing(m_pTransCom->Get_Pos());
	m_pFishingPreView->Set_ShaderType(1.f);
	m_pFishingPreView->Set_Target(this, false);

	return S_OK;
}

void CPlayer::LateReady_Object(void)
{
	m_pNaviMeshCom = (Engine::CNaviMesh*)(Engine::Get_NaviMesh());
	NULL_CHECK(m_pNaviMeshCom, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
	auto vPos = m_pTransCom->Get_Pos();
	auto dwIndex = m_pNaviMeshCom->Get_Index(vPos);
	m_pNaviMeshCom->Set_Index(dwIndex);

	auto pmapHpBar = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"HpBar_Player");
	if (pmapHpBar.first != pmapHpBar.second)
	{
		auto pHpBar = m_pHpBar_Player = (CHpBar_Player*)pmapHpBar.first->second;
		if (nullptr != pHpBar)
		{
			pHpBar->Set_Target(this);
		}
	}

	auto pmapUI = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"UI");
	if (pmapUI.first != pmapUI.second)
	{
		auto pUI = pmapUI.first->second;
		if (nullptr != pUI)
		{
			pUI->Set_Target(this);
		}
	}

	auto pair = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MiniGame");
	if (pair.first != pair.second)
		m_pMiniGame = (CMiniGame*)pair.first->second;

	auto pair_Mouse = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Mouse");
	if (pair_Mouse.first != pair_Mouse.second)
		m_pMouseUI = (CMouseUI*)pair_Mouse.first->second;

}

_int CPlayer::Update_Object(const _float& _fTimeDelta)
{
	float fTimeDelta = _fTimeDelta;
	if (nullptr != m_pEffect_Aura)
	{
		m_fAuraTime += fTimeDelta;
		if (m_fAuraTime > 13.f)
		{
			End_Aura();
		}
		fTimeDelta *= 1.3f;
	}

	Engine::CGameObject::LateReady_Object();
	Update_Jump(fTimeDelta);



	//레벨 업!
	if (m_pStatusCom->LevelUp())
	{
		Engine::Get_SoundMgr()->StartSound(L"LvUp_Player", Engine::CSoundMgr::SE, false);
		m_abAniCount[ANICOUNT_NOIDLE] = true;
		m_strAniName = "LevelUp";
		auto pLvUp = CLvUp::Create(m_pGraphicDev, this);
		Engine::Add_Object(L"GameLogic", L"LvUp", pLvUp);
	}
	bool bUpGound = true;
	if (true == m_bJump)
	{
		_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(_vec3(0.f, 0.f, 0.f)), !m_bJump, &bUpGound);
		if (false == bUpGound)
			Set_Jump(false);
	}
	HRESULT hr = S_OK;
	if (m_strAniName != "LevelUp")
	{
		hr = Behavior(fTimeDelta);
	}
	m_pMeshAdminCom->Set_AnimationSet(m_strAniName);
	if (false == m_abAniCount[ANICOUNT_NOIDLE])
		m_strAniName = "Idle";

	CMeshObj::Update_Object(fTimeDelta);

	if (m_pMeshAdminCom->Get_AnimationSet() == "Summon")
		m_fTimeDelta_AniPlay = fTimeDelta * 1.8f;
	else if (m_pMeshAdminCom->Get_AnimationSet() == "FireMaxPower")
		m_fTimeDelta_AniPlay = fTimeDelta * 2.f;
	else
		m_fTimeDelta_AniPlay = fTimeDelta;
	
	if (m_eBuildMode != BUILDMODE_NONE)
		m_pTowerPreView->Update_Object(fTimeDelta);
	if (m_eFishingMode != FISHINGMODE_NONE)
		m_pFishingPreView->Update_Object(fTimeDelta);

	return m_iEvent;
}

void CPlayer::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
	if (m_eBuildMode != BUILDMODE_NONE)
		m_pTowerPreView->LateUpdate_Object();
	if (m_eFishingMode != FISHINGMODE_NONE)
		m_pFishingPreView->LateUpdate_Object();
}

void CPlayer::Render_Object(void)
{
	CMeshObj::Render_Object();
	m_pNaviMeshCom->Render_NaviMesh();
}
                                                                                                  
void CPlayer::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_SRC)
	{
		Engine::Get_SoundMgr()->StartSound(L"Hurt_Player", Engine::CSoundMgr::SE, false);
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
		//	m_iEvent = EVENT_DEAD;
		}
		auto pair_Camera = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"ShoulderCamera");
		if (pair_Camera.first != pair_Camera.second)
		{
			auto pCamera = (CShoulderCamera*)pair_Camera.first->second;
			if (nullptr != pCamera)
			{
				pCamera->DoShack();
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Hurt";
			}
		}
	}
}

HRESULT CPlayer::Behavior(const _float & fTimeDelta)
{
	Key_Input(fTimeDelta);
	return S_OK;
}

_vec3 CPlayer::Get_StartBulletPos()
{
	const Engine::D3DXFRAME_DERIVED*		pFrame = m_pMeshAdminCom->Get_FrameByName("b_FingersR");
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	auto matrix_Frame = pFrame->CombinedTransformationMatrix;

	_vec3 vInterval = { -7.f, 5.f, 5.f };
	D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	*((_vec3*)matWorld.m[3]) += vInterval;
	auto matrix = pFrame->CombinedTransformationMatrix * matWorld;

	return (_vec3)matrix.m[3];

	//_matrix matWorld;
	//m_pTransCom->Get_WorldMatrix(&matWorld);
	//_vec3 vInterval = { -5.f, 18.f, 5.f };
	//D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	//return (_vec3)matWorld.m[3] + vInterval;
}

HRESULT CPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"OrcBruiser");
	
	//pComponent = m_pNaviMeshCom = (Engine::CNaviMesh*)(Engine::Get_NaviMesh());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);
	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_SRC);
	//m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_SRC | COLLISION_PUSH_DST);

	return S_OK;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer *	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPlayer::Free(void)
{
	Engine::Safe_Release(m_pTowerPreView);
	Engine::Safe_Release(m_pFishingPreView);
	Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pStatusCom->Recovery_Mp(15.f * fTimeDelta);

	DWORD dwKey = 0;
	if (Engine::KeyDown(KEY_1))
		dwKey |= KEY_1;
	if (Engine::KeyDown(KEY_2))
		dwKey |= KEY_2;
	else if (Engine::KeyDown(KEY_3))
		dwKey |= KEY_3;
	else if (Engine::KeyDown(KEY_4))
		dwKey |= KEY_4;
	else if(Engine::KeyDown(KEY_Q))
		dwKey |= KEY_Q;

	if (m_eMode == Engine::CONTROLTYPE_BASE)
	{
		if (m_abAniCount[ANICOUNT_ATTACK] == false)
		{
			if (m_pStatusCom->m_fMp >= 10.f)
				if (Engine::KeyPressing(KEY_RBUTTON))
					dwKey |= KEY_RBUTTON;
		}
		if (Engine::KeyPressing(KEY_LBUTTON))
			dwKey |= KEY_LBUTTON;
		if (false == m_bJump)
			if (Engine::KeyPressing(KEY_SPACE))
				dwKey |= KEY_SPACE;
	}

	m_pHpBar_Player->Set_DelayTimeKey(&dwKey);

	bool bUpGound = true;
	//if (true == m_bJump)
	//{
	//	_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(_vec3(0.f, 0.f, 0.f)), !m_bJump, &bUpGound);
	//	if (false == bUpGound)
	//		Set_Jump(false);
	//}

	if (Engine::KeyDown(KEY_0))
	{
		auto pair_MarioHouse = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MarioHouse");
		if (pair_MarioHouse.first != pair_MarioHouse.second)
		{
			auto pMarioHouse = (CMarioHouse*)pair_MarioHouse.first->second;
			m_pTransCom->Set_Pos(pMarioHouse->Get_StartBulletPos());
			Seek_NaviIndex();
		}
	}
	if (dwKey & KEY_1 || dwKey & KEY_2 || dwKey & KEY_3) // 포탑 설치
	{
		InitMode();
		auto pMeshAdmin = (Engine::CMeshAdmin*)m_pTowerPreView->Get_Component(L"Com_MeshAdmin", Engine::ID_STATIC);
		if (dwKey & KEY_1)
		{
			m_iTowerType = 0;
			pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"MageBlockade");
		}
		else if (dwKey & KEY_2)
		{
			m_iTowerType = 1;
			pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"MissleTower");
		}
		else if (dwKey & KEY_3)
		{
			m_iTowerType = 2;
			pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"StrikerTower");
		}
	
		auto pair_Camera = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"ShoulderCamera");
		if (pair_Camera.first != pair_Camera.second)
		{
			auto pCamera = (CShoulderCamera*)(pair_Camera.first->second);
			m_eMode = (Engine::CONTROLTYPE_BASE != m_eMode) ? Engine::CONTROLTYPE_BASE : Engine::CONTROLTYPE_BUILD;
			pCamera->Change_Mode(m_eMode);
		}
	}
	if (dwKey & KEY_4) // 낚시 하기
	{
		InitMode();
		auto pair_Camera = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"ShoulderCamera");
		if (pair_Camera.first != pair_Camera.second)
		{
			auto pCamera = (CShoulderCamera*)(pair_Camera.first->second);
			m_eMode = (Engine::CONTROLTYPE_BASE != m_eMode) ? Engine::CONTROLTYPE_BASE : Engine::CONTROLTYPE_FISHING;
			pCamera->Change_Mode(m_eMode);
		}
	}
	if (m_eMode == Engine::CONTROLTYPE_BASE)
	{
		if(nullptr != m_pMouseUI)
			m_pMouseUI->Set_MouseType(CMouseUI::MOUSE_AIM);
		_vec3 vLook, vRight;
		m_pTransCom->Get_Info(Engine::INFO_LOOK, &vLook);
		m_pTransCom->Get_Info(Engine::INFO_RIGHT, &vRight);
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		if (Engine::KeyPressing(KEY_W))
		{
			m_strAniName = "RunForward";
			_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(vLook * fTimeDelta * m_fSpeed), !m_bJump, &bUpGound);
			if (false == bUpGound)
				Set_Jump(false);
			m_pTransCom->Set_Pos(vTemp);
		}

		if (Engine::KeyPressing(KEY_S))
		{
			m_strAniName = "Move_backward";
			_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(-vLook * fTimeDelta * m_fSpeed), !m_bJump, &bUpGound);
			if (false == bUpGound)
				Set_Jump(false);
			m_pTransCom->Set_Pos(vTemp);
		}

		if (Engine::KeyPressing(KEY_A))
		{
			m_strAniName = "Move_left";
			_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(-vRight * fTimeDelta * m_fSpeed), !m_bJump, &bUpGound);
			if (false == bUpGound)
				Set_Jump(false);
			m_pTransCom->Set_Pos(vTemp);
		}

		if (Engine::KeyPressing(KEY_D))
		{
			m_strAniName = "Move_right";
			_vec3 vTemp = m_pNaviMeshCom->MoveOn_NaviMesh(&m_pTransCom->Get_Pos(), &(vRight * fTimeDelta * m_fSpeed), !m_bJump, &bUpGound);
			if (false == bUpGound)
				Set_Jump(false);
			m_pTransCom->Set_Pos(vTemp);
		}

		if (dwKey & KEY_SPACE)
		{
			Set_Jump(true);
		}

		if (dwKey & KEY_Q)
		{
			if (nullptr == m_pEffect_Aura)
			{
				if(m_pStatusCom->m_fMp >= 80.f)
				{
					m_pStatusCom->Damage_Mp(80.f);
					m_pEffect_Aura = Create_Effect_Aura();
					m_abAniCount[ANICOUNT_NOIDLE] = true;
					m_strAniName = "Detonate";
					Engine::Get_SoundMgr()->StartSound(L"Hurt_Aura", Engine::CSoundMgr::SE, false);
					Engine::Get_SoundMgr()->StartSound(L"Full_Aura", Engine::CSoundMgr::SE, false);
				}
			}
			else
				End_Aura();
		}

		if (Engine::KeyPressing(KEY_LBUTTON))
		{
			m_strAniName = "FireMaxPower";
			Engine::RANGEFLOAT tRangeFloat;
			tRangeFloat.fMax = 0.6f;
			tRangeFloat.fMin = 0.5f;
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
			{
				if (false == m_abAniCount[ANICOUNT_ATTACK])
				{
					m_abAniCount[ANICOUNT_ATTACK] = true;
					auto pMultiMap = Engine::Get_ObjectMultimap(L"GameLogic");
					std::for_each(pMultiMap->begin(), pMultiMap->end(), [&](auto _pair)
					{
						if (_pair.second->Get_Team() == Engine::TEAMTYPE_NONE) return;
						if (_pair.second->Get_Team() == m_eTeam) return;
						auto pTrans = (Engine::CTransform*)(_pair.second->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC));
						if (nullptr == pTrans) return;
						auto vPos_Target = pTrans->Get_Pos();
						auto vPos = m_pTransCom->Get_Pos();
						auto fDist = D3DXVec3Length(&(vPos_Target - vPos));
						if (fDist < 3.f)
						{
							_pair.second->BeCollision(COLLISION_BASE_SRC, this);
							Create_Effect_Hit((CMeshObj*)_pair.second);
						}
					});
					Engine::Get_SoundMgr()->StartSound(L"Melee_Attack", Engine::CSoundMgr::SE, false);
				}
			}
		}
		if (Engine::KeyPressing(KEY_RBUTTON))
		{
				m_abAniCount[ANICOUNT_NOIDLE] = true;
				m_strAniName = "Summon";

				//Engine::RANGEFLOAT tRangeFloat;
				//tRangeFloat.fMax = 0.5f;
				//tRangeFloat.fMin = 0.4f;
				//if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
				//{
				//	if (false == m_abAniCount[ANICOUNT_ATTACK])
				//	{
				//		m_abAniCount[ANICOUNT_ATTACK] = true;
				//		//auto pBullet = CBullet::Create(m_pGraphicDev, 0);
				//		auto pBullet = CFireBall::Create(m_pGraphicDev, 0);
				//		pBullet->InitSetting(&m_eTeam, &Get_StartBulletPos(),
				//			nullptr, &m_pTransCom->Get_Radian(),
				//			&m_pStatusCom->m_fAtt);
				//		Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
				//	}
				//}				
		}
		if (m_strAniName == "Summon")
		{
			Engine::RANGEFLOAT tRangeFloat;
			tRangeFloat.fMax = 0.5f;
			tRangeFloat.fMin = 0.4f;
			if (true == m_pMeshAdminCom->Is_AnimationSetEnd(&tRangeFloat))
			{

				if (false == m_abAniCount[ANICOUNT_ATTACK])
				{
					m_abAniCount[ANICOUNT_ATTACK] = true;
					if (m_pStatusCom->Damage_Mp(10.f))
					{
						//auto pBullet = CBullet::Create(m_pGraphicDev, 0);	
						CBullet::BULLET_DESC tBullet;
						m_pTransCom->Get_Info(Engine::INFO::INFO_LOOK, &tBullet.vDir);
						D3DXVec3Normalize(&tBullet.vDir, &tBullet.vDir);
						tBullet.eTeam = m_eTeam;
						tBullet.fAtt = m_pStatusCom->m_fAtt;
						tBullet.vPos = Get_StartBulletPos();
						tBullet.vRadian = m_pTransCom->Get_Radian();

						CBullet* pBullet = nullptr;
						if (nullptr != m_pEffect_Aura)
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
						
					}
				}
			}
		}
		Move_Mouse();
		m_eBuildMode = BUILDMODE_NONE;
	}
	else if(m_eMode == Engine::CONTROLTYPE_BUILD)
	{
		if(nullptr != m_pMouseUI)
			m_pMouseUI->Set_MouseType(CMouseUI::MOUSE_BASE);
		if (m_eBuildMode == BUILDMODE_NONE)
			m_eBuildMode = BUILDMODE_POSITION;
		m_strAniName = "Repair";
		if (Engine::KeyDown(KEY_RBUTTON))
		{
			//m_eBuildMode = BUILDMODE_NONE;
			m_eBuildMode = BUILDMODE_POSITION;
		}
		if (Engine::KeyDown(KEY_LBUTTON))
		{
			if (!(m_eBuildMode == BUILDMODE_POSITION && 2.f == m_pTowerPreView->Get_ShaderType()))
			{
				m_eBuildMode = (BUILDMODE)((m_eBuildMode + 1) % BUILDMODE_END);
				if(m_eBuildMode == BUILDMODE_ROTATE)
					Engine::Get_SoundMgr()->StartSound(L"Put1_Tower", Engine::CSoundMgr::SE, false);
				if (m_eBuildMode == BUILDMODE_NONE)
				{
					Engine::Get_SoundMgr()->StartSound(L"Put2_Tower", Engine::CSoundMgr::SE, false);
					auto pTrans_TowerPreView = (Engine::CTransform*)m_pTowerPreView->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
					auto pTower = CreateTower(pTrans_TowerPreView->Get_Pos(), m_vTowerScale, pTrans_TowerPreView->Get_Radian());
					pTower->Set_Activate(true);
					Engine::Add_Object(L"GameLogic", L"Tower", pTower);
					
					if (m_iTowerType == 0)
						m_pStatusCom->m_iGold -= 10;
					else if (m_iTowerType == 1)
						m_pStatusCom->m_iGold -= 15;
					else
						m_pStatusCom->m_iGold -= 20;
				}
			}
		}
		Put_Tower(TOWERTYPE::TOWERTYPE_BASE, m_eBuildMode);
	}
	else if (m_eMode == Engine::CONTROLTYPE_FISHING)
	{
		if (nullptr != m_pMouseUI)
			m_pMouseUI->Set_MouseType(CMouseUI::MOUSE_BASE);
		if (m_eFishingMode == FISHINGMODE_NONE)
			m_eFishingMode = FISHINGMODE_POSITION;
		Fishing_Mode(fTimeDelta);
	}
}

void CPlayer::Fix_Mouse(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CPlayer::Move_Mouse(bool bFixMouse)
{
	if(bFixMouse)
		Fix_Mouse();
	_matrix		matCamWorld, matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matCamWorld, 0, &matView);
	
	_long	dwMouse = 0;

	if (dwMouse = Engine::Get_DIMouseMove(Engine::DIMS_Y))
	{
		auto fRadianX = m_pTransCom->Get_Radian(Engine::ROT_X);

		//-30 ~ 80 사이를 회전할수있게끔 해야함
		auto fAngle = D3DXToDegree(fRadianX);
		//int iAngle = (int)fAngle % 360;
		//if ((iAngle < -45 && dwMouse > 0) || (iAngle > 25 && dwMouse < 0) || (iAngle >= -45 && iAngle <= 25))
		if ((fAngle < -45 && dwMouse > 0) || (fAngle > 25 && dwMouse < 0) || (fAngle >= -45 && fAngle <= 25))
		{
			m_pTransCom->Rotation(Engine::ROT_X, D3DXToRadian(dwMouse / 30.f));
		}
	}

	if (dwMouse = Engine::Get_DIMouseMove(Engine::DIMS_X))
	{
		m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(dwMouse / 20.f));
	}
}

void CPlayer::Fishing_Mode(const _float & fTimeDelta)
{
	if (Engine::KeyDown(KEY_RBUTTON))
	{
		m_eFishingMode = FISHINGMODE_POSITION;
		//m_eFishingMode = FISHINGMODE_NONE;
	}
	if (Engine::KeyDown(KEY_LBUTTON))
	{
		if (!(m_eFishingMode == FISHINGMODE_POSITION && 2.f == m_pFishingPreView->Get_ShaderType()))
		{
			m_eFishingMode = (FISHINGMODE)((m_eFishingMode + 1) % FISHINGMODE_END);

			if (m_eFishingMode == FISHINGMODE_NONE)
			{
				//CreateTower();
				//auto pTrans_TowerPreView = (Engine::CTransform*)m_pTowerPreView->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
				//auto pTower = CreateTower(pTrans_TowerPreView->Get_Pos(), m_vTowerScale, pTrans_TowerPreView->Get_Radian());
				//pTower->Set_Activate(true);
				//Engine::Add_Object(L"GameLogic", L"Tower", pTower);
			}
		}
	}
	m_strAniName = "Repair";
	switch (m_eFishingMode)
	{
	case CPlayer::FISHINGMODE_NONE:
		m_pFishingPreView->InitMonster();
		break;
	case CPlayer::FISHINGMODE_POSITION:
		m_strAniName = "Heal";
		Put_Fishing(m_eFishingMode);
		break;
	case CPlayer::FISHINGMODE_START:
		m_strAniName = "Summon";
		Put_Fishing(m_eFishingMode);
		break;
	case CPlayer::FISHINGMODE_PLUCK:
		m_strAniName = "Heal";
		Put_Fishing(m_eFishingMode);
		break;
	case CPlayer::FISHINGMODE_END:
		break;
	default:
		break;
	}
}

void CPlayer::Move_BuildMouse(bool bFixMouse)
{
	if (bFixMouse)
		Fix_Mouse();
	if (nullptr == m_pTowerPreView) return;
	auto m_pTrans_Target = (Engine::CTransform*)m_pTowerPreView->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	_vec3 vRadian;
	Engine::DirToRadian(&vRadian, &(m_pTrans_Target->Get_Pos() - m_pTransCom->Get_Pos()));
	m_pTransCom->Set_Radian(_vec3(vRadian.x, vRadian.y, 0.f));
	m_pTrans_Target->Set_Radian(_vec3(0.f, vRadian.y, 0.f));

	//MovePostion///////////////////////////////////////////////////////////
	int iPrice = 0;
	if (m_iTowerType == 0)
		iPrice = 10;
	else if (m_iTowerType == 1)
		iPrice = 15;
	else
		iPrice = 20;

	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);
	auto pvecCell = m_pNaviMeshCom->Get_CellList();
	for (auto pCell : *pvecCell)
	{
		_vec3 vPoint;
		if (m_pCalculatorCom->Collision_Mouse_Tri(g_hWnd, pCell->m_vPoint, &matTemp, &vPoint))
		{
			//std::cout << pCell->m_pObjectInfo->Get_Option() << std::endl;


			if (pCell->m_pObjectInfo->Get_Option() & CELL_NOBUILD || m_pStatusCom->m_iGold < iPrice)
				m_pTowerPreView->Set_ShaderType(2.f);
			else
				m_pTowerPreView->Set_ShaderType(1.f);
			auto pTrans = (Engine::CTransform*)m_pTowerPreView->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
			pTrans->Set_Pos(vPoint);
		}
	}
}

void CPlayer::Rotation_BuildMouse()
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_vec3 vMousePos, vRayPos, vRayDir;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	auto m_pTrans_Target = (Engine::CTransform*)m_pTowerPreView->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	_matrix		matCameraWorld, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCameraWorld);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_vec3 vPos = m_pTrans_Target->Get_Pos();
	D3DXMatrixInverse(&matCameraWorld, nullptr, &matCameraWorld);
	// 뷰스페이스 영역의 마우스로 변환
	vRayDir.x = (ptMouse.x / (ViewPort.Width * 0.5f) - 1.f) /  matProj._11;
	vRayDir.y = (ptMouse.y / (ViewPort.Height * -0.5f) + 1.f) / matProj._22;
	vRayDir.z = 1.f;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matCameraWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matCameraWorld);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	auto fDist = (vPos.y - vRayPos.y) / vRayDir.y;
	vMousePos = vRayPos + vRayDir * fDist;
	_vec3 vTowerDir, vTowerRadian;
	D3DXVec3Normalize(&vTowerDir, &(vMousePos - vPos));
	Engine::DirToRadian(&vTowerRadian, &vTowerDir);
	m_pTrans_Target->Set_Radian(vTowerRadian);
}

HRESULT CPlayer::Put_Tower(TOWERTYPE eType, BUILDMODE eBuildMode)
{
	switch (eType)
	{
	case CPlayer::TOWERTYPE_BASE:
		break;
	default:
		break;
	}
	
	switch (eBuildMode)
	{
	case CPlayer::BUILDMODE_NONE:
		break;
	case CPlayer::BUILDMODE_POSITION:
	{
		Move_BuildMouse(false);
		break;
	}
	case CPlayer::BUILDMODE_ROTATE:
		Rotation_BuildMouse();
		break;
	default:
		break;
	}
	return S_OK;
}

void CPlayer::Move_FishingMouse()
{
	if (nullptr == m_pFishingPreView) return;
	auto m_pTrans_Target = (Engine::CTransform*)m_pFishingPreView->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	_vec3 vRadian;
	Engine::DirToRadian(&vRadian, &(m_vFishingPickPoint - m_pTransCom->Get_Pos()));
	m_pTransCom->Set_Radian(_vec3(vRadian.x, vRadian.y, 0.f));
	m_pTrans_Target->Set_Radian(_vec3(D3DXToRadian(90), vRadian.y - D3DXToRadian(90), 0.f));
	auto vPos = m_pTransCom->Get_Pos();
	vPos.y += 5.f;
	//MovePostion///////////////////////////////////////////////////////////
	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);
	auto pvecCell = m_pNaviMeshCom->Get_CellList();
	for (auto pCell : *pvecCell)
	{
		_vec3 vPoint;
		if (m_pCalculatorCom->Collision_Mouse_Tri(g_hWnd, pCell->m_vPoint, &matTemp, &vPoint))
		{
			//std::cout << pCell->m_pObjectInfo->Get_Option() << std::endl;
			if (pCell->m_pObjectInfo->Get_Option() & CELL_YESFISH)
				m_pFishingPreView->Set_ShaderType(1.f);
			else
				m_pFishingPreView->Set_ShaderType(2.f);
			auto pTrans = (Engine::CTransform*)m_pFishingPreView->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
			pTrans->Set_Pos(vPos);
			m_vFishingPickPoint = vPoint;
		}
	}
}

HRESULT CPlayer::Put_Fishing(FISHINGMODE eFishingMode)
{
	static FISHINGMODE eFishingMode_Before;
	switch (eFishingMode)
	{
	case CPlayer::FISHINGMODE_NONE:
		m_pMiniGame->Set_Activate(false);
		break;
	case CPlayer::FISHINGMODE_POSITION:
		m_pMiniGame->Set_Activate(false);
		Move_FishingMouse();
		break;
	case CPlayer::FISHINGMODE_START:
		if (eFishingMode_Before != eFishingMode)
		{
			m_pFishingPreView->Set_ShaderType(0.f);
			m_pFishingPreView->Set_TargetPos(m_vFishingPickPoint, true);
		}
		if (true == m_pFishingPreView->Get_Arrived() && false == m_pMiniGame->Get_Activate())
		{
			m_pMiniGame->Set_Activate(true);
			m_pMiniGame->Ready_Button();
		}
		if (true == m_pMiniGame->Get_Clear() && true == m_pMiniGame->Get_Activate())
		{
			m_pFishingPreView->Create_Monster(m_pMiniGame->Get_Combo());
			m_eFishingMode = CPlayer::FISHINGMODE_PLUCK;
		}
		break;
	case CPlayer::FISHINGMODE_PLUCK:
		m_pMiniGame->Set_Activate(false);
		if (eFishingMode_Before != eFishingMode)
		{
			m_pFishingPreView->Set_ShaderType(0.f);
			m_pFishingPreView->Set_TargetPos(m_pTransCom->Get_Pos() + _vec3(0.f, 4.f, 0.f));
		}
		if (true == m_pFishingPreView->Get_Arrived())
			m_eFishingMode = eFishingMode = CPlayer::FISHINGMODE_NONE;
		break;
	case CPlayer::FISHINGMODE_END:
		m_pMiniGame->Set_Activate(false);
		break;
	default:
		break;
	}
	eFishingMode_Before = eFishingMode;
	return E_NOTIMPL;
}

CMeshObj * CPlayer::CreateTower(_vec3 vPos, _vec3 vScale, _vec3 vRadian)
{
	auto pTower = CTower::Create(m_pGraphicDev, 0);
	auto pTrans = (Engine::CTransform*)pTower->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
	pTrans->Set_Pos(vPos);
	pTrans->Set_Scale(vScale);
	pTrans->Set_Radian(vRadian);
	auto pMeshAdmin = (Engine::CMeshAdmin*)pTower->Get_Component(L"Com_MeshAdmin", Engine::ID_STATIC);
	if(m_iTowerType == 0)
		pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"MageBlockade");
	else if (m_iTowerType == 1)
		pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"MissleTower");
	else if (m_iTowerType == 2)
		pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"StrikerTower");
	return pTower;
}

CFishing * CPlayer::CreateFishing(_vec3 vPos, _vec3 vScale, _vec3 vRadian)
{
	auto pFishing = CFishing::Create(m_pGraphicDev, 0);
	auto pTrans = (Engine::CTransform*)pFishing->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
	pTrans->Set_Pos(vPos);
	pTrans->Set_Scale(vScale);
	pTrans->Set_Radian(vRadian);
	auto pMeshAdmin = (Engine::CMeshAdmin*)pFishing->Get_Component(L"Com_MeshAdmin", Engine::ID_STATIC);
	pMeshAdmin->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Fishing");
	return pFishing;
}

void CPlayer::InitMode()
{
	m_eFishingMode = FISHINGMODE_NONE;
	m_eBuildMode = BUILDMODE_NONE;
	m_pFishingPreView->InitMonster();
}

void CPlayer::Set_Jump(bool bJump)
{
	if(m_bJump != bJump && false == m_bJump)
		Engine::Get_SoundMgr()->StartSound(L"Jump_Player", Engine::CSoundMgr::SE, false);
	if(false == bJump)
		m_fJumpAccel = 0.f;
	m_bJump = bJump;
}

void CPlayer::Update_Jump(float fTimeDelta)
{
	if (false == m_bJump) return;
	m_pTransCom->Move_Pos(&_vec3(0.f, (6.f - 5.5f * m_fJumpAccel) * fTimeDelta*1.6f, 0.f));
	m_fJumpAccel += 2.f*fTimeDelta;
}


HRESULT CPlayer::Create_Effect_Hit(CMeshObj* pTarget)
{
	CEffect::EFFECT_DESC tEffect_Info;
	tEffect_Info.eType = CEffect::EFFECT_ONCE;
	tEffect_Info.strImageKey = L"Texture_Effect_Hit";
	tEffect_Info.bNoTargetDead = false;
	tEffect_Info.dwColor = D3DCOLOR_ARGB(0, 55, 200, 200);
	auto pEffect = CEffect::Create(m_pGraphicDev, &tEffect_Info);
	pEffect->Get_Transform()->Set_Scale(m_pTransCom->Get_Scale() * 45.f);
	pEffect->Get_Transform()->Set_Pos(pTarget->Get_StartBulletPos());
	//pEffect->Get_Transform()->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));
	Engine::Add_Object(L"GameLogic", L"Effect", pEffect);

	return S_OK;
}

CEffect* CPlayer::Create_Effect_Aura()
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

void CPlayer::End_Aura()
{
	m_pStatusCom->m_fAtt /= 1.2f;
	m_pEffect_Aura->Set_Event(EVENT_DEAD);
	m_pEffect_Aura = nullptr;
	m_fAuraTime = 0.f;
}
