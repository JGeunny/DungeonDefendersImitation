#ifndef OBJECTINFO_H__
#define OBJECTINFO_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CObjectInfo : public CComponent
{
private:
	explicit		CObjectInfo(const _tchar* pObjectKey);
	virtual			~CObjectInfo();

public:
	std::wstring	Get_ObjectKey() { return m_strObjectKey; }
	void			Set_ObjectKey(const _tchar* pObjectKey) { m_strObjectKey = pObjectKey; }

	_ulong			Get_Option() { return m_dwOption; }
	void			Set_Option(DWORD dwOption) { m_dwOption = dwOption; }
	void			Add_Option(DWORD dwOption) { m_dwOption |= dwOption; }
	void			Del_Option(DWORD dwOption) { if(m_dwOption & dwOption) m_dwOption -= dwOption; }
	_bool			Exist_Option(DWORD dwOption) { return (_bool)((m_dwOption & dwOption) != 0); }
private:
	std::wstring	m_strObjectKey;	// 오브젝트 키
	_ulong			m_dwOption;		// 옵션 값

public:
	static CObjectInfo* Create(const _tchar* pObjectKey);
private:
	virtual void		Free(void);
};

END

#endif // !OBJECTINFO_H__
