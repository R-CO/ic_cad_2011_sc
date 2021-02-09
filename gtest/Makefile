SOURCE_FILES = test.cpp verilog_parser_test.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

# x64 / x86
TARGET_MACHINE = x64
# windows / linux
TARGET_OS = linux

TARGET_PATH = bin
EXECUTABLE_NAME = test

OPTIMIZATION =
DEBUG_SYMBOL =
COMMON_WARNINGS = -Wall

CXX_WARNINGS = $(COMMON_WARNING)
CXX_STD = -std=c++17
CXX_OPTIONS = $(OPTIMIZATION) $(DEBUG_SYMBOL) $(COMMON_WARNINGS) $(CXX_WARNINGS) $(CXX_STD)

LIBS = -lgtest -lpthread

VCPKG_ROOT = ~/vcpkg
VCPKG_INCLUDE_PATH = $(VCPKG_ROOT)/installed/$(TARGET_MACHINE)-$(TARGET_OS)/include
VCPKG_LIB_PATH = $(VCPKG_ROOT)/installed/$(TARGET_MACHINE)-$(TARGET_OS)/lib
VCPKG_BIN_PATH = $(VCPKG_ROOT)/installed/$(TARGET_MACHINE)-$(TARGET_OS)/bin

INCLUDE_PATHS = -I$(VCPKG_INCLUDE_PATH) -I../src
LIB_PATHS = -L$(VCPKG_LIB_PATH)

CXX = g++
LINK = g++
RM = rm
CP = cp

all: $(TARGET_PATH)/$(EXECUTABLE_NAME)
	@echo "Make done!"

$(TARGET_PATH)/$(EXECUTABLE_NAME): $(OBJECT_FILES)
	$(LINK) $(OBJECT_FILES) $(LIB_PATHS) $(LIBS) -o $(TARGET_PATH)/$(EXECUTABLE_NAME)


### Please manually add new rules ###
test.o: test.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o test.o test.cpp

verilog_parser_test.o: verilog_parser_test.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o verilog_parser_test.o verilog_parser_test.cpp
	

.PHONY:
clean:
	$(RM) $(OBJECT_FILES)
	$(RM) $(TARGET_PATH)/$(EXECUTABLE_NAME)
