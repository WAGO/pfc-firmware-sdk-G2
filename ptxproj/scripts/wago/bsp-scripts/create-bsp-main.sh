#!/bin/bash

set -o nounset
set -o pipefail
set -o errexit
set -o errtrace

# init global variables inside a function to
# simplify unit testing
function init_global_vars()
{
    # Env variables set externally (i.e. by build server)
    DEBUG_OUTPUT=${BSP_SCRIPT_ENABLE_DEBUG:-""}

    ARCH=${BSP_TARGET_ARCH:?}
    PLATFORM=${BSP_TARGET_PLATFORM:?}
    TOOLCHAIN_NAME=${BSP_TOOLCHAIN_NAME:?}

    SELECT_PTXDISTCONFIG=${BSP_SELECT_PTXDISTCONFIG:-""}
    SELECT_PTXDISTPLATFORMCONFIG=${BSP_SELECT_PTXDISTPLATFORMCONFIG:-""}

    WORKAROUND_RULE_WHITELIST=${BSP_WORKAROUND_RULE_WHITELIST:-""}

    # overwrite for unit tests
    SCRIPT_DIR=${__SCRIPT_DIR:-"$(cd -P "$(dirname "$0")" && pwd)"}
    
    SHARED_FOLDER="${SCRIPT_DIR}/share"

    OUTPUT_FOLDER="${BSP_OUTPUT_DIR:-output}"

    BUILD_SW_VERSION="${BSP_SW_VERSION:-"NNNNNN"}"
    BUILD_SW_INDEX="${BSP_SW_INDEX:-"NN"}"
    BUILD_SW_REVISION="${BSP_SW_REVISION:-"RRRRR"}"

    PTXPROJ_VERSION="${BSP_PTXPROJ_VERSION:-"2013.03.0"}"

    MAIN_BSP_FILENAME="${BSP_MAIN_FILENAME:-"WAGO-PFC-BSP"}"

    # Note the different parameter substituation here: when
    # BSP_MAIN_FILENAME_SUFFIX is set but empty, MAIN_BSP_FILENAME_SUFFIX shall
    # be empty as well
    MAIN_BSP_FILENAME_SUFFIX="${BSP_MAIN_FILENAME_SUFFIX-"_BETA"}"

    BASH="bash"
    if [[ "${DEBUG_OUTPUT}" != "" ]]; then
        set -xv
        BASH="bash -xv"
    fi
}

function bailout()
{
    echo "Error during BSP creation! Aborting."
    echo
    echo "Backtrace:"

    local level=0

    while caller ${level}; do
        level=$((level+1))
    done

    exit 1
}

function create_bsp()
{
    local cmd="${BASH} ${SCRIPT_DIR}/vm-main.sh -a ${ARCH} -p ${PLATFORM} -s ${SHARED_FOLDER}"

    [[ "$WORKAROUND_RULE_WHITELIST" ]] && cmd+=" -w ${WORKAROUND_RULE_WHITELIST}"
    [[ "$SELECT_PTXDISTCONFIG" ]] && cmd+=" -C ${SELECT_PTXDISTCONFIG}"
    [[ "$SELECT_PTXDISTPLATFORMCONFIG" ]] && cmd+=" -P ${SELECT_PTXDISTPLATFORMCONFIG}"
    [[ "$PTXPROJ_VERSION" ]] && cmd+=" -V ${PTXPROJ_VERSION}"

    echo "Running ${cmd}..."
   
    # FIXME: above variables are currently not whitespace-safe 
    ${cmd}
}

function create_bsp_specific_artifacts()
{
    local result
    result=0
    pushd "${SCRIPT_DIR}"
    
    mkdir -p "${OUTPUT_FOLDER}/customer-artifacts"

    local bsp_version="UNKNOWN_FW_REVISION"
    local ptxdist_version="UNKNOWN_PTXDIST_VERSION"

    [[ -f "${SHARED_FOLDER}/REVISIONS" ]] && bsp_version=$(< "${SHARED_FOLDER}/REVISIONS")
    bsp_version="${bsp_version#FIRMWARE=}"

    if [[ -f "./ptxdist/configure" ]]; then
        ptxdist_version=$(grep 'PACKAGE_VERSION=' ptxdist/configure)

        # Extract version number
        ptxdist_version=${ptxdist_version#PACKAGE_VERSION=\'}
        ptxdist_version=${ptxdist_version%\'}
    fi
 
    VERSION_INFO_TXT="V${BUILD_SW_VERSION}_${BUILD_SW_INDEX}_r${BUILD_SW_REVISION}"

    # ToDo: move toolchain creation into separate build job 
    mv "./toolchain-src/${ARCH}-src.tar.gz" "${OUTPUT_FOLDER}/customer-artifacts/gcc-${TOOLCHAIN_NAME,,}-${ARCH}-src.tar.gz" && \
    mv "./toolchain/${ARCH}.tar.gz" "${OUTPUT_FOLDER}/customer-artifacts/gcc-${TOOLCHAIN_NAME,,}-${ARCH}.tar.gz" && \

    tar cjf - ./bsp     > "${OUTPUT_FOLDER}/customer-artifacts/${MAIN_BSP_FILENAME}_${VERSION_INFO_TXT}${MAIN_BSP_FILENAME_SUFFIX}.tar.bz2" && \

    # ToDo: move packaging of PTXdist into separate build job
    tar cjf - ./ptxdist > "${OUTPUT_FOLDER}/customer-artifacts/ptxdist_${ptxdist_version}_${bsp_version}.tar.bz2" 

    result=$?

    popd > /dev/null

    return $result
}

###########################################################

# Don't execute anything when the file is being sourced.
# Required for unit testing.

if [[ "$0" == "${BASH_SOURCE[0]}" ]]; then
    trap bailout ERR

    echo "### Create BSP ###"
    echo "Arguments        : $@"
    echo "Working directory: $(pwd)"
    echo "Environment      : $(env)"
    echo ""


    init_global_vars
    
    create_bsp                    || bailout
    create_bsp_specific_artifacts || bailout
fi
