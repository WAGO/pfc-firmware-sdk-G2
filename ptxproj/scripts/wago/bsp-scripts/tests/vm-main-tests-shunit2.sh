#!/usr/bin/env bash
#shellcheck disable=SC1091,SC2034

# Globally disabled warnings:
# SC2034: foo appears unused. Verify it or export it.
#   local variables used in external functions trigger this warning (essential
#   technique for unit tests)
#
# SC1091: Not following: (error message here)
#
#   vim shellcheck plugin cannot follow the sourced scripts
#

source ../vm-main.sh

source util-definition.sh

test_cleanup()
{
    ( 
        find() 
        { 
            echo "find $*"
        }

        local PLATFORM=dummy

        assertEquals \
            "cleanup: output differs from resources/vm_main_cleanup.out." \
            "$(< resources/vm_main_cleanup.out)" \
            "$(cleanup "ptxdistdummy" "mybasedir" "myworkspace")"
    )
    local result=$?

    assertTrue "cleanup: assertEquals in subshell failed." "$result"
}

test_cleanup_with_real_rm()
{
    local PLATFORM=myplatform

    mkdir -p ./mybasedir/bsp/{src,src.orig,local_src,local_src.orig}
    mkdir -p ./mybasedir/bsp/myworkspace/{src,src.orig,local_src,local_src.orig,\@tmp}
    mkdir -p ./mybasedir/dir{1,2,3}
    mkdir -p ./mybasedir/bsp/myworkspace/dir{4,5,4711}
    mkdir -p ./mybasedir/bsp/myworkspace/platform
    mkdir -p ./mybasedir/bsp/myworkspace/platform-myplatform
    mkdir -p ./mybasedir/ptxdist

    mkdir -p ./mybasedir/bsp/\@tmp
    mkdir -p ./mybasedir/\@tmp

    touch ./mybasedir/bsp/myworkspace/{p,selected_platformconfig,selected_ptxconfig,selected_toolchain,a,b,c}
    touch ./mybasedir/bsp/\@tmp/folder-is-not-empty

    (
        rm()
        {
          /bin/rm "$@"
        }

        pushd()
        {
            command pushd "$@"
        }

        popd()
        {
            command popd
        }

        cleanup "ptxdistdummy" "./mybasedir" "./mybasedir/bsp/myworkspace"
    )
    local result=$?

    assertTrue "cleanup: assertEquals in subshell failed." "$result"

    for file in ./mybasedir/bsp/{src,local_src}.orig ./mybasedir/bsp/{\@tmp,p,selected_*,platform,platform-myplatform}; do
        assertFalse "cleanup: could not remove $(basename "$file")" "[[ -e $file ]]"
    done

    /bin/rm -rf ./mybasedir
}

test_create_distribution_files_no_makefile_found()
{
    local WS="./resources"

    local output
    local result

    output="$(create_distribution_files)"
    result="$?"

    assertTrue "create_distribution_files shall return true." "$result"

    assertEquals    "create_distribution_files: result does not match expected output" \
                    "INFO: No makefile found, skipping 'make dist'!" \
                    "${output}"

}

test_create_distribution_files_makefile()
{
    local WS="./resources"
    local BSP_OUTPUT_DIR="dummy"

    local output
    local result

    touch ${WS}/makefile

    output="$(create_distribution_files)"
    result="$?"

    assertTrue "create_distribution_files shall return true." "$result"

    assertEquals    "create_distribution_files: result does not match expected output" \
                    "make -f makefile -C ./resources dist OUT_DIR=dummy" \
                    "${output}"

    /bin/rm -f ${WS}/makefile

}

test_create_distribution_files_Makefile()
{
    local WS="./resources"
    local BSP_OUTPUT_DIR="dummy"

    local output
    local result

    touch ${WS}/Makefile

    output="$(create_distribution_files)"
    result="$?"

    assertTrue "create_distribution_files shall return true." "$result"

    assertEquals    "create_distribution_files: result does not match expected output" \
                    "make -f Makefile -C ./resources dist OUT_DIR=dummy" \
                    "${output}"

    /bin/rm -f ${WS}/Makefile

}

test_create_distribution_files_GNUMakefile()
{
    local WS="./resources"
    local BSP_OUTPUT_DIR="dummy"

    local output
    local result

    touch ${WS}/GNUmakefile

    output="$(create_distribution_files)"
    result="$?"

    assertTrue "create_distribution_files shall return true." "$result"

    assertEquals    "create_distribution_files: result does not match expected output" \
                    "make -f GNUmakefile -C ./resources dist OUT_DIR=dummy" \
                    "${output}"

    /bin/rm -f ${WS}/GNUmakefile

}

test_create_distribution_files_multiple_makefiles()
{
    local WS="./resources"
    local BSP_OUTPUT_DIR="dummy"

    local output
    local result

    touch "${WS}"/{GNUmakefile,makefile,Makefile}

    output="$(create_distribution_files)"
    result="$?"

    assertTrue "create_distribution_files shall return true." "$result"

    assertEquals    "create_distribution_files: result does not match expected output" \
                    "make -f GNUmakefile -C ./resources dist OUT_DIR=dummy" \
                    "${output}"

    /bin/rm -f "${WS}"/{GNUmakefile,makefile,Makefile}

}

test_pre_cleanup_empty_HEADDIR()
{
    local output
    local result

    output="$(pre_cleanup)"
    result="$?"

    assertFalse "pre_cleanup should abort with an error on empty HEADDIR." "$result"
}

test_pre_cleanup_custom_ptxproj_name()
{
    local HEADDIR="./resources"
    local WS="./resources/sample_workspace_pre_cleanup/myproj"
    local PLATFORM="myplatform"
    local expected_output="resources/test_vm_main_pre_cleanup.out"

    SELECT_PTXDISTCONFIG="ptxconfig.used"
    SELECT_PTXDISTPLATFORMCONFIG="platformconfig.used"

    local CP
    CP="$(which cp)"

    local output
    local result

    output="$(pre_cleanup)"
    result="$?"

    assertTrue "pre_cleanup should not abort with an error" "$result"

    assertTrue "WS variable should not be empty." '[[ -n "${WS}"  ]]'
}

test_pre_cleanup_real_ptxproj_name()
{
    local HEADDIR="./resources"
    local WS="./resources/sample_workspace_pre_cleanup2/ptxproj.2013.03.0"
    local PLATFORM="myplatform"
    local expected_output="resources/test_vm_main_pre_cleanup2.out"

    SELECT_PTXDISTCONFIG="ptxconfig.used"
    SELECT_PTXDISTPLATFORMCONFIG="platformconfig.used"

    local CP
    CP="$(which cp)"

    local output
    local result

    output="$(pre_cleanup)"
    result="$?"

    assertTrue "pre_cleanup should not abort with an error" "$result"

    assertTrue "WS variable should not be empty." '[[ -n "${WS}"  ]]'
}

test_pre_cleanup_links_to_projectroot_and_platform()
{
    local HEADDIR="./resources"
    local WS="./resources/sample_workspace_pre_cleanup_links/ptxproj.2013.03.0"
    local PLATFORM="myplatform"
    local expected_output="resources/test_vm_main_pre_cleanup_links.out"

    SELECT_PTXDISTCONFIG="ptxconfig.used"
    SELECT_PTXDISTPLATFORMCONFIG="platformconfig.used"

    local CP
    CP="$(which cp)"

    local output
    local result

    output="$(pre_cleanup)"
    result="$?"

    assertTrue "pre_cleanup should not abort with an error" "$result"

    assertEquals    "${FUNCNAME[0]}: output not as in ${expected_output}:" \
                    "$(sed 's+@PWD@+'$(pwd)'+g' "${expected_output}")" \
                    "$(pre_cleanup)"

    assertTrue "WS variable should not be empty." '[[ -n "${WS}"  ]]'
}



test_custom_workspace_path()
{
    local PTXPROJ_VERSION=47.11
    init_global_vars 

    assertEquals "${HEADDIR}/bsp/ptxproj.47.11" "${WS}"
}

test_default_headdir()
{
    init_global_vars

    #shellcheck disable=SC2153
    # HEADDIR may not be defined.
    # It is defined in init_global_vars above. 
    assertEquals "Default head dir should be set to PWD." "$(pwd)" "${HEADDIR}"

}

test_parse_config_mandatory_and_defaults()
{
    parse_config -p myplatform -V 1234.56.78
    assertTrue $?

    assertEquals "myplatform" ${PLATFORM}
    assertEquals "1234.56.78" ${PTXPROJ_VERSION}

    assertEquals "ptxconfig_generic" $SELECT_PTXDISTCONFIG
    assertEquals "platformconfig" $SELECT_PTXDISTPLATFORMCONFIG
        
}

test_parse_config_all_arguments()
{
    parse_config -a arm-cortexa8-linux-gnueabihf -p wago-pfcXXX -s /workspace/workspace/Playground/AGa/pfcXXX.wago_pfcXXX-nightly.BSP/share -w rules/wago-config-007-separate-config-tools.in -V 2013.03.0 -C myptxconfig -P myplatformconfig 
    assertTrue $?

    assertEquals "arm-cortexa8-linux-gnueabihf" "${ARCH}"
    assertEquals "myptxconfig" "${SELECT_PTXDISTCONFIG}"
    assertEquals "wago-pfcXXX" "${PLATFORM}"
    assertEquals "myplatformconfig" "${SELECT_PTXDISTPLATFORMCONFIG}"
    assertEquals "/workspace/workspace/Playground/AGa/pfcXXX.wago_pfcXXX-nightly.BSP/share" "${SHARED_FOLDER}"
    assertEquals "2013.03.0" "${PTXPROJ_VERSION}"
    assertEquals "rules/wago-config-007-separate-config-tools.in" "${WORKAROUND_RULE_WHITELIST}"

}

test_parse_config_abort_on_unknown()
{
    ( parse_config -q ) >&/dev/null
    assertFalse "Unknown option -q should make parse_config fail." $?

}

test_sanity_check_abort_on_empty_patches_directory()
{
    #setup
    local WS=./run
    /bin/rm -rf ${WS}/*
    /bin/mkdir -p "${WS}/patches"
    #/setup

    ( run_sanity_checks ) >&/dev/null
    assertFalse "sanity check doesn't fail on empty patches directory." $?

    #teardown
    /bin/rm -rf ${WS}/*
    #/teardown
}

test_sanity_check_succeed_on_non_empty_patches_directory()
{
    #setup
    local WS=./run
    /bin/rm -rf ${WS}/*
    /bin/mkdir -p "${WS}/patches/mypatch"
    #/setup

    ( run_sanity_checks ) >&/dev/null
    assertTrue "sanity check fails on non-empty patches directory." $?

    #teardown
    /bin/rm -rf ${WS}/*
    #/teardown
}

test_bsp_set_release_index_to_99_no_platformspecific_config() {
    #setup
    local WS=myworkspace
    local PLATFORM=myplatform
    #/setup

    local output
    output=$(
        sed() {
            echo "sed $*"
        }

        bsp_set_release_index_to_99
    )
    assertTrue "bsp_set_release_index_to_99 returns an error." $?

    assertEquals "bsp_set_release_index: output not as expected:" \
                 'sed -i s+^\(FIRMWARE=..\...\...\)(..)$+\1(99)+ myworkspace/projectroot/etc/REVISIONS' \
                 "${output}"

}

test_bsp_set_release_index_to_99_with_platformspecific_config() {
    #setup
    local WS=resources/sample_workspace0
    local PLATFORM=myplatform
    #/setup

    local output
    output=$(
        sed() {
            echo "sed $*"
        }

        bsp_set_release_index_to_99
    )
    assertTrue "bsp_set_release_index_to_99 returns an error." $?

    assertEquals "bsp_set_release_index: output not as expected:" \
                 'sed -i s+^\(FIRMWARE=..\...\...\)(..)$+\1(99)+ resources/sample_workspace0/projectroot.myplatform/etc/REVISIONS' \
                 "${output}"

}

test_bsp_set_release_index_to_99_contents() {
    #setup
    local WS=tmp/sample_workspace0
    local PLATFORM=myplatform

    /bin/cp -r resources/sample_workspace0 tmp/
    #/setup

    bsp_set_release_index_to_99

    assertTrue "bsp_set_release_index_to_99 returns an error." $?

    assertEquals "bsp_set_release_index: release index not changed to 99" \
                 'FIRMWARE=23.47.11(99)' \
                 "$(cat tmp/sample_workspace0/projectroot.myplatform/etc/REVISIONS)"

    /bin/rm -rf tmp/*

}

. shunit2


