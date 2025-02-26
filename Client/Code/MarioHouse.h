#ifndef MarioHouse_h__
#define MarioHouse_h__

#include "Defines.h"
#include "MeshObj.h"
#include "CameraObj.h"
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
class CRcTex;
class CTexture;
class CShader;

END
class CCameraObj;
class CWaveInfo;

class CMarioHouse : public CMeshObj
{
private:
	explicit CMarioHouse(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMarioHouse(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
	bool			EmergenceBoss();

public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);
	HRESULT			Behavior(const _float& fTimeDelta);
	HRESULT			Attack(const _float& fTimeDelta, Engine::CGameObject* pTarget);

	void			Key_Input(_float fTimeDelta);


	void			Update_MonsterInfo();

	bool			IsEndGame() { return m_bEndGame; }
private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable_Texture(LPD3DXEFFECT pEffect);
	_matrix			BillBoard(_matrix matWorld);
private:
	Engine::CStatus*	m_pStatusCom = nullptr;
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CShader*	m_pShaderCom_Texture = nullptr;

	_float				m_fSpeed;
	int					m_iLevel = 1;
	bool				m_bCreateStart = false;
	bool				m_bActivate_E_Key = false;
	_matrix				m_matProj;
	CCameraObj*			m_pCameraWave[3];
	CWaveInfo*			m_pWaveInfo = nullptr;

	bool				m_bEmergenceBoss = false;

	bool				m_bEndGame = false;
public:
	static CMarioHouse*				Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void) override;


};

#endif // MarioHouse_h__
