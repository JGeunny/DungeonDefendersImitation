#ifndef Renderer_h__
#define Renderer_h__

#include "Engine_Defines.h"
#include "Component.h"
#include "GameObject.h"
#include "Shader.h"

BEGIN(Engine)

class CLightMgr;
class CTarget_Manager;
class CVIBuffer_ViewPort;
class ENGINE_DLL CRenderer : public CComponent
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer(void);
	virtual ~CRenderer(void);

public:
	HRESULT		Ready_Renderer(LPDIRECT3DDEVICE9 pGraphicDev);

	void		Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9 pGraphicDev, bool bRemove = true);
	void		Clear_RenderGroup(void);

private:
	void		Render_Priority(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void		Render_LightAcc(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Render_Blend(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	list<CGameObject*>				m_RendererGroup[RENDER_END];
	LPDIRECT3DDEVICE9				m_pGraphicDev;

private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CLightMgr*					m_pLight_Manager = nullptr;
	CShader*					m_pShader_LightAcc = nullptr;
	CShader*					m_pShader_Blend = nullptr;
	CVIBuffer_ViewPort*			m_pVIBuffer = nullptr;

	void Ready_RenderState();
	void Begin_RenderState(LPDIRECT3DDEVICE9 pGraphicDev);
	void End_RenderState(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	unordered_map<_D3DRENDERSTATETYPE, DWORD> m_mapRenderState;
private:
	virtual void	Free(void);
};

END
#endif // Renderer_h__
