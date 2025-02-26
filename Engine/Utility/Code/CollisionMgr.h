#ifndef CollisionMgr_h__
#define CollisionMgr_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CCalculator;

class ENGINE_DLL CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr(void);
	virtual ~CCollisionMgr(void);

public:
	HRESULT	Ready_CollisionMgr(LPDIRECT3DDEVICE9 pGraphicDev);
	void	LateUpdate_CollisionMgr(void);
	void	ReleaseData_CollisionMgr(void);

public:
	void	Add_CollisionGroup(int iCollisionUsingInfo, CGameObject* pGameObject);
	void	Del_CollisionGroup(CGameObject* pGameObject);

private:
	void	CollisionBase(std::list<CGameObject*>& rDst, std::list<CGameObject*>& rSrc);
	void	CollisionPush(std::list<CGameObject*>& rDst, std::list<CGameObject*>& rSrc);
	//static bool RayPickCube(Engine::RAY_INFO * _pOutput, Engine::CTransform * _pDstInfoCom, Engine::CTransform * _pSrcInfoCom);

private:
	CCalculator*		m_pCalculatorCom;
	map<std::wstring, std::list<CGameObject*>>  m_mapCollisionObject;

private:
	void	Free(void);
};

END

#endif // CollisionMgr_h__
