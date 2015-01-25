#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

template<int BLOCK_SIZE>
class MemoryPool
{
public:
	struct MemoryBlock
	{
		MemoryBlock*	ptr_prev;
		MemoryBlock*	ptr_next;
		unsigned char	data[BLOCK_SIZE];
#if _DEBUG
		unsigned int 	id;
		unsigned int 	status;		// 0 repersent free, 1 repersent working
#endif
	};

private:
	MemoryBlock*		memory_block_buf_;
	uintptr_t*			memory_block_ptr_buf_;

	unsigned int		alloc_index_;
	unsigned int		free_index_;

	unsigned int 		size_;
	unsigned int		working_num_;

	bool 				enable_realloc_;
#if _DEBUG
	unsigned int 		peak_;		// // record the peak of memory allocate number
#endif

public:
	MemoryBlock 		working_list_head_;
	MemoryBlock 		working_list_tail_;

public:
	MemoryPool(unsigned int init_size = 32, bool enable_realloc = false)
	{
		unsigned int i = 0;

		size_ = init_size;
		enable_realloc_ = enable_realloc;

		memory_block_buf_ 		= (MemoryBlock*)malloc( size_ * sizeof(MemoryBlock) );
		memory_block_ptr_buf_	= (uintptr_t*)malloc( size_ * sizeof(uintptr_t) );

		memset( memory_block_buf_, 0, size_ * sizeof(MemoryBlock) );
		memset( memory_block_ptr_buf_, 0, size_ * sizeof(uintptr_t) );

		for (i=0; i<size_; ++i)
		{
			memory_block_ptr_buf_[i] = (uintptr_t)&memory_block_buf_[i];
#ifdef _DEBUG
			memory_block_buf_[i].id = i;
#endif
		}

		memset( &working_list_head_, 0, sizeof(MemoryBlock) );
		memset( &working_list_tail_, 0, sizeof(MemoryBlock) );

		working_list_head_.ptr_next = &working_list_tail_;
		working_list_tail_.ptr_prev = &working_list_head_;

		alloc_index_ = 0;
		free_index_ = 0;
		working_num_ = 0;
#if _DEBUG
		peak_ = 0;
#endif
	}
	~MemoryPool()
	{
		free(memory_block_buf_);
		free(memory_block_ptr_buf_);
	}

	bool EnsureSize(unsigned int size)
	{
		if ( size <= size_ )
		{
			return true;
		}

		// TODO: realloc operate
		/*
		if ( enable_realloc_ )
		{
			MemoryBlock* new_memory_block_buf = (MemoryBlock*)malloc( size * sizeof(MemoryBlock) );
			uintptr_t* new_memory_block_ptr_buf = (uintptr_t*)malloc( size * sizeof(uintptr_t) );

			bool is_new_address_bigger = ((uintptr_t)new_memory_block_buf > (uintptr_t)memory_block_buf_);
			bool is_new_ptr_address_bigger = (uintptr_t)new_memory_block_ptr_buf > (uintptr_t)memory_block_ptr_buf_;

			uintptr_t delta_address = is_new_address_bigger ? 
				(uintptr_t)new_memory_block_buf - (uintptr_t)memory_block_buf_ :
				(uintptr_t)memory_block_buf_ - (uintptr_t)new_memory_block_buf;

			uintptr_t delta_ptr_address = is_new_ptr_address_bigger ? 
				(uintptr_t)new_memory_block_ptr_buf - (uintptr_t)memory_block_ptr_buf_ :
				(uintptr_t)memory_block_ptr_buf_ - (uintptr_t)new_memory_block_ptr_buf;

			memset( new_memory_block_buf, 0, size * sizeof(MemoryBlock) );
			memset( new_memory_block_ptr_buf, 0, size * sizeof(uintptr_t) );

			for (i=0; i<size_; ++i)
			{
				new_memory_block_ptr_buf[i] = (uintptr_t)&new_memory_block_buf[i];
#ifdef _DEBUG
				new_memory_block_buf[i].id = i;
#endif
			}

			memcpy( new_memory_block_buf,  )

			return true;
		}
		*/
		
		return false;
	}

	MemoryBlock* Alloc()
	{
		if ( working_num_ >= size_ && !EnsureSize(size_ * 2) )
		{
#ifdef _DEBUG
			// assert(0);	// just for note user to notice memory is not enough in debug
			printf("memory pool warning: the number of allocated block is not enough");
#endif
			return NULL;
		}

		MemoryBlock* ptr_block = (MemoryBlock*)memory_block_ptr_buf_[alloc_index_];
#ifdef _DEBUG
		assert(ptr_block->status == 0);	// Repeat alloc a memory block
		ptr_block->status = 1;
#endif
		++alloc_index_;
		if ( alloc_index_ == size_ )
		{
			alloc_index_ = 0;
		}

		++working_num_;
#ifdef _DEBUG
		if ( peak_ < working_num_ ) 
		{
			peak_ = working_num_; 
		}
#endif

		LinkToTail(ptr_block);

		return ptr_block;
	}

	void Free(MemoryBlock* ptr_block)
	{
#ifdef _DEBUG
		assert(working_num_ > 0);
#endif
		memory_block_ptr_buf_[free_index_] = (uintptr_t)ptr_block;
#ifdef _DEBUG
		assert(ptr_block->status == 1); // Free a memory block that not alloced
		ptr_block->status = 0;
#endif

		++free_index_;
		if (free_index_ == size_)
		{
			free_index_ = 0;
		}

		--working_num_;

		ptr_block->ptr_prev->ptr_next = ptr_block->ptr_next;
		ptr_block->ptr_next->ptr_prev = ptr_block->ptr_prev;
	}

	void LinkToHead(MemoryBlock* ptr_block)
	{
		working_list_head_.ptr_next->ptr_prev 	= ptr_block;
		ptr_block->ptr_next 					= working_list_head_.ptr_next;
		working_list_head_.ptr_next 			= ptr_block;
		ptr_block->ptr_prev 					= &working_list_head_;

	}
	void LinkToTail(MemoryBlock* ptr_block)
	{
		working_list_tail_.ptr_prev->ptr_next	= ptr_block;
		ptr_block->ptr_prev 					= working_list_tail_.ptr_prev;
		working_list_tail_.ptr_prev 			= ptr_block;
		ptr_block->ptr_next 					= &working_list_tail_;
	}

	void Print()
	{
		MemoryBlock* ptr_block = working_list_head_.ptr_next;
		unsigned int i = 0;

		printf("current memory pool status: \n");
		printf("memory size: %d, memory used: %d, alloc ptr in %d, free ptr in %d\n", 
			size_, working_num_, alloc_index_, free_index_);

#ifdef _DEBUG
		printf("current working list: \n");
		printf("    head  -->");
		while (ptr_block != &working_list_tail_)
		{
			printf("%5d  -->", ptr_block->id);
			ptr_block = ptr_block->ptr_next;
		}
		printf("  tail\n");

		printf("current memory pointer array status: \n");
		for (i=0; i<size_; ++i)
		{
			printf("%5d[%d](%d) | ", 
				((MemoryBlock*)memory_block_ptr_buf_[i])->id, 
				((MemoryBlock*)memory_block_ptr_buf_[i])->status, i);
		}
		printf("\n");
#endif
		printf("\n\n");
	}
};

#endif