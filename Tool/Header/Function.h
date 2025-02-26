#ifndef Function_h__
#define Function_h__

//algorithm
template<class _Pr>
inline HTREEITEM find_if_tree(CTreeCtrl* _pTreeCtr, HTREEITEM _hRoot, _Pr _Pred)
{
	for (HTREEITEM hItem = _pTreeCtr->GetChildItem(_hRoot);
		nullptr != hItem;
		hItem = _pTreeCtr->GetNextSiblingItem(hItem))
	{
		if (_Pred(_pTreeCtr->GetItemData(hItem)))
			return hItem;
		HTREEITEM hItem_Child = find_if_tree(_pTreeCtr, hItem, _Pred);
		if (nullptr != hItem_Child)
			return hItem_Child;
	}	
	return nullptr;
}

template<class _Pr>
inline void remove_if_tree(CTreeCtrl* _pTreeCtr, HTREEITEM _hRoot, _Pr _Pred)
{
	for (HTREEITEM hItem = _pTreeCtr->GetChildItem(_hRoot);
		nullptr != hItem;
		hItem = _pTreeCtr->GetNextSiblingItem(hItem))
	{
		if (_Pred(_pTreeCtr->GetItemData(hItem)))
			_pTreeCtr->DeleteItem(hItem);
		remove_if_tree(_pTreeCtr, hItem, _Pred);
	}
}

template<class _Fn1>
inline _Fn1 for_each_tree(CTreeCtrl* _pTreeCtr, HTREEITEM _hRoot, _Fn1 _Func)
{
	for (HTREEITEM hItem = _pTreeCtr->GetChildItem(_hRoot);
		nullptr != hItem;
		hItem = _pTreeCtr->GetNextSiblingItem(hItem))
	{
		_Func(_pTreeCtr->GetItemData(hItem));
		for_each_tree(_pTreeCtr, hItem, _Func);
	}
	return (_Func);
}

#endif // Function_h__