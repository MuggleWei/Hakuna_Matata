# variables
PROJECT_DIR := $(realpath ..)
SOURCE_DIR := $(PROJECT_DIR)/src
BUILD_DIR := $(PROJECT_DIR)/build
OBJECT_DIR := $(BUILD_DIR)/objects

ABS_SOURCES := $(shell find $(SOURCE_DIR) -name "*.c")
SOURCES := $(ABS_SOURCES:$(SOURCE_DIR)/%=%)

OBJECTS_C := $(addprefix $(OBJECT_DIR)/,$(SOURCES))
OBJECTS := $(addsuffix .o,$(OBJECTS_C))

OBJECT_DIRS := $(sort $(dir $(OBJECTS)))

# targets
.PHONY: directories
.PHONY: clean

all: directories bin/hello

########### create object directories ###########
directories: $(OBJECT_DIRS)

$(OBJECT_DIRS):
	mkdir -p $(OBJECT_DIRS)

########### sources -> objects ###########
bin/hello: $(OBJECTS)
	#$(CC) $(LDFLAGS) -o $@ $^
	mkdir -p bin
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJECTS): $(OBJECT_DIR)/%.c.o: $(SOURCE_DIR)/%.c
	$(CC) -I$(SOURCE_DIR) $(CPPFLAGS) $(CFLAGS) -o $@ -c $< 

########### clean ###########
clean:
	rm -rf objects hello
