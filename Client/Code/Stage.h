#ifndef Stage_h__
#define Stage_h__

#include "Defines.h"
#include "Scene.h"

#include "Terrain.h"
#include "DynamicCamera.h"
#include "ShoulderCamera.h"
#include "SkyBox.h"
#include "Player.h"
#include "Stone.h"
#include "Sword.h"
#include "MeshObj.h"

class CCameraObj;

class CStage : public Engine::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

	virtual void* Get_NaviMesh();

private:
	//HRESULT		Ready_EnvironmentLayer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogicLayer(const _tchar* pLayerTag);
	//HRESULT		Ready_UILayer(const _tchar* pLayerTag);
	HRESULT		Ready_LightInfo(void);

	HRESULT		Load_Layer(const _tchar* pStagePath);
	HRESULT		Load_Camera(const _tchar* pCameraPath);
	HRESULT		Load_Direction(const _tchar* pDirectionPath);

private:
	CShoulderCamera*				m_pShoulderCamera;
	_matrix							m_matView, m_matProj;
	CCameraObj*						m_pCamera;
public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};

#endif // Stage_h__
