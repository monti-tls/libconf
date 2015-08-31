-include Makefile.inc

## Source management
SOURCES:=$(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS:=$(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDENCIES:=$(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJECTS))

OBJECTS+=$(CUOBJECTS)

## Top-level targets
all: $(BINARY)

$(BINARY): $(OBJECTS)
	@echo "${blue}Linking product '$@'${rcol}"
	@mkdir -p $(@D)
	@$(CXX) $(OBJECTS) -o $(BINARY) $(LDFLAGS)

run: $(BINARY)
	$(BINARY)

memcheck: $(BINARY)
	valgrind --tool=memcheck --leak-check=full $(BINARY)

-include $(DEPENDENCIES)

## Translation rules
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@echo "${green}Building object file '$@'${rcol}"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

## Phony targets
.PHONY: clean
clean:
	@echo "${blue}Removing build directories${rcol}"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

