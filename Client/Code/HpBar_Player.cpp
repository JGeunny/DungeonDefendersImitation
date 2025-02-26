#include "stdafx.h"
#include "HpBar_Player.h"

#include "Export_Function.h"

#include "MonsterGate.h"

CHpBar_Player::CHpBar_Player(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurrentImageCount(0)
	, m_fTimeDelta(0)
	, m_fCurHp(0.f)
	, m_fCurMp(0.f)
	, m_fCurMonsterHp(0.f)
	, m_vColorHp(1.f, 1.f, 1.f, 1.f)
	, m_iCurGold(0)
{
	m_eRenderType = Engine::RENDERID::RENDER_UI;
}

CHpBar_Player::~CHpBar_Player(void)
{
}

HRESULT CHpBar_Player::Ready_Object(Engine::CGameObject* pTarget)
{
	//NULL_CHECK_RETURN(pTarget, E_FAIL);
	Set_Target(pTarget);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Ready_SkillInfo();

	m_fViewZ = 999.f;
	return S_OK;
}

_int CHpBar_Player::Update_Object(const _float& fTimeDelta)
{
	if (nullptr == Get_Target())
	{
//		m_iEvent = EVENT_DEAD;
	//	return m_iEvent;
	}
	FollowTarget();
	Update_SkillInfo(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);
	//m_fTimeDelta += fTimeDelta * 0.75f;
	//m_iCurrentImageCount = (int)m_fTimeDelta % m_pTextureCom->Get_MaxCount() - 1;
	//++m_iCurrentImageCount;
	//if (m_pTextureCom->Get_MaxCount() == m_iCurrentImageCount + 1)
	//	m_fTimeDelta = 0.f;
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CHpBar_Player::LateUpdate_Object(void)
{
}

void CHpBar_Player::Render_Object(void)
{
	//BillBoard();
	_matrix matWorld;
	//m_pTransCom->Get_WorldMatrix(&matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	LPD3DXEFFECT	pEffect = nullptr;
	for (int i = 0; i < 3; ++i)
	{
		pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);

		SetUp_ConstantTable(pEffect, i);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
		pEffect->BeginPass(0);					// 0패스의 쉐이더를 적용

		m_pBufferCom->Render_Buffer();
		
		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}

	for (int i = 0; i < m_vecSkillInfo.size(); ++i)
	{
		pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);

		SetUp_ConstantTable_Skill(pEffect, i);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
		pEffect->BeginPass(0);					// 0패스의 쉐이더를 적용

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}

	for (int i = 0; i <= 1; ++i)
	{
		pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);

		SetUp_ConstantTable_EnemyHpBar(pEffect, i);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
		pEffect->BeginPass(0);					// 0패스의 쉐이더를 적용

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
	Render_MonsterHpInfo();
}

void CHpBar_Player::Update_SkillInfo(float fTimeDelta)
{
	for (auto& tSkillInfo : m_vecSkillInfo)
	{
		tSkillInfo.fCurDelayTime -= tSkillInfo.fDelayTimeSpeed*fTimeDelta;
		if (tSkillInfo.fCurDelayTime < 0.f)
			tSkillInfo.fCurDelayTime = 0.f;
	}
}

void CHpBar_Player::Ready_SkillInfo()
{
	m_vecSkillInfo.reserve(10);
	SKILL_DESC tSkillInfo;
	auto fInitX = WINCX - 464.f;
	tSkillInfo.vPos.x = fInitX;
	tSkillInfo.vPos.y = 70.f;
	tSkillInfo.vSize.x = 52.f;
	tSkillInfo.vSize.y = 52.f;
	tSkillInfo.fDelayTimeSpeed = 1.f;

	for (int i = 0; i < 10; ++i)
	{
		if (i == 5)
		{
			tSkillInfo.vPos.y += 63.f;
			tSkillInfo.vPos.x = fInitX;
		}
		m_vecSkillInfo.emplace_back(tSkillInfo);
		tSkillInfo.vPos.x += 67.f;
	}
}

void CHpBar_Player::Set_DelayTimeKey(DWORD * pdwKey)
{
	auto& dwKey = *pdwKey;
	for (int i = 0; i < m_vecSkillInfo.size(); ++i)
	{
		if (m_vecSkillInfo[i].fCurDelayTime > 0.f)
		{
			switch (i)
			{
			case 0:
				if (dwKey & KEY_1)
					dwKey ^= KEY_1;
				break;
			case 1:
				if (dwKey & KEY_2)
					dwKey ^= KEY_2;
				break;
			case 2:
				if (dwKey & KEY_3)
					dwKey ^= KEY_3;
				break;
			case 3:
				if (dwKey & KEY_4)
					dwKey ^= KEY_4;
				break;
			case 4:
				if (dwKey & KEY_SPACE)
					dwKey ^= KEY_SPACE;
				break;
			case 6:
				if (dwKey & KEY_Q)
					dwKey ^= KEY_Q;
				break;
				break;
			case 7:
				if (dwKey & KEY_LBUTTON)
					dwKey ^= KEY_LBUTTON;
				break;
			case 8:
				if (dwKey & KEY_RBUTTON)
					dwKey ^= KEY_RBUTTON;
				break;
			}	
		}
		else
		{
			switch (i)
			{
			case 0:
				if (dwKey & KEY_1)
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
				break;
			case 1:
				if (dwKey & KEY_2)
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
				break;
			case 2:
				if (dwKey & KEY_3)
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
				break;
			case 3:
				if (dwKey & KEY_4)
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
				break;
			case 4:
				if (dwKey & KEY_SPACE)
				{
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
					m_vecSkillInfo[i].fDelayTimeSpeed = 2.f;
				}
				break;
			case 6:
				if (dwKey & KEY_Q)
				{
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
					m_vecSkillInfo[i].fDelayTimeSpeed = 0.5f;
				}
				break;
			case 7:
				if (dwKey & KEY_LBUTTON)
				{
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
					m_vecSkillInfo[i].fDelayTimeSpeed = 3.f;
				}
				break;
			case 8:
				if (dwKey & KEY_RBUTTON)
				{
					m_vecSkillInfo[i].fCurDelayTime = 1.f;
					m_vecSkillInfo[i].fDelayTimeSpeed = 2.f;
				}
				break;
			}
		}
	}
}

void CHpBar_Player::Render_MonsterHpInfo(void)
{
	TCHAR szMonsterHpInfo[MIN_STR];
	ZeroMemory(szMonsterHpInfo, sizeof(szMonsterHpInfo));

	swprintf_s(szMonsterHpInfo, L"%00d / %00d", m_iMonsterCount, m_iMonsterMax);
	Engine::Render_Font(L"Font_Default", szMonsterHpInfo, &_vec2(370.f, 12.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CHpBar_Player::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_PlayerGage"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pTextureCom_Skill = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_Skill"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture_Skill", pComponent);

	pComponent = m_pTextureCom_EnemyHpBar = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_HpBar"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture_EnemyHpBar", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	//m_pTransCom->Set_Scale(_vec3(2.f, 0.3f, 2.f));

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CHpBar_Player::FollowTarget()
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
		return E_FAIL;
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	m_fCurHp = pStatus->m_fHp / pStatus->m_fMaxHp;
	m_vColorHp = _vec4(1.f, 1.f, 1.f, 1.f);

	m_fCurMp = pStatus->m_fMp / pStatus->m_fMaxMp;
	m_fCurExp = pStatus->m_fExp / pStatus->m_fMaxExp;
	m_iCurGold = pStatus->m_iGold;

	m_iMonsterCount = 0;
	m_iMonsterMax = 0;
	auto pair_MonsterGate = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"MonsterGate");
	std::for_each(pair_MonsterGate.first, pair_MonsterGate.second, [&](auto pair) {
		auto pMonsterGate = (CMonsterGate*)pair.second;
		m_iMonsterMax += pMonsterGate->Get_MonsterMax();
		m_iMonsterCount += pMonsterGate->Get_MonsterCount();
	});

	auto pair_Monster = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Monster");
	m_iMonsterCount += std::count_if(pair_Monster.first, pair_Monster.second, [&](auto pair) {
							auto pMonster = (CGameObject*)pair.second;
							return pMonster->Get_Team() == Engine::TEAMTYPE_TEAM2;
						});
	m_iMonsterCount = m_iMonsterMax - m_iMonsterCount;

	m_fCurMonsterHp = (float)m_iMonsterCount / m_iMonsterMax;

	return S_OK;
}

HRESULT CHpBar_Player::SetUp_ConstantTable(LPD3DXEFFECT pEffect, int iHpBar)
{
	pEffect->AddRef();

	float fCurGage = 1.f;

	if (iHpBar == 0)
		fCurGage = m_fCurMp;
	else if (iHpBar == 1)
		fCurGage = m_fCurHp;
	else if (iHpBar == 2)
		fCurGage = m_fCurExp;
	
	_matrix		matWorld, matView, matProj;	
	m_pTransCom->Get_WorldMatrix(&matWorld);

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	
	bool bVertical = true;
	if (iHpBar == 0)
	{
		m_fSizeX = 70.f;
		m_fSizeY = 130.f;
		m_fX = WINCX - m_fSizeX * 0.5f - 90;
		m_fY = m_fSizeY* 0.5f + 70;
		m_vColorHp = _vec4(0.3f, 0.3f, 0.3f, 5.f);
	}
	else if (iHpBar == 1)
	{
		m_fSizeX = 70.f;
		m_fSizeY = 130.f;
		m_fX = WINCX - m_fSizeX * 0.5f - 20;
		m_fY = m_fSizeY* 0.5f + 70;
		m_vColorHp = _vec4(0.3f, 0.3f, 0.3f, 5.f);
	}
	else if (iHpBar == 2)
	{
		m_fSizeX = 470.f;
		m_fSizeY = 35.f;
		m_fX = WINCX - m_fSizeX * 0.5f;
		m_fY = m_fSizeY* 0.5f + 3.f;
		m_vColorHp = _vec4(0.5f, 1.f, 0.5f, 1.f);
		bVertical = false;
	}
	

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);
	pEffect->SetInt("g_iType", 0);
	pEffect->SetFloat("g_fCurHp", fCurGage);

	pEffect->SetFloatArray("g_vColorHp", (float*)&m_vColorHp, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", bVertical);

	m_pTextureCom->Render_Texture(pEffect, "g_BaseTexture", iHpBar);

	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CHpBar_Player::SetUp_ConstantTable_Skill(LPD3DXEFFECT pEffect, int index)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;
	m_pTransCom->Get_WorldMatrix(&matWorld);

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	matWorld._11 = m_vecSkillInfo[index].vSize.x;
	matWorld._22 = m_vecSkillInfo[index].vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = m_vecSkillInfo[index].vPos.x - (WINCX >> 1);
	matWorld._42 = -m_vecSkillInfo[index].vPos.y + (WINCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);

	pEffect->SetInt("g_iType", 1);
	pEffect->SetFloat("g_fCurHp", m_vecSkillInfo[index].fCurDelayTime);

	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", true);

	m_pTextureCom_Skill->Render_Texture(pEffect, "g_BaseTexture", index);

	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CHpBar_Player::SetUp_ConstantTable_EnemyHpBar(LPD3DXEFFECT pEffect, int index)
{
	pEffect->AddRef();

	float fCurGage = 1.f;

	fCurGage = m_fCurMonsterHp;

	_matrix		matWorld, matView;
	m_pTransCom->Get_WorldMatrix(&matWorld);

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	bool bVertical = true;

	m_fSizeX = 400.f;
	m_fSizeY = 50.f;
	m_fX = m_fSizeX * 0.5f + 220;
	m_fY = m_fSizeY* 0.5f + 3.f;
	m_vColorHp = _vec4(0.8f, 0.8f, 0.5f, 1.f);
	bVertical = false;

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);
	pEffect->SetInt("g_iType", 0);
	pEffect->SetFloat("g_fCurHp", fCurGage);

	pEffect->SetFloatArray("g_vColorHp", (float*)&m_vColorHp, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", bVertical);

	m_pTextureCom_EnemyHpBar->Render_Texture(pEffect, "g_BaseTexture", index);

	Safe_Release(pEffect);
	return S_OK;
}

CHpBar_Player * CHpBar_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget)
{
	CHpBar_Player *	pInstance = new CHpBar_Player(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTarget)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CHpBar_Player::Free(void)
{
	Engine::CGameObject::Free();
}

void CHpBar_Player::BillBoard(void)
{
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}
