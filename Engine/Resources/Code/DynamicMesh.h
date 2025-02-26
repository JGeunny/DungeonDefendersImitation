#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "HierarchyLoader.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh : public CMesh
{
private:
	explicit					CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit					CDynamicMesh(const CDynamicMesh& rhs);
	virtual						~CDynamicMesh(void);

public:
	HRESULT						Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void						Render_Meshes(LPD3DXEFFECT pEffect = nullptr);

public:
	virtual HRESULT				Make_Range(void);

public:
	LPD3DXMESH					Get_Mesh();
	const D3DXFRAME_DERIVED*	Get_FrameByName(const char* pFrameName);
	_bool						Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat = nullptr);
	virtual const RANGE&		Get_Range(void) const { return m_tRange; }

public:
	void						Set_AnimationSet(const _uint& iIndex);
	void						Set_AnimationSet(std::string strAniName);
	std::string					Get_AnimationSet();
	void						Play_Animation(const _float& fTimeDelta, _int* piCount = nullptr);



private:// 모든 뼈를 순회하면서 뼈가 가지고 있는 행렬 정보를 갱신하는 함수
	void						Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);
	void						SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*								m_pRootFrame;
	CHierarchyLoader*						m_pLoader;
	CAniCtrl*								m_pAniCtrl;
	list<D3DXMESHCONTAINER_DERIVED*>		m_MeshContainerList;

private:
	//여기서 컴퓨팅

public:
	static CDynamicMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CResources*		Clone(void);
private:
	virtual void			Free(void);
};

END
#endif // DynamicMesh_h__


//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;					// 뼈의 이름(x파일에서 최초 로드 시 저장되어있는 뼈대의 이름)
//	D3DXMATRIX              TransformationMatrix;	// 최초 로드 상태에서 뼈대가 지니고 있는 상태 행렬	
//
//	LPD3DXMESHCONTAINER     pMeshContainer;			// 메쉬 컨테이너는 메쉬의 각종 정보를 저장하는 구조체, 
//													// 대부분 null로 채워져 있고, 메쉬의 시작 뼈대가 되는 녀석의 정보만을 지니는 편
//	struct _D3DXFRAME       *pFrameSibling;			// 형제 뼈의 주소를 저장하기 위한 포인터
//	struct _D3DXFRAME       *pFrameFirstChild;		// 자식 뼈의 주소를 저장하기 위한 포인터
//} D3DXFRAME, *LPD3DXFRAME;
//
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;			// 뼈 이름의 경우에는 확실히 기명하는데 비해 메쉬 컨테이너 이름은 대부분 null을 채워준다.
//
//	D3DXMESHDATA            MeshData;		// 메쉬 컴객체를 담고 있는 구조체(우리는 첫 번째 멤버 변수를 사용할 예정)
//
//	LPD3DXMATERIAL          pMaterials;		// 메쉬의 재질 정보를 담기 위한 구조체
//	LPD3DXEFFECTINSTANCE    pEffects;		// 사용하지 않는다/
//	DWORD                   NumMaterials;	// 재질의 개수( == 텍스처의 개수 == 서브셋의 개수)
//	DWORD                  *pAdjacency;		// 인접한 폴리곤 정보를 보관하기 위한 포인터
//
//	LPD3DXSKININFO          pSkinInfo;		// 뼈가 아닌 메쉬와 관련된 각종 정보를 저장하는 컴 객체(ex : 메쉬가 그려질 때 어떤 뼈들에 영향을 받는지 알려주는 함수, 몇 개의 뼈들에 영향을 받고 있는지 알려주는 함수)
//
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// 다음 메쉬컨테이너의 주소를 저장하는 포인터
//
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;

