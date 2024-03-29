###############################################################################
#
# $Id$ $Name$
#
# The contents of this file are subject to the AAF SDK Public Source
# License Agreement Version 2.0 (the "License"); You may not use this
# file except in compliance with the License.  The License is available
# in AAFSDKPSL.TXT, or you may obtain a copy of the License from the
# Advanced Media Workflow Association, Inc., or its successor.
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
# the License for the specific language governing rights and limitations
# under the License.  Refer to Section 3.3 of the License for proper use
# of this Exhibit.
#
# WARNING:  Please contact the Advanced Media Workflow Association,
# Inc., for more information about any additional licenses to
# intellectual property covering the AAF Standard that may be required
# to create and distribute AAF compliant products.
# (http://www.amwa.tv/policies).
#
# Copyright Notices:
# The Original Code of this file is Copyright 1998-2012, licensor of the
# Advanced Media Workflow Association.  All rights reserved.
#
# The Initial Developer of the Original Code of this file and the
# licensor of the Advanced Media Workflow Association is
# Avid Technology.
# All rights reserved.
#
###############################################################################

cmake_minimum_required(VERSION 3.6.0)

if(NOT DEFINED AAFSDK_ROOT)
    message(FATAL_ERROR "'AAFSDK_ROOT' must be set. Run a build from the top-level CMakeLists.txt script.")
endif()

# MSVC: ./AAFWinSDK/<vsXX>/<Win32|x64>/<Debug|Release>/Test
# Makefile: ./AAFx86_64LinuxSDK/g++/bin/<debug|>
set(TARGET_DIR "${AAFSDK_TARGET_DIR}/Test")

add_executable(XMLStoredFormat
    TestCharacter.cpp
    TestEnum.cpp
    TestExampleFiles.cpp
    TestExtEnum.cpp
    TestFixedArray.cpp
    TestIndirect.cpp
    TestInteger.cpp
    TestOpaque.cpp
    TestRecord.cpp
    TestRename.cpp
    TestSet.cpp
    TestStream.cpp
    TestString.cpp
    TestVarArray.cpp
    TestWeakRef.cpp
    XMLStoredFormatTest.cpp
)

target_link_libraries(XMLStoredFormat PRIVATE
    AAFIID
    AAFLIB
)

set_target_properties(XMLStoredFormat PROPERTIES
    VERSION ${AAFSDK_VERSION_STRING}
)

set_target_properties(XMLStoredFormat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TARGET_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TARGET_DIR}
)

set_target_properties(XMLStoredFormat PROPERTIES
    OUTPUT_NAME "XMLStoredFormatTest"
    OUTPUT_NAME_DEBUG "XMLStoredFormatTest"
    OUTPUT_NAME_RELEASE "XMLStoredFormatTest"
)

add_custom_command(TARGET XMLStoredFormat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/input
        ${TARGET_DIR}/input
)

if(WIN32)
    SET_WIN32_TEST_VARS(${TARGET_DIR} $<TARGET_FILE_NAME:XMLStoredFormat>)

    add_test(NAME XMLStoredFormat_Debug
             CONFIGURATIONS Debug
             WORKING_DIRECTORY ${TARGET_DIR_DEBUG}
             COMMAND ${TARGET_COMMAND_DEBUG})

    add_test(NAME XMLStoredFormat_Release
             CONFIGURATIONS Release
             WORKING_DIRECTORY ${TARGET_DIR_RELEASE}
             COMMAND ${TARGET_COMMAND_RELEASE})
else()
    add_test(NAME XMLStoredFormat
             WORKING_DIRECTORY ${TARGET_DIR}
             COMMAND XMLStoredFormat)
    set_property(TEST XMLStoredFormat PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${AAFSDK_TARGET_DIR}/RefImpl:$ENV{LD_LIBRARY_PATH}")
endif()
