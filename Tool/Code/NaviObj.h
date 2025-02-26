#ifndef NaviObj_h__
#define NaviObj_h__

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

class CNaviObj : public Engine::CGameObject
{
private:
	explicit CNaviObj(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNaviObj(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

public:
	std::vector<Engine::CCell*>* Get_CellList();
	HRESULT			Add_Cell(Engine::CCell** ppCell, _vec3 vPointA, _vec3 vPointB, _vec3 vPointC);

	HRESULT			Del_Cell(Engine::CCell* pCell);

private:
	HRESULT			Add_Component(void);

private:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	//Engine::CMeshAdmin*		m_pMeshAdminCom = nullptr;
	//Engine::CMesh*			m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	//Engine::CCalculator*		m_pCalculatorCom = nullptr;
	//Engine::CCollider*		m_pColliderCom = nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom = nullptr;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag ;
	
public:
	static CNaviObj*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // NaviObj_h__
