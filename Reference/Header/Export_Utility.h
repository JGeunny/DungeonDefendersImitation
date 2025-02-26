#ifndef Export_Utility_h__
#define Export_Utility_h__

#include "Engine_Defines.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Transform.h"
#include "ObjectInfo.h"
#include "Renderer.h"
#include "Camera.h"
#include "Calculator.h"
#include "LightMgr.h"
#include "Collider.h"
#include "PrototypeMgr.h"
#include "Shader.h"
#include "Status.h"
#include "Target_Manager.h"
#include "RenderTarget.h"

BEGIN(Engine)

// Management
// Get
inline CComponent*	Get_Component(const _tchar* pLayerTag,	const _tchar* pObjTag,	const _tchar* pComponentTag, COMPONENTID eID);
inline multimap<std::wstring, CGameObject*>*
					Get_ObjectMultimap(const _tchar* pLayerTag);
// Set
inline HRESULT		SetUp_Scene(CScene* pScene);
// General
inline HRESULT		Add_Object(const _tchar * pLayerTag, const _tchar * pObjTag, CGameObject * pGameObject);
inline HRESULT		Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement);
inline Engine::CGameObject* Find_NearTarget(const _tchar* pszLayer, const _tchar* pszTarget, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist);
inline Engine::CGameObject* Find_NearTarget(const _tchar * pszLayer, Engine::TEAMTYPE eTeam, Engine::CGameObject* pGameObject_MySelf, _float fMaxDist);
inline void*		Get_NaviMesh();
// CollisionMgr
// Get
// Set
// General
inline HRESULT		Ready_CollisionMgr(LPDIRECT3DDEVICE9 pGraphicDev);
inline void			LateUpdate_CollisionMgr(void);
inline void			ReleaseData_CollisionMgr(void);
inline void			Add_CollisionGroup(int iCollisionUsingInfo, CGameObject* pGameObject);
inline void			Del_CollisionGroup(CGameObject* pGameObject);

// Renderer
// Get
inline CRenderer*	Get_Renderer(void);

// Light
// Get
inline const D3DLIGHT9*	Get_LightInfo(const _uint& iIndex = 0);
// General
inline HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, 	const D3DLIGHT9* pLightInfo,	const _uint& iIndex);

// PrototypeMgr
// General
inline HRESULT			Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance);
inline CComponent*		Clone_Prototype(const _tchar* pProtoTag);


// Release Utility
inline void			Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
