-include Makefile.inc

## Source management
SOURCES:=$(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS:=$(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDENCIES:=$(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJECTS))

## Top-level targets
all: $(BINARY)

$(BINARY): $(OBJECTS)
	@echo "${blue}Linking product '$@'${rcol}"
	@mkdir -p $(@D)
#@$(CXX) -shared $(OBJECTS) -o $(BINARY) $(LDFLAGS)
	@ar rcs $(BINARY) $(OBJECTS)

-include $(DEPENDENCIES)

build-%: $(BINARY)
	@if [ ! -f "$(TEST_DIR)/$*.cpp" ]; then \
	echo "${red}Test case '$*' does not exists${rcol}";\
	false;\
	else \
	echo "${green}Building test object '$(BUILD_DIR)/$*.to'${rcol}"; \
	$(CXX) $(CXXFLAGS) -MMD -c $(TEST_DIR)/$*.cpp -o $(BUILD_DIR)/$*.to; \
	echo "${blue}Linking test '$(BIN_DIR)/$*'${rcol}"; \
	$(CXX) $(OBJECTS) $(BUILD_DIR)/$*.to -o $(BIN_DIR)/$* $(BINARY_LINK_FLAGS); \
	fi

run-%: build-%
	@echo "${blue}Starting test '$*'...${rcol}"
	@$(BIN_DIR)/$*

memcheck-%: build-%
	@valgrind --tool=memcheck --leak-check=full $(BIN_DIR)/$*

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

