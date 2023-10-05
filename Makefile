
CC=clang
CFLAGS=-std=c17 -pedantic -pedantic-errors -Wall -Wextra -Werror

CXX=clang++
SPECIAL_WARNINGS_FLAGS=-Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wfloat-equal -Wcast-qual -Wextra-semi
WARNINGS_FLAGS=-pedantic-errors -Wall -Wextra $(SPECIAL_WARNINGS_FLAGS) -Werror
CXXFLAGS=-std=c++17 $(WARNINGS_FLAGS)

# name of the dir with SFML includes, frameworks, libs and extlibs
SFML_ROOT=SFML_2_6_0

INCLUDE_DIR=include/

C_SOURCES=$(shell find src -type f -name "*.c")
CPP_SOURCES=$(shell find src -type f -name "*.cpp")
M_SOURCES=$(shell find src -type f -name "*.m")
OBJECTS=$(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o) $(M_SOURCES:.m=.o)

EXECUTABLE=NetworkFourier21

NFD_FRAMEWORKS=-framework Foundation -framework AppKit -framework UniformTypeIdentifiers

FRAMEWORKS_DIR=$(SFML_ROOT)/Frameworks/
FRAMEWORKS=-framework SFML -framework sfml-graphics -framework sfml-system -framework sfml-window

DYLIBS=-lobjc

# bundle variables

MACOS_BUNDLE_NAME=NetworkFourier.app
MACOS_BUNDLE_EXECUTABLE_NAME=NetworkFourier
MACOS_FRAMEWORKS=$(shell find $(SFML_ROOT)/extlibs $(SFML_ROOT)/Frameworks -type d -name "*.framework")
DATA_FILES=$(shell echo data/*)
MACOS_BUNDLE_DATA_DIR=bundle_data
INFO_PLIST_FILE=$(MACOS_BUNDLE_DATA_DIR)/Info.plist
PKGINFO_FILE=$(MACOS_BUNDLE_DATA_DIR)/PkgInfo

all: $(OBJECTS)
	$(CXX) -v -o $(EXECUTABLE) $(OBJECTS) -L$(SFML_ROOT)/lib/ -F$(FRAMEWORKS_DIR) $(FRAMEWORKS) $(NFD_FRAMEWORKS) $(DYLIBS)
	rm -rf $(MACOS_BUNDLE_NAME)
	mkdir -p $(MACOS_BUNDLE_NAME)/Contents
	cp $(INFO_PLIST_FILE) $(PKGINFO_FILE) $(MACOS_BUNDLE_NAME)/Contents
	mkdir -p $(MACOS_BUNDLE_NAME)/Contents/Frameworks
	cp -R $(MACOS_FRAMEWORKS) $(MACOS_BUNDLE_NAME)/Contents/Frameworks/
	mkdir -p $(MACOS_BUNDLE_NAME)/Contents/MacOS
	cp $(EXECUTABLE) $(MACOS_BUNDLE_NAME)/Contents/MacOS/$(MACOS_BUNDLE_EXECUTABLE_NAME)
	mkdir -p $(MACOS_BUNDLE_NAME)/Contents/Resources/data
	cp -Rf $(DATA_FILES) $(MACOS_BUNDLE_NAME)/Contents/Resources/data
	install_name_tool -add_rpath @executable_path/../Frameworks $(MACOS_BUNDLE_NAME)/Contents/MacOS/$(MACOS_BUNDLE_EXECUTABLE_NAME)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -rf $(MACOS_BUNDLE_NAME)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ -I$(INCLUDE_DIR) -I$(SFML_ROOT)/include $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ -I$(INCLUDE_DIR) -I$(SFML_ROOT)/include $<

.m.o:
	$(CC) $(CFLAGS) -c -o $@ -I$(INCLUDE_DIR) -I$(SFML_ROOT)/include $<
