#ifndef Calculator_h__
#define Calculator_h__

#include "Component.h"
#include "TerrainTex.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator : public CComponent
{
private:
	typedef struct tagOBB
	{
		_vec3		vPoint[8];
		_vec3		vCenter;
		_vec3		vProjAxis[3];   // 중점에서 콜라이더의 세 평면으로 뻗어나가는 벡터
		_vec3		vAxis[3];		// 임의의 축 벡터

	}OBB;

private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculator(void);

public:
	HRESULT		Ready_Calculator(void);
	_float		Compute_HeightTerrain(const _vec3* pPos, 
									const _vec3* pTerrainVtxPos, 
									const _ulong& dwCntX, 
									const _ulong& dwCntZ);

	_vec3		Pick_OnTerrain(HWND hWnd,
								const CTerrainTex* pTerrainBufferCom,
								const CTransform* pTerrainTransform);
	
	_bool		Collision_Sphere(const RANGE* pDstRange, const _matrix* pmatDstWorld,
								const RANGE* pSrcRange, const _matrix* pmatSrcWorld);

	_bool		Collision_AABB(const RANGE* pDstRange, const _matrix* pmatDstWorld,
								const RANGE* pSrcRange, const _matrix* pmatSrcWorld);

	_bool		Collision_OBB(const RANGE* pDstRange, const _matrix* pmatDstWorld,
							   const RANGE* pSrcRange, const _matrix* pmatSrcWorld);

	_bool		Collision_Ray(const LPD3DXMESH pDstMesh, const _matrix* pmatDstWorld,
								_vec3* pvRayPos, _vec3* pvRayDir, 
								_vec3* pvCollisionPoint = nullptr);
	_bool		Collision_Mouse(HWND hWnd, const LPD3DXMESH pDstMesh, const _matrix* pmatDstWorld,
								_vec3* pvCollisionPoint = nullptr);

	_bool		Collision_Ray_Tri(const _vec3* pDstTri, const _matrix* pmatDstWorld,
								_vec3* pvRayPos, _vec3* pvRayDir,
								_vec3* pvCollisionPoint = nullptr);
	_bool		Collision_Mouse_Tri(HWND hWnd, const _vec3* pDstTri, const _matrix* pmatDstWorld,
								_vec3* pvCollisionPoint = nullptr);

	_bool		InterSectAABB(RANGE* pOut_Range, const RANGE* pDstRange, const _matrix* pmatDstWorld,
								const RANGE* pSrcRange, const _matrix* pmatSrcWorld);
	void		GetShortLine(float * pDstMin, float * pDstMax, float fSrcMin1, float fSrcMax1, float fSrcMin2, float fSrcMax2);

public:
	void		Make_MouseRayPos(HWND hWnd, _vec3* pvOut_RayPos, _vec3* pvOut_RayDir, const _matrix * pmatDstWorld);
	void		Set_Point(OBB* pObb, const RANGE* pRange);
	void		Set_Axis(OBB* pObb);


private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;

public:
	static CCalculator*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);

};

END
#endif // Calculator_h__
