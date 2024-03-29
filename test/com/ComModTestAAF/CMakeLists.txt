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
set(TARGET_DIR "${AAFSDK_TARGET_DIR}/Test")

file(GLOB MODULETESTS_FILES "ModuleTests/*.cpp")
source_group("ModuleTests" FILES ${MODULETESTS_FILES})

add_executable(ComModTestAAF
    AAFWideString.cpp
    AAFWideString.h
    CAAFModuleTest.cpp
    CAAFModuleTest.h
    ComModTestAAF.cpp
    ModuleTest.cpp
    ModuleTest.h
    ${MODULETESTS_FILES}
)

# does not work with cmake 3.0.2
#target_sources(ComModTestAAF PRIVATE ${MODULETESTS_FILES})

target_include_directories(ComModTestAAF PRIVATE
    .
    ModuleTests
    ${AAFSDK_ROOT}/ref-impl/src/impl
    ${AAFSDK_ROOT}/ref-impl/src/com-api
    ${AAFSDK_ROOT}/test/com/commodtestaaf
    ${AAFSDK_ROOT}/Utilities/Include
)

target_link_libraries(ComModTestAAF PRIVATE
    AAFIID
    AAFLIB
)

set_target_properties(ComModTestAAF PROPERTIES
    VERSION ${AAFSDK_VERSION_STRING}
)


set_target_properties(ComModTestAAF PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TARGET_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TARGET_DIR}
)

add_custom_command(TARGET ComModTestAAF POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/Laser.wav
        ${TARGET_DIR}
)

add_dependencies(ComModTestAAF
    MakeSDK
)

if(WIN32)
    SET_WIN32_TEST_VARS(${TARGET_DIR} $<TARGET_FILE_NAME:ComModTestAAF>)

    add_test(NAME ComModTestAAF_Debug
             CONFIGURATIONS Debug
             WORKING_DIRECTORY ${TARGET_DIR_DEBUG}
             COMMAND ${TARGET_COMMAND_DEBUG})
    add_test(NAME ComModTestAAF-XML_Debug
             CONFIGURATIONS Debug
             WORKING_DIRECTORY ${TARGET_DIR_DEBUG}
             COMMAND ${TARGET_COMMAND_DEBUG} -e XML -s AAFRandomRawStorage)

    add_test(NAME ComModTestAAF_Release
             CONFIGURATIONS Release
             WORKING_DIRECTORY ${TARGET_DIR_RELEASE}
             COMMAND ${TARGET_COMMAND_RELEASE})
    add_test(NAME ComModTestAAF-XML_Release
             CONFIGURATIONS Release
             WORKING_DIRECTORY ${TARGET_DIR_RELEASE}
             COMMAND ${TARGET_COMMAND_RELEASE} -e XML -s AAFRandomRawStorage)
else()
    add_test(NAME ComModTestAAF
             WORKING_DIRECTORY ${TARGET_DIR}
             COMMAND ComModTestAAF)
    add_test(NAME ComModTestAAF-XML
             WORKING_DIRECTORY ${TARGET_DIR}
             COMMAND ComModTestAAF -e XML -s AAFRandomRawStorage)
    set_property(TEST ComModTestAAF PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${AAFSDK_TARGET_DIR}/RefImpl:$ENV{LD_LIBRARY_PATH}")
    set_property(TEST ComModTestAAF-XML PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${AAFSDK_TARGET_DIR}/RefImpl:$ENV{LD_LIBRARY_PATH}")
endif()
