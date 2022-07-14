#pragma once

#include <stdio.h>
#include <math.h>
#include <memory.h>

#ifndef __PE_TYPES_HEADER__
#define __PE_TYPES_HEADER__

typedef char			Char;
typedef unsigned char	Byte;
typedef bool			Bool;
typedef short			Word;
typedef int				Dword;
typedef int				Int;
typedef short			Int16;
typedef long			Long;
typedef double			Double;
typedef unsigned int	Uint;
typedef unsigned short	Uint16;
typedef unsigned short  Half;
typedef float			Float;
typedef double			Double;
typedef void*			Pointer;
typedef const void*		ConstructData;
typedef const char*		String;
typedef const wchar_t*	WString;
typedef void			Void;

#define PI			3.14159265359
#define HALF_PI		1.57079632679
#define TOW_PI		6.28318530718
#define E			2.71828182846

template<class T>
struct Triangle
{
	T v1;
	T v2;
	T v3;

	T& operator[](size_t index)
	{
		return ((T*)this)[index];
	}
};


template<class T>
class DynamicArray
{
	protected:
		Uint size;
		Uint allocator;

		T* buffer;

		Void Allocate_Memory(const Uint new_size)
		{
			if (new_size == 0)
			{
				Clear();
				return;
			}

			Uint saved_size = size;

			if (new_size > allocator || new_size <= (allocator >> 1) )
			{
				//Define the size of the new buffer...
				size = new_size - 1;
				if (!size)
				{
					size = 1;
					allocator = 1;
				}
				else
				{
					Uint l2 = 0;
					while (size >>= 1)
						l2++;

					size = new_size;
					allocator = 1 << (l2 + 1);
				}

				T* new_buffer = new T[allocator];//Create the new buffer
				if(saved_size)
					memcpy(new_buffer, buffer, saved_size * sizeof(T));//Copy memory to new buffer
				if(buffer != nullptr)
					delete[] buffer;//Reallese the last buffer
				buffer = new_buffer;//Set points to the new buffer
				return;
			}
			else
			{
				size ++;
			}
		}

	public:
		DynamicArray()
		{
			size = Uint();
			allocator = Uint();
			buffer = nullptr;
		}

		Void Add_Element(const T& new_element)
		{
			Allocate_Memory(size + 1);
			memcpy(&buffer[size - 1], &new_element, sizeof(T));
		}

		Void Clear()
		{
			size = 0;
			allocator = 0;
			if(buffer)
				delete[] buffer;
			buffer = nullptr;
		}

		Uint Get_Size() const { return size; }
		T* Get_Data() const { return buffer; }
		
		const T* At(Uint index) const { return buffer + index; }
};

#endif
