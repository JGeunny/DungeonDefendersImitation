#include "Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

Engine::CRenderer::CRenderer(void)
{

}

Engine::CRenderer::~CRenderer(void)
{

}

void Engine::CRenderer::Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject)
{
	if (RENDER_END <= eGroup)
		return;

	m_RendererGroup[eGroup].push_back(pGameObject);

	pGameObject->AddRef();
}

void Engine::CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 pGraphicDev, bool bRemove)
{
	//for (_uint i = 0; i < RENDER_END; ++i)
	//{
	//	for (auto& iter : m_RendererGroup[i])
	//	{
	//		iter->CGameObject::Render_Object();
	//		if (bRemove)
	//			Safe_Release(iter);			// 레퍼런스 카운트 감수
	//	}
	//	if(bRemove)
	//		m_RendererGroup[i].clear();
	//}

	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	if (bRemove)
		Clear_RenderGroup();
}

void Engine::CRenderer::Clear_RenderGroup(void)
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RendererGroup[i].begin(), m_RendererGroup[i].end(), CDeleteObj());
		m_RendererGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();
	BOOL bLighting, bZenable, bZwriteenable, bAlpha;
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bLighting);
	pGraphicDev->GetRenderState(D3DRS_ZENABLE, (DWORD*)&bZenable);
	pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, (DWORD*)&bZwriteenable);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& iter : m_RendererGroup[RENDER_PRIORITY])
		iter->CGameObject::Render_Object();

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, bLighting);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, bZenable);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, bZwriteenable);

	Safe_Release(pGraphicDev);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	for (auto& iter : m_RendererGroup[RENDER_NONALPHA])
		iter->CGameObject::Render_Object();

	Safe_Release(pGraphicDev);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	BOOL bLighting, bAlpha;
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bLighting);
	pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD*)&bAlpha);
	_D3DBLEND eSrcBlend, eDestBlend;
	pGraphicDev->GetRenderState(D3DRS_SRCBLEND, (DWORD*)&eSrcBlend);
	pGraphicDev->GetRenderState(D3DRS_DESTBLEND, (DWORD*)&eDestBlend);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RendererGroup[RENDER_ALPHA].sort([&](auto pGameObject1, auto pGameObject2) {
		return pGameObject1->Get_ViewZ() < pGameObject2->Get_ViewZ();
	});

	for (auto& iter : m_RendererGroup[RENDER_ALPHA])
		iter->CGameObject::Render_Object();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, bAlpha);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, bLighting);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, eSrcBlend);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, eDestBlend);

	Safe_Release(pGraphicDev);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	BOOL bLighting, bZenable, bZwriteenable, bAlpha;
	pGraphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bLighting);
	pGraphicDev->GetRenderState(D3DRS_ZENABLE, (DWORD*)&bZenable);
	pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, (DWORD*)&bZwriteenable);
	pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD*)&bAlpha);

	_D3DBLEND eSrcBlend, eDestBlend;
	pGraphicDev->GetRenderState(D3DRS_SRCBLEND, (DWORD*)&eSrcBlend);
	pGraphicDev->GetRenderState(D3DRS_DESTBLEND, (DWORD*)&eDestBlend);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& iter : m_RendererGroup[RENDER_UI])
		iter->CGameObject::Render_Object();

	pGraphicDev->SetRenderState(D3DRS_ZENABLE, bZenable);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, bZwriteenable);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, bAlpha);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, bLighting);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, eSrcBlend);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, eDestBlend);

	Safe_Release(pGraphicDev);
}

void Engine::CRenderer::Free(void)
{
	Clear_RenderGroup();

}

