#pragma once
#include "../Workspace.h"
#include <map>

class CBasicAllocator
{
	protected:
		std::vector<Uint>	m_FreeElements;
		std::vector<Bool>	m_Avalible;

	public:
		CBasicAllocator(Uint range_)
		{
			if (range_ == 0)
			{
				throw std::runtime_error("CBasicAllocator Constructor Error -> range must be large then 0!");
				return;
			}

			m_Avalible.resize(range_);
			m_FreeElements.resize(range_);

			for (Uint i = 0; i < range_; i++)
				m_FreeElements.at(i) = range_ - i - 1;
		}

		Uint AllocateElement()
		{
			Uint index = 0;

			if (m_FreeElements.size() > 0)
			{
				//Pick empty location from the empty array head...
				Uint new_free_size = m_FreeElements.size() - 1;
				index = m_FreeElements.at(new_free_size);
				m_FreeElements.resize(new_free_size);
			}
			else
			{
				throw std::runtime_error("CBasicAllocator Error -> Out of memory!!!");
				return INFINITY;
			}

			m_Avalible.at(index) = true;

			return index;
		}

		Void Erase(Uint id_)
		{
			if (!IsAvalible(id_))
			{
				throw std::runtime_error("CBasicAllocator :: Erase Error -> Try to erase not-avalible-element");
				return;
			}

			m_FreeElements.push_back(id_);
			m_Avalible.at(id_) = false;
		}

		Bool IsAvalible(Uint id_)
		{
			return m_Avalible.at(id_);
		}
};

class CBlocksAllocator
{
	protected:
		struct DataBlockBase
		{
			Uint offset;
			Uint size;
		};

		typedef std::multimap<Uint, DataBlockBase*> BlockMap;
		typedef std::pair<Uint, DataBlockBase*> Pair;

		BlockMap free_blocks_so;
		BlockMap free_blocks_lo;

		const Uint size;

	public:
		CBlocksAllocator(Uint size_) : size(size_)
		{
			DataBlockBase* new_block = new DataBlockBase;
			new_block->offset	= 0;
			new_block->size		= size_;

			free_blocks_so.insert(Pair(new_block->size, new_block));
			free_blocks_lo.insert(Pair(new_block->offset, new_block));
		}

		Bool AllocatePossible(Uint size_)
		{
			BlockMap::iterator found = free_blocks_so.lower_bound(size_);

			if (found == free_blocks_so.end())
				return false;

			return true;
		}

		Uint Allocate(Uint size_)
		{
			BlockMap::iterator found = free_blocks_so.lower_bound(size_);

			if (found == free_blocks_so.end())
			{
				throw std::runtime_error("Out of memory");
				return 0xffffff;
			}

			Uint loc = found->second->offset;

			if (found->second->size > size_)
			{
				DataBlockBase* new_block = new DataBlockBase;
				new_block->offset	= found->second->offset + size_;
				new_block->size		= found->second->size - size_;

				free_blocks_so.insert(Pair(new_block->size, new_block));
				free_blocks_lo.insert(Pair(new_block->offset, new_block));
			}

			
			free_blocks_lo.erase(free_blocks_lo.find(found->second->offset));
			free_blocks_so.erase(found);
			
			return loc;
		}

		BlockMap::iterator FindSOIterator(BlockMap::iterator loc_iterator_)
		{
			BlockMap::iterator found = free_blocks_so.end();
			for (BlockMap::iterator it = free_blocks_so.find(loc_iterator_->second->size) ; (it != free_blocks_so.end() && it->second->size == loc_iterator_->second->size) ; it++)
			{
				if (it->second == loc_iterator_->second)
				{
					found = it;
					break;
				}
			}

			if(found == free_blocks_so.end())
				throw std::runtime_error("Corenspoding iterator not found");

			return found;
		}

		Void Allocate(Uint size_, Uint loc_)
		{
			if (size_ == 0)
				return;

			if(loc_ + size_ > size)
				throw std::runtime_error("Out of memory");

			BlockMap::iterator found = free_blocks_lo.upper_bound(loc_);
			if(found != free_blocks_lo.begin())
				found--;

			if(found->first + found->second->size < loc_ + size_)
				throw std::runtime_error("Try to emplace in populate rigion");

			DataBlockBase* pre_block	= new DataBlockBase;
			pre_block->offset		= found->first;
			pre_block->size			= loc_ - found->first;

			DataBlockBase* affter_block = new DataBlockBase;
			affter_block->offset	= loc_ + size_;
			affter_block->size		= (found->first + found->second->size) - (loc_ + size_);

			free_blocks_so.erase(FindSOIterator(found));
			free_blocks_lo.erase(found);

			if (pre_block->size > 0)
			{
				free_blocks_so.insert(Pair(pre_block->size, pre_block));
				free_blocks_lo.insert(Pair(pre_block->offset, pre_block));
			}

			if (affter_block->size > 0)
			{
				free_blocks_so.insert(Pair(affter_block->size, affter_block));
				free_blocks_lo.insert(Pair(affter_block->offset, affter_block));
			}
		}

		Uint GetSize() { return size; }

};


template<class T>
class CAllocatedList : public CBasicAllocator
{
	struct Iterator
	{
		Uint prev;
		Uint next;
		T value;
	};

	private:
		Iterator* memory;
		Uint lenght;
		Uint begin;
		Uint end;

	public:
		CAllocatedList(Uint size_) : CBasicAllocator(size_ + 2)
		{
			memory = new Iterator[size_ + 2];

			end		= CBasicAllocator::AllocateElement();//loc = 0
			begin	= CBasicAllocator::AllocateElement();//loc = 1

			memory[begin].prev	= 0xffffffff;
			memory[begin].next	= end;
			memory[begin].value	= {};

			memory[end].prev	= begin;
			memory[end].next	= 0xffffffff;
			memory[end].value	= {};

			lenght = 0;
		}

		Uint PushBack(const T& new_element_)
		{
			Uint id = CBasicAllocator::AllocateElement();
			Uint prev = memory[end].prev;

			memory[id].prev		= prev;
			memory[id].next		= end;
			memory[id].value	= new_element_;
			
			memory[prev].next = id;
			memory[end].prev  = id;

			lenght++;

			return id;
		}

		Void Erase(Uint id_)
		{
			if (id == end)
			{
				throw std::runtime_error("CAllocatedList :: Erase Error -> Try to remove the 'end' iterator!");
				return;
			}

			CBasicAllocator::Erase(id);

			Uint prev = memory[id].prev;
			Uint next = memory[id].next;

			memory[prev].next = next;
			memory[next].prev = prev;

			lenght--;
		}

		T& At(Uint id_)
		{
			return memory[id_].value;
		}

		Iterator* Begin() { return &memory[memory[begin].next]; }
		Iterator* End() { return &memory[end]; }

		Iterator* Next(const Iterator* it_) { return &memory[it_->next]; }
		Iterator* Prev(const Iterator* it_) { return &memory[it_->prev]; }

		Bool IsEnd(const Iterator* it_) { return ((Long)(it_) == (Long)memory); }
};