SOURCE_FILES = test.cpp verilog_parser_test.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.obj)

# x64 / x86
TARGET_MACHINE = x64
# windows / linux
TARGET_OS = windows

TARGET_PATH = bin
EXECUTABLE_NAME = test.exe

OPTIMIZATION =
DEBUG_SYMBOL =
COMMON_WARNINGS = /Wall

CXX_WARNINGS = $(COMMON_WARNING)
CXX_STD = /std:c++17
CXX_OPTIONS = $(OPTIMIZATION) $(DEBUG_SYMBOL) $(COMMON_WARNINGS) $(CXX_WARNINGS) $(CXX_STD) /EHsc

LIBS = gtest.lib

VCPKG_ROOT = G:\dev\vcpkg
VCPKG_INCLUDE_PATH = $(VCPKG_ROOT)\installed\$(TARGET_MACHINE)-$(TARGET_OS)\include
VCPKG_LIB_PATH = $(VCPKG_ROOT)\installed\$(TARGET_MACHINE)-$(TARGET_OS)\lib
VCPKG_BIN_PATH = $(VCPKG_ROOT)\installed\$(TARGET_MACHINE)-$(TARGET_OS)\bin

INCLUDE_PATHS = /I$(VCPKG_INCLUDE_PATH) /I..\src
LIB_PATHS = /LIBPATH:$(VCPKG_LIB_PATH)

CXX = cl
LINK = link
RM = del
CP = copy

all: $(TARGET_PATH)\$(EXECUTABLE_NAME) $(TARGET_PATH)\gtest.dll
	@echo "Make done!"

$(TARGET_PATH)\$(EXECUTABLE_NAME): $(OBJECT_FILES)
	$(LINK) $(OBJECT_FILES) $(LIB_PATHS) $(LIBS) /OUT:$(TARGET_PATH)\$(EXECUTABLE_NAME)

### Please manually add new rules for coping dll files ###
$(TARGET_PATH)\gtest.dll:
	$(CP) $(VCPKG_BIN_PATH)\gtest.dll $(TARGET_PATH)\

### Please manually add new rules for compiling source code ###
test.obj: test.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:test.obj test.cpp

verilog_parser_test.obj: verilog_parser_test.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:verilog_parser_test.obj verilog_parser_test.cpp

.PHONY:
clean:
	$(RM) $(OBJECT_FILES)
	$(RM) $(TARGET_PATH)\gtest.dll
	$(RM) $(TARGET_PATH)\$(EXECUTABLE_NAME)

