#!/bin/bash

# grep patchin:\ quilt.\ apply\ \'ptxproj.2013.03.0/.*/patches/[^/]* logfile -o

function find_dir_in_list()
{
    local name=$1
    shift 1
    local list=("$@")

    for dir in "${list[@]}"; do
        # Cheap way to truncate workspace-specific prefix from dir argument
        # that looks like path/to/my/workspace/[pattern_in_dir]
        if [[ "${name}" =~ "${dir}"$ ]]; then
            true
            return
        fi
    done

    false
}

function main()
{
    local original_logfile=$1
    local WS=$2
    local PLATFORM=$3

    echo "$0: logfile: $original_logfile"

    local used_patch_dirs

   # use awk to find patch directories used in the previous build run.
    # Notes:    \047 stands for ' (apostrophe).
    #           [/generic]/series is truncated from the path end.
    #           some packages do not include patches but still apply autogen.sh
    #          located in the patches directory.
 
    # shellcheck disable=SC1004
    # literal backslashes are consumed by gawk 
    mapfile -t used_patch_dirs < <( gawk \
                           'match($0, /(patchin: (quilt|patch): apply |pkg_patch_autogen: )\047(ptxproj.*)\047/, path) \
                            \
                             { \
                                 processed_path=gensub(/\/autogen.sh$/, "", "g", path[3]);\
                                 processed_path=gensub(/\/series$/, "", "g", processed_path);\
                                 processed_path=gensub(/\/generic$/, "", "g", processed_path); \
                                 processed_path=gensub(/ptxproj[^/]*\//, "ptxproj/", "g", processed_path); \
                                 if(processed_path != "") { print processed_path; }; \
                             }' "${original_logfile}" | sort -u)

    echo "$0: used patch dirs:"
    for dir in "${used_patch_dirs[@]}"; do
        echo "    $dir"
    done

    for patch_dir in "${WS}/patches/"* "${WS}/configs/${PLATFORM}/patches/"*; do
        if [[ -d "${patch_dir}" ]]; then
            
            if ! find_dir_in_list "${patch_dir}" "${used_patch_dirs[@]}"; then
                echo "$0: deleting unused patch directory: ${patch_dir}..."
                rm -rf "${patch_dir}"
            fi

        fi
    done
}

main "$@"
