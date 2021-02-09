SOURCE_FILES = final.cpp checker.cpp little_tools.cpp node_testing.cpp power_intent_parser.cpp \
report.cpp verilog_parser.cpp verilog_testing.cpp
OBJECT_FILES = $(SOURCE_FILES:.cpp=.obj)

# x64 / x86
TARGET_MACHINE = x64
# windows / linux
TARGET_OS = windows

TARGET_PATH = ..\bin
EXECUTABLE_NAME = checker.exe

OPTIMIZATION =
DEBUG_SYMBOL =
COMMON_WARNINGS = /Wall

CXX_WARNINGS = $(COMMON_WARNING)
CXX_STD = /std:c++17
CXX_OPTIONS = $(OPTIMIZATION) $(DEBUG_SYMBOL) $(COMMON_WARNINGS) $(CXX_WARNINGS) $(CXX_STD) /EHsc

LIBS =

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

all: $(TARGET_PATH)\$(EXECUTABLE_NAME)
	@echo "Make done!"

$(TARGET_PATH)\$(EXECUTABLE_NAME): $(OBJECT_FILES)
	$(LINK) $(OBJECT_FILES) $(LIB_PATHS) $(LIBS) /OUT:$(TARGET_PATH)\$(EXECUTABLE_NAME)

### Please manually add new rules ###
final.obj: final.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:final.obj final.cpp

checker.obj: checker.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:checker.obj checker.cpp

little_tools.obj: little_tools.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:little_tools.obj little_tools.cpp

node_testing.obj: node_testing.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:node_testing.obj node_testing.cpp

power_intent_parser.obj: power_intent_parser.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:power_intent_parser.obj power_intent_parser.cpp

report.obj: report.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:report.obj report.cpp

verilog_parser.obj: verilog_parser.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:verilog_parser.obj verilog_parser.cpp

verilog_testing.obj: verilog_testing.cpp
	$(CXX) $(CXX_OPTIONS) $(INCLUDE_PATHS) /c /Fo:verilog_testing.obj verilog_testing.cpp

.PHONY:
clean:
	$(RM) $(OBJECT_FILES)
	$(RM) $(TARGET_PATH)\$(EXECUTABLE_NAME)

