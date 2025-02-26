#include "ObjectInfo.h"

USING(Engine)

CObjectInfo::CObjectInfo(const _tchar* pObjectKey)
	: m_strObjectKey(pObjectKey)
	, m_dwOption(0)
{
}

CObjectInfo::~CObjectInfo()
{
}

CObjectInfo * CObjectInfo::Create(const _tchar* pObjectKey)
{
	return new CObjectInfo(pObjectKey);
}

void CObjectInfo::Free(void)
{
}
