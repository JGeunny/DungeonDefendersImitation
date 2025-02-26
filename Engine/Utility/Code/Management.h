#ifndef Management_h__
#define Management_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Scene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement(void);
	virtual ~CManagement(void);

public:
	CComponent*	Get_Component(const _tchar* pLayerTag,
								const _tchar* pObjTag,
								const _tchar* pComponentTag,
								COMPONENTID eID);
	multimap<std::wstring, CGameObject*>*
				Get_ObjectMultimap(const _tchar* pLayerTag);
	HRESULT		Add_Object(const _tchar* pLayerTag,
								const _tchar* pObjTag,
								CGameObject* pGameObject);
	Engine::CGameObject*	Find_NearTarget(const _tchar* pszLayer, 
											const _tchar* pszTarget, 
											Engine::CGameObject* pGameObject_MySelf,
											_float fMaxDist);

	Engine::CGameObject*	Find_NearTarget(const _tchar* pszLayer,
											Engine::TEAMTYPE eTeam,
											Engine::CGameObject* pGameObject_MySelf,
											_float fMaxDist);

	void*					Get_NaviMesh() { 
							if (nullptr == m_pScene)
								return nullptr;
							return m_pScene->Get_NaviMesh(); }
public:
	HRESULT		SetUp_Scene(CScene* pScene);
	HRESULT		Ready_Management(LPDIRECT3DDEVICE9 pGraphicDev);
	_int		Update_Scene(const _float& fTimeDelta);
	void		LateUpdate_Scene(void);
	void		Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CScene*			m_pScene = nullptr;

public:
	virtual void	Free(void);
};

END
#endif // Management_h__
