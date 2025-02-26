#ifndef Archer_h__
#define Archer_h__

#include "Defines.h"
#include "Monster.h"


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

class CArcher : public CMonster
{
private:
	explicit CArcher(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArcher(void);

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

	virtual _vec3	Get_StartBulletPos();
private:
	HRESULT			Add_Component(void);
	

private:
	bool			m_bPreShotSound = true;

public:
	static CArcher*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);


};

#endif // Archer_h__
