#!/usr/bin/env bats

source ../vm-main.sh

@test "Check if applied patches differ" {

    local PRINT_MSG=""
    run verify_applied_patches resources/logfile.fw9.1st-stage resources/logfile.fw9.2nd-stage
    [[ "$status" == "0" ]]
    
    echo "Output: ${lines[@]}"
    
    [[ "${lines[@]}" =~ ^\ *$  ]]
}

#@test "Check if installed packages differ" {
#
#    #local PRINT_MSG=""
#    run verify_installed_packages resources/logfile.fw9.1st-stage resources/logfile.fw9.2nd-stage
#    [[ "$status" == "0" ]]
#
#    echo "Output: ${lines[@]}"
#
#    [[ "${lines[@]}" =~ ^\ *$  ]]
#}
#
#function __print_differences_ref_bsp()
#{
#    local file1=$1
#    local file2=$2 
#    local awk_script=$3
#
#    local whitespaces="                                                 "
#
#    echo
#    echo "Only in default FW${whitespaces}Only in BSP"
#    echo "${whitespaces}"
#
#    comm -3 --output-delimiter="${whitespaces}" \
#    <(gawk --re-interval "${awk_script}" "${file1}" | sort | uniq) \
#    <(gawk --re-interval "${awk_script}" "${file2}" | sort | uniq)
#
#    echo "==============================================================================================="
#}
#
#function verify_installed_packages()
#{
#    echo "Checking installed packages in comparison to the reference (stage 1) build..."
#
#    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
#    # shellcheck disable=SC2016 
#    __print_differences_ref_bsp "${1}" "${2}" 'match($0, /Installing (.*) \((.*)\) to root/, a) {print a[1],a[2]}'
#
#}
#
#function verify_applied_patches()
#{
#    echo "Checking applied patches in comparison to the reference (stage 1) build..."
#
#    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
#    # shellcheck disable=SC2016 
#    __print_differences_ref_bsp \
#        "${1}" "${2}" \
#        'match($0, /patchin: (quilt|patch): apply \047(.*)\047/, a) \
#         { \
#             gsub(/^ptxdist-[0-9]{4}\.[0-9]{2}\.[0-9]+/, "ptxdist", a[2]); \
#             b=gensub(/ptxproj[^/]*\//, "ptxproj/", "g", a[2]); \
#             print b \
#         }'
#}
#
#function verify_platformconfig_differences()
#{
#    echo "Checking platform configuration in comparison to the reference (stage 1) build..."
#    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
#    # shellcheck disable=SC2016 
#    __print_differences_ref_bsp "${1}" "${2}" '/^[^#]/ {print $1}'
#}
#
#function verify_ptxconfig_differences()
#{
#    echo "Checking ptxdist configuration in comparison to the reference (stage 1) build..."
#
#    # Expressions don't expand in single-quotes. Is okay: wildcards are evaluated by awk
#    # shellcheck disable=SC2016 
#    __print_differences_ref_bsp "${1}" "${2}" '/^[^#]/ {print $1}'
#}
#
#
#function verify_build()
#{
#    verify_installed_packages   "${HEADDIR}/logfile.release_build" "$(get_log_file)"
#    
#    verify_applied_patches      "${HEADDIR}/logfile.release_build" "$(get_log_file)"
#
#    verify_platformconfig_differences "${RES}/${SELECT_PTXDISTPLATFORMCONFIG}" "${WS}/selected_platformconfig"
#
#    verify_ptxconfig_differences "${RES}/${SELECT_PTXDISTCONFIG}" "${WS}/selected_ptxconfig"
#
#    # TODO:
#    #  o write to a protocol file?
#}
#
#
#
