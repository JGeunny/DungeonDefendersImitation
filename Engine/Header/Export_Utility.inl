#include "Export_Utility.h"
// Management
// Get
CComponent*			Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	return CManagement::GetInstance()->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}
multimap<std::wstring, CGameObject*>*
					Get_ObjectMultimap(const _tchar* pLayerTag)
{
	return CManagement::GetInstance()->Get_ObjectMultimap(pLayerTag);
}

// Set
HRESULT		SetUp_Scene(CScene* pScene)
{
	return CManagement::GetInstance()->SetUp_Scene(pScene);
}
// General
inline HRESULT Add_Object(const _tchar * pLayerTag, const _tchar * pObjTag, CGameObject * pGameObject)
{
	return CManagement::GetInstance()->Add_Object(pLayerTag, pObjTag, pGameObject);
}

HRESULT	Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement)
{
	CManagement*		pManagement = CManagement::GetInstance();
	
	if (nullptr == pManagement)
		return E_FAIL;

	FAILED_CHECK_RETURN(pManagement->Ready_Management(pGraphicDev), E_FAIL);

	*ppManagement = pManagement;

	return S_OK;
}

Engine::CGameObject* Find_NearTarget(const _tchar* pszLayer, const _tchar* pszTarget, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist)
{
	return CManagement::GetInstance()->Find_NearTarget(pszLayer, pszTarget, pGameObject_MySelf, fMaxDist);
}
Engine::CGameObject * Find_NearTarget(const _tchar * pszLayer, Engine::TEAMTYPE eTeam, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist)
{
	return CManagement::GetInstance()->Find_NearTarget(pszLayer, eTeam, pGameObject_MySelf, fMaxDist);
}
inline void*		Get_NaviMesh()
{
	return CManagement::GetInstance()->Get_NaviMesh();
}

// CollisionMgr
// Get
// Set
// General
inline HRESULT Ready_CollisionMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return CCollisionMgr::GetInstance()->Ready_CollisionMgr(pGraphicDev);
}
inline void LateUpdate_CollisionMgr(void)
{
	CCollisionMgr::GetInstance()->LateUpdate_CollisionMgr();
}
inline void ReleaseData_CollisionMgr(void)
{
	CCollisionMgr::GetInstance()->ReleaseData_CollisionMgr();
}
inline void Add_CollisionGroup(int iCollisionUsingInfo, CGameObject * pGameObject)
{
	CCollisionMgr::GetInstance()->Add_CollisionGroup(iCollisionUsingInfo, pGameObject);
}
inline void Del_CollisionGroup(CGameObject * pGameObject)
{
	CCollisionMgr::GetInstance()->Del_CollisionGroup(pGameObject);
}
// Renderer
// Get
CRenderer*		Get_Renderer(void)
{
	return CRenderer::GetInstance();
}
// Light
// Get
const D3DLIGHT9*		Get_LightInfo(const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Get_LightInfo(iIndex);
}
// General
HRESULT	Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

// PrototypeMgr
// General
HRESULT			Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance)
{
	return CPrototypeMgr::GetInstance()->Ready_Prototype(pProtoTag, pInstance);
}

CComponent*		Clone_Prototype(const _tchar* pProtoTag)
{
	return CPrototypeMgr::GetInstance()->Clone_Prototype(pProtoTag);
}

// Release Utility
void		Release_Utility(void)
{
	CPrototypeMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CLightMgr::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();
}