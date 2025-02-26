#ifndef TresureChest_h__
#define TresureChest_h__

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

class CTresureChest : public CMonster
{
private:
	explicit CTresureChest(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTresureChest(void);

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

private:
	HRESULT			Add_Component(void);
	_vec3			Get_StartBulletPos();

private:
	float			m_fDeathTime;
public:
	static CTresureChest*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);


};

#endif // TresureChest_h__
