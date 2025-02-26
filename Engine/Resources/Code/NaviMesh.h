#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh : public CMesh
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh(void);

public:
	void		Set_Index(const _ulong& dwIndex) { m_dwIndex = dwIndex; }
	DWORD		Get_Index(_vec3 vPos);
public:
	HRESULT		Ready_NaviMesh(void);
	void		Render_NaviMesh(const _matrix* pmatWorld = nullptr);
	_vec3		MoveOn_NaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, bool bComputtingHeight = true, bool* pbUpGround = nullptr);

	_vec3 Compute_SlidingPos(_vec3 vStartPos, _vec3 vMovePos);

	HRESULT		Add_Cell(Engine::CCell **ppCell, _vec3 vPointA, _vec3 vPointB, _vec3 vPointC);
	HRESULT		Del_Cell(Engine::CCell * pCell);
	int			Check_CCW(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC);

	virtual HRESULT	Make_Range(void) { return E_FAIL; }

	vector<CCell*>* Get_CellList() { return &m_vecCell; }

	HRESULT		Link_Cell(void);

	_float		Compute_HeightCell(const _vec3* pPos, const _vec3* pvPoint);

private:
	vector<CCell*>			m_vecCell;
	_ulong					m_dwIndex = 0;

public:
	static CNaviMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources*		Clone(void);
	virtual void			Free(void);
};

END

#endif // NaviMesh_h__
