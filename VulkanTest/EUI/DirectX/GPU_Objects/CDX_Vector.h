#pragma once

#include "CDX_Object.h"

template<class T> class CDX_Vector : public CDX_Object
{
	private:
		UINT32 ElementNum;

	public:
		CDX_Vector(UINT32 max_num_of_elements, UINT32 state);

		HRESULT push_back(const T &new_element);

		UINT32 size() { return ElementNum; }
		T at(UINT32 index);
};