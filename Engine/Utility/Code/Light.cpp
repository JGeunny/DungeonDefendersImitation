#include "Light.h"

USING(Engine)

Engine::CLight::CLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_iIndex(0)
{
	ZeroMemory(&m_tLightInfo, sizeof(D3DLIGHT9));
	m_pGraphicDev->AddRef();
}

Engine::CLight::~CLight(void)
{

}

HRESULT Engine::CLight::Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	memcpy(&m_tLightInfo, pLightInfo, sizeof(D3DLIGHT9));

	m_iIndex = iIndex;

	//m_pGraphicDev->SetLight(iIndex, pLightInfo);
	//m_pGraphicDev->LightEnable(iIndex, TRUE);

	D3DVIEWPORT9			ViewPortDesc;
	m_pGraphicDev->GetViewport(&ViewPortDesc);

	m_pVIBuffer = CVIBuffer_ViewPort::Create(m_pGraphicDev, 0.0f, 0.f, ViewPortDesc.Width, ViewPortDesc.Height);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::Render_Light(CShader * pShader)
{
	if (nullptr == m_pVIBuffer ||
		nullptr == pShader)
		return E_FAIL;

	//pShader->Set_Value("g_vLightDir", &_vec4(m_tLightInfo.Direction, 0.f), sizeof(_vec4));

	//pShader->Commit_Changes();

	//m_pVIBuffer->Render_Buffer();

	_uint		iPassIndex = 0;

	if (D3DLIGHT_DIRECTIONAL == m_tLightInfo.Type)
	{
		iPassIndex = 0;
		pShader->Set_Value("g_vLightDir", &_vec4(m_tLightInfo.Direction, 0.f), sizeof(_vec4));
	}

	else if (D3DLIGHT_POINT == m_tLightInfo.Type)
	{
		iPassIndex = 1;
		pShader->Set_Value("g_vLightPos", &_vec4(m_tLightInfo.Position, 1.f), sizeof(_vec4));
		pShader->Set_Value("g_fRange", &m_tLightInfo.Range, sizeof(_float));
	}

	pShader->Set_Value("g_vLightDiffuse", (_vec4*)&m_tLightInfo.Diffuse, sizeof(_vec4));
	pShader->Set_Value("g_vLightAmbient", (_vec4*)&m_tLightInfo.Ambient, sizeof(_vec4));
	pShader->Set_Value("g_vLightSpecular", (_vec4*)&m_tLightInfo.Specular, sizeof(_vec4));

	pShader->Commit_Changes();

	pShader->Begin_Pass(iPassIndex);

	m_pVIBuffer->Render_Buffer();

	pShader->End_Pass();

	return NOERROR;
}

CLight* Engine::CLight::Create(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	CLight*	pInstance = new CLight(pGraphicDev);

	if (FAILED(pInstance->Ready_Light(pLightInfo, iIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CLight::Free(void)
{
	//m_pGraphicDev->LightEnable(m_iIndex, FALSE);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGraphicDev);
}

