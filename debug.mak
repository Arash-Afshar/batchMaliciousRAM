#Generated by VisualGDB (http://visualgdb.com)
#DO NOT EDIT THIS FILE MANUALLY UNLESS YOU ABSOLUTELY NEED TO
#USE VISUALGDB PROJECT PROPERTIES DIALOG INSTEAD

BINARYDIR := Debug

#Toolchain
CC := gcc
CXX := g++
LD := $(CXX)
AR := ar
OBJCOPY := objcopy

#Additional flags
PREPROCESSOR_MACROS := NDEBUG RELEASE
INCLUDE_DIRS := ./libBDX ./libBDXTests ./thirdparty/linux/boost/includes
LIBRARY_DIRS := ./thirdparty/linux/boost/stage/lib ./thirdparty/linux/cryptopp ./thirdparty/linux/Miracl/source ./thirdparty/linux/mpir/.libs ./thirdparty/linux/ntl/src ./Debug
LIBRARY_NAMES := libBDX libBDXTests boost_system boost_filesystem boost_thread mpir ntl miracl cryptopp pthread rt
ADDITIONAL_LINKER_INPUTS := 
MACOS_FRAMEWORKS := 
LINUX_PACKAGES := 

CFLAGS := -ggdb -ffunction-sections -O0 -Wall -std=c++11 -maes -msse2 -msse4.1 -mpclmul -Wfatal-errors -pthread
CXXFLAGS := -ggdb -ffunction-sections -O0 -Wall -std=c++11 -maes -msse2 -msse4.1 -mpclmul -Wfatal-errors -pthread
ASFLAGS := 
LDFLAGS := -Wl,-gc-sections
COMMONFLAGS := 
LINKER_SCRIPT := 

START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group

#Additional options detected from testing the toolchain
IS_LINUX_PROJECT := 1
