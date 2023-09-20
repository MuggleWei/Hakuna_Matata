SOURCE_DIR = .
BUILD_DIR = build

BUILD_FLAGS = 
LD_FLAGS = 

BUILD_TYPE_LOWER = $(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')
ifeq ($(BUILD_TYPE_LOWER), debug)
	BUILD_FLAGS += --gstabs
endif

ifeq ($(target), x64)
	BUILD_FLAGS += --64
	LD_FLAGS = -m elf_x86_64
else ifeq ($(target), x86)
	BUILD_FLAGS += --32
	LD_FLAGS = -m elf_i386
endif

EXECUTABLE = hello
SOURCES = $(wildcard $(SOURCE_DIR)/*.s)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	ld $(LD_FLAGS) $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	as $(BUILD_FLAGS) $< -o $@ -alh=$(basename $@).lst

clean:
	rm -rf $(BUILD_DIR)
