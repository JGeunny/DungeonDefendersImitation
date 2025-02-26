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



private:// ��� ���� ��ȸ�ϸ鼭 ���� ������ �ִ� ��� ������ �����ϴ� �Լ�
	void						Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);
	void						SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*								m_pRootFrame;
	CHierarchyLoader*						m_pLoader;
	CAniCtrl*								m_pAniCtrl;
	list<D3DXMESHCONTAINER_DERIVED*>		m_MeshContainerList;

private:
	//���⼭ ��ǻ��

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
//	LPSTR                   Name;					// ���� �̸�(x���Ͽ��� ���� �ε� �� ����Ǿ��ִ� ������ �̸�)
//	D3DXMATRIX              TransformationMatrix;	// ���� �ε� ���¿��� ���밡 ���ϰ� �ִ� ���� ���	
//
//	LPD3DXMESHCONTAINER     pMeshContainer;			// �޽� �����̳ʴ� �޽��� ���� ������ �����ϴ� ����ü, 
//													// ��κ� null�� ä���� �ְ�, �޽��� ���� ���밡 �Ǵ� �༮�� �������� ���ϴ� ��
//	struct _D3DXFRAME       *pFrameSibling;			// ���� ���� �ּҸ� �����ϱ� ���� ������
//	struct _D3DXFRAME       *pFrameFirstChild;		// �ڽ� ���� �ּҸ� �����ϱ� ���� ������
//} D3DXFRAME, *LPD3DXFRAME;
//
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;			// �� �̸��� ��쿡�� Ȯ���� ����ϴµ� ���� �޽� �����̳� �̸��� ��κ� null�� ä���ش�.
//
//	D3DXMESHDATA            MeshData;		// �޽� �İ�ü�� ��� �ִ� ����ü(�츮�� ù ��° ��� ������ ����� ����)
//
//	LPD3DXMATERIAL          pMaterials;		// �޽��� ���� ������ ��� ���� ����ü
//	LPD3DXEFFECTINSTANCE    pEffects;		// ������� �ʴ´�/
//	DWORD                   NumMaterials;	// ������ ����( == �ؽ�ó�� ���� == ������� ����)
//	DWORD                  *pAdjacency;		// ������ ������ ������ �����ϱ� ���� ������
//
//	LPD3DXSKININFO          pSkinInfo;		// ���� �ƴ� �޽��� ���õ� ���� ������ �����ϴ� �� ��ü(ex : �޽��� �׷��� �� � ���鿡 ������ �޴��� �˷��ִ� �Լ�, �� ���� ���鿡 ������ �ް� �ִ��� �˷��ִ� �Լ�)
//
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// ���� �޽������̳��� �ּҸ� �����ϴ� ������
//
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;

