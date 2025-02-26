#include "RenderTarget.h"
#include "VIBuffer_ViewPort.h"

USING(Engine)

CRenderTarget::CRenderTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CRenderTarget::Ready_RenderTarget(_uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	// 1.텍스쳐를 생성한다.
	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTexture)))
		return E_FAIL;

	// 2.렌더타겟을 얻어온다.
	if (FAILED(m_pTexture->GetSurfaceLevel(0, &m_pSurface)))
		return E_FAIL;

	m_ClearColor = ClearColor;

	return NOERROR;
}

HRESULT CRenderTarget::SetUp_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pSurface)
		return E_FAIL;

	m_pGraphic_Device->GetRenderTarget(iIndex, &m_pOldSurface);

	if (FAILED(m_pGraphic_Device->SetRenderTarget(iIndex, m_pSurface)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderTarget::Release_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;	

	m_pGraphic_Device->SetRenderTarget(iIndex, m_pOldSurface);

	Safe_Release(m_pOldSurface);

	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (0 == iIndex)
	//	{
	//		D3DXSaveTextureToFile(L"../Bin/Diffuse.jpg", D3DXIFF_JPG, m_pTexture, nullptr);
	//	}

	//	if (1 == iIndex)
	//	{
	//		D3DXSaveTextureToFile(L"../Bin/Normal.jpg", D3DXIFF_JPG, m_pTexture, nullptr);
	//	}
	//}

	return NOERROR;
}

HRESULT CRenderTarget::Clear_RenderTarget()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pSurface)
		return E_FAIL;

	m_pGraphic_Device->GetRenderTarget(0, &m_pOldSurface);

	if (FAILED(m_pGraphic_Device->SetRenderTarget(0, m_pSurface)))
		return E_FAIL;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	m_pGraphic_Device->SetRenderTarget(0, m_pOldSurface);

	Safe_Release(m_pOldSurface);

	return NOERROR; 
}

HRESULT CRenderTarget::Ready_Debug_Buffer(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (nullptr != m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_ViewPort::Create(m_pGraphic_Device, fX, fY, fSizeX, fSizeY);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return NOERROR;
}
HRESULT CRenderTarget::Render_Debug_Buffer()
{
	if (nullptr == m_pVIBuffer || 
		nullptr == m_pTexture || 
		nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetTexture(0, m_pTexture);
	m_pVIBuffer->Render_Buffer();
	
	return NOERROR;
}

CRenderTarget * CRenderTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pGraphic_Device);

	if (pInstance->Ready_RenderTarget(iWidth, iHeight, Format, ClearColor))
	{
		MSG_BOX("CRenderTarget Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pOldSurface);
	Safe_Release(m_pSurface);
	Safe_Release(m_pTexture);	
	Safe_Release(m_pGraphic_Device);
}
