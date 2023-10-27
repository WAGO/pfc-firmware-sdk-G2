#!/bin/bash

# Assumption: JFROG_APIKEY is set.

set -o errexit
set -o pipefail

CURL="$(command -v curl)"

declare -a downloads

if [[ -z "${CURL}" ]]; then
    "Error: curl not available. Please install."
    exit 1
fi

function sanity_check {
	{
		if [ -z "${JFROG_APIKEY}" ]; then 
            echo 
            echo "*********************************************************************************************"
            echo "*                                                                                           *"
            echo "* Error during Artifactory access.                                                          *"
            echo "* Please make sure the environment variable JFROG_APIKEY is set and is part of the ptxdist  *"
            echo "* whitelist.                                                                                *"
            echo "*                                                                                           *"
            echo "* Check Confluence for further details: https://waw/x/esvtFg                                *"
            echo "*                                       https://waw/x/ZiUNFw                                *"
            echo "*                                                                                           *"
            echo "*********************************************************************************************"
            return 1
        fi
    } > /dev/stderr
}

function remove_downloads {
    rm -rfv "${downloads[@]}"
}

function usage {
    printf '%s fetch [URL] [TARGET_FILE] [MD5_FILE]\n' "$0"
    printf ' URL:\t\t\tartifactory URL\n'
    printf '     \t\t\tCan be an REST API request to recursively look for latest deployed artifact\n'
    printf '     \t\t\tExample: https://artifactory/api/storage/[search path]?lastModified\n'
    printf ' TARGET_FILE:\t\tpath to store the artifact\n'
    printf ' MD5_FILE:\t\tpath to store the artifact'"'"'s md5 checksum\n'
}

function fetch_latest {
    local search_path_url="$1"
    local output_file="$2"
    local md5sum_file="$3"

    rm -f "${output_file}"
    rm -f "${md5sum_file}"

    local latest_artifact_url

    latest_artifact_url="$(_get_latest_artifact_url "${search_path_url}")"        
    fetch "${latest_artifact_url}" "${output_file}" "${md5sum_file}"
}

function fetch {
    local url="$1"
    local output_file="$2"
    local md5sum_file="$3"

    if [[ -f "${output_file}" ]] && [[ -f "${md5sum_file}" ]]; then
        echo "Info: ${output_file} and ${md5sum_file} exist."
        return 0
    elif [[ -f "${output_file}" ]]; then
        echo "Info: ${md5sum_file} missing, overwriting ${output_file}."
    elif [[ -f "${md5sum_file}" ]]; then
        echo "Info: ${output_file} missing, overwriting ${md5sum_file}."
    fi

    # in case of a snapshot artifactory url (i.e. https://artifactory/project/artifact-1.0.0\[INTEGRATION\].bin):
    # get real snapshot file name and create an according url
    local real_url
    real_url="$(dirname "${url}")/$("${CURL}" -H "X-JFrog-Art-API:$JFROG_APIKEY" -X GET "${url}" -f -L -sSI | sed -ne 's/^X-Artifactory-Filename:\s*\([0-9a-f]*\)/\1/Ip' | sed -e 's/\r$//')" && \
    _fetch_source "${real_url}" "${output_file}" && \
    _get_md5sum "${real_url}" "${md5sum_file}"
}

function _fetch_source {
    local url="$1"
    local file="$2"

    downloads+=("${file}")

    echo "Downloading ${url}..."

    wget \
        --passive-ftp \
        --progress=bar:force \
        --timeout=30 \
        --tries=5 \
        --user-agent="PTXdist ${PTXDIST_VERSION_FULL}" \
        ${PTXDIST_QUIET:+--quiet} \
        --header="X-JFrog-Art-API:${JFROG_APIKEY}" \
        -O "${file}" \
        "${url}"
}

function _get_md5sum {
   local url="$1"
   local file="${2:-}"

   downloads+=("${file}")

   "${CURL}" -H "X-JFrog-Art-API:$JFROG_APIKEY" -X GET "${url}" -f -L -sI | sed -ne 's/^X-Checksum-Md5:\s*\([0-9a-f]*\)/\1/Ip' | sed -e 's/\r$//' > "${file}"
}

function _get_latest_artifact_url {
   local search_path_url="$1"

   local artifactory_base_url
   artifactory_base_url="$(echo "${search_path_url}" |sed -e 's+^\(https://[^/]*\).*$+\1+')"
   
   local repo
   repo="$(echo "${search_path_url}" |sed -e 's+^.*[&,?]repos=\([^&]*\).*$+\1+')"

   local name
   name="$(echo "${search_path_url}" |sed -e 's+^.*&a=\([^&]*\).*$+\1+')"

   local latest_version
   local download_url_request
   local metadata_url
   local download_url
   
   local ret=0

   if latest_version="$("${CURL}" -H "X-JFrog-Art-API:$JFROG_APIKEY" "${search_path_url}" --fail --location --silent)"; then
      download_url_request="$(printf '%s/artifactory/api/search/artifact?name=%s-%s&repos=%s' "${artifactory_base_url}" "${name}" "${latest_version}" "${repo}")"
   else
      ret=1
   fi

   if [[ "$ret" == "0" ]]; then
      if ! metadata_url="$("${CURL}" -H "X-JFrog-Art-API:$JFROG_APIKEY" "${download_url_request}" --fail --location --silent | sed -ne 's/\s*"uri" :\s*"\(.*\)*"/\1/p')"; then
         ret=1
      fi
   fi
 
   if [[ "$ret" == "0" ]]; then
      if download_url="$("${CURL}" -H "X-JFrog-Art-API:$JFROG_APIKEY" "${metadata_url}" --fail --location --silent | sed -ne 's/\s*"downloadUri" :\s*"\(.*\)",/\1/p')"; then
         echo "${download_url}"
      else
         ret=1
      fi
   fi
      
   if [[ "$ret" == "1" ]]; then
       false
   fi
}


# fetch_md5sum, fetch_source and get_latest_artifact_url are only exposed for testing purposes and should not be invoked directly.

function main {
    sanity_check || return 1

    local command="${1:-}"

    trap remove_downloads SIGTERM SIGINT EXIT

    case "${command}" in
        fetch_source)
            shift 1
            _fetch_source "$@"
            ;;
        fetch_md5sum)
            shift 1
            _get_md5sum "$@"
            ;;
        get_latest_artifact_url)
            shift 1
            _get_latest_artifact_url "$@"
            ;;
        fetch)
            shift 1

            if [[ "$#" == "3" ]]; then
                if [[ "$1" =~ /artifactory/api/search/latestVersion\? ]]; then
                    fetch_latest "$@"
                else
                    fetch "$@"
                fi
            else
                usage
                exit 1
            fi
            ;;
       *)
            echo "Unknown parameter: ${command}"
            echo
            usage
            exit 1;
            ;;
    esac
}

main "$@" && \
trap - SIGTERM SIGINT EXIT
