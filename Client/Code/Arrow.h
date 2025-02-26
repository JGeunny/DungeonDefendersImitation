#ifndef Arrow_h__
#define Arrow_h__

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

class CArrow : public CBullet
{
protected:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrow(void);

public:
	virtual HRESULT Ready_Object(BULLET_DESC* pArg = nullptr);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	void			Movement(const _float& fTimeDelta);

private:
	HRESULT			Add_Component(void);

public:
	static CArrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;


};

#endif // Arrow_h__
