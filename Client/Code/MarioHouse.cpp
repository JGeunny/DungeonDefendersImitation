#include "stdafx.h"
#include "MarioHouse.h"

#include "Export_Function.h"

#include "Bullet.h"
#include "HpBar_B.h"
#include "Damage.h"
#include "MonsterGate.h"
#include "UI.h"
#include "HpBar_Player.h"
#include "CameraObj.h"
#include "WaveInfo.h"

CMarioHouse::CMarioHouse(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_fSpeed(3.f)
{
	m_bActivate = false;
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CMarioHouse::~CMarioHouse(void)
{
}

HRESULT CMarioHouse::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	
	return S_OK;
}

void CMarioHouse::LateReady_Object(void)
{
	auto pHpBar = CHpBar_B::Create(m_pGraphicDev, this);
	Engine::Add_Object(L"GameLogic", L"HpBar_B", pHpBar);
	CMeshObj::LateReady_Object();

	auto pair_Player = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
	if (pair_Player.first != pair_Player.second)
		Set_Target(pair_Player.first->second);

	ZeroMemory(m_pCameraWave, sizeof(m_pCameraWave));
	auto pair_Camera_Wave1 = Engine::Get_ObjectMultimap(L"GameLogic_Camera")->equal_range(L"Camera_Wave1");
	if (pair_Camera_Wave1.first != pair_Camera_Wave1.second)
		m_pCameraWave[0] = (CCameraObj*)pair_Camera_Wave1.first->second;

	auto pair_Camera_Wave2 = Engine::Get_ObjectMultimap(L"GameLogic_Camera")->equal_range(L"Camera_Wave2");
	if (pair_Camera_Wave2.first != pair_Camera_Wave2.second)
		m_pCameraWave[1] = (CCameraObj*)pair_Camera_Wave2.first->second;

	auto pair_Camera_Wave3 = Engine::Get_ObjectMultimap(L"GameLogic_Camera")->equal_range(L"Camera_Wave3");
	if (pair_Camera_Wave3.first != pair_Camera_Wave3.second)
		m_pCameraWave[2] = (CCameraObj*)pair_Camera_Wave3.first->second;

	auto pair_WaveInfo = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"WaveInfo");
	if (pair_WaveInfo.first != pair_WaveInfo.second)
		m_pWaveInfo = (CWaveInfo*)pair_WaveInfo.first->second;

	if (nullptr != m_pWaveInfo)
		m_pWaveInfo->Ready_UI(m_iLevel, true);
}

_int CMarioHouse::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	HRESULT hr = E_FAIL;
	hr = Behavior(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);

	Update_MonsterInfo();
	return m_iEvent;
}

void CMarioHouse::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CMarioHouse::Render_Object(void)
{
	CMeshObj::Render_Object();

	if (true == m_bActivate_E_Key)
	{
		LPD3DXEFFECT	pEffect = m_pShaderCom_Texture->Get_EffectHandle();
		NULL_CHECK(pEffect);
		SetUp_ConstantTable_Texture(pEffect);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
		pEffect->BeginPass(0);	// 0패스의 쉐이더를 적용

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();
		Safe_Release(pEffect);
	}
}

bool CMarioHouse::EmergenceBoss()
{
	return m_bEmergenceBoss;
}

void CMarioHouse::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_SRC)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			//Engine::Get_SoundMgr()->StartSound(L"DeadBuild", Engine::CSoundMgr::SE, false);
			//m_iEvent = EVENT_DEAD;
		}
		else
		{
			Engine::Get_SoundMgr()->StartSound(L"HitBuild", Engine::CSoundMgr::SE, false);
			CMeshObj::Effect_BeAttacked();
			auto pDamage = CDamage::Create(m_pGraphicDev, this, pStatus_Enemy->m_fAtt);
			Engine::Add_Object(L"GameLogic", L"Damage", pDamage);
			Effect_KnockBack(m_pTransCom->Get_Pos());
		}
	}
}

HRESULT CMarioHouse::Behavior(const _float & fTimeDelta)
{
	Key_Input(fTimeDelta);
	return S_OK;
}

HRESULT CMarioHouse::Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget)
{
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	
	auto pBullet = CBullet::Create(m_pGraphicDev, 0);
	auto pTrans = (Engine::CTransform*)pBullet->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);

	_vec3 vBulletPos = m_pTransCom->Get_Pos();
	vBulletPos.y += 2.f;
	pTrans->Set_Pos(vBulletPos);
	_vec3 vRadian;
	_vec3 vTarget_Pos = pTrans_Target->Get_Pos();
	vTarget_Pos.y += pCollider_Target->Get_Range()->Get_Vertical().y*  pTrans_Target->Get_Scale().y;
	Engine::DirToRadian(&vRadian, &(vTarget_Pos - vBulletPos));
	pTrans->Set_Radian(vRadian);

	m_pTransCom->Set_Radian(_vec3(0.f, vRadian.y, 0.f));

	Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
	
	return S_OK;
}

void CMarioHouse::Key_Input(_float fTimeDelta)
{
	m_bActivate_E_Key = false;
	auto pPlayer = Get_Target();
	if (nullptr != pPlayer)
	{
		if (false == m_bCreateStart || m_iLevel > 3)
		{
			auto pTrans = (Engine::CTransform*)pPlayer->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
			auto fDist = D3DXVec3Length(&(pTrans->Get_Pos() - m_pTransCom->Get_Pos()));
			if (fDist <= 5.f)
				m_bActivate_E_Key = true;
		}
	}
	if (true == m_bActivate_E_Key)
	{
		if (Engine::KeyDown(KEY_E))
		{
			if (m_iLevel > 3)
			{
				m_bEndGame = true;
			}
			else
			{
				Engine::Get_SoundMgr()->StopAll();
				Engine::Get_SoundMgr()->StartSound(L"Activate", Engine::CSoundMgr::ENV, false);
				Engine::Get_SoundMgr()->StartSound(L"MainTheme", Engine::CSoundMgr::BGM, true);

				if (nullptr != m_pWaveInfo)
					m_pWaveInfo->Ready_UI(m_iLevel, false);

				if (3 <= m_iLevel)
					m_bEmergenceBoss = true;
				if (m_iLevel < 4)
				{
					if (nullptr != m_pCameraWave[m_iLevel - 1])
						m_pCameraWave[m_iLevel - 1]->Play_Camera();
				}
				m_bCreateStart = true;
				auto pPair_MonsterGate = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MonsterGate");
				std::for_each(pPair_MonsterGate.first, pPair_MonsterGate.second, [&](auto pair) {
					auto pMonsterGate = (CMonsterGate*)pair.second;
					pMonsterGate->Start(true);
				});

				auto pair_UI = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"UI");
				if (pair_UI.first != pair_UI.second)
				{
					auto pUI = (CUI*)pair_UI.first->second;
					pUI->Set_Combat(true);
				}
			}
		}
	}
}

void CMarioHouse::Update_MonsterInfo()
{
	auto pair_HpBar_Player = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"HpBar_Player");
	if (pair_HpBar_Player.first != pair_HpBar_Player.second)
	{
		auto pHpBar_Player = (CHpBar_Player*)pair_HpBar_Player.first->second;
		auto fClear = pHpBar_Player->Get_CurMonsterHp();
		if (fClear < 1.f)
			return;
	}
	if (m_iLevel >= 3) //레벨 3일 경우 보스가 있으면 나감
	{
		auto pair_Boss = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Boss");
		if (pair_Boss.first != pair_Boss.second)
			return;
	}
	if (m_iLevel >= 4)
		return;
	++m_iLevel;
	Engine::Get_SoundMgr()->StopAll();
	Engine::Get_SoundMgr()->StartSound(L"BuildUp", Engine::CSoundMgr::BGM, false);
	Engine::Get_SoundMgr()->StartSound(L"Win", Engine::CSoundMgr::ENV, false);

	if (m_iLevel <= 3)
	{
		m_bCreateStart = false;
		auto pair_MonsterGate = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MonsterGate");
		std::for_each(pair_MonsterGate.first, pair_MonsterGate.second, [&](auto pair) {
			auto pMonsterGate = (CMonsterGate*)pair.second;
			pMonsterGate->Start(false);
			pMonsterGate->Ready_Level(m_iLevel);
		});
	}
	auto pair_UI = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"UI");
	if (pair_UI.first != pair_UI.second)
	{
		auto pUI = (CUI*)pair_UI.first->second;
		pUI->Set_Combat(false);
	}
	if (nullptr != m_pWaveInfo)
		m_pWaveInfo->Ready_UI(m_iLevel, true);
}

HRESULT CMarioHouse::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"OrcBruiser");
	//m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_DST);
	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_MarioHouse"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Shader
	pComponent = m_pShaderCom_Texture = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ShaderTexture", pComponent);


	m_pTransCom->m_vInterval = _vec3(0.f, 2.f, 0.f);

	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_SRC);

	m_pStatusCom->m_fHp = m_pStatusCom->m_fMaxHp = 1500.f;
	return S_OK;
}

HRESULT CMarioHouse::SetUp_ConstantTable_Texture(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;
	//auto vPos = m_pTransCom->Get_Pos();
	//vPos.y += 1.f;
	//_vec3 vSize = _vec3(1.f, 0.5f, 1.f);
	//D3DXMatrixScaling(&matWorld, vSize.x, vSize.y, vSize.z);
	//*(_vec3*)&matWorld.m[3] = vPos;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matWorld);
	
	_vec2 vPos;
	_vec2 vSize;

	vSize.x = 200.f;
	vSize.y = 80.f;
	vPos.x = (WINCX >> 1) + vSize.x;
	vPos.y = WINCY >> 1;

	matWorld._11 = vSize.x;
	matWorld._22 = vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = vPos.x - (WINCX >> 1);
	matWorld._42 = -vPos.y + (WINCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);
	pEffect->SetFloat("g_fCurHp", 1.f);
	pEffect->SetInt("g_iType", 0);
	pEffect->SetFloatArray("g_vColorHp", (float*)&_vec4(1.f, 1.f, 1.f, 1.f), 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);
	m_pTextureCom->Render_Texture(pEffect, "g_BaseTexture", 0);

	Safe_Release(pEffect);
	return S_OK;
}

_matrix CMarioHouse::BillBoard(_matrix matWorld)
{
	_matrix		matView, matBill;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	matBill = matView;
	*(_vec4*)&matBill.m[3] = _vec4(0.f, 0.f, 0.f, 1.f);
	D3DXMatrixInverse(&matBill, 0, &matBill);
	return matBill * matWorld;
}

CMarioHouse * CMarioHouse::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CMarioHouse *	pInstance = new CMarioHouse(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CMarioHouse::Free(void)
{
	CMeshObj::Free();
}
