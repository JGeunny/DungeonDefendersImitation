#ifndef Sword_h__
#define Sword_h__

#include "Defines.h"
#include "GameObject.h"
#include "MeshObj.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

END

//class CSword : public Engine::CGameObject
class CSword : public CMeshObj
{
private:
	explicit CSword(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSword(void);

public:
	HRESULT Ready_Object(const _uint& iFlag);
	virtual void LateReady_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	//_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);

private:
	//Engine::CStaticMesh*		m_pMeshCom = nullptr;
	//Engine::CTransform*			m_pTransCom = nullptr;
	//Engine::CRenderer*			m_pRendererCom = nullptr;
	//Engine::CCalculator*		m_pCalculatorCom = nullptr;
	//Engine::CCollider*			m_pColliderCom = nullptr;

	//_float						m_fFrame = 0.f;
	//_vec3						m_vDir;
	//_float						m_fSpeed = 5.f;
	//_uint						m_iFlag ;

	const	_matrix*			m_pParentBoneMatrix = nullptr;
	const	_matrix*			m_pParentWorldMatrix = nullptr;
	
public:
	static CSword*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // Sword_h__
