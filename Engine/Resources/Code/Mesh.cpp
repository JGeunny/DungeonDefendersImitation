#include "Mesh.h"

USING(Engine)
Engine::CMesh::CMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev)
	, m_eMeshId(MESH_DEFAULT)
	, m_bClone(false)
{

}

Engine::CMesh::CMesh(const CMesh& rhs)
	: CResources(rhs)
	, m_eMeshId(MESH_DEFAULT)
	, m_bClone(true)
	, m_tRange(rhs.m_tRange)
{
}

Engine::CMesh::~CMesh(void)
{

}

void Engine::CMesh::Free(void)
{
	CResources::Free();
}

