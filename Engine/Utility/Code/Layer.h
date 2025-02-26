#ifndef Layer_h__
#define Layer_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer(void);
	virtual ~CLayer(void);

public:
	CComponent*		Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);
	multimap<std::wstring, CGameObject*>* 
					Get_ObjectMultimap();
	HRESULT			Add_Object(const _tchar* pObjTag, CGameObject* pGameObject);

public:
	HRESULT			Ready_Layer(void);
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(void);

	multimap<std::wstring, CGameObject*>*		Get_ObjectMap() { return &m_mapObject; }
	void			Get_ObjectList(std::list<CGameObject*>* plstObject);
private:
	multimap<std::wstring, CGameObject*>		m_mapObject;

public:
	static	CLayer*	Create(void);
	virtual	void	Free(void);
};

END
#endif // Layer_h__
