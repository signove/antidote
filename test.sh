#!/bin/sh
####################################################################
#  Copyright (C) 2010 Signove Tecnologia Corporation.
#  All rights reserved.
#  Contact: Signove Tecnologia Corporation (contact@signove.com)
#
#  $LICENSE_TEXT$
# 
#  test.sh
#
#  Created on: Jun 16, 2010
#      Author: fabricio.silva

####################################################################

BUILD_FOLDER=test_build
TEST_REPORT_DIR=$BUILD_FOLDER/test-reports
COVERAGE_REPORT_DIR=$BUILD_FOLDER/coverage-reports 

TESTMODES=""
MEMCHECK=0
THREAD_ENABLED=0
RUNCMD=""
RUNCMD_STATUS=0
OPT_NO_BUILD=0

export HEALTHD_TMP=/tmp/hudson_healthd/

usage() {
    USAGE="Usage: `basename $0` [-m <test modes>] [-c] \n\n\
    -m \t Test mode (comma separated without spaces), values: auto, \
     console, text. Default is text \n\
       \t You can inform one test modes per time  \n\n \
    -c \t Execute the test with Memory Checking \n\n\
    -b \t Use last build compilation to run tests \n\n\
    -t \t Enable thread profiling support \n\n\
    
    Usage example: \n\
    \t test.sh -c \n
    \t test.sh -m text -c \n
    \t test.sh -c -t"
 
    echo $USAGE >&2
    exit $1
}

#################################### Functions ##################################

clean ()
{
 echo "Cleaning auto generated files \n"
 rm CUnitAutomated*
 rm -rf $BUILD_FOLDER
}

build () 
{
	clean
	
	if [ "$OPT_NO_BUILD" -eq "0" ]; then
		echo "Building project \n"
	   ./autogen.sh
	   ./configure --enable-coverage=yes
	   make clean
	   make
	   STATUS=$?
	   
	   if [ "$STATUS" -eq "0" ]; then
	     echo "\n Build finshed! \n"
	   else
		  echo "\n Sorry, cannot build the project. \n"
		  exit 1;
	   fi
   	fi
   
   mkdir -p $TEST_REPORT_DIR
   mkdir -p $COVERAGE_REPORT_DIR
   
}

gen_test_reports()
{
		
	CUNIT_RESULT=CUnitAutomated-Results.xml
	INPUT=""
	
	if [ -f $CUNIT_RESULT ]; then
		mv  $CUNIT_RESULT $TEST_REPORT_DIR
		INPUT="$TEST_REPORT_DIR/$CUNIT_RESULT"
		rm CUnitAutomated*
	fi
	
	OUTPUT=$TEST_REPORT_DIR/test_report_results.xml
  	if [ -f $INPUT ]; then
  		echo "Generating JUnit compatible xml report \n"
		xsltproc --stringparam suitename TestAll \
			--path tests/resources/cunit-report \
			-o $OUTPUT \
			tests/resources/cunit-report/cunit-to-junit.xsl \
			$INPUT
	fi
	
}


create_test_io() 
{
	#Create I/O fifos for test application
	
	if [ ! -p read_fifo  ]; then
		 mkfifo read_fifo 
	fi

	if [ ! -p write_fifo  ]; then
		 mkfifo write_fifo 
	fi
}

coverage_phase_pre_execution()
{
  lcov -q --directory src --zerocounters
}

gen_coverage_reports()
{
	lcov -q --directory src --capture --output-file coverage.info
	genhtml -q coverage.info -o $COVERAGE_REPORT_DIR
	rm coverage.info
}


#################################### Execution Phases ################################## 

run_phase_pre_execution()
{
	build
	create_test_io
	coverage_phase_pre_execution
}

run_phase_execution()
{
	VALGRIND_CMD=""

	if [ "$TESTMODES" != "" ]; then
	    echo "Test Modes: $TESTMODES"
	fi
	
	RUNCMD="./tests/main_test_suite $TESTMODES"
	
	if [ "$THREAD_ENABLED" -eq "1" ]; then

	    echo "Thread Check Activated"
	    VALGRIND_CMD="valgrind --error-exitcode=1 --tool=helgrind \
	     --read-var-info=yes "

	elif [ "$MEMCHECK" -eq "1" ]; then
        export G_SLICE=always-malloc
        export G_DEBUG=gc-friendly
	    echo "Memory Check Activated"
	    VALGRIND_CMD="valgrind --suppressions=tests/resources/memcheck.supp \
	    --error-exitcode=1 --tool=memcheck --leak-check=full " 

	fi

	RUNCMD="$VALGRIND_CMD $RUNCMD"	

	echo "Running: $RUNCMD \n\n"
	$RUNCMD
	RUNCMD_STATUS=$?
	
	return $RUNCMD_STATUS;

}

run_phase_post_execution()
{
	if [ "$RUNCMD_STATUS" -eq "0" ]; then
		gen_test_reports
		gen_coverage_reports
 	fi
}

################################## Main #######################################

# Get options

if [ $# -eq 0 ]; then
    usage 1
fi

while getopts m:tcb option; do

    case $option in
    c)  MEMCHECK=1
        ;;
    m)  TESTMODES=$(echo $OPTARG|sed 's/,/ /g')
        ;;
    b)  OPT_NO_BUILD=1
        ;; 
    t)  THREAD_ENABLED=1
        ;;
    \?) # getopts issues an error message
        usage 1
        ;;
    *)  usage 0
        ;;
    esac
done


run_phase_pre_execution

run_phase_execution

run_phase_post_execution

exit $RUNCMD_STATUS;
