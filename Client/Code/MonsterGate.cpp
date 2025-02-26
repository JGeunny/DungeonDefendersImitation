#include "stdafx.h"
#include "MonsterGate.h"
#include "Monster.h"
#include "Archer.h"

#include "Export_Function.h"
#include "Engine_Defines.h"

CMonsterGate::CMonsterGate(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fCreateTime(0.f)
	, m_fCreateTimeMax(5.f)
	, m_iLevel(0)
	, m_iCreateCount(0)
{
	m_eRenderType = Engine::RENDER_UI;
}

CMonsterGate::~CMonsterGate(void)
{
}

HRESULT CMonsterGate::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	Ready_Level(1);
	//m_bStart = true;
	return S_OK;
}

void CMonsterGate::LateReady_Object(void)
{
	//Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
	m_pTransCom->Set_Scale(_vec3(4.f, 3.f, 4.f));
	m_pTransCom->Move_Pos(&_vec3(0.f, 2.f, 0.f));
}

_int CMonsterGate::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);

	if(true == m_bStart)
		Create_Monster(fTimeDelta);
	else
		m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CMonsterGate::LateUpdate_Object(void)
{
}

void CMonsterGate::Render_Object(void)
{
	BillBoard();
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	SetUp_ConstantTable(pEffect, m_pTextureCom, m_iLevel - 1);
	pEffect->AddRef();

	_uint iPassCnt = 0;
	pEffect->Begin(&iPassCnt, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

void CMonsterGate::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

HRESULT CMonsterGate::Ready_Level(int iLevel)
{
	m_bStart = false;
	m_lstCreateList.clear();
	m_iLevel = iLevel;
	if ((m_pObjectInfo->Get_Option() / 10) == 3)// 바텀일 경우
		m_iLevel = (m_iLevel == 3) ? 4 : m_iLevel;
	switch (m_iLevel)
	{
	case 1:
	{
		for (int i = 0; i < 2; ++i)
		{
			MONSTERGATE_DESC tCreateInfo;
			tCreateInfo.iCreateType = 0;
			m_lstCreateList.emplace_back(tCreateInfo);
		}
	}
		break;
	case 2:
	{
		for (int i = 0; i < 1; ++i)
		{
			MONSTERGATE_DESC tCreateInfo;
			tCreateInfo.iCreateType = 0;
			m_lstCreateList.emplace_back(tCreateInfo);
		}
		for (int i = 0; i < 3; ++i)
		{
			MONSTERGATE_DESC tCreateInfo;
			tCreateInfo.iCreateType = 1;
			m_lstCreateList.emplace_back(tCreateInfo);
		}
	}
		break;
	case 3:
	{
		{
			for (int i = 0; i < 3; ++i)
			{
				MONSTERGATE_DESC tCreateInfo;
				tCreateInfo.iCreateType = 2;//오크
				m_lstCreateList.emplace_back(tCreateInfo);
			}
			for (int i = 0; i < 2; ++i)
			{
				MONSTERGATE_DESC tCreateInfo;
				tCreateInfo.iCreateType = 1;//아처
				m_lstCreateList.emplace_back(tCreateInfo);
			}
			
		}
	}
		break;
	case 4:
	{
		for (int i = 0; i < 3; ++i)
		{
			MONSTERGATE_DESC tCreateInfo;
			tCreateInfo.iCreateType = 0;
			m_lstCreateList.emplace_back(tCreateInfo);
		}
		for (int i = 0; i < 2; ++i)
		{
			MONSTERGATE_DESC tCreateInfo;
			tCreateInfo.iCreateType = 2;//오크
			m_lstCreateList.emplace_back(tCreateInfo);
		}
	}
	break;
	default:
		break;
	}

	m_iMonsterMax = m_lstCreateList.size();
	
	return E_NOTIMPL;
}

_bool CMonsterGate::Is_Create(float fTimeDelta)
{
	//_bool bCreate = (m_fCreateTime == 0.f);
	m_fCreateTime += fTimeDelta;
	_bool bCreate = (m_fCreateTime > m_fCreateTimeMax);
	if(bCreate)
		m_fCreateTime = 0.f;
	return bCreate;
}

HRESULT CMonsterGate::Create_Monster(float fTimeDelta)
{
	if (m_lstCreateList.size() == 0) return E_FAIL;
	if (false == Is_Create(fTimeDelta))
		return E_FAIL;
	
	auto pair_Monster = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Monster");
	auto iCount = std::count_if(pair_Monster.first, pair_Monster.second, [&](auto pair){
		return pair.second->Get_Team() == Engine::TEAMTYPE_TEAM2;
	});
	if (iCount >= 10)
		return E_FAIL;

	auto iter = m_lstCreateList.begin();

	CMonster* pMonster = nullptr;
	switch ((*iter).iCreateType)
	{
	case 0:
		pMonster = CMonster::Create(m_pGraphicDev, 1);
		break;
	case 1:
		pMonster = CArcher::Create(m_pGraphicDev, 0);
		break;
	case 2:
		pMonster = CMonster::Create(m_pGraphicDev, 0);
		break;
	}

	if (nullptr != pMonster)
	{
		Engine::Add_Object(L"GameLogic", L"Monster", pMonster);
		auto pTrans = (Engine::CTransform*)pMonster->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		pTrans->Set_Pos(m_pTransCom->Get_Pos());
		pMonster->Set_Direction(Get_Direction(m_pObjectInfo->Get_Option()));
	}
	m_lstCreateList.erase(iter);
	return S_OK;
}

HRESULT CMonsterGate::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// ObejctKey
	pComponent = m_pObjectInfo = Engine::CObjectInfo::Create(L"MonsterGate");
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ObjectInfo", pComponent);

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_MonsterGate"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CMonsterGate::SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture* pTexture, int iTextureIndex)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fCurHp", 1.f);
	pEffect->SetInt("g_iType", 0);
	//pEffect->SetFloatArray("g_vColorHp", (float*)&m_vColorHp, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);
	pTexture->Render_Texture(pEffect, "g_BaseTexture", iTextureIndex);

	Safe_Release(pEffect);
	return S_OK;
}

void CMonsterGate::BillBoard(void)
{
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);
	matBill = matView;
	*((_vec4*)&matBill.m[3]) = _vec4(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}

CMonsterGate * CMonsterGate::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CMonsterGate *	pInstance = new CMonsterGate(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CMonsterGate::Free(void)
{
	Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}
