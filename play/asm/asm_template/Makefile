SOURCE_DIR = .
BUILD_DIR = build

EXECUTABLE = hello
SOURCES = $(wildcard $(SOURCE_DIR)/*.s)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.s,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	ld $^ -o $@

$(OBJECTS): $(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	as --gstabs $< -o $@ -alh=$(basename $@).lst

clean:
	rm -rf $(BUILD_DIR)
