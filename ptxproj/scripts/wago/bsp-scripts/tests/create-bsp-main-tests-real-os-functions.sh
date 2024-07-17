#!/bin/bash
#shellcheck disable=SC1091,SC2034

# 
# This test suite does not overwrite cp, mv and friends so that these operations
# can be tested.
#

# Globally disabled warnings:
# SC2034: foo appears unused. Verify it or export it.
#   local variables used in external functions trigger this warning (essential
#   technique for unit tests)
#
# SC1091: Not following: (error message here)
#   Shellcheck vim plugin cannot follow the sourced scripts
#

source ../create-bsp-main.sh
BSP_TARGET_ARCH=arm-cortexa8-linux-gnueabihf-
BSP_TARGET_PLATFORM=wago-pfcXXX
BSP_TOOLCHAIN_NAME=OSELAS.Bla-4711

init_global_vars

source ./unittest-common.sh
#source ./util-definition.sh

# Do not abort on error.
set +e

test_create_bsp_specific_artifacts()
{

    local BSP_SW_VERSION="080910"
    local BSP_SW_INDEX="11"
    local BSP_SW_REVISION="12345"

    init_global_vars
    ARCH=arm-linux-gnueabihf

    local SHARED_FOLDER=resources/sample_bsp_shared_folder

    mkdir ./{bsp,ptxdist,toolchain,toolchain-src}
    touch toolchain/arm-linux-gnueabihf.tar.gz
    touch toolchain-src/arm-linux-gnueabihf-src.tar.gz

    local output

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls "${OUTPUT_FOLDER}"/customer-artifacts/WAGO-PFC-BSP*)" =~ ${OUTPUT_FOLDER}/customer-artifacts/WAGO-PFC-BSP_V080910_11_r12345_BETA.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: build timestamp shall contain V080910_11_r12345 but was $(ls "${OUTPUT_FOLDER}"/customer-artifacts/WAGO-PFC-BSP*)" \
        $result

    local result

    ls "${OUTPUT_FOLDER}"/customer-artifacts/ptxdist_* 1>/dev/null
    result=$?

    assertTrue "create_bsp_specific_artifacts: ptxdist_*.tar.gz not found" \
       $result 

    ls "${OUTPUT_FOLDER}/customer-artifacts/gcc-oselas.bla-4711-arm-linux-gnueabihf.tar.gz" 1>/dev/null
    result=$?

    assertTrue "create_bsp_specific_artifacts: toolchain artifact not found" \
       $result 

    ls "${OUTPUT_FOLDER}/customer-artifacts/gcc-oselas.bla-4711-arm-linux-gnueabihf-src.tar.gz" 1>/dev/null
    result=$?

    assertTrue "create_bsp_specific_artifacts: toolchain src artifact not found" \
       $result 

#    find "${OUTPUT_FOLDER}" -type f

    /bin/rm -rf ./bsp ./ptxdist ./toolchain ./toolchain-src "${OUTPUT_FOLDER}"


}

. shunit2
