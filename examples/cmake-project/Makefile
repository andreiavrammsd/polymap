BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: build
build:
	$(CMAKE) -S . -B $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR)