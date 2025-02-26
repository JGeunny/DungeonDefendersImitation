#ifndef PreViewScene_h__
#define PreViewScene_h__

#include "ToolScene.h"

class CPreViewScene : public CToolScene
{
protected:
	explicit		CPreViewScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd = nullptr);
	virtual			~CPreViewScene(void);

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

public:
	static CPreViewScene*		Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd = nullptr);

private:
	virtual void Free(void) override;

};

#endif // PreViewScene_h__
