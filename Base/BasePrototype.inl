CBasePrototype::CBasePrototype(void)
	: m_pdwRefCnt(new unsigned long(0))
{

}
CBasePrototype:: ~CBasePrototype(void)
{

}

unsigned long CBasePrototype::AddRef(void)
{
	return ++(*m_pdwRefCnt);
}
unsigned long CBasePrototype::Release(void)
{
	if (0 == *m_pdwRefCnt)
	{
		delete m_pdwRefCnt;
		m_pdwRefCnt = nullptr;

		Free();
		delete this;
		return 0;
	}
	auto m_dwRefCnt = (*m_pdwRefCnt);
	(*m_pdwRefCnt)--;
	delete this;
	return m_dwRefCnt;
}