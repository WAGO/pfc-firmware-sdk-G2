#!/bin/bash

. unittest-common.sh

while getopts ":r" opt; do
    case $opt in
        r)
            export SHUNIT_OUTPUTDIR=./test-reports
            mkdir -p "${SHUNIT_OUTPUTDIR}"
            ;;
    esac
done

OPTIND=1

TESTCASES="delete-unused-rules-tests.sh vm-main-tests.sh delete-unused-patches-tests.sh vm-main-tests-shunit2.sh create-bsp-main-tests.sh delete-unused-rules-tests.bats"
TESTCASES="$TESTCASES ./create-bsp-main-tests-real-os-functions.sh"
TESTCASES="$TESTCASES test.bats delete-unused-rules-tests.bats"
echo

if ./generic-tests.sh; then

    FAILURES=""
    echo
    for testcase in $TESTCASES; do
        "./${testcase}";
        if [[ "$?" != "0" ]]; then
            echo -e "\e[31merror: failure in ${testcae}\e[0m"
            echo
            FAILURES="${FAILURES} ${testcase}"
        fi
    done
    
    echo
    if [[ "${FAILURES}" == "" ]]; then
            echo -e "\e[32m\u2713 All OK\e[0m"
    else
            echo -e "\e[31m\u274C Failed testcases: ${FAILURES}\e[0m"
            exit 1
    fi
else
    echo -n "Generic tests......................"
    print_failure
    printf "\nNo further tests have been executed.\n"
    exit 1
fi
