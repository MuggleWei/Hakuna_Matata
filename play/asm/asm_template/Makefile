SOURCE_DIR = .
BUILD_DIR = build

BUILD_TYPE_LOWER = $(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')
ifeq ($(BUILD_TYPE_LOWER), debug)
	BUILD_FLAGS = --gstabs
endif

EXECUTABLE = hello
SOURCES = $(wildcard $(SOURCE_DIR)/*.s)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	ld $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	as $(BUILD_FLAGS) $< -o $@ -alh=$(basename $@).lst

clean:
	rm -rf $(BUILD_DIR)
