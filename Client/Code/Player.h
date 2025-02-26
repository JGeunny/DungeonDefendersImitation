#ifndef Player_h__
#define Player_h__

#include "Defines.h"
#include "GameObject.h"
#include "MeshObj.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CDynamicMesh;
class CCollider;
class CNaviMesh;
class CStatus;

END
class CFishing;
class CMiniGame;
class CHpBar_Player;
class CMouseUI;
class CEffect;

class CPlayer : public CMeshObj
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);

	enum TOWERTYPE { TOWERTYPE_BASE };
	enum BUILDMODE { BUILDMODE_NONE, BUILDMODE_POSITION, BUILDMODE_ROTATE, BUILDMODE_END};
	enum FISHINGMODE { FISHINGMODE_NONE, FISHINGMODE_POSITION, FISHINGMODE_START, FISHINGMODE_PLUCK, FISHINGMODE_END};

public:
	virtual HRESULT Ready_Object(void) override;
	virtual void	LateReady_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	HRESULT			Behavior(const _float& fTimeDelta);
	void			Set_Mode(Engine::CONTROLTYPE eMode) { m_eMode = eMode; }
	Engine::CONTROLTYPE	Get_Mode() { return m_eMode; }
	virtual _vec3	Get_StartBulletPos();

	bool			IsAura() { return (nullptr != m_pEffect_Aura); }
private:
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);
	void			Fix_Mouse(void);
	void			Move_Mouse(bool bFixMouse = true);

	void			Fishing_Mode(const _float& fTimeDelta);

	void			Move_BuildMouse(bool bFixMouse = true);
	void			Rotation_BuildMouse();
	HRESULT			Put_Tower(TOWERTYPE eType, BUILDMODE eBuildMode);

	CMeshObj*		m_pTowerPreView = nullptr;

	void			Move_FishingMouse();
	//void			Rotation_BuildMouse();
	HRESULT			Put_Fishing(FISHINGMODE eFishingMode);

	CFishing*		m_pFishingPreView = nullptr;

	_vec3			m_vTowerScale = {0.01f, 0.01f, 0.01f};
	CMeshObj*		CreateTower(_vec3 vPos = { 0.f, 0.f, 0.f }, _vec3 vScale = { 0.01f, 0.01f, 0.01f }, _vec3 vRadian = { 0.f, 0.f, 0.f });
	CFishing*		CreateFishing(_vec3 vPos = { 0.f, 0.f, 0.f }, _vec3 vScale = { 2.f, 2.f, 4.f }, _vec3 vRadian = { 0.f, 0.f, 0.f });
	
	void			InitMode();

	void			Set_Jump(bool bJump);
	void			Update_Jump(float fTimeDelta);
	HRESULT			Create_Effect_Hit(CMeshObj* pTarget);
	CEffect*		Create_Effect_Aura();
	void			End_Aura();

private:
	float			m_fJumpAccel = 0.f;
	bool			m_bJump = false;
	int				m_iTowerType = 0;
	Engine::CNaviMesh*	m_pNaviMeshCom = nullptr;
	Engine::CStatus*	m_pStatusCom = nullptr;

	Engine::CONTROLTYPE	m_eMode;
	BUILDMODE			m_eBuildMode;
	FISHINGMODE			m_eFishingMode;
	_vec3				m_vFishingPickPoint;
	CMiniGame*			m_pMiniGame = nullptr;

	CHpBar_Player*		m_pHpBar_Player = nullptr;
	CMouseUI*			m_pMouseUI = nullptr;
	CEffect*			m_pEffect_Aura = nullptr;
	_float				m_fAuraTime = 0.f;
public:
	static CPlayer*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void Free(void) override;


};

#endif // Player_h__
