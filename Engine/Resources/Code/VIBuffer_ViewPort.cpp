#include "VIBuffer_ViewPort.h"

USING(Engine)

CVIBuffer_ViewPort::CVIBuffer_ViewPort(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{

}

CVIBuffer_ViewPort::CVIBuffer_ViewPort(const CVIBuffer_ViewPort & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_ViewPort::Ready_Component_Prototype(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_dwVtxSize = sizeof(VTXVIEWPORT); // Stride
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwVtxFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	m_VertexElement[0] = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
	m_VertexElement[1] = { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
	m_VertexElement[2] = D3DDECL_END();

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXVIEWPORT*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec4(fX, fY, 0.0f, 1.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec4(fX + fSizeX, fY, 0.f, 1.f);
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPosition = _vec4(fX + fSizeX, fY + fSizeY, 0.f, 1.f);
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPosition = _vec4(fX, fY + fSizeY, 0.f, 1.f);
	pVertices[3].vTexUV = _vec2(0.0f, 1.f);
	
	m_pVB->Unlock();

	INDEX16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

HRESULT CVIBuffer_ViewPort::Ready_Component(void * pArg)
{
	return NOERROR;
}

HRESULT CVIBuffer_ViewPort::Render_Buffer(void)
{
	LPDIRECT3DDEVICE9	pGraphic_Device = m_pGraphicDev;
	if (nullptr == pGraphic_Device)
		return E_FAIL;

	Safe_AddRef(pGraphic_Device);

	pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	pGraphic_Device->SetFVF(m_dwVtxFVF);
	pGraphic_Device->SetIndices(m_pIB);
	pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

	Safe_Release(pGraphic_Device);

	return NOERROR;
}

CVIBuffer_ViewPort * CVIBuffer_ViewPort::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CVIBuffer_ViewPort*		pInstance = new CVIBuffer_ViewPort(pGraphic_Device);

	if (pInstance->Ready_Component_Prototype(fX, fY, fSizeX, fSizeY))
	{
		MSG_BOX("CVIBuffer_ViewPort Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Engine::CResources* CVIBuffer_ViewPort::Clone(void)
{
	return new CVIBuffer_ViewPort(*this);
}

void CVIBuffer_ViewPort::Free()
{
	CVIBuffer::Free();
}

