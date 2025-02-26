#ifndef TriCol_h__
#define TriCol_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTriCol : public CVIBuffer
{
private:
	explicit CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriCol(const CTriCol& rhs);
	virtual ~CTriCol(void);

public:
	virtual HRESULT		Ready_Buffer(void);

public:
	void Set_Property(_vec3* pvPosArr, DWORD dwColor);

public:
	static CTriCol*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources* Clone(void);
	virtual void	Free(void);

private:
	_vec3 m_vPosArr[3];
	DWORD m_dwColor;
};


END
#endif // RcCol_h__
