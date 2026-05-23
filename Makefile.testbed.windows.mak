DIR := $(subst /,\\,$(CURDIR)) # substitute / with '\'
BUILD_DIR := bin
OBJ_DIR := obj

ASSEMBLY := testbed
EXTENSION := .exe
COMPILER_FLAGS := -g -Wno-missing-braces -fdeclspec # -fPIC
INCLUDE_FLAGS := -Iengine\src -Itestbed\src
LINKER_FLAGS := -g -lengine.lib -L$(OBJ_DIR)\engine -L$(BUILD_DIR)
DEFINES := -D_DEBUG -DRQ_IMPORT

# Make does not offer a recursive wildcard function, so here's one:
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRC_FILES := $(call rwildcard,$(ASSEMBLY)/,*.c) # Get all .c files.
DIRECTORIES := $(ASSEMBLY)\src $(subst $(DIR),,$(shell dir $(ASSEMBLY)\src /S /AD /B | findstr /i src)) # Get all directories under src.
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o) # Get all compiled .c.o objects for engine

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding folder build structure...
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo Done.

	
.PHONY: link
link: scaffold $(OBJ_FILES) # link
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile 
compile: # compile .c files
	@echo Compiling...

.PHONY: clean
clean: # clean build directory
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)\$(ASSEMBLY)

$(OBJ_DIR)/%.c.o: %.c
	@echo $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)