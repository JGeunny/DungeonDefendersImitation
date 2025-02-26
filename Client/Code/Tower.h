#ifndef Tower_h__
#define Tower_h__

#include "Defines.h"
#include "MeshObj.h"

BEGIN(Engine)

class CObjectInfo;
class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

class CMesh;
class CMeshAdmin;
class CStatus;

END

class CTower : public CMeshObj
{
private:
	explicit CTower(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTower(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	HRESULT			Behavior(const _float& fTimeDelta);
	HRESULT			Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget);
private:
	HRESULT			Add_Component(void);

private:
	Engine::CStatus*	m_pStatusCom = nullptr;
	_float			m_fSpeed;
	
public:
	static CTower*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void) override;


};

#endif // Tower_h__
