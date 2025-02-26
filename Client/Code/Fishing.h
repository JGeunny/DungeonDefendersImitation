#ifndef Fishing_h__
#define Fishing_h__

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

class CCurve;//≥¨Ω√ ¡Ÿ

END

class CFishing : public CMeshObj
{
private:
	explicit CFishing(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFishing(void);

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

	void			Set_TargetPos(_vec3 vTargetPos, _bool bCreateMonster = false);
	bool			Get_Arrived() { return m_bArrived;	}
	void			InitMonster();

	HRESULT			Create_Monster(int iCombo = 1);

private:
	HRESULT			Add_Component(void);
	HRESULT			Add_Node(Engine::CCurve** ppNode, _vec3 vPoint);
	HRESULT			Set_CenterPoint(_vec3 vStartPoint, _vec3 vEndPoint);
	
private:
	Engine::CStatus*	m_pStatusCom = nullptr;
	_float			m_fSpeed;
	
	std::vector<Engine::CCurve*> m_vecLine;
	_vec3			m_vShackLine;
	bool			m_bShackDir;

	bool			m_bTargetPos;
	_vec3			m_vTargetPos;

	CMeshObj*		m_pMonster;
	bool			m_bCreateMonster;
	bool			m_bArrived;
public:
	static CFishing*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void) override;


};

#endif // Fishing_h__
