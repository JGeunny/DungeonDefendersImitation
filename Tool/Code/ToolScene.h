#ifndef TOOLSCENE_H__
#define TOOLSCENE_H__

#include "Scene.h"

class CToolScene : public Engine::CScene
{
protected:
	explicit				CToolScene(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd = nullptr);
	virtual					~CToolScene();

public:
	virtual HRESULT			Ready_Scene(void) override;
	virtual _int			Update_Scene(const _float& fTimeDelta) override;
	virtual void			LateUpdate_Scene(void) PURE;
	virtual void			Render_Scene(void) PURE;

private:
	HRESULT					Ready_LightInfo(void);

public:
	void					Add_Hwnd(HWND hWnd);
	const std::vector<HWND>	Get_HwndList() const;
	
public:
	std::vector<HWND>		m_vecHwnd;

public:
	virtual void			Free(void) override;
};

#endif // !TOOLSCENE_H__
