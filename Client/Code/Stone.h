#ifndef Stone_h__
#define Stone_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)


class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

END

class CStone : public Engine::CGameObject
{
private:
	explicit CStone(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStone(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

private:
	HRESULT		Add_Component(void);
	void		Key_Input(const _float& fTimeDelta);
	void		SetUp_OnTerrain(void);
	_vec3		PickUp_OnTerrain(void);

private:
	Engine::CStaticMesh*		m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag ;
	
public:
	static CStone*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // Stone_h__
