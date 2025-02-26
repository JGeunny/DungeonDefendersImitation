#ifndef Bullet_h__
#define Bullet_h__

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

END

class CBullet : public CMeshObj
{
public:
	struct BULLET_DESC
	{
		Engine::TEAMTYPE eTeam = Engine::TEAMTYPE::TEAMTYPE_NONE;
		_float fAtt = 10.f;
		int   iBulletType = 0;
		_vec3 vPos = _vec3(0.f, 0.f, 0.f);
		_vec3 vDir = _vec3(0.f, 0.f, 0.f);
		_vec3 vScale = _vec3(0.001f, 0.001f, 0.001f);
		_vec3 vRadian = _vec3(0.f, 0.f, 0.f);
		_float fSpeed = 15.f;

		bool bRotateBullet = false;
		_vec3 vRotateDir = _vec3(0.f, 0.f, 0.f);
		_float vRotateDist = 0.f;
		_float fRotateSpeed = D3DXToRadian(90);

		bool bTimeDelta_Target = false;
		_float fTimeDelta_Target = 0.f;
	};

protected:
	explicit		 CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			 ~CBullet(void);
					 
public:				 
	virtual HRESULT  Ready_Object(BULLET_DESC* pArg = nullptr);
	virtual void	 LateReady_Object(void);
	virtual _int	 Update_Object(const _float& fTimeDelta) override;
	virtual void	 LateUpdate_Object(void) override;
	virtual void	 Render_Object(void) override;

public:
	virtual void	 BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	void			 Movement(const _float& fTimeDelta);

	void			 InitSetting(Engine::TEAMTYPE* peTeam = nullptr, _vec3* pvPos = nullptr, _vec3* pvScale = nullptr, _vec3* pvRadian = nullptr, float* fAtt = nullptr);
	void			 Synchronization_BulletData(BULLET_DESC& tBullet);

	void			 Update_TimeDelta_Target(float fTimeDelta);
private:
	HRESULT			 Add_Component(void);

protected:
	Engine::CStatus* m_pStatusCom = nullptr;
	_vec3			 m_vBeginPos;
	BULLET_DESC		 m_tBullet;
public:
	static CBullet*				Create(LPDIRECT3DDEVICE9 pGraphicDev, BULLET_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;


};

#endif // Bullet_h__
