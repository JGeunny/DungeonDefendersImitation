#ifndef Scene_h__
#define Scene_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit					CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual						~CScene(void);

public:
	CComponent*					Get_Component(const _tchar* pLayerTag,
												const _tchar* pObjTag,
												const _tchar* pComponentTag,
												COMPONENTID eID);

	multimap<std::wstring, CGameObject*>*
								Get_ObjectMultimap(const _tchar* pLayerTag);
	HRESULT						Add_Object(const _tchar* pLayerTag,
											const _tchar* pObjTag,
											CGameObject* pGameObject);

	map<const std::wstring, CLayer*>* Get_LayerMap() { return &m_mapLayer; }
	virtual void*				Get_NaviMesh() { return nullptr; }

public:
	virtual HRESULT				Ready_Scene(void);
	virtual _int				Update_Scene(const _float& fTimeDelta);
	virtual void				LateUpdate_Scene(void);
	virtual void				Render_Scene(void) PURE;
	 
protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	map<const std::wstring, CLayer*> m_mapLayer;

public:
	virtual void Free(void);
};
END
#endif // Scene_h__
