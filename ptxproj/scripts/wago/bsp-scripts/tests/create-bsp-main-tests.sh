#!/bin/bash
#shellcheck disable=SC1091,SC2034

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
source ./util-definition.sh

# Do not abort on error.
set +e


test_create_bsp_specific_artifacts_set_custom_suffix()
{
    local BSP_MAIN_FILENAME_SUFFIX="_GAMMA"
    
    init_global_vars


    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_VNNNNNN_NN_rRRRRR.*_GAMMA.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: SUFFIX expected to be _GAMMA but the filename was $(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_set_empty_custom_suffix()
{
    local BSP_MAIN_FILENAME_SUFFIX=""
    
    init_global_vars


    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_VNNNNNN_NN_rRRRRR.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: filename expected to be WAGO-PFC-BSP_VNNNNNN_NN_rRRRRR.tar.bz but was $(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_set_default_bsp_version()
{
    init_global_vars

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_VNNNNNN_NN_rRRRRR.*_BETA.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: when REVISIONS file could not be parsed, revision shall be set to VNNNNNN_NN_rRRRRR" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_set_custom_bsp_filename()
{
    init_global_vars

    local MAIN_BSP_FILENAME=custom-filename

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/custom-filename*)" =~ ./output/customer-artifacts/custom-filename_VNNNNNN_NN_rRRRRR.*_BETA.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: main bsp artifact filename should be custom-filename_VNNNNNN_NN_rRRRRR_BETA.tar.bz2 but was different: $(ls ./output/customer-artifacts/*)" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_parse_bsp_swversion()
{
    local BSP_SW_VERSION="080910"
    
    init_global_vars
    
    local SHARED_FOLDER=resources/sample_bsp_shared_folder

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_V080910_.* ]]
    local result=$?
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_parse_bsp_swindex()
{
    local BSP_SW_INDEX="11"

    init_global_vars
    
    local SHARED_FOLDER=resources/sample_bsp_shared_folder

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_.*_11_.* ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: sw index shall be set to 11, but was $(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_parse_bsp_swrevision()
{
    local BSP_SW_REVISION="12345"

    init_global_vars
    
    local SHARED_FOLDER=resources/sample_bsp_shared_folder

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" =~ ./output/customer-artifacts/WAGO-PFC-BSP_.*_r12345_.* ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: sw revision shall be set to r12345, but was $(ls ./output/customer-artifacts/WAGO-PFC-BSP*)" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_default_ptxdist_version()
{
    init_global_vars

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/ptxdist*)" =~ ./output/customer-artifacts/ptxdist_UNKNOWN_PTXDIST_VERSION_.*.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: when ptxdist/configure file could not be parsed, ptxdist version shall be set to UNKNOWN_PTXDIST_VERSION" \
                $result
    
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_parse_ptxdist_version()
{
    init_global_vars

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    /bin/cp ./resources/ptxdist_configure ./ptxdist/configure

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls ./output/customer-artifacts/ptxdist*)" =~ ./output/customer-artifacts/ptxdist_2013.03.0_.*.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: ptxdist version shall be set to 2013_03_0, but was $(ls ./output/customer-artifacts/ptxdist*)" \
                ${result}
   
    /bin/rm -r ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_create_bsp_specific_artifacts_with_all_sw_tags()
{
    local BSP_SW_VERSION="080910"
    local BSP_SW_INDEX="11"
    local BSP_SW_REVISION="12345"

    init_global_vars
    
    local SHARED_FOLDER=resources/sample_bsp_shared_folder

    mkdir ./{bsp,ptxdist,"${BSP_TOOLCHAIN_NAME}"}

    create_bsp_specific_artifacts > /dev/null
    assertTrue "create_bsp_specific_artifacts shall return true with valid input." "$?"

    [[ "$(ls "${OUTPUT_FOLDER}"/customer-artifacts/WAGO-PFC-BSP*)" =~ ${OUTPUT_FOLDER}/customer-artifacts/WAGO-PFC-BSP_V080910_11_r12345_BETA.tar.bz2 ]]
    local result=$?

    assertTrue "create_bsp_specific_artifacts: build timestamp shall contain V080910_11_r12345 but was $(ls "${OUTPUT_FOLDER}"/WAGO-PFC-BSP*)" \
                $result

    /bin/rm -rf ./bsp ./ptxdist "${BSP_TOOLCHAIN_NAME}" "${OUTPUT_FOLDER}"
}

test_default_ptxproj_version()
{
    init_global_vars

    assertEquals "2013.03.0" "${PTXPROJ_VERSION}"
}

test_custom_ptxproj_version()
{
    local BSP_PTXPROJ_VERSION="4711"
    init_global_vars

    assertEquals "4711" "${PTXPROJ_VERSION}"
}

test_create_bsp_default_settings()
{
    local __SCRIPT_DIR="myscriptdir"
    init_global_vars

    local output
    local result
    output=$(create_bsp)
    local result=$?
    
    assertTrue "$result"
    assertEquals "$(printf "%s\n%s" \
                 "Running bash myscriptdir/vm-main.sh -a arm-cortexa8-linux-gnueabihf- -p wago-pfcXXX -s myscriptdir/share -V 2013.03.0..." \
                 "bash myscriptdir/vm-main.sh -a arm-cortexa8-linux-gnueabihf- -p wago-pfcXXX -s myscriptdir/share -V 2013.03.0")" \
                 "${output}" 
}

test_create_bsp_custom_ptxproj_version()
{
    local BSP_PTXPROJ_VERSION="4711"
    local __SCRIPT_DIR="myscriptdir"
    init_global_vars

    local output
    local result
    output=$(create_bsp)
    local result=$?
    
    assertTrue "$result"
    assertEquals "$(printf "%s\n%s" \
                 "Running bash myscriptdir/vm-main.sh -a arm-cortexa8-linux-gnueabihf- -p wago-pfcXXX -s myscriptdir/share -V 4711..." \
                 "bash myscriptdir/vm-main.sh -a arm-cortexa8-linux-gnueabihf- -p wago-pfcXXX -s myscriptdir/share -V 4711")" \
                 "${output}"
}

test_default_script_dir()
{
    init_global_vars

    #shellcheck disable=SC2153
    # SCRIPT_DIR may not be defined.
    # It is defined in init_global_vars above. 
    assertEquals "Default script dir should be set to PWD." "$(pwd)" "${SCRIPT_DIR}"
}

. shunit2
