#ifndef Renderer_h__
#define Renderer_h__

#include "Engine_Defines.h"
#include "Component.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CComponent
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer(void);
	virtual ~CRenderer(void);

public:
	void		Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9 pGraphicDev, bool bRemove = true);
	void		Clear_RenderGroup(void);

private:
	void		Render_Priority(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	list<CGameObject*>				m_RendererGroup[RENDER_END];
	LPDIRECT3DDEVICE9				m_pGraphicDev;

private:
	virtual void	Free(void);
};

END
#endif // Renderer_h__
