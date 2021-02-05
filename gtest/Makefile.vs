SOURCE_FILES = test.cpp verilog_parser_test.cpp

TARGET_PATH = bin
EXECUTABLE_NAME = test

OPTIMIZATION =
DEBUG_SYMBOL =
COMMON_WARNINGS = /Wall

CXX_WARNINGS = $(COMMON_WARNING)
CXX_STD = /std:c++17
CXX_OPTIONS = $(OPTIMIZATION) $(DEBUG_SYMBOL) $(COMMON_WARNINGS) $(CXX_WARNINGS) $(CXX_STD) /EHsc

LIBS = gtest.lib

VCPKG_ROOT = G:\dev\vcpkg
VCPKG_INCLUDE_PATH = $(VCPKG_ROOT)\installed\x64-windows\include
VCPKG_LIB_PATH = $(VCPKG_ROOT)\installed\x64-windows\lib
VCPKG_BIN_PATH = $(VCPKG_ROOT)\installed\x64-windows\bin

INCLUDE_PATHS = /I$(VCPKG_INCLUDE_PATH) /I..\src
LIB_PATHS = /LIBPATH:$(VCPKG_LIB_PATH)

CXX = cl
LINK = link
RM = del
CP = copy

all: $(TARGET_PATH)\$(LIBS:.lib=.dll) $(TARGET_PATH)\$(EXECUTABLE_NAME).exe


$(TARGET_PATH)\$(LIBS:.lib=.dll):
	@$(CP) $(VCPKG_BIN_PATH)\$(LIBS:.lib=.dll) $(TARGET_PATH)\

$(TARGET_PATH)\$(EXECUTABLE_NAME).exe: $(TARGET_PATH)\$(SOURCE_FILES:.cpp=.obj)
	$(LINK) $(LIB_PATHS) $(LIBS) $(TARGET_PATH)\$(SOURCE_FILES:.cpp=.obj) /OUT:$(TARGET_PATH)\$(EXECUTABLE_NAME).exe

$(TARGET_PATH)\test.obj: test.cpp
	$(CXX) test.cpp $(CXX_OPTIONS) $(INCLUDE_PATHS) /Fo:$(TARGET_PATH)\test.obj

.PHONY:
clean:
	$(RM) $(TARGET_PATH)\$(LIBS:.lib=.dll)
	$(RM) $(TARGET_PATH)\$(SOURCE_FILES:.cpp=.obj)
	$(RM) $(TARGET_PATH)\$(EXECUTABLE_NAME).exe
