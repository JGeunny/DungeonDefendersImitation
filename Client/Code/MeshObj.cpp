#include "stdafx.h"
#include "MeshObj.h"

#include "Export_Function.h"

CMeshObj::CMeshObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_strAniName("")
	, m_fTimeDelta_AniPlay(0.f)
	, m_fShaderType_Buffer(-1.f)
	, m_bKnockBack_UpGround(false)
{
	ZeroMemory(m_abAniCount, sizeof(bool) * ANICOUNT_END);

	//m_eRenderType = Engine::RENDERID::RENDER_ALPHA;
	m_eRenderType = Engine::RENDERID::RENDER_NONALPHA;
}

CMeshObj::~CMeshObj(void)
{
}

HRESULT CMeshObj::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	return S_OK;
}

void CMeshObj::LateReady_Object(void)
{
	Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CMeshObj::Update_Object(const _float& fTimeDelta)
{
	if (m_fHurtTime < 1.f)
		m_fHurtTime += fTimeDelta;
	Effect_Update(fTimeDelta);
	Init_AniCount();
	m_fTimeDelta_AniPlay = fTimeDelta;
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CMeshObj::LateUpdate_Object(void)
{
}

void CMeshObj::Render_Object(void)
{
	m_pMeshAdminCom->Play_Animation(m_fTimeDelta_AniPlay, &m_iAniCount);
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	SetUp_ConstantTable(pEffect);
	pEffect->AddRef();

	_uint iPassCnt = 0;
	pEffect->Begin(&iPassCnt, 0);
	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	_matrix		matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pColliderCom->Set_CollType(m_eCollType);
	m_pColliderCom->Render_Collider(&matWorld);
}

void CMeshObj::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

void CMeshObj::Effect_Update(float fTimeDelta)
{
	Effect_BeAttacked_Update(fTimeDelta);
	Effect_KnockBack_Update(fTimeDelta);
}

void CMeshObj::Effect_BeAttacked_Update(float fTimeDelta)
{
	if (m_fShaderType_Buffer <= -1.f) return;

	m_fShaderType_BufferTime -= fTimeDelta;
	if (m_fShaderType_BufferTime <= 0.f)
	{
		m_fShaderType = m_fShaderType_Buffer;
		m_fShaderType_Buffer = -1.f;
	}
}

void CMeshObj::Effect_KnockBack_Update(float fTimeDelta)
{
	if (false == m_bKnockBack_UpGround) return;
	auto vPos = m_pTransCom->Get_Pos();
	_vec3 vDir = m_vKnockBack_Dir;

	vDir *= m_vKnockBack_Power * fTimeDelta;
	vDir.y += (3.f - 4.5f * m_fJumpAccel) * fTimeDelta*0.8f;
	m_fJumpAccel += 2.f*fTimeDelta;
	Set_NaviPos(&vDir, false, &m_bKnockBack_UpGround);
}

void CMeshObj::Effect_BeAttacked()
{
	m_fShaderType_BufferTime = 0.5f;
	if (m_fShaderType_Buffer > -1.f) return;
	m_fShaderType_Buffer = m_fShaderType;
	m_fShaderType = 2;// 빨간색으로 변하기
}

void CMeshObj::Effect_KnockBack(_vec3 vTargetPos, float fPower)
{
	if (true == m_bKnockBack_UpGround) return;
	auto vPos = m_pTransCom->Get_Pos();
	D3DXVec3Normalize(&m_vKnockBack_Dir, &(vPos - vTargetPos));
	m_vKnockBack_Power = fPower;
	m_fJumpAccel = 0.f;
	m_bKnockBack_UpGround = true;
}

HRESULT CMeshObj::Init_AniCount()
{
	if (m_iAniCount == m_iAniCount_Before)
		return E_FAIL;
	m_iAniCount_Before = m_iAniCount;
	ZeroMemory(m_abAniCount, sizeof(bool) * ANICOUNT_END);
	return S_OK;
}

HRESULT CMeshObj::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// ObejctKey
	pComponent = m_pObjectInfo = Engine::CObjectInfo::Create(L"MeshObj");
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ObjectInfo", pComponent);

	// Collider
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	//m_pColliderCom->Set_UsingCollision(COLLISION_PUSH_SRC | COLLISION_PUSH_DST);

	// Mesh
	pComponent = m_pMeshAdminCom = Engine::CMeshAdmin::Create(&m_pMeshCom, &m_pColliderCom);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_MeshAdmin", pComponent);
	m_pMeshAdminCom->Change_Mesh(RESOURCE_STATIC, L"Mesh_Default");

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
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CMeshObj::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//const D3DLIGHT9*		pLightInfo = Engine::Get_LightInfo();
	//NULL_CHECK_RETURN(pLightInfo, E_FAIL);	

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetValue("g_vTexUV", m_vTexUV, sizeof(_vec2));
	
	D3DMATERIAL9			tMtrlInfo;
	ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrlInfo.Power = 10.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	pEffect->SetFloat("g_fPower", tMtrlInfo.Power);
	pEffect->SetFloat("g_fType", m_fShaderType);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	Safe_Release(pEffect);

	return S_OK;
}

CMeshObj * CMeshObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CMeshObj *	pInstance = new CMeshObj(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CMeshObj::Free(void)
{
	Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}

HRESULT CMeshObj::Set_NaviPos(_vec3* pvTargetDir, bool bComputtingHeight, bool* pbUpGround)
{
	auto pCom1 = Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCom2 = Get_Component(L"Com_NaviMesh", Engine::COMPONENTID::ID_STATIC);
	if (nullptr == pCom1 || nullptr == pCom2)
		return E_FAIL;

	auto pTrans = (Engine::CTransform*)pCom1;
	auto pNavi = (Engine::CNaviMesh*)pCom2;

	auto vPos = pTrans->Get_Pos();
	//auto dwIndex = pNavi->Get_Index(pTrans->Get_Pos());
	//pNavi->Set_Index(dwIndex);
	auto vPos_Navi = pNavi->MoveOn_NaviMesh(&vPos, pvTargetDir, bComputtingHeight, pbUpGround);
	pTrans->Set_Pos(vPos_Navi);
	return S_OK;
}

HRESULT CMeshObj::Seek_NaviIndex()
{
	auto pCom1 = Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCom2 = Get_Component(L"Com_NaviMesh", Engine::COMPONENTID::ID_STATIC);
	if (nullptr == pCom1 || nullptr == pCom2)
		return E_FAIL;
	auto pTrans = (Engine::CTransform*)pCom1;
	auto pNavi = (Engine::CNaviMesh*)pCom2;

	auto vPos = pTrans->Get_Pos();
	auto dwIndex = pNavi->Get_Index(pTrans->Get_Pos());
	pNavi->Set_Index(dwIndex);
	auto vPos_Navi = pNavi->MoveOn_NaviMesh(&vPos, &_vec3(0.f, 0.f, 0.f));
	pTrans->Set_Pos(vPos_Navi);
	return S_OK;
}

_vec3 CMeshObj::Get_StartBulletPos()
{
	_matrix matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	_vec3 vInterval = { 0.f, 13.f, 0.f };
	D3DXVec3TransformNormal(&vInterval, &vInterval, &matWorld);
	return (_vec3)matWorld.m[3] + vInterval;
}
