#ifndef __LIST_MEMORY_POOL_H__
#define __LIST_MEMORY_POOL_H__

#include <stdint.h>

template<int BLOCK_SIZE>
class ListMemoryPool
{
public:
    struct ListMemoryBlock
    {
        ListMemoryBlock*    ptr_prev;
        ListMemoryBlock*    ptr_next;
        unsigned int        id;
        unsigned char       data[BLOCK_SIZE];
#if _DEBUG
        unsigned int        status;        // 0 repersent free, 1 repersent working
#endif
    };

private:
    ListMemoryBlock*    memory_block_buf_;
    uintptr_t*          memory_block_ptr_buf_;

    unsigned int        alloc_index_;
    unsigned int        free_index_;

    unsigned int        size_;
    unsigned int        working_num_;

    bool                enable_realloc_;
#if _DEBUG
    unsigned int        peak_;        // // record the peak of memory allocate number
#endif

public:
    ListMemoryBlock     working_list_head_;
    ListMemoryBlock     working_list_tail_;

public:
    ListMemoryPool(unsigned int init_size = 32, bool enable_realloc = false)
    {
        unsigned int i = 0;

        size_ = init_size;
        enable_realloc_ = enable_realloc;

        memory_block_buf_        = (ListMemoryBlock*)malloc( size_ * sizeof(ListMemoryBlock) );
        memory_block_ptr_buf_    = (uintptr_t*)malloc( size_ * sizeof(uintptr_t) );

        memset( memory_block_buf_, 0, size_ * sizeof(ListMemoryBlock) );
        memset( memory_block_ptr_buf_, 0, size_ * sizeof(uintptr_t) );

        for (i=0; i<size_; ++i)
        {
            memory_block_ptr_buf_[i] = (uintptr_t)&memory_block_buf_[i];
            memory_block_buf_[i].id = i;
        }

        memset( &working_list_head_, 0, sizeof(ListMemoryBlock) );
        memset( &working_list_tail_, 0, sizeof(ListMemoryBlock) );

        working_list_head_.ptr_next = &working_list_tail_;
        working_list_tail_.ptr_prev = &working_list_head_;

        alloc_index_ = 0;
        free_index_ = 0;
        working_num_ = 0;
#if _DEBUG
        peak_ = 0;
#endif
    }
    ~ListMemoryPool()
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

        if ( enable_realloc_ )
        {
            unsigned int i = 0;

            // alloc memory block and memory ptr block, and init
            ListMemoryBlock* new_memory_block_buf = (ListMemoryBlock*)malloc( size * sizeof(ListMemoryBlock) );
            uintptr_t* new_memory_block_ptr_buf = (uintptr_t*)malloc( size * sizeof(uintptr_t) );

            memset( new_memory_block_buf, 0, size * sizeof(ListMemoryBlock) );
            memset( new_memory_block_ptr_buf, 0, size * sizeof(uintptr_t) );

            for (i=size_; i<size; ++i)
            {
                new_memory_block_ptr_buf[i] = (uintptr_t)&new_memory_block_buf[i];
                new_memory_block_buf[i].id = i;
            }

            // fill old data
            memcpy( new_memory_block_buf,  memory_block_buf_, size_ * sizeof(ListMemoryBlock) );
            memcpy( new_memory_block_ptr_buf, memory_block_ptr_buf_, size_ * sizeof(uintptr_t) );

            // get delta of address 
            uintptr_t delta_address = (uintptr_t)new_memory_block_buf - (uintptr_t)memory_block_buf_;
            uintptr_t delta_ptr_address = (uintptr_t)new_memory_block_ptr_buf - (uintptr_t)memory_block_ptr_buf_;

            // correct ptr block 
            for (i=0; i<size_; ++i)
            {
                new_memory_block_ptr_buf[i] += delta_address;
            }

            // correct working list
            ListMemoryBlock* ptr_cur_block = &working_list_head_;
            while (ptr_cur_block->ptr_next != &working_list_tail_)
            {
                ptr_cur_block->ptr_next = (ListMemoryBlock*)( (uintptr_t)ptr_cur_block->ptr_next + delta_address );
                ptr_cur_block = ptr_cur_block->ptr_next;
            }

            ptr_cur_block = &working_list_tail_;
            while (ptr_cur_block->ptr_prev != &working_list_head_)
            {
                ptr_cur_block->ptr_prev = (ListMemoryBlock*)( (uintptr_t)ptr_cur_block->ptr_prev + delta_address );
                ptr_cur_block = ptr_cur_block->ptr_prev;
            }

            // free old data
            free(memory_block_buf_);
            free(memory_block_ptr_buf_);

            // alloc index, correct size, memory block buf, memory ptr block buf
            alloc_index_ = size_;
            free_index_ = 0;
            size_ = size;
            memory_block_buf_ = new_memory_block_buf;
            memory_block_ptr_buf_ = new_memory_block_ptr_buf;            

            return true;
        }
        
        return false;
    }

    unsigned int Alloc()
    {
        if ( working_num_ >= size_ && !EnsureSize(size_ * 2) )
        {
#ifdef _DEBUG
            // assert(0);    // just for note user to notice memory is not enough in debug
            printf("memory pool warning: the number of allocated block is not enough");
#endif
            return -1;
        }

        ListMemoryBlock* ptr_block = (ListMemoryBlock*)memory_block_ptr_buf_[alloc_index_];
#ifdef _DEBUG
        assert(ptr_block->status == 0);    // Repeat alloc a memory block
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

        return ptr_block->id;
    }
    void* GetData(unsigned int id)
    {
        return (void*)memory_block_buf_[id].data;
    }

    void Free(ListMemoryBlock* ptr_block)
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
    void Free(unsigned int block_index)
    {
        Free(&memory_block_buf_[block_index]);
    }

    unsigned int GetSize()
    {
        return size_;
    }
    unsigned int GetWorkingSize()
    {
        return working_num_;
    }

    bool IsEnableRealloc()
    {
        return enable_realloc_;
    }
    void EnableRealloc(bool is_enable)
    {
        enable_realloc_ = is_enable;
    }

    void LinkToHead(ListMemoryBlock* ptr_block)
    {
        working_list_head_.ptr_next->ptr_prev     = ptr_block;
        ptr_block->ptr_next                     = working_list_head_.ptr_next;
        working_list_head_.ptr_next             = ptr_block;
        ptr_block->ptr_prev                     = &working_list_head_;

    }
    void LinkToTail(ListMemoryBlock* ptr_block)
    {
        working_list_tail_.ptr_prev->ptr_next    = ptr_block;
        ptr_block->ptr_prev                     = working_list_tail_.ptr_prev;
        working_list_tail_.ptr_prev             = ptr_block;
        ptr_block->ptr_next                     = &working_list_tail_;
    }

    void Print()
    {
        ListMemoryBlock* ptr_block = working_list_head_.ptr_next;
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
                ((ListMemoryBlock*)memory_block_ptr_buf_[i])->id, 
                ((ListMemoryBlock*)memory_block_ptr_buf_[i])->status, i);
        }
        printf("\n");
#endif
        printf("\n\n");
    }
};

#endif