#ifndef __LIST_MEMORY_POOL_INDEX_H__
#define __LIST_MEMORY_POOL_INDEX_H__

#include <stdint.h>

template<int BLOCK_SIZE>
class ListMemoryPool_UseIndex
{
public:
    struct ListMemoryBlock_UseIndex
    {
        unsigned int    index_prev;
        unsigned int    index_next;
        unsigned int     id;
        unsigned char    data[BLOCK_SIZE];
#if _DEBUG
        unsigned int     status;        // 0 repersent free, 1 repersent working
#endif
    };

    enum
    {
        HEAD_ID = (unsigned int)(-1),
        TAIL_ID = (unsigned int)(-2),
    };

private:
    ListMemoryBlock_UseIndex*    memory_block_buf_;
    unsigned int*        memory_block_index_buf_;

    unsigned int        alloc_index_;
    unsigned int        free_index_;

    unsigned int         size_;
    unsigned int        working_num_;

    bool                 enable_realloc_;
#if _DEBUG
    unsigned int         peak_;        // // record the peak of memory allocate number
#endif

public:
    ListMemoryBlock_UseIndex         working_list_head_;
    ListMemoryBlock_UseIndex         working_list_tail_;

public:
    ListMemoryPool_UseIndex(unsigned int init_size = 32, bool enable_realloc = false)
    {
        unsigned int i = 0;

        size_ = init_size;
        enable_realloc_ = enable_realloc;

        memory_block_buf_         = (ListMemoryBlock_UseIndex*)malloc( size_ * sizeof(ListMemoryBlock_UseIndex) );
        memory_block_index_buf_    = (unsigned int*)malloc( size_ * sizeof(unsigned int) );

        memset( memory_block_buf_, 0, size_ * sizeof(ListMemoryBlock_UseIndex) );
        memset( memory_block_index_buf_, 0, size_ * sizeof(unsigned int) );

        for (i=0; i<size_; ++i)
        {
            memory_block_index_buf_[i] = i;
            memory_block_buf_[i].id = i;
        }

        memset( &working_list_head_, 0, sizeof(ListMemoryBlock_UseIndex) );
        memset( &working_list_tail_, 0, sizeof(ListMemoryBlock_UseIndex) );

        working_list_head_.id = HEAD_ID;
        working_list_tail_.id = TAIL_ID;
        working_list_head_.index_next = TAIL_ID;
        working_list_tail_.index_prev = HEAD_ID;

        alloc_index_ = 0;
        free_index_ = 0;
        working_num_ = 0;
#if _DEBUG
        peak_ = 0;
#endif
    }
    ~ListMemoryPool_UseIndex()
    {
        free(memory_block_buf_);
        free(memory_block_index_buf_);
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
            ListMemoryBlock_UseIndex* new_memory_block_buf = (ListMemoryBlock_UseIndex*)malloc( size * sizeof(ListMemoryBlock_UseIndex) );
            unsigned int* new_memory_block_index_buf = (unsigned int*)malloc( size * sizeof(unsigned int) );

            memset( new_memory_block_buf, 0, size * sizeof(ListMemoryBlock_UseIndex) );
            memset( new_memory_block_index_buf, 0, size * sizeof(unsigned int) );

            for (i=size_; i<size; ++i)
            {
                new_memory_block_index_buf[i] = i;
                new_memory_block_buf[i].id = i;
            }

            // fill old data
            memcpy( new_memory_block_buf,  memory_block_buf_, size_ * sizeof(ListMemoryBlock_UseIndex) );
            memcpy( new_memory_block_index_buf, memory_block_index_buf_, size_ * sizeof(uintptr_t) );

            // free old data
            free(memory_block_buf_);
            free(memory_block_index_buf_);

            // alloc index, correct size, memory block buf, memory ptr block buf
            alloc_index_ = size_;
            free_index_ = 0;
            size_ = size;
            memory_block_buf_ = new_memory_block_buf;
            memory_block_index_buf_ = new_memory_block_index_buf;            

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

        ListMemoryBlock_UseIndex* ptr_block = memory_block_buf_ + memory_block_index_buf_[alloc_index_];
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

    ListMemoryBlock_UseIndex* GetNext(ListMemoryBlock_UseIndex* ptr_block)
    {
        return ptr_block->index_next != TAIL_ID ? &memory_block_buf_[ptr_block->index_next] : &working_list_tail_;
    }
    ListMemoryBlock_UseIndex* GetPrev(ListMemoryBlock_UseIndex* ptr_block)
    {
        return ptr_block->index_prev != HEAD_ID ? &memory_block_buf_[ptr_block->index_prev] : &working_list_head_;
    }

    void Free(ListMemoryBlock_UseIndex* ptr_block)
    {
        Free(ptr_block->id);
    }
    void Free(unsigned int block_index)
    {
#ifdef _DEBUG
        assert(working_num_ > 0);
#endif
        memory_block_index_buf_[free_index_] = block_index;
        ListMemoryBlock_UseIndex* ptr_block = memory_block_buf_ + block_index;
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

        GetPrev(ptr_block)->index_next = ptr_block->index_next;
        GetNext(ptr_block)->index_prev = ptr_block->index_prev;
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

    void LinkToHead(ListMemoryBlock_UseIndex* ptr_block)
    {
        GetNext(&working_list_head_)->index_prev    = ptr_block->id;
        ptr_block->index_next                        = working_list_head_.index_next;
        working_list_head_.index_next                = ptr_block->id;
        ptr_block->index_prev                        = HEAD_ID;
    }
    void LinkToTail(ListMemoryBlock_UseIndex* ptr_block)
    {
        GetPrev(&working_list_tail_)->index_next    = ptr_block->id;
        ptr_block->index_prev                        = working_list_tail_.index_prev;
        working_list_tail_.index_prev                = ptr_block->id;
        ptr_block->index_next                        = TAIL_ID;
    }

    void Print()
    {
        ListMemoryBlock_UseIndex* ptr_block = GetNext(&working_list_head_);
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
            ptr_block = GetNext(ptr_block);
        }
        printf("  tail\n");

        printf("current memory pointer array status: \n");
        for (i=0; i<size_; ++i)
        {
            ListMemoryBlock_UseIndex* ptr_block = memory_block_buf_ + memory_block_index_buf_[i];
            printf("%5d[%d](%d) | ", ptr_block->id, ptr_block->status, i);
        }
        printf("\n");
#endif
        printf("\n\n");
    }
};

#endif