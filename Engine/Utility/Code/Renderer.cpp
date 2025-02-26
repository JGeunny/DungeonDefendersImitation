#include "Renderer.h"

#include "LightMgr.h"
#include "Target_Manager.h"
#include "VIBuffer_ViewPort.h"

#include "Export_System.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

Engine::CRenderer::CRenderer(void)
	: m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLightMgr::GetInstance())
{

}

Engine::CRenderer::~CRenderer(void)
{
	Free();
}

HRESULT CRenderer::Ready_Renderer(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 디퍼드를 위한 렌더타겟들을 생성하자.
	D3DVIEWPORT9			ViewPortDesc;
	pGraphicDev->GetViewport(&ViewPortDesc);

	//// For.Target_Diffuse
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Diffuse", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	// For.Target_Diffuse
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Diffuse", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Normal
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Normal", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Depth
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Depth", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Shade
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Shade", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Specular
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(pGraphicDev, L"Target_Specular", ViewPortDesc.Width, ViewPortDesc.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// 동시에 장치에 셋팅되는 타겟들을 묶어놓자(MRT)
	// For.MRT_Deferred
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_LightAcc.fx");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_Blend
	m_pShader_Blend = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	// For.VIBuffer_ViewPort
	m_pVIBuffer = CVIBuffer_ViewPort::Create(pGraphicDev, 0.f, 0.f, ViewPortDesc.Width, ViewPortDesc.Height);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Diffuse", 0.f, 0.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Normal", 0.f, 200.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Depth", 0.f, 400.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", 200.f, 0.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", 200.f, 200.0f, 200.f, 200.f)))
		return E_FAIL;

	Ready_RenderState();
	return NOERROR;
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
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_LightAcc(pGraphicDev);
	Render_Blend(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	if (nullptr == m_pTarget_Manager)
		return;

	if (true == Engine::Is_DebugRender(DEBUG_RENDER_TARGET))
	{
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred")))
			return;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc")))
			return;
	}
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
	Begin_RenderState(pGraphicDev);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& iter : m_RendererGroup[RENDER_PRIORITY])
		iter->CGameObject::Render_Object();

	End_RenderState(pGraphicDev);
	Safe_Release(pGraphicDev);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	// Diffuse + Normal
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Deferred")))
		return;

	Begin_RenderState(pGraphicDev);
	//pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& iter : m_RendererGroup[RENDER_NONALPHA])
		iter->CGameObject::Render_Object();

	End_RenderState(pGraphicDev);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Deferred")))
		return;

	Safe_Release(pGraphicDev);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	Begin_RenderState(pGraphicDev);
	
	//pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	//pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	m_RendererGroup[RENDER_ALPHA].sort([&](auto pGameObject1, auto pGameObject2) {
		return pGameObject1->Get_ViewZ() < pGameObject2->Get_ViewZ();
	});

	for (auto& iter : m_RendererGroup[RENDER_ALPHA])
		iter->CGameObject::Render_Object();

	End_RenderState(pGraphicDev);
	Safe_Release(pGraphicDev);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 pGraphicDev)
{
	pGraphicDev->AddRef();

	Begin_RenderState(pGraphicDev);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RendererGroup[RENDER_UI].sort([&](auto pGameObject1, auto pGameObject2) {
		return pGameObject1->Get_ViewZ() < pGameObject2->Get_ViewZ();
	});

	for (auto& iter : m_RendererGroup[RENDER_UI])
		iter->CGameObject::Render_Object();

	End_RenderState(pGraphicDev);
	Safe_Release(pGraphicDev);
}

void CRenderer::Render_LightAcc(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pShader_LightAcc ||
		nullptr == m_pLight_Manager)
		return;

	// Shade
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_LightAcc")))
		return;

	if (FAILED(m_pShader_LightAcc->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_TargetTexture(L"Target_Normal"))))
		return;
	if (FAILED(m_pShader_LightAcc->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_TargetTexture(L"Target_Depth"))))
		return;

	_matrix			matViewInv, matProjInv;
	_vec3			m_vEye;
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProjInv);
	pGraphicDev->GetTransform(D3DTS_VIEW, &matViewInv);

	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);
	m_vEye = *(_vec3*)&matViewInv.m[3];

	if (FAILED(m_pShader_LightAcc->Set_Value("g_matProjInv", &matProjInv, sizeof(_matrix))))
		return;
	if (FAILED(m_pShader_LightAcc->Set_Value("g_matViewInv", &matViewInv, sizeof(_matrix))))
		return;
	if (FAILED(m_pShader_LightAcc->Set_Value("g_vCamPosition", &_vec4(m_vEye, 1.f), sizeof(_vec4))))
		return;

	Begin_RenderState(pGraphicDev);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pShader_LightAcc->Begin_Shader(nullptr);
	//m_pShader_LightAcc->Begin_Pass(0);

	m_pLight_Manager->Render_Light(m_pShader_LightAcc);

	//m_pShader_LightAcc->End_Pass();
	m_pShader_LightAcc->End_Shader();
	End_RenderState(pGraphicDev);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_LightAcc")))
		return;

}

void CRenderer::Render_Blend(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pShader_Blend)
		return;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_TargetTexture(L"Target_Diffuse"))))
		return;
	if (FAILED(m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_TargetTexture(L"Target_Shade"))))
		return;
	if (FAILED(m_pShader_Blend->Set_Texture("g_SpecularTexture", m_pTarget_Manager->Get_TargetTexture(L"Target_Specular"))))
		return;

	Begin_RenderState(pGraphicDev);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pShader_Blend->Begin_Shader(nullptr);
	m_pShader_Blend->Begin_Pass(0);

	m_pVIBuffer->Render_Buffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();
	End_RenderState(pGraphicDev);
}

void CRenderer::Ready_RenderState()
{
	m_mapRenderState.clear();
	m_mapRenderState.emplace(D3DRS_LIGHTING, 0);
	m_mapRenderState.emplace(D3DRS_ZENABLE, 0);
	m_mapRenderState.emplace(D3DRS_ZWRITEENABLE, 0);
	m_mapRenderState.emplace(D3DRS_ALPHABLENDENABLE, 0);
	m_mapRenderState.emplace(D3DRS_SRCBLEND, 0);
	m_mapRenderState.emplace(D3DRS_DESTBLEND, 0);
}

void CRenderer::Begin_RenderState(LPDIRECT3DDEVICE9 pGraphicDev)
{
	for (auto& pair : m_mapRenderState)
		pGraphicDev->GetRenderState(pair.first, &pair.second);
}

void CRenderer::End_RenderState(LPDIRECT3DDEVICE9 pGraphicDev)
{
	for (auto& pair : m_mapRenderState)
		pGraphicDev->SetRenderState(pair.first, pair.second);
}

void Engine::CRenderer::Free(void)
{
	Clear_RenderGroup();
	//Safe_Release(m_pTarget_Manager);

	Safe_Release(m_pVIBuffer);
	//Safe_Release(m_pLight_Manager);

	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_LightAcc);
}

