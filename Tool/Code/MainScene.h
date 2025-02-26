#ifndef MainScene_h__
#define MainScene_h__

#include "ToolScene.h"
//#include "BackGround.h"
#include "Loading.h"

class CToolCamera;

class CMainScene : public CToolScene
{
protected:
	explicit		CMainScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd = nullptr);
	virtual			~CMainScene(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Scene(void) override;
	virtual void	Render_Scene(void) override;

private:
	//HRESULT		Ready_EnvironmentLayer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogicLayer(const _tchar* pLayerTag);
	//HRESULT		Ready_UILayer(const _tchar* pLayerTag);
	HRESULT			Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eMax);
	HRESULT			Ready_LightInfo(void);

public:
	HRESULT			Ready_Camera(void);

private:
	CToolCamera*		m_pToolCamera = nullptr;
	CLoading*			m_pLoading = nullptr;

public:
	static CMainScene*		Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd = nullptr);

private:
	virtual void Free(void) override;

};

#endif // MainScene_h__
