#ifndef BasePrototype_h__
#define BasePrototype_h__

class _declspec(dllexport) CBasePrototype
{
protected:
	inline explicit CBasePrototype(void);
	inline virtual ~CBasePrototype(void);

public:
	inline unsigned long AddRef(void);
	inline unsigned long GetRef(void) { return *m_pdwRefCnt; }
	inline virtual unsigned long Release(void);

private:
	unsigned long*		m_pdwRefCnt;

public:
	inline virtual void Free(void) = 0;

};

#include "BasePrototype.inl"

#endif // BasePrototype_h__
