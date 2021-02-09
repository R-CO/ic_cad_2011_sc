SOURCE_FILES = final.cpp checker.cpp little_tools.cpp node_testing.cpp power_intent_parser.cpp \
report.cpp verilog_parser.cpp verilog_testing.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

# x64 / x86
TARGET_MACHINE = x64
# windows / linux
TARGET_OS = linux

TARGET_PATH = ../bin
EXECUTABLE_NAME = checker

OPTIMIZATION =
DEBUG_SYMBOL =
COMMON_WARNINGS = -Wall -Weffc++

CXX_WARNINGS = $(COMMON_WARNING)
CXX_STD = -std=c++17
CXX_OPTIONS = $(OPTIMIZATION) $(DEBUG_SYMBOL) $(COMMON_WARNINGS) $(CXX_WARNINGS) $(CXX_STD)

LIBS = -lpthread

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
final.o: final.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o final.o final.cpp

checker.o: checker.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o checker.o checker.cpp

little_tools.o: little_tools.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o little_tools.o little_tools.cpp

node_testing.o: node_testing.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o node_testing.o node_testing.cpp

power_intent_parser.o: power_intent_parser.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o power_intent_parser.o power_intent_parser.cpp

report.o: report.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o report.o report.cpp

verilog_parser.o: verilog_parser.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o verilog_parser.o verilog_parser.cpp

verilog_testing.o: verilog_testing.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) -c -o verilog_testing.o verilog_testing.cpp

.PHONY:
clean:
	$(RM) $(OBJECT_FILES)
	$(RM) $(TARGET_PATH)/$(EXECUTABLE_NAME)