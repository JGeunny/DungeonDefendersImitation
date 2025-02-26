#ifndef PickPoint_h__
#define PickPoint_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CObjectInfo;
class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

class CMesh;
class CMeshAdmin;

END

class CPickPoint : public Engine::CGameObject
{
private:
	explicit CPickPoint(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPickPoint(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

private:
	HRESULT			Add_Component(void);

private:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag;

public:
	static CPickPoint*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // PickPoint_h__
