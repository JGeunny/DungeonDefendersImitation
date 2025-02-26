#ifndef Boss_h__
#define Boss_h__

#include "Defines.h"
#include "MeshObj.h"
#include "Bullet.h"

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

class CHpBar;
class CEffect;

class CBoss : public CMeshObj
{
public:
	struct BOSS_BULLET_DESC 
	{
		float	fDelayTime = 0.f;
		CBullet::BULLET_DESC tBullet;
	};
public:
	enum BOSSSTATE { BOSSSTATE_BASE, BOSSSTATE_FLY, BOSSSTATE_END};
private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	HRESULT			Behavior(const _float& fTimeDelta);
	HRESULT			Movement(const _float& fTimeDelta, Engine::CGameObject* pTarget);
	HRESULT			Movement_GuidLine(const _float& fTimeDelta);
	HRESULT			Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget);

	void			Set_Direction(std::wstring strDirection) { m_strDirection = strDirection; }

	virtual	_vec3	Get_StartBulletPos();

	HRESULT			Create_HpBar();
	HRESULT			Create_Gold(Engine::CGameObject* pTarget);
	int				m_iCountGold = 50;

	HRESULT			Ready_SpoilBullet(int iType, float fDelayTime = 0.f);
	HRESULT			Update_SpoilBullet(_float fTimeDelta);
private:
	HRESULT			Add_Component(void);

	HRESULT			Add_Pattern();
	
	CEffect*		Create_Effect_Aura();
private:
	CEffect*		m_pEffect_Aura = nullptr;
	CDirectionObj*		m_pDirectionObj = nullptr;
	_int				m_iDirectIdx	= 0;
	Engine::CNaviMesh*	m_pNaviMeshCom = nullptr;
	Engine::CStatus*	m_pStatusCom = nullptr;
	_float				m_fSpeed;

	std::wstring		m_strDirection = L"";


	BOSSSTATE			m_eBossState = BOSSSTATE::BOSSSTATE_BASE;
	_float				m_fFlyCount = 0.f;
	

	_vec3				m_vBeginPos;
	bool				m_bAttack = false;

	std::vector<_vec3>	m_vecPos;

private:
	CHpBar*				m_pHpBar = nullptr;
	_vec2				m_vFontPos = _vec2(0.f, 0.f);
	int					m_iRage = 1;

	std::list<BOSS_BULLET_DESC> m_lstSpoilBullet;

public:
	static CBoss*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);


};

#endif // Boss_h__
