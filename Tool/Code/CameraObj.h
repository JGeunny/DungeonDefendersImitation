#ifndef CameraObj_h__
#define CameraObj_h__

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
class CCurve;

END

class CToolCamera;

class CCameraObj : public Engine::CGameObject
{
private:
	explicit CCameraObj(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraObj(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

	_int			Get_PlayIndex() { return m_iPlay_Index;}
	_int			Get_PlayMax() { return m_vecNode[Engine::CAMERATYPE_EYE].size(); }
	void			Play_Camera();
	void			Stop_Camera();
	void			Pause_Camera();

public:
	std::vector<Engine::CCurve*>* Get_NodeList(Engine::CAMERATYPE eType);
	HRESULT			Add_Node(Engine::CAMERATYPE eType, Engine::CCurve** ppNode, _vec3 vPoint);
	HRESULT			Del_Node(Engine::CAMERATYPE eType, Engine::CCurve* pNode);

private:
	HRESULT			Add_Component(void);
	HRESULT			Set_PlayIndex(_int* piOut_Index);

private:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	//Engine::CMeshAdmin*		m_pMeshAdminCom = nullptr;
	//Engine::CMesh*			m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	//Engine::CCalculator*		m_pCalculatorCom = nullptr;
	//Engine::CCollider*		m_pColliderCom = nullptr;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 0.5f;
	_uint						m_iFlag;
	
	std::vector<Engine::CCurve*> m_vecNode[Engine::CAMERATYPE_END];

	_matrix						m_matView;
	_vec3						m_vEye;
	_vec3						m_vAt;
	_vec3						m_vUp;

	_bool						m_bPlay;
	_int						m_iPlay_Index;
	_float						m_fPlay;
public:
	static CCameraObj*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // CameraObj_h__
