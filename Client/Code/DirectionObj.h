#ifndef DirectionObj_h__
#define DirectionObj_h__

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

class CDirectionObj : public Engine::CGameObject
{
private:
	explicit CDirectionObj(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDirectionObj(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

	_vec3			Get_DirectPos(_int iDirectionIdx);
	_bool			Is_DirectionIdx(_int iDirectionIdx);
public:
	std::vector<Engine::CCurve*>* Get_NodeList();
	HRESULT			Add_Node(Engine::CCurve** ppNode, _vec3 vPoint);
	HRESULT			Del_Node(Engine::CCurve* pNode);

private:
	HRESULT			Add_Component(void);

private:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;

	_uint						m_iFlag;
	
	std::vector<Engine::CCurve*> m_vecNode;

public:
	static CDirectionObj*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // DirectionObj_h__
