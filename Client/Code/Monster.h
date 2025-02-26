#ifndef Monster_h__
#define Monster_h__

#include "Defines.h"
#include "MeshObj.h"


class CDirectionObj;
BEGIN(Engine)

class CObjectInfo;
class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

class CMesh;
class CMeshAdmin;
class CNaviMesh;
class CStatus;

END

class CMonster : public CMeshObj
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	virtual HRESULT	Behavior(const _float& fTimeDelta);
	virtual HRESULT	Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget);
	virtual HRESULT	Movement_GuidLine(const _float& fTimeDelta);
	virtual HRESULT	Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget);

	virtual void	Set_Direction(std::wstring strDirection) { m_strDirection = strDirection; }

protected:
	virtual HRESULT Create_Gold(Engine::CGameObject* pTarget);
private:
	HRESULT			Add_Component(void);

protected:
	CDirectionObj*		m_pDirectionObj = nullptr;
	_int				m_iDirectIdx	= 0;
	Engine::CNaviMesh*	m_pNaviMeshCom = nullptr;
	Engine::CStatus*	m_pStatusCom = nullptr;
	_float				m_fSpeed;

	std::wstring		m_strDirection = L"";

	int					m_iCountGold = 12;

public:
	static CMonster*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);


};

#endif // Monster_h__
