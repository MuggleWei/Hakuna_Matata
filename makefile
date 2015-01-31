Target_File = unit_test_memory_aligned\
	unit_test_ptr_hide_data\
	unit_test_memory_pool\
	unit_test_memory_pool_use_index

all: $(Target_File)

unit_test_memory_aligned: memory_aligned.o
unit_test_ptr_hide_data: memory_aligned.o

.PHONY: clean
clean:
	rm -f *.o $(Target_File)