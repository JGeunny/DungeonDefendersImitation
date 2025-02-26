#ifndef VIBuffer_h__
#define VIBuffer_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CResources
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void);

public:
	HRESULT		Ready_Buffer(void);
	void		Render_Buffer(void);

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;

	// 정점의 구성 요소(정점구조체의 멤버변수)를 정의한 객체.
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration = nullptr;
	D3DVERTEXELEMENT9				m_VertexElement[MAX_FVF_DECL_SIZE];

protected:
	_ulong		m_dwVtxSize;
	_ulong 		m_dwVtxCnt;
	_ulong 		m_dwTriCnt;
	_ulong 		m_dwVtxFVF;

	_ulong 		m_dwIdxSize;
	D3DFORMAT	m_IdxFmt;

public:	
	virtual CResources*	Clone(void)PURE;
	virtual void Free(void);

};

END
#endif // VIBuffer_h__
