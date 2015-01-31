all: build_all

memory_aligned.out: memory_aligned.c unit_test_memory_aligned.c
	gcc -o memory_aligned.out memory_aligned.c unit_test_memory_aligned.c

ptr_hide.out: unit_test_ptr_hide_data.c memory_aligned.c
	gcc -o ptr_hide.out unit_test_ptr_hide_data.c memory_aligned.c

memory_pool.out: unit_test_memory_pool.cpp
	g++ -o memory_pool.out unit_test_memory_pool.cpp

memory_pool_use_index.out: unit_test_memory_pool_use_index.cpp
	g++ -o memory_pool_use_index.out unit_test_memory_pool_use_index.cpp

.PHONY: build_all clean
build_all: \
	memory_aligned.out\
	ptr_hide.out\
	memory_pool.out\
	memory_pool_use_index.out

clean:
	rm -f *.o *.out *.exe