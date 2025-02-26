#include "stdafx.h"
#include "ElecBall.h"

#include "Export_Function.h"
#include "Effect.h"

CElecBall::CElecBall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
{
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CElecBall::~CElecBall(void)
{
}

HRESULT CElecBall::Ready_Object(BULLET_DESC* pArg)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(0), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_iFlag = iFlag;
	m_fShaderType = 3.f;
	m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	
	if (nullptr != pArg)
		CBullet::Synchronization_BulletData(*pArg);
	return S_OK;
}

void CElecBall::LateReady_Object(void)
{
	CMeshObj::LateReady_Object();

	//m_vBeginPos = m_pTransCom->Get_Pos();

	Ready_Effect();
}

_int CElecBall::Update_Object(const _float& fTimeDelta)
{
	Update_TimeDelta_Target(fTimeDelta);
	m_vTexUV.x += fTimeDelta * 1.f;
	m_vTexUV.y += fTimeDelta * 1.f;
	if (m_vTexUV.x > 1.f)
		m_vTexUV.x = 0.f;
	if (m_vTexUV.y > 1.f)
		m_vTexUV.y = 0.f;
	CGameObject::LateReady_Object();

	//Movement(fTimeDelta);
	CBullet::Movement(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);
	//_matrix matRotate;
	//_vec3 vLook;
	//m_pTransCom->Get_Info(Engine::INFO::INFO_LOOK, &vLook);
	//m_fRotateZ += D3DXToRadian(100.f * fTimeDelta);
	//D3DXMatrixRotationAxis(&matRotate, &vLook, m_fRotateZ);

	//m_pTransCom->Set_ParentMatrix(&matRotate);
	
	auto fDist = D3DXVec3Length(&(m_vBeginPos - m_pTransCom->Get_Pos()));
	if (fDist > 100.f)
		m_iEvent = EVENT_DEAD;
	return m_iEvent;
}

void CElecBall::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CElecBall::Render_Object(void)
{
	CMeshObj::Render_Object();
}

HRESULT CElecBall::Ready_Effect()
{
	float fScale = m_pTransCom->Get_Scale().x / 0.002f;
	CEffect::EFFECT_DESC tEffect_Info;
	tEffect_Info.eType = CEffect::EFFECT_REPEAT;
	//tEffect_Info.strImageKey = L"Texture_TorchElec";
	tEffect_Info.strImageKey = L"Texture_Elect";
	tEffect_Info.bNoTargetDead = true;
	//tEffect_Info.vIntervalPos = _vec3(0.f, 0.5f, 0.f) * fScale;
	auto pEffect = CEffect::Create(m_pGraphicDev, &tEffect_Info);
	pEffect->Set_Target(this);
	pEffect->Get_Transform()->Set_Scale(m_pTransCom->Get_Scale() * 450.f);
	//pEffect->Get_Transform()->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));
	Engine::Add_Object(L"GameLogic", L"Effect", pEffect);

	return S_OK;
}

void CElecBall::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	if (m_iEvent == EVENT_DEAD) return;
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	if (dwUsingCollision & COLLISION_BASE_DST)
	{
		auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
		if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
		{
			m_iEvent = EVENT_DEAD;
			Engine::Get_SoundMgr()->StartSound(L"Bullet_Electball_Dead", Engine::CSoundMgr::SE);
			Create_Effect_Elect();
		}
	}
}

void CElecBall::Movement(const _float& fTimeDelta)
{
	_vec3 vPos, vLook;
	vPos = m_pTransCom->Get_Pos();
	m_pTransCom->Get_Info(Engine::INFO::INFO_LOOK, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);
	//auto fLength = D3DXVec3Length(&m_pTransCom->Get_Scale());
	m_pTransCom->Set_Pos(vPos + vLook * m_fSpeed * fTimeDelta);
}

HRESULT CElecBall::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	//Bolt
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"EnergyBall");
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Bolt");
	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	pComponent = m_pStatusCom = Engine::CStatus::Create(10.f, 1.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);

	m_pTransCom->Set_Scale(_vec3(0.002f, 0.002f, 0.002f));

	return S_OK;
}

HRESULT CElecBall::Create_Effect_Elect()
{
	for (int i = 0; i < 5; ++i)
	{
		CEffect::EFFECT_DESC tEffect_Info;
		tEffect_Info.eType = CEffect::EFFECT_ONCE;
		tEffect_Info.strImageKey = L"Texture_Elect";
		tEffect_Info.bNoTargetDead = false;
		tEffect_Info.fAnimationSpeed = 30.f;
		tEffect_Info.bParabora = true;
		D3DXVec3Normalize(&tEffect_Info.vDir, &_vec3((rand() % 100) - 50, (rand() % 100) - 50, (rand() % 100) - 50));

		auto pEffect = CEffect::Create(m_pGraphicDev, &tEffect_Info);
		pEffect->Get_Transform()->Set_Scale(m_pTransCom->Get_Scale() * 375.f);
		//pEffect->Get_Transform()->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));
		pEffect->Get_Transform()->Set_Pos(m_pTransCom->Get_Pos());
		Engine::Add_Object(L"GameLogic", L"Effect", pEffect);
	}
	return S_OK;
}

CElecBall * CElecBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg)
{
	CElecBall *	pInstance = new CElecBall(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CElecBall::Free(void)
{
	CMeshObj::Free();
}
