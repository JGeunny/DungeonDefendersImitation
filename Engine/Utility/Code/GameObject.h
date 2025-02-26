#ifndef GameObject_h__
#define GameObject_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit			CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual				~CGameObject(void);

public:
	CComponent*			Find_Component(const _tchar* pComponentTag, COMPONENTID eID);
	CComponent*			Get_Component(const _tchar* pComponentTag, COMPONENTID eID);
	_float				Get_ViewZ(void) { return m_fViewZ; }

public:
	virtual		HRESULT	Ready_Object(void) { return S_OK; }
	virtual		void	LateReady_Object(void);
	virtual		_int	Update_Object(const _float& fTimeDelta);
	virtual		void	LateUpdate_Object(void) PURE;
	virtual		void	Render_Object(void);
	virtual		void	BeCollision(_ulong dwUsingCollision, CGameObject* pGameObject) {}

	virtual		HRESULT Set_NaviPos(_vec3* pvTargetDir) { return E_FAIL; }

protected:
	void				Compare_ViewZ(const _vec3* pPos);

public:
	const _int			Get_Event() const { return m_iEvent; }
	void				Set_Event(_int iEvent) { m_iEvent = iEvent; }

	const _bool			Get_Selected() const { return m_bSelected; }
	void				Set_Selected(_bool bSelected) { m_bSelected = bSelected; }

	void				Set_Target(CGameObject* pTarget, bool bTargetRef = true);
	CGameObject*		Get_Target();

	const _bool			Get_Activate() const { return m_bActivate; }
	void				Set_Activate(_bool bActivate) { m_bActivate = bActivate; }

	Engine::TEAMTYPE	Get_Team() { return m_eTeam; }
	void				Set_Team(Engine::TEAMTYPE eTeam) { m_eTeam = eTeam; }

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev = nullptr;
	map<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	_float								m_fViewZ = 0.f;

protected:
	Engine::RENDERID	m_eRenderType;
	_int				m_iEvent;
	_bool				m_bLateReady;
	_bool				m_bSelected;
	_bool				m_bActivate;
	_float				m_fTimeDelta;
	Engine::TEAMTYPE	m_eTeam;
	Engine::COLLTYPE	m_eCollType;

private:
	CGameObject*		m_pTarget;
	bool				m_bTargetRef = true;
public:
	virtual void Free(void);
};

END
#endif // GameObject_h__
