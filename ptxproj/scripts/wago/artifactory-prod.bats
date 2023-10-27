#!/usr/bin/env bats

#
# This suite contains similar tests to artifactory.bats
# The difference is that the functionality is tested against productive artifactory server.
# In past there has been configuration differences between test and productive servers.
# To avoid breaking productive pfc setup it is necessary to run both suites.
# The reason they cannot be combined is that different Artifactory API keys have to be
# used to connect to each instance. 

# CAVEAT: as of now, prod artifacts are being used (opposed to artifactory-test).
# This will lead to test failures, should newer versions of Libbacnet-Source get released.

function setup {
    TESTCASE_TMPDIR="$(mktemp -d "${BATS_TMPDIR}/${BATS_TEST_NAME}.XXXXXX")"
}

function teardown {
    rm -rf "${TESTCASE_TMPDIR}"
}

@test "environment: curl is installed" {
    run command -v curl

    [[ "${status}" == "0" ]]
}

@test "environment: JFROG_APIKEY is set" {
    [[ -n "${JFROG_APIKEY}" ]]
}

@test "environment: JFROG_APIKEY is valid" {
    "$(command -v curl)" -H "X-JFrog-Art-API:$JFROG_APIKEY" -X GET https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz -f -L -sSI
}

@test "environment: gzip is installed" {
    run command -v gzip

    [[ "${status}" == "0" ]]
}

@test "environment: wget is installed" {
    run command -v wget

    [[ "${status}" == "0" ]]
}

@test "environment: native bats is used instead of the docker version" {
    run grep "repository.*bats" /image_info.xml
    [[ "$status" != "0" ]]
}

@test "artifactory.sh: illegal command line parameter => return error" {
    run ./artifactory.sh \
        unknown_parameter

    [[ "$status" != "0" ]]
    [[ "${output}" =~ "Unknown parameter: unknown_parameter" ]]
}

#@test "fetch_md5sum: valid URL, no output file => print md5sum to stdout" {
#
#    run ./artifactory.sh \
#        fetch_md5sum \
#        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz"
#
#    echo "${output}"
#
#    [[ "$status" == "0" ]]
#
#    #[[ "${lines[0]}" == $'6f06c42553730ab2f91c3c91ee9e00d2\r' ]]
#    [[ "${output}" == '9aa0622617774d9b25cdb3d88cf9b737' ]]
#}

@test "fetch_md5sum: valid url => checksum file is created" {

    local file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_md5sum \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]
    [[ "${output}" == '897134195989bd5c97c5dca94b00c434' ]]

}

@test "fetch_md5sum: invalid url => return error" {

    run ./artifactory.sh \
        fetch_md5sum \
        "invalid_url"

    echo "${output}"

    [[ "$status" != "0" ]]

}

@test "fetch_md5sum: invalid url => no checksum file is created" {

    local file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_md5sum \
        "illegal_url" \
        "${file}"

    echo "${output}"

    [[ "$status" != "0" ]]

   run cat "${file}"

   echo "${output}"

   [[ "$status" != "0" ]]
   [[ "${output}" =~ 'No such file or directory' ]] || [[ "${output}" =~ 'Datei oder Verzeichnis nicht gefunden' ]]
}

@test "fetch_source: valid url => download and store a valid file" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_source \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    [[ -f "${file}" ]]

    run tar tf "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

}

@test "fetch_source: invalid url => do not create anything" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch_source \
        "invalid_url" \
        "${file}"

    echo "${output}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
}

@test "fetch: not enough parameters (0) => abort" {

    run ./artifactory.sh \
        fetch

    [[ "$status" != "0" ]]
}

@test "fetch: not enough parameters (1) => abort" {

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz"

    [[ "$status" != "0" ]]
}

@test "fetch: not enough parameters (2) => abort" {

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \

    [[ "$status" != "0" ]]
}

@test "fetch: source file and md5 exist => do nothing, return true" {

    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${file}"
    touch "${md5sum}"

    local file_metadata=

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${file} and ${md5sum} exist\..*") -ne 0 ]]
}

# Required for the BSP workflow to not publish proprietary sources by accident
@test "fetch: source file path does not exist => do nothing, return error" {

    local file="${TESTCASE_TMPDIR}/wago_intern/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${md5sum}" ]]
}


@test "fetch: only source file exists => remove existing file and download everything" {

    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${file}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${md5sum} missing, overwriting ${file}\..*") -ne 0 ]]

    run tar tf "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${md5sum}"
    [[ "${output}" == '897134195989bd5c97c5dca94b00c434' ]]
}

@test "fetch: only md5 file exists => remove existing file and download everything" {
    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${md5sum}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
    
    echo "$output"
    [[ $(expr "$output" : ".*Info: ${file} missing, overwriting ${md5sum}\..*") -ne 0 ]]
 
    run tar tf "${file}"

    echo "${output}"

    [[ "$status" == "0" ]]

    run cat "${md5sum}"
    [[ "${output}" == '897134195989bd5c97c5dca94b00c434' ]]
}

@test "fetch: valid url => download file and checksum" {

    local outputfile="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${outputfile}" \
        "${md5file}"

    echo "fetch: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${outputfile}" ]]

    run tar tf "${outputfile}"

    echo "gzip: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${md5file}" ]]

    run cat "${md5file}"

    echo "cat: ${output}"

    [[ "${output}" == '897134195989bd5c97c5dca94b00c434' ]]
}

@test "fetch: valid snapshot url => download file and checksum" {

    local outputfile="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
            "https://artifactory.wago.local/artifactory/pl_controller-generic-prod/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz" \
        "${outputfile}" \
        "${md5file}"

    echo "fetch: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${outputfile}" ]]

    run tar tf "${outputfile}"

    echo "tar: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${md5file}" ]]
}


@test "fetch: invalid url => create nothing, return error" {

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${checksum}" ]]
    
    [[ -n "${output}" ]]
}

@test "fetch: invalid url ending with ?lastModified => create nothing, return error" {

    run ./artifactory.sh \
        fetch \
        "invalid_url?lastModified" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${checksum}" ]]
    
    [[ -n "${output}" ]]
}


@test "fetch: JFROG_APIKEY empty => return error and output message" {

    export JFROG_APIKEY=

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    echo "${output}"

    [[ "${output}" =~ 'Please make sure the environment variable JFROG_APIKEY is set' ]]
}

@test "fetch: JFROG_APIKEY not set => return error and output message" {

    unset JFROG_APIKEY

    run ./artifactory.sh \
        fetch \
        "invalid_url" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    echo "${output}"

    [[ "${output}" =~ 'Please make sure the environment variable JFROG_APIKEY is set' ]]
}

@test "fetch: empty url => create nothing, return error" {

    run ./artifactory.sh \
        fetch \
        "" \
        "${file}" \
        "${checksum}"

    [[ "$status" != "0" ]]

    [[ ! -f "${file}" ]]
    [[ ! -f "${checksum}" ]]
    
    [[ -n "${output}" ]]
}

@test "get_latest_artifact_url: invalid URL => do nothing, return error" {

    run ./artifactory.sh \
        get_latest_artifact_url \
        "https:///blabla"

    echo "${output}"

    [[ "$status" != "0" ]]

    [[ "${output}" == "" ]]
}


@test "get_latest_artifact_url: valid URL (artifactory) => print URL to stdout" {

    run ./artifactory.sh \
        get_latest_artifact_url \
        'https://artifactory/api/search/latestVersion?g=wago_intern%2FLibbacnet%2Freleases&a=Libbacnet-Source&repos=pl_controller-generic-prod-local&v=*'

    echo "${output}"

    [[ "$status" == "0" ]]

    [[ "${output}" == 'https://artifactory/artifactory/pl_controller-generic-prod-local/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz' ]]
}

@test "get_latest_artifact_url: valid URL (artifactory.wago.local) => print URL to stdout" {

    run ./artifactory.sh \
        get_latest_artifact_url \
        'https://artifactory/api/search/latestVersion?g=wago_intern%2FLibbacnet%2Freleases&a=Libbacnet-Source&repos=pl_controller-generic-prod-local&v=*'

    echo "${output}"

    [[ "$status" == "0" ]]

    [[ "${output}" == 'https://artifactory/artifactory/pl_controller-generic-prod-local/wago_intern/Libbacnet/releases/Libbacnet-Source/Libbacnet-Source-2.0.0.tgz' ]]
}

@test "bash: parse for artifactory base url" {

    [[ "$(echo 'https://artifactory/artifactory/api/search/latestVersion?g=wago_intern%2FBACNet_Stack%2Ffeat-jenkinsfile-refactor&a=BACnet_Stack-Source&repos=pl_controller-generic-staging-local&v=*' | sed -e 's+^\(https://[^/]*\).*$+\1+')" == 'https://artifactory' ]]
}

@test "bash: parse for artifact name" {

    [[ "$(echo 'https://artifactory/artifactory/api/search/latestVersion?g=wago_intern%2FBACNet_Stack%2Ffeat-jenkinsfile-refactor&a=BACnet_Stack-Source&repos=pl_controller-generic-staging-local&v=*' | sed -e 's+^.*&a=\([^&]*\).*$+\1+')" == 'BACnet_Stack-Source' ]]
}

@test "bash: parse for api search pattern" {
    [[ "https://bla/artifactory/api/search/latestVersion?g=abc&a=def&repos=jkl&v=*" =~ /artifactory/api/search/latestVersion\? ]]
}

@test "fetch: URL contains REST API request to download latest version => always re-download" {

    local file="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5sum="${TESTCASE_TMPDIR}/md5sum.${BATS_TEST_NUMBER}"

    touch "${file}"
    touch "${md5sum}"

    local file_last_modification_epoch="$(date +'%s%N' -r "${file}")"
    local md5sum_last_modification_epoch="$(date +'%s%N' -r "${md5sum}")"
    
    echo "PRE: Modification date of payload file: $(stat -c '%y' "${file}")"   
    echo "PRE: Modification date of md5sum file: $(stat -c '%y' "${md5sum}")"   
    
    run ./artifactory.sh \
        fetch \
        'https://artifactory/api/search/latestVersion?g=wago_intern%2FLibbacnet%2Freleases&a=Libbacnet-Source&repos=pl_controller-generic-prod-local&v=*' \
        "${file}" \
        "${md5sum}"

    [[ "$status" == "0" ]]
   
    # downloaded gzip contains original change date of 2019 => use -lt to compare
    echo "POST: Modification date of payload file: $(stat -c '%y' "${file}")"   
    [[ "$(date -r "${file}" +'%s%N')" -lt "${file_last_modification_epoch}" ]]
    
    echo "POST: Modification date of md5sum file: $(stat -c '%y' "${md5sum}")"   
    [[ "$(date -r "${md5sum}" +'%s%N')" -gt "${md5sum_last_modification_epoch}" ]]
}

@test "fetch: valid url containing REST API request to download latest version => download latest deployed file and checksum" {

    local outputfile="${TESTCASE_TMPDIR}/sourcefile.${BATS_TEST_NUMBER}"
    local md5file="${TESTCASE_TMPDIR}/checksum.${BATS_TEST_NUMBER}"

    run ./artifactory.sh \
        fetch \
        'https://artifactory/api/search/latestVersion?g=wago_intern%2FLibbacnet%2Freleases&a=Libbacnet-Source&repos=pl_controller-generic-prod-local&v=*' \
        "${outputfile}" \
        "${md5file}"

    echo "fetch: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${outputfile}" ]]

    run tar tf "${outputfile}"

    echo "tar: ${output}"

    [[ "$status" == "0" ]]

    [[ -f "${md5file}" ]]

    run cat "${md5file}"

    echo "cat: ${output}"

    [[ "${output}" == '897134195989bd5c97c5dca94b00c434' ]]
}
