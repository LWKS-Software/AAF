#! /bin/bash

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
# The Original Code of this file is Copyright 1998-2009, licensor of the
# Advanced Media Workflow Association.  All rights reserved.
#
# The Initial Developer of the Original Code of this file and the
# licensor of the Advanced Media Workflow Association is
# Avid Technology.
# All rights reserved.
#
###############################################################################

# Terry Skotz
# modified by Tom Ransdell 15-DEC-1999, 05-JAN-2000.

# VS10 - Set the COMPILER variable to vs10/Win32 or vs10/x64 before running.
# e.g.:
# AAFPLATFORM=AAFWinSDK COMPILER=vs10/x64 ./RunModTestAndExamples.bash -r -a
#

CHECK_DEBUG=0
CHECK_RELEASE=0
ESSENCETEST=0
MODULETEST=0
TIMDUMP=0
CLIENTTEST=0
CUTSTEST=0
##PROPDIRECTDUMP=0
PROPDIRECTACCESS=0
CREATESEQUENCE=0
ESSENCEACCESS=0
##AAFINFO=0
AAFOMFTEST=0
EXPORTAUDIOEXAMPLE=0
EXPORTSIMPLECOMPOSITION=0
IMPORTAUDIOEXAMPLE=0
PERSONNELEXTENSIONTEST=0
PERSONNELEXTENSIONDIFF=0
FILEEXTVERIFY=0
ALL=0
PRINTPATH=0
AAFWATCHDOG=0

#WATCHDOGLOGFILE=D:/AAFWatchDog/AAFWatchDog.log
WATCHDOGLOGFILE=/tmp/AAFWatchDog/AAFWatchDog.log

# this script must be run from the AAF root (which is where it is checked in)
AAFBASEDIR=`pwd`

# set AAFPLATFORM using same script as for make
if [ -z $AAFPLATFORM ]; then
    AAFPLATFORM="AAF`build/aafplatform.sh`SDK"
fi

# set default COMPILER 
# for Win to vs10
# otherwise g++

if [ -z $COMPILER ] ; then
    if [ "$AAFPLATFORM" == "AAFWinSDK" ] ; then
	COMPILER="vs10/Win32"
    else
	COMPILER="g++"
    fi
fi

PrintHelp ()
{
	printf "\nTo run module test and all examples for specified target, use either:\n"
	echo "-r  = Release"
	echo "-d  = Debug"
	printf "\nTo select a compiler other than the default, use:\n"
	echo "-c name   for example -c vs8"
	printf "\nTo specify which tests to run, add any of the following:\n"
	echo "-a  = Run ALL Tests"
	echo "-ao = AafOmf"
##	echo "-i  = ComAAFInfo"
	echo "-cl = ComClientTest"
	echo "-cu = ComCutsTest"
	echo "-e  = ComEssenceDataTest"
	echo "-m  = ComModAAF"
	echo "-pd = ComPropDirectDump"
	echo "-pa = ComPropDirectAccess" 
	echo "-ea = EssenceAccess"
	echo "-cs = CreateSequence"
	echo "-pe = Personnel Extension tests"
	echo "-eae = ExportAudioExample"
	echo "-esc = ExportSimpleComposition"
	echo "-iae = ImportAudioExample"
	echo "-ext = dev util file extension independence test"
	echo "-t  = dump\n\n"
	echo "-s  = update AAFWatchDog.log with exit code results"
	echo "-p = Print PATH variable\n\n"
}


#####################
# Parse Command Line
#####################
if [ $# -eq 0 ]; then
	PrintHelp
	exit 1
elif [ $# -eq 1 ]; then
	if [ $1 = "-d" ] || [ $1 = "-r" ]; then
		ALL=1;
	fi
fi

until [ $# = 0 ]
do
	case $1 in
		-c ) shift; COMPILER="$1" ;;
		-a ) ALL=1;;
		-r ) CHECK_RELEASE=1 ;;
		-d ) CHECK_DEBUG=1 ;;
		-e ) ESSENCETEST=1;;
		-m ) MODULETEST=1;;
		-t ) TIMDUMP=1;;
		-cl ) CLIENTTEST=1;;
		-cu ) CUTSTEST=1;;
##		-pd ) PROPDIRECTDUMP=1;;
		-pa ) PROPDIRECTACCESS=1;;
		-ea) ESSENCEACCESS=1;;
		-cs ) CREATESEQUENCE=1;;
##		-i ) AAFINFO=1;;
		-ao ) AAFOMFTEST=1;;
		-pe ) PERSONNELEXTENSIONTEST=1;;
		-eae ) EXPORTAUDIOEXAMPLE=1;;
		-esc ) EXPORTSIMPLECOMPOSITION=1;;
		-iae ) IMPORTAUDIOEXAMPLE=1;;
	        -ext ) FILEEXTVERIFY=1;;
		-p ) PRINTPATH=1;;
		-s ) AAFWATCHDOG=1;;
		-h ) PrintHelp
			 exit 1 ;;
	esac
	shift
done

if [ ! -d "$AAFBASEDIR/$AAFPLATFORM/$COMPILER" ] ; then
	echo "$AAFBASEDIR/$AAFPLATFORM/$COMPILER not found"
	exit 1
fi

OLD_PATH=""

STATUS=0

RESULTS=""

CheckExitCode ()
{
	ExitCode=$1
	PROGRAM=$2

	#Generate list of all the results
	RESULTS="${RESULTS}$ExitCode  $PROGRAM\n"

	if [ $ExitCode -ne 0 ]; then
		STATUS=1
	fi
}


SetPath ()
{
	New=$1

	printf "\nAdding $New/RefImpl to PATH\n"
	OLD_PATH=$PATH
	PATH="${New}/RefImpl:$PATH"
}


ResetPath ()
{
	printf "\nResetting PATH\n"
	PATH=$OLD_PATH
}


PrintSeparator ()
{
	TEXT=$1
	printf "\n\n"
	printf "****************************************************************************\n"
	printf "$TEXT\n"
	printf "****************************************************************************\n\n"
}


VerifyFiles ()
{
	FileList=$1

	RemoveIfExists ()
	{
		FileToBeRemove=$1

		if [ -a $FileToBeRemove ]; then
			rm $FileToBeRemove
		fi
	}

	printf "\nRunning Files thru validation suite"

	for File in $FileList; do
		printf "\n ${File}"

		RemoveIfExists ${File}.Info.log
		${ExamplesDir}/ComAAFInfo "$File" > tempdump.log
		Stat=$?
		printf "  $Stat    ComAAFInfo.exe"
		if [ $Stat -ne 0 ]; then
			CheckExitCode $Stat   "     $File  <- ComAAFInfo.exe reported error.  See ${File}.Info.log"
			mv tempdump.log ${File}.Info.log
		fi

		RemoveIfExists ${File}.dump.log
		${DumpDir}/dump -s -p "$File" > tempdump.log
		Stat=$?
		printf "  $Stat    dump.exe"
		if [ $Stat -ne 0 ]; then
			CheckExitCode $Stat   "     $File  <- dump.exe reported error.  See ${File}.dump.log"
			mv tempdump.log ${File}.dump.log
		fi

		RemoveIfExists ${File}.CPDDump.log
		${ExamplesDir}/ComPropDirectDump "$File" > tempdump.log
		Stat=$?
		printf "  $Stat    ComPropDirectDump.exe"
		if [ $Stat -ne 0 ]; then
			CheckExitCode $Stat   "     $File  <- ComPropDirectDump.exe reported error.  See ${File}.CPDDump.log"
			mv tempdump.log ${File}.CPDDump.log
		fi

#		RemoveIfExists ${File}.CPDAccess.log
#		${ExamplesDir}/ComPropDirectAccess "$File" > tempdump.log
#		Stat=$?
#		printf "  $Stat    ComPropDirectAccess.exe"
#		if [ $Stat -ne 0 ]; then
#			CheckExitCode $Stat   "     $File  <- ComPropDirectAccess.exe reported error.  See ${File}.CPDAccess.log"
#			mv tempdump.log ${File}.CPDAccess.log
#		fi

	done

	
	RemoveIfExists tempdump.log
}


RunMainScript ()
{
	Target=$1
	SetPath "$AAFBASEDIR/$AAFPLATFORM/$COMPILER/${Target}"

	START_DIR="`PWD`"

	printf "\n\nStartDir: $START_DIR \nPlatform: $AAFPLATFORM \nCompiler: $COMPILER \nTarget:   $Target \n"
	
	cd "$AAFBASEDIR/$AAFPLATFORM/$COMPILER/${Target}"
	
	TargetDir="`PWD`"
	DumpDir="`PWD`/DevUtils"
	ExamplesDir="`PWD`/Examples/Com"
	UtilitiesDir="`PWD`/Utilities"

	if [ $PRINTPATH -eq 1 ]; then 
		printf "PATH = $PATH \n\n" 
		printf "cd $TargetDir \n\n"
	fi

	if [ $MODULETEST -eq 1 ] || [ $ALL -eq 1 ]; then
		cd Test
		cp $AAFBASEDIR/Test/Com/ComModTestAAF/Laser.wav .
		./ComModAAF
		CheckExitCode $? "ComModAAF"
		
		VerifyFiles "`ls *.aaf`"

		cd $TargetDir
	fi

	# AAFOMFTEST is not executed when $ALL is set because it is
	# not built as part of the standard SDK. This remains here for
	# historical purposes in the event someone continues to use it
	# privately. Note, The OMF library that is necessary to build
	# the AafOmf source code remains available for download from
	# AMWA.  See the ReleaseNotes.txt comments.
	if [ $AAFOMFTEST -eq 1 ]; then
		PrintSeparator "AafOmf Convertor Test 1 -  AAF -> OMF"
		cd $AAFBASEDIR/AAFWinSDK/Debug/Utilities
                
		##cp ../AAFWinSDK/Debug/U/AAFSequenceTest.aaf .

		# Copy the correct omf dll if necessary.
		if [ $CHECK_DEBUG -eq 1 ]; then
		    OMFTOOLKITDLL="omfToolkitd.dll"
		else
		    OMFTOOLKITDLL="omfToolkit.dll"  
		fi
		if [ ! -f $OMFTOOLKITDLL ] || [ $OMFTOOLKITDLL -ot "../../../OMF/$OMFTOOLKITDLL" ]; then
		    #echo "cp ../../../Omf/$OMFTOOLKITDLL `PWD`"
		    cp "../../../Omf/$OMFTOOLKITDLL" .
	    	#else
		    #echo "$OMFTOOLKITDLL is up-to-date"
		fi

		./AafOmf -omf AAFSequenceTest.aaf
		CheckExitCode $? "AafOmf Convertor Test 1 -  AAF -> OMF"

		VerifyFiles "AAFSequenceTest.aaf"

		PrintSeparator "AafOmf Convertor Test 2 -  OMF -> AAF"
		cp ../../Utilities/AafOmf/Debug/Complx2x.omf .
		./AafOmf Complx2x.omf
		CheckExitCode $? "AafOmf Convertor Test 2 -  OMF -> AAF"

		cd $TargetDir
	fi

	if [ $CLIENTTEST -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running COMClientAAF"
		cd Examples/com
		./COMClientAAF
		CheckExitCode $? "COMClientAAF"

		VerifyFiles "`ls *.aaf`"

		cd $TargetDir
	fi

	if [ $CUTSTEST -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ComCutsTestAAF"
		cd Examples/com
		./ComCutsTestAAF
		CheckExitCode $? "ComCutsTestAAF"

		VerifyFiles "CutsOnly.aaf"

		cd $TargetDir
	fi

	if [ $PROPDIRECTACCESS -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ComPropDirectAccess"
		cd Examples/com
		./ComPropDirectAccess
		CheckExitCode $? "ComPropDirectAccess"

		VerifyFiles "PropAccess.aaf"

		cd $TargetDir
	fi

	if [ $ESSENCETEST -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ComEssenceDataTest"
		cd Examples/com
		cp $AAFBASEDIR/Test/Com/ComModTestAAF/Laser.wav .
		./ComEssenceDataTest
		CheckExitCode $? "ComEssenceDataTest"

		VerifyFiles "EssenceTest.aaf"
		VerifyFiles "ExternalAAFEssence.aaf"

		cd $TargetDir
	fi

	
	if [ $CREATESEQUENCE -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running CreateSequence"
		cd Test
		./CreateSequence 10 CreateSequence10
		CheckExitCode $? "CreateSequence" 

		VerifyFiles "CreateSequence10.aaf"

		cd $TargetDir
	fi

	if [ $ESSENCEACCESS -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running EssenceAccess"
		cd Test
		./EssenceAccess 10
		CheckExitCode $? "EssenceAccess" 

		VerifyFiles "ExternalAAFEssence.aaf"
		VerifyFiles "ExternalStandardAAF.aaf"
		VerifyFiles "InternalMulti.aaf"
		VerifyFiles "InternalStandard.aaf"

		cd $TargetDir
	fi

	if [ $PERSONNELEXTENSIONTEST -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ComExtensionWrite"
		cd Examples/com
		
		./ComExtensionWrite > ComExtensionWrite.txt
		CheckExitCode $? "ComExtensionWrite"
		cat ComExtensionWrite.txt

		VerifyFiles "extension.aaf"

		PrintSeparator "Running ComPersonnelPluginWrite"

		./ComPersonnelPluginWrite > ComPersonnelPluginWrite.txt
		CheckExitCode $? "ComPersonnelPluginWrite"
		cat ComPersonnelPluginWrite.txt

		VerifyFiles "extensionPlugin.aaf"

		PrintSeparator "Comparing the output from ComExtensionWrite and ComPersonnelPluginWrite"

		# Strip off the ONLY lines that should be different between the output
		# of ComExtensionWrite and ComPersonnelPluginWrite.
		cat ComExtensionWrite.txt | grep -v 'extension\.aaf' > ComExtensionWrite1.txt
		cat ComPersonnelPluginWrite.txt | grep -v 'extensionPlugin\.aaf' > ComPersonnelPluginWrite1.txt
		diff ComExtensionWrite1.txt ComPersonnelPluginWrite1.txt
		Stat=$?
		if [ Stat -ne 0 ]; then
		    CheckExitCode $Stat "diff ComExtensionWrite1.txt ComPersonnelPluginWrite1.txt"
		    echo "Edit sources in ComExtensionWrite and ComPersonnelPluginWrite"
		    echo "until diff ComExtensionWrite1.txt ComPersonnelPluginWrite1.txt"
		    echo "detects no differences."
		else
		    echo "Succeeded."
		fi

		cd $TargetDir
	fi

	
	if [ $EXPORTAUDIOEXAMPLE -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ExportAudioExample"
		cd Examples/Com
		cp $AAFBASEDIR/Test/Com/ComModTestAAF/Laser.wav .
		./ExportAudioExample
		CheckExitCode $? "ExportAudioExample" 

		VerifyFiles "ExportAudioExample.aaf"

		cd $TargetDir
	fi

	
	if [ $EXPORTSIMPLECOMPOSITION -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ExportSimpleComposition"
		cd Examples/Com
		./ExportSimpleComposition
		CheckExitCode $? "ExportSimpleComposition" 

		VerifyFiles "ExportSimpleComposition.aaf"

		cd $TargetDir
	fi

	
	if [ $IMPORTAUDIOEXAMPLE -eq 1 ] || [ $ALL -eq 1 ]; then
		PrintSeparator "Running ImportAudioExample"
		cd Examples/Com
		./ImportAudioExample
		CheckExitCode $? "ImportAudioExample" 

		VerifyFiles "ExportAudioExample.aaf"

		cd $TargetDir
	fi


	if [ $FILEEXTVERIFY -eq 1 ] || [ $ALL -eq 1 ]; then
	    PrintSeparator "Running ComFileExtDevUtilVerify.sh"
	    # TBC
	fi

	cd $START_DIR

	ResetPath
}


PrintExitCodes ()
{
	ExTarget=$1

	printf "\nPrinting $ExTarget Test Exit Codes:\n$RESULTS"

	if [ $AAFWATCHDOG -eq 1 ]; then
		if [ -e $WATCHDOGLOGFILE ]; then
		    printf "\nPrinting $ExTarget Test Exit Codes:\n$RESULTS" >> $WATCHDOGLOGFILE
		    printf "\nAll Generated Test and Example files have been run thru the following programs:" >> $WATCHDOGLOGFILE
		    printf "ComAAFInfo, dump, ComPropDirectDump" >> $WATCHDOGLOGFILE
#		    printf "ComAAFInfo, dump, ComPropDirectDump, ComPropDirectAccess" >> $WATCHDOGLOGFILE
		else
		    printf "Watchdog file not found: $WATCHDOGLOGFILE\n"
  	        fi
		
	fi
}



if [ $CHECK_DEBUG -eq 1 ]; then
	RunMainScript "Debug"
	PrintExitCodes "Debug"
fi


if [ $CHECK_RELEASE -eq 1 ]; then
	RunMainScript "Release"
	PrintExitCodes "Release"
fi



exit $STATUS
