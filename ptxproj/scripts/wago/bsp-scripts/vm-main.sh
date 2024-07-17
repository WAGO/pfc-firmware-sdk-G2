#!/bin/bash

function init_global_vars()
{
#    SUCCESS=0
    FAILURE=1

    PRINT_MSG=yes

    PTXDIST_FLAGS_EXTRA=${PTXDIST_FLAGS_EXTRA:-'-q'}

    # overwrite for unit tests
    HEADDIR=${__HEADDIR:-"$(cd -P "$(dirname "$0")" && pwd)"}

    RES=${SHARED_FOLDER:-"/tmp/shared"}

    WS=${HEADDIR}/bsp/ptxproj.${PTXPROJ_VERSION}
    LOG="" # can be overridden by unit tests

    # Required for legacy unit tests
    CP="$(command -v cp)"
}

function get_log_file()
{
    if [[ -n "$LOG" ]]; then
        echo $LOG
    else
        echo "${WS}/platform-${PLATFORM}/logfile"
    fi
}

function preserve_resources()
{
    mkdir -p "${RES}"
    ${CP} "${WS}/configs/${PLATFORM}/${SELECT_PTXDISTCONFIG}" "${RES}/"
    ${CP} "${WS}/configs/${PLATFORM}/${SELECT_PTXDISTPLATFORMCONFIG}" "${RES}/"
}

function create_distribution_files()
{
    local makefile=""
    
    for default_makefile in GNUmakefile makefile Makefile; do
        if [[ -f "${WS}/${default_makefile}" ]]; then
            makefile="${default_makefile}"
            break
        fi
    done

    if [[ -f "${WS}/${makefile}" ]]; then
        make -f "${makefile}" -C "${WS}" dist OUT_DIR="${BSP_OUTPUT_DIR}"
    else
        echo "INFO: No makefile found, skipping 'make dist'!"
    fi
}

function remove_unused_files()
{
    echo "Remove unused files (.in, .make, patches)..."
    
    pushd "${HEADDIR}/bsp"
    rm -rf doc
    rm -rf .git
    popd > /dev/null

    pushd "${WS}"
    "${HEADDIR}/delete-unused-rules.sh" "${HEADDIR}/ptxdist" "${PLATFORM}" selected_ptxconfig:selected_platformconfig "${WS}" "${WORKAROUND_RULE_WHITELIST}"
    "${HEADDIR}/delete-unused-patches.sh" "${HEADDIR}/logfile.release_build" "${WS}" "${PLATFORM}"
    popd > /dev/null

}

function pre_cleanup()
{
	if [[ -z "${HEADDIR}" ]]; then
        echo "Assertion failed: HEADDIR is empty!"
        return 1
    fi
    
    local bsp_root
    bsp_root="$(readlink --canonicalize "${WS}/..")"

    echo "Remove known internal infrastructure files..."
    rm -fv "${bsp_root}"/{wproject,.project,.gitignore,.revisionInfo,verify_jenkins_config.sh}
    rm -fv "${bsp_root}"/*jenkinsfile
    rm -rvf "${bsp_root}/jenkins"

	echo "Remove inactive platforms..."
    remove_unused_resource_dirs "${WS}/projectroot.${PLATFORM}" "${WS}/projectroot.*"
    remove_unused_resource_dirs "${WS}/configs/${PLATFORM}" "${WS}/configs/*"

	echo "Remove wago_intern..."
	rm -rf "${HEADDIR}/bsp/wago_intern" "${WS}/wago_intern"

    echo "Remove inactive configurations in ${WS}/configs/${PLATFORM}..."
    for configuration in "${WS}/configs/${PLATFORM}"/{ptx,platform}config*; do
        if [[ "$(basename "${configuration}")" != "${SELECT_PTXDISTCONFIG}" ]] && \
           [[ "$(basename "${configuration}")" != "${SELECT_PTXDISTPLATFORMCONFIG}" ]]; then
    
            rm -fv "${configuration}"
        fi
    done

    echo "Rename active FW workspace to 'ptxproj' and remove inactive ones..."

    rm -f "$(dirname "${WS}")/ptxproj"
    mv "${WS}" "$(dirname "${WS}")/ptxproj"
    WS="$(dirname "${WS}")/ptxproj"

    local dirs=("$(dirname "${WS}")"/ptxproj*)

    for ptxproj_dir in "${dirs[@]}"; do

        case "$(basename "${ptxproj_dir}")" in
            *@tmp|ptxproj)      continue ;;
        esac

        rm -rfv "${ptxproj_dir}"
    done

}

function remove_unused_resource_dirs()
{
    local used_resource_dir=$1
    local unused_resource_dir_pattern=$2

    for dir in ${unused_resource_dir_pattern}; do

        # if the actual resource dir is a link, do not delete the directory it points to 
        [[ "$(readlink --canonicalize "${used_resource_dir}")" == "$(readlink --canonicalize "${dir}")" ]] && continue

        if [[ "$(basename "${dir}")" != "$(basename "${used_resource_dir}")" ]]; then
            echo "Removing ${dir}..."
			rm -rf "${dir}"
		fi
	done

}

#
# Assumption: toolchain is a tar.gz archive
##
function setup_toolchain()
{
	pushd "${HEADDIR}/toolchain"
	if [[ ! -L "${ARCH}" ]]; then
		rm -rf "${ARCH}"
	fi
	tar xvzf "${ARCH}.tar.gz"
	TOOLCHAIN="$(dirname "$(find "${ARCH}/" -name "${ARCH}-gcc" | grep -v gcc-first)")"
	popd > /dev/null
}

function setup_ptxdist()
{
	pushd "${HEADDIR}/ptxdist"
	./autogen.sh && ./configure && make
	popd > /dev/null
	ln -sf "${HEADDIR}/ptxdist/bin/ptxdist" "${WS}/p"
}

function prepare_workspace()
{
	echo "Configuring workspace..."

	setup_ptxdist

	pushd "${WS}"
	./p "select" "${WS}/configs/${PLATFORM}/${SELECT_PTXDISTCONFIG}"
	./p platform "${WS}/configs/${PLATFORM}/${SELECT_PTXDISTPLATFORMCONFIG}"

	# sets TOOLCHAIN var
	setup_toolchain

	echo "Setting toolchain to ${HEADDIR}/toolchain/${TOOLCHAIN}..."
	./p toolchain "${HEADDIR}/toolchain/${TOOLCHAIN}"
	popd

# FIXME: not needed for docker workflow. Is it needed at all?
#    echo "Extracting binary artefacts to the expected location..."
#    mkdir -p "${WS}/configs/${PLATFORM}/packages/"
#    tar xvJf "${HEADDIR}/binary-artefacts.tar.xz" -C "${WS}/configs/${PLATFORM}/packages/"

}

function ptxdist_disable_config()
{
	local config=$1
	local type=$2
	local configfile=${WS}/selected_${type}config

	echo "Disabling ${type}config key ${config}..."
	sed -i --follow-symlinks 's+^PTXCONF_\('"${config}"'\)=y$+# PTXCONF_\1 is not set+' "${configfile}"
}

function ptxdist_enable_config()
{
	local config=$1
	local type=$2
	local configfile=${WS}/selected_${type}config

	echo "Enabling ${type}config key ${config}..."
	sed -i --follow-symlinks 's+^# PTXCONF_\('"${config}"'\) is not set$+PTXCONF_\1=y+' "${configfile}"
}

function ptxdist_go_get_only_needed_sources
{
    pushd "${HEADDIR}/bsp"
    for dir in src local_src patches shared_public; do
        rm -f "${WS}/${dir}"
        mv "${dir}" "${WS}/${dir}"
    done
    popd > /dev/null

    pushd "${WS}"
    python3 scripts/wago/bsp-scripts/build-bsp.py
    popd > /dev/null
}

function build_binaries_version()
{
	ptxdist_disable_config WAGO_TOOLS_BUILD_VERSION_TRUNK ptx
	ptxdist_disable_config  WAGO_TOOLS_BUILD_VERSION_RELEASE ptx
	ptxdist_enable_config WAGO_TOOLS_BUILD_VERSION_BINARIES ptx

	mkdir -p "${WS}/configs/${PLATFORM}/packages/tmp"


	ptxdist_go_get_only_needed_sources
}

function bsp_set_release_index_to_99()
{
    local generic_revisions_file="${WS}/projectroot/etc/REVISIONS"    
    local platform_specific_revisions_file="${WS}/projectroot.${PLATFORM}/etc/REVISIONS"

    local revisions_file="${generic_revisions_file}"


    if [[ -e "${platform_specific_revisions_file}" ]]; then
        revisions_file="${platform_specific_revisions_file}"
    fi

	sed -i 's+^\(FIRMWARE=..\...\...\)(..)$+\1(99)+' "${revisions_file}"
}

function ptxdist_fix_config()
{
	pushd "${WS}"

    # yes always returns EXIT_FAILURE
	yes '' | ./p oldconfig || true
	popd > /dev/null
}

function cleanup()
{
    local ptxdist_bin="${1}"
    local basedir="${2}"
    local workspace="${3}"

    # remove garbage created by jenkins, if any
    # shellcheck disable=SC1117
    # [backslash is literal - use \\@]
    # Reason: input to find
    find "${basedir}/bsp" -name "*\@tmp" -prune -exec rm -rf {} \;

    rm -rf "${basedir}"/bsp/{src,local_src}.orig "${workspace}"/{src,local_src}.orig

	pushd "${workspace}"

    "${ptxdist_bin}" --quiet clean

    rm ./p
    rm ./selected_{platformconfig,ptxconfig,toolchain}

    rm -rf ./platform
    rm -rf "./platform-${PLATFORM}"

    popd > /dev/null

    # cleanup ptxdist directory
    pushd "${basedir}/ptxdist"

    make distclean

    popd > /dev/null

}

# NOTE: Setting global arguments here
function parse_config()
{
    WORKAROUND_RULE_WHITELIST=""
    SELECT_PTXDISTCONFIG="ptxconfig_generic"
    SELECT_PTXDISTPLATFORMCONFIG="platformconfig"


    local OPTIND opt

    while getopts "a:C:p:P:s:V:w:" opt; do
        case ${opt} in
            a)   ARCH=${OPTARG}                         ;;
            C)   SELECT_PTXDISTCONFIG=${OPTARG}         ;;
            p)   PLATFORM=${OPTARG}                     ;;
            P)   SELECT_PTXDISTPLATFORMCONFIG=${OPTARG} ;;
            s)   SHARED_FOLDER=${OPTARG}                ;;
            V)   PTXPROJ_VERSION=${OPTARG}              ;;
            w)   WORKAROUND_RULE_WHITELIST=${OPTARG}    ;;
            *)
                echo "Illegal argument: $opt. Aborting";
                exit 1
                ;;
        esac
    done
}

#
# Helper function to compare differences in the reference and bsp build.
#
# Uses 2 files and an awk script for input. The awk script is applied to the 2
# files representing the reference and the bsp builds (i.e. logfiles). The
# result is compared and printed in 2 columns (reference-only and bsp-only
# entries). 
function __print_differences_ref_bsp()
{
    local file1=$1
    local file2=$2 
    local awk_script=$3

    local whitespaces="                                                 "

    print_msg ""
    print_msg "Only in default FW${whitespaces}Only in BSP"
    print_msg "${whitespaces}"

    comm -3 --output-delimiter="${whitespaces}" \
    <(gawk --re-interval "${awk_script}" "${file1}" | sort | uniq) \
    <(gawk --re-interval "${awk_script}" "${file2}" | sort | uniq)

    print_msg "==============================================================================================="
}

function verify_installed_packages()
{
    print_msg "Checking installed packages in comparison to the reference (stage 1) build..."

    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
    # shellcheck disable=SC2016 
    __print_differences_ref_bsp "${1}" "${2}" 'match($0, /Installing (.*) \((.*)\) to root/, a) {print a[1],a[2]}'

}

function verify_applied_patches()
{
    print_msg "Checking applied patches in comparison to the reference (stage 1) build..."

    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
    # shellcheck disable=SC2016 
    # This backslash+linefeed is literal. Break outside single quotes if you just want to break the line.
    # Is okay: newlines are passed to awk.
    # shellcheck disable=SC1004
    __print_differences_ref_bsp \
        "${1}" "${2}" \
        'match($0, /patchin: (quilt|patch): apply \047(.*)\047/, a) \
         { \
             gsub(/^ptxdist-[0-9]{4}\.[0-9]{2}\.[0-9]+/, "ptxdist", a[2]); \
             b=gensub(/ptxproj[^/]*\//, "ptxproj/", "g", a[2]); \
             print b \
         }'
}

function verify_platformconfig_differences()
{
    echo "Checking platform configuration in comparison to the reference (stage 1) build..."
    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
    # shellcheck disable=SC2016 
    __print_differences_ref_bsp "${1}" "${2}" '/^[^#]/ {print $1}'
}

function verify_ptxconfig_differences()
{
    echo "Checking ptxdist configuration in comparison to the reference (stage 1) build..."

    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
    # shellcheck disable=SC2016 
    __print_differences_ref_bsp "${1}" "${2}" '/^[^#]/ {print $1}'
}


function verify_build()
{
    verify_installed_packages   "${HEADDIR}/logfile.release_build" "$(get_log_file)"
    
    verify_applied_patches      "${HEADDIR}/logfile.release_build" "$(get_log_file)"

    verify_platformconfig_differences "${RES}/${SELECT_PTXDISTPLATFORMCONFIG}" "${WS}/selected_platformconfig"

    verify_ptxconfig_differences "${RES}/${SELECT_PTXDISTCONFIG}" "${WS}/selected_ptxconfig"

    # TODO:
    #  o write to a protocol file?
}

function print_package_list()
{
    local log=${1}

    echo "================================================================"
    echo
    echo "List of installed packages:"
    gawk  'match($0, /Installing (.*) \((.*)\) (to|on) root/, a) {print a[1],a[2]}' "${log}" | sort -d | uniq
    echo
    echo "================================================================"

}

function print_provided_sources()
{
    echo "================================================================"
    echo
    echo "Contents of src/local_src:"
    ls "${WS}/"{local_,}src -lA
    echo
    echo "================================================================"

}

function print_stats()
{
    echo "${FUNCNAME[0]}: Not yet fully implemented!"

    print_provided_sources
    print_package_list "$(get_log_file)"

    # TODO: 
    #   o all removed files with the reason
    #   o all changed files (with diff?)
}

function run_sanity_checks()
{
    pushd "${WS}"

    if [[ -z "$(ls -A "${WS}/patches")" ]]; then

        echo "Sanity check failed: ${WS}/patches directory is empty."
        echo "Possible cause: error in delete-unused-patches.sh script."
        echo
        echo "Aborting!"

        popd > /dev/null
        
        exit $FAILURE
    fi

    popd > /dev/null
}

function print_msg()
{
    if [[ "${PRINT_MSG}" == "yes" ]]; then
        echo "$@"
    fi
}

function main()
{
    parse_config "$@"
    init_global_vars

    # Remove static resources that we know of in advance
    pre_cleanup

    # Preserve files needed for post-build verification
    preserve_resources

    # Configure workspace for build start 
    prepare_workspace
    
    # Deduce other unused files according to the settings from
    # prepare_workspace and remove them
    remove_unused_files

    # Apply additional fixes
    ptxdist_fix_config

    # Check for obvious problems with the workspace
    run_sanity_checks

    # Only build open source stuff, use prebuilt
    # binaries for closed source
    build_binaries_version

    verify_build

    print_stats

    create_distribution_files

    bsp_set_release_index_to_99
    
    cleanup "./p" "${HEADDIR}" "${WS}" "${PLATFORM}"
}

# Don't run main when the file is being sourced.
# Required for unit testing.
if [[ "$0" == "${BASH_SOURCE[0]}" ]]; then
    
    if [[ "${LOG_ALL}" != "" ]]; then
        exec > >(tee -i "logfile_$(basename "$0").txt")
        exec 2>&1
    fi

    set -o errexit
    set -o nounset
    set -o pipefail
    set -o errtrace

    echo "### BSP Build ###"
    echo "Script           : ${BASH_SOURCE[0]}"
    echo "Arguments        : $@"
    echo "Working directory: $(pwd)"
    echo "Environment      : $(env)"
    echo ""

    main "$@"
fi
