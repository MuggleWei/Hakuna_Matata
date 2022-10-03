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

# implicit variables
LOCAL_USR = ${HOME}/c_pkg

CPPFLAGS = \
	-I$(SOURCE_DIR) \
	-I/usr/include \
	-I/usr/local/include \
	-I$(LOCAL_USR)/include \
	-DMUGGLE_HOLD_LOG_MACRO

LDFLAGS = \
	-L/usr/lib \
	-L/usr/local/lib \
	-L$(LOCAL_USR)/lib
	
LDLIBS = \
	-lmugglec

# targets
.PHONY: directories
.PHONY: clean

all: directories bin/hello bin/hello_standalone

########### create object directories ###########
directories: $(OBJECT_DIRS)

$(OBJECT_DIRS):
	mkdir -p $(OBJECT_DIRS)

########### sources -> objects ###########
bin/hello: $(OBJECTS)
	mkdir -p bin
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^ 

bin/hello_standalone: $(OBJECTS)
	mkdir -p bin
	$(CC) -static -o $@ $^ $(LOCAL_USR)/lib/static/libmugglec.a

$(OBJECTS): $(OBJECT_DIR)/%.c.o: $(SOURCE_DIR)/%.c
	$(CC) -I$(SOURCE_DIR) $(CPPFLAGS) $(CFLAGS) -o $@ -c $< 

########### clean ###########
clean:
	rm -rf objects bin
