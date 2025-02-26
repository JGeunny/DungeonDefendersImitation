#ifndef ElecBall_h__
#define ElecBall_h__

#include "Defines.h"
#include "Bullet.h"

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

class CElecBall : public CBullet
{
protected:
	explicit CElecBall(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CElecBall(void);

public:
	virtual HRESULT Ready_Object(BULLET_DESC* pArg = nullptr);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

	virtual HRESULT Ready_Effect();

public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	void			Movement(const _float& fTimeDelta);

protected:
	_float				m_fRotateZ = 0.f;
private:
	HRESULT			Add_Component(void);
	HRESULT			Create_Effect_Elect();

public:
	static CElecBall* Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;
};

#endif // ElecBall_h__
