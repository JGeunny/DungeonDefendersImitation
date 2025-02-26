#include "LightMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLightMgr)

Engine::CLightMgr::CLightMgr(void)
{

}

Engine::CLightMgr::~CLightMgr(void)
{
	Free();
}

HRESULT Engine::CLightMgr::Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
	const D3DLIGHT9* pLightInfo,
	const _uint& iIndex)
{

	CLight*	pLight = CLight::Create(pGraphicDev, pLightInfo, iIndex);
	NULL_CHECK_RETURN(pLight, E_FAIL);

	m_VecLight.push_back(pLight);

	return S_OK;
}

HRESULT CLightMgr::Render_Light(CShader * pShader)
{
	for (auto& pLight : m_VecLight)
		pLight->Render_Light(pShader);

	return NOERROR;
}

void CLightMgr::Clear_Light()
{
	for_each(m_VecLight.begin(), m_VecLight.end(), CDeleteObj());
	m_VecLight.clear();
}

void Engine::CLightMgr::Free(void)
{
	Clear_Light();
}

