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

add_subdirectory(axDump)
add_subdirectory(axExample)
add_subdirectory(axFileGen)
add_subdirectory(axHrMap)
add_subdirectory(axLib)
add_subdirectory(axMasterMobEx)
add_subdirectory(axPlugin)
add_subdirectory(axPluginInfo)

project(MakeSDKExamples2 LANGUAGES CXX)

add_custom_target(MakeSDKExamples2)

add_dependencies(MakeSDKExamples2
    axDump
    axExample
    axFileGen
    axLib
    axMasterMobEx
    axPluginInfo
)
