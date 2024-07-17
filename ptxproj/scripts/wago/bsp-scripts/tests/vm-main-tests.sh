#!/bin/bash

source ./unittest-common.sh

source ../vm-main.sh

ARCH=arm-cortexa8-linux-gnueabihf-
PLATFORM=wago-pfcXXX
init_global_vars

####################
# Legacy stubbing mechanism
function __cp() {
    echo "cp $*"
}

CP=__cp
#####################

# Do not abort on error.
set +e

mkdir -p run

function test_list_installed_packages0()
{
    runtest print_package_list resources/logfile.fw9.2nd-stage
}

function test_list_installed_packages1()
{
    runtest print_package_list resources/logfile.fw12_snapshot.2nd-stage
}

function test_verify_installed_packages0()
{
    local leftfile=resources/logfile.fw8.1st-stage
    local rightfile=resources/logfile.fw9.2nd-stage

    runtest verify_installed_packages ${leftfile} ${rightfile} 
}

function test_verify_installed_packages1()
{
    local leftfile=resources/logfile.fw9.1st-stage
    local rightfile=resources/logfile.fw9.2nd-stage

    runtest verify_installed_packages ${leftfile} ${rightfile}
}

function test_verify_applied_patches0()
{
    local leftfile=resources/logfile.fw9.1st-stage
    local rightfile=resources/logfile.fw9.2nd-stage

    runtest verify_applied_patches ${leftfile} ${rightfile}
}

function test_print_packages0()
{
    runtest print_package_list resources/logfile.fw9.2nd-stage
}

function test_platformconfig_differences()
{
    local leftfile=resources/platformconfig.trunk
    local rightfile=resources/platformconfig.binaries
    
    runtest verify_platformconfig_differences ${leftfile} ${rightfile}
}

function test_ptxconfig_differences()
{
    local leftfile=resources/ptxconfig.release
    local rightfile=resources/ptxconfig.binaries
    
    runtest verify_platformconfig_differences ${leftfile} ${rightfile}
}

test_list_installed_packages0
test_list_installed_packages1

test_verify_installed_packages0
test_verify_installed_packages1

test_verify_applied_patches0

test_print_packages0

test_platformconfig_differences
test_ptxconfig_differences
