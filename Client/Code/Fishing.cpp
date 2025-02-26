#include "stdafx.h"
#include "Fishing.h"
#include "Player.h"
#include "Monster.h"
#include "Archer.h"
#include "Export_Function.h"

CFishing::CFishing(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMeshObj(pGraphicDev)
	, m_fSpeed(3.f)
	, m_vShackLine(0.f, 0.f, 0.f)
	, m_vTargetPos(0.f, 0.f, 0.f)
	, m_bTargetPos(false)
	, m_bShackDir(true)
	, m_pMonster(nullptr)
	, m_bCreateMonster(false)
	, m_bArrived(true)
{
	m_bActivate = false;
	m_eTeam = Engine::TEAMTYPE_TEAM1;
}

CFishing::~CFishing(void)
{
}

HRESULT CFishing::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Ready_Object(iFlag), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	_vec3 vPoint = { 0.f , 0.f, 0.f };
	Engine::CCurve* pLine = nullptr;
	Add_Node(&pLine, vPoint);
	Add_Node(&pLine, vPoint);
	Add_Node(&pLine, vPoint);
	Add_Node(&pLine, vPoint);
	Add_Node(&pLine, vPoint);
	return S_OK;
}

void CFishing::LateReady_Object(void)
{
	CMeshObj::LateReady_Object();
	m_eCollType = Engine::COLLTYPE::COL_SELECT;
}

_int CFishing::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	HRESULT hr = E_FAIL;

	//if(m_bActivate)
		hr = Behavior(fTimeDelta);
	CMeshObj::Update_Object(fTimeDelta);

	//if (EVENT_DEAD == m_iEvent)
	//{
	//	if (SUCCEEDED(hr))
	//		return EVENT_DEFAULT;
	//	else
	//		return m_iEvent;
	//}
	return m_iEvent;
}

void CFishing::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CFishing::Render_Object(void)
{
	CMeshObj::Render_Object();
	for (auto& pNode : m_vecLine)
		pNode->Render_Curve();
		//pNode->Render_Curve(&m_pTransCom->m_matWorld);
}

void CFishing::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
	//if (dwUsingCollision & COLLISION_BASE_SRC)
	//{
	//	auto pStatus_Enemy = (Engine::CStatus*)pGameObject->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	//	if (false == m_pStatusCom->Damage(pStatus_Enemy->m_fAtt))
	//		m_iEvent = EVENT_DEAD;
	//}
}

HRESULT CFishing::Behavior(const _float & fTimeDelta)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
	{
		return E_FAIL;
	}
	auto pPlayer = (CPlayer*)pTarget;
	
	if (nullptr != m_pMonster)
	{
		auto pTrans = (Engine::CTransform*)m_pMonster->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		pTrans->Set_Pos(m_pTransCom->Get_Pos());
	}
	if (true == m_bTargetPos)
	{
		auto vDir = m_vTargetPos - m_pTransCom->Get_Pos();
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransCom->Move_Pos(&(vDir*fTimeDelta * 10.f));
		auto fDist = D3DXVec3Length(&(m_pTransCom->Get_Pos() - m_vTargetPos));
		if (fDist < 2.f)
		{
			m_bArrived = true;
			m_pTransCom->Set_Pos(m_vTargetPos);
			m_bTargetPos = false;
			m_pTransCom->Rotation(&_vec3(0.f, D3DXToRadian(180), 0.f));
			//if (true == m_bCreateMonster)
			//{
			//	m_bCreateMonster = false;
			//	m_pMonster = CMonster::Create(m_pGraphicDev, 0);
			//	m_pMonster->Set_Team(Engine::TEAMTYPE::TEAMTYPE_TEAM1);
			//	Engine::Add_Object(L"GameLogic", L"Monster", m_pMonster);
			//}
			//else
			//{
				if (nullptr != m_pMonster)
				{
					auto pTrans = (Engine::CTransform*)m_pMonster->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
					pTrans->Set_Pos(m_pTransCom->Get_Pos());
					m_pMonster->CMeshObj::Seek_NaviIndex();
					m_pMonster = nullptr;
				}
			//}
			//m_pMonster = nullptr;
		}
	}

	m_vShackLine.x += (m_bShackDir) ? fTimeDelta : -fTimeDelta;
	if (m_vShackLine.x > 0.2f)
		m_bShackDir = false;
	if (m_vShackLine.x < -0.2f)
		m_bShackDir = true;

	//m_pTransCom->Move_Pos(&m_vShackLine);
	auto vPos_End = m_pTransCom->Get_Pos();
	auto vPos_Start = pPlayer->Get_StartBulletPos();
	Set_CenterPoint(vPos_Start, vPos_End);

	return S_OK;
}

HRESULT CFishing::Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget)
{
	//auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	//auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	//
	//auto pBullet = CBullet::Create(m_pGraphicDev, 0);
	//Engine::CComponent* pComponent = nullptr;
	//pComponent = pBullet->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	//if (nullptr != pComponent)
	//{
	//	_vec3 vBulletPos = m_pTransCom->Get_Pos();
	//	vBulletPos.y += 2.f;
	//	auto pTrans = (Engine::CTransform*)pComponent;
	//	pTrans->Set_Scale(_vec3(0.001f, 0.001f, 0.001f));
	//	pTrans->Set_Pos(vBulletPos);
	//	_vec3 vRadian;
	//	_vec3 vTarget_Pos = pTrans_Target->Get_Pos();
	//	vTarget_Pos.y += pCollider_Target->Get_Range()->Get_Vertical().y*  pTrans_Target->Get_Scale().y;
	//	Engine::DirToRadian(&vRadian, &(vTarget_Pos - vBulletPos));
	//	pTrans->Set_Radian(vRadian);
	//	m_pTransCom->Set_Radian(_vec3(0.f, vRadian.y, 0.f));
	//	//std::cout << vPos.x << "|" << vPos.y << "|" << vPos.z << std::endl;

	//	Engine::Add_Object(L"GameLogic", L"Bullet", pBullet);
	//}
	return S_OK;
}

void CFishing::Set_TargetPos(_vec3 vTargetPos, _bool bCreateMonster)
{
	m_bTargetPos = true;
	m_bArrived = false;
	m_bCreateMonster = bCreateMonster;
	m_vTargetPos = vTargetPos;
}

HRESULT CFishing::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	//m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"OrcBruiser");
	//m_pColliderCom->Add_UsingCollision(COLLISION_PUSH_DST);
	pComponent = m_pStatusCom = Engine::CStatus::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Status", pComponent);
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"Fishing");
	m_pTransCom->m_vInterval = _vec3(0.f, 100.f, 0.f);
	return S_OK;
}

HRESULT CFishing::Add_Node(Engine::CCurve ** ppNode, _vec3 vPoint)
{
	if (m_vecLine.size() > 0)
	{
		*ppNode = m_vecLine.back()->Clone(vPoint);
		(*ppNode)->AddRef();
	}
	else
		*ppNode = Engine::CCurve::Create(m_pGraphicDev, Engine::CAMERATYPE::CAMERATYPE_EYE, &vPoint);

	m_vecLine.emplace_back(*ppNode);
	return S_OK;
}

HRESULT CFishing::Set_CenterPoint(_vec3 vStartPoint, _vec3 vEndPoint)
{
	auto vPos_Vecter = (vEndPoint - vStartPoint) * 0.1f;
	auto fRand = (rand() % 10) * 0.5f;

	m_vecLine[0]->Set_Point(vStartPoint);
	auto vPos_Temp = vStartPoint + vPos_Vecter * 2.5f + m_vShackLine * 2.f;
	m_vecLine[1]->Set_Point(vPos_Temp);
	vPos_Temp = vStartPoint + vPos_Vecter * 5.f - m_vShackLine * 0.5f;
	m_vecLine[2]->Set_Point(vPos_Temp);
	vPos_Temp = vStartPoint + vPos_Vecter * 7.5f + m_vShackLine;
	m_vecLine[3]->Set_Point(vPos_Temp);
	vPos_Temp = vEndPoint - m_vShackLine * 0.1f;
	m_vecLine[4]->Set_Point(vPos_Temp);
	m_pTransCom->Set_Pos(vPos_Temp);
	return S_OK;
}

void CFishing::InitMonster()
{
	if (nullptr != m_pMonster)
	{
		m_pMonster->Set_Event(EVENT_DEAD);
		m_pMonster = nullptr;
	}
}

HRESULT CFishing::Create_Monster(int _iCombo)
{
	if (_iCombo < 1)
		return E_FAIL;
	if ((rand() % 2) == 1)
		m_pMonster = CArcher::Create(m_pGraphicDev, 0);
	else
		m_pMonster = CMonster::Create(m_pGraphicDev, 0);
	m_pMonster->Set_Team(Engine::TEAMTYPE::TEAMTYPE_TEAM1);
	
	auto iCombo = _iCombo - 50;
	auto pTrans = (Engine::CTransform*)m_pMonster->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	if (nullptr != pTrans)
	{
		auto vScale = pTrans->Get_Scale();
		vScale +=  _vec3(iCombo* vScale.x * 0.01f, iCombo * vScale.y * 0.01f, iCombo * vScale.z* 0.01f);
		pTrans->Set_Scale(vScale);
	}
	auto pStatus = (Engine::CStatus*)m_pMonster->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	if (nullptr != pStatus)
	{
		pStatus->m_fAtt += _iCombo * 10;
		pStatus->m_fMaxHp += _iCombo * 20;
		pStatus->Recovery(pStatus->m_fMaxHp);
	}
	Engine::Add_Object(L"GameLogic", L"Monster", m_pMonster);
	return S_OK;
}

CFishing * CFishing::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CFishing *	pInstance = new CFishing(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CFishing::Free(void)
{
	for_each(m_vecLine.begin(), m_vecLine.end(), Engine::Safe_Release<Engine::CCurve*>);
	CMeshObj::Free();
}
