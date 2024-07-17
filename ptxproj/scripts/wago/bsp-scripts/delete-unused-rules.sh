#!/bin/bash

# DEBUG: log output to file
if [[ "${SCR_DBG}" != "" ]]; then
    exec > >(tee -i logfile_$(basename $0).txt)
    exec 2>&1
fi

# used_rules_array: associative array with entries like "rules/foo.in" => "y"
declare -A used_rules_array
# unused_rules_array: simple array ( "rules/bar.in", "rules/baz.in" )
declare -a unused_rules_array

function init_file_lists()
{
    local configs=$1
    local used_rules

    local ifs=$IFS
    IFS=':'
 
    local enabled_keys
    for config in $configs; do
        enabled_keys="$enabled_keys $(grep ^PTXCONF_ "$config" | cut -d= -f1)"
    done
    
    echo "[WORKAROUND] Process rules whitelist..."
 
    for rule in ${WORKAROUND_RULE_WHITELIST}; do
        echo "    - ${rule}"
        used_rules_array["${rule}"]="y"
    done
    IFS=$ifs

    echo "Analyzing rules:"
    echo
    for key in $enabled_keys; do
        used_rules=$(grep -l 'config[[:space:]]\+'"${key#PTXCONF_}"'$' $(find rules/ configs/${PLATFORM}/rules/ -name "*.in"))

        if [[ ! -z "$used_rules" ]]; then

            # multiple hits possible
            for rule in $used_rules; do 
                used_rules_array["$rule"]="y"
                echo "$rule is in use: it contains an enabled key $key..."
            done
        fi
    done

    for rule in rules/*.in configs/${PLATFORM}/rules/*.in; do
        if [[ "${used_rules_array[$rule]}" != "y" ]]; then
                unused_rules_array+=($rule)
        fi
    done
}


function print_unused_rules()
{
    for rule in ${unused_rules_array[@]}; do
        echo "Unused: $rule"
    done
}

function delete_unused_rules()
{
    for rule in ${unused_rules_array[@]}; do
        rm -vf ${rule} ${rule%.in}.make ${rule%.in}.postinst
    done
}

function print_unused_source_statements()
{
    local section
    local file

    local delete=${1:-""}

    declare -A unused_sections
    local ptxdist_global_rules=(${PTXDIST_DIR}/rules/*.in)

    for rule in "${unused_rules_array[@]}"; do
        section=$(gawk 'match($0, /^## SECTION=(.*)$/, a) {print a[1]}' $rule)

        if ! grep -q '^## SECTION='"${section}"'$' ${!used_rules_array[@]} ${ptxdist_global_rules[*]}; then
            unused_sections["$section"]="y"
        fi

    done

    for section in "${!unused_sections[@]}"; do
        file=$(grep -l 'source.*'$section'\.in' rules/*.in configs/${PLATFORM}/rules/*.in Kconfig)
        # file may be "" for ptxdist default groups
        if [[ "$file" != "" ]] && [[ -f "$file" ]]; then
            echo -n "File $file contains unused source statement: $(grep 'source.*'$section'\.in' $file)!"
            if [[ "${delete}" == "delete=yes" ]]; then
                sed -i '/^[[:space:]]*source "generated\/'${section}'\.in"[[:space:]]*$/d' $file
                echo " Deleting..."
            else
                echo
            fi
        fi
    done
}

#function delete_empty_submenus()
#{
#    true
#}

# Needs to be called before removing unused .in files
function cleanup_select_statements()
{
    local key
    local files
    local pattern
    local select_line

    for rule in "${unused_rules_array[@]}"; do
        keys=$(gawk 'match ($0, /[[:space:]]*config[[:space:]]+([A-Z][A-Z0-9_]+)$/, a) {print a[1]}' ${rule})



        for key in ${keys}; do
            pattern='^[[:space:]]*select[[:space:]]+'"${key}"'([[:space:]]*|[[:space:]]+if[[:space:]]+.*)$' 
            files=$(grep -El ${pattern} ${!used_rules_array[@]})

            if [[ "$files" != "" ]]; then

                echo "=========================================="
                echo 
                echo "removed unused 'select ${key}' statements:"
                echo
                grep -HE ${pattern} ${files}
                echo

                sed -i  -e '/^[[:space:]]*select[[:space:]]\+'"${key}"'[[:space:]]*$/d' \
                        -e '/^[[:space:]]*select[[:space:]]\+'"${key}"'[[:space:]]\+if[[:space:]]\+.*$/d' ${files}
            fi

        done
    done
}

# Needs to be called before removing unused .in files
function delete_unused_source_statements()
{
    print_unused_source_statements 'delete=yes'
}

function print_file_without_unused_menu_entries()
{
    # ATTENTION:
    # To make unit testing easier this function prints results to stdout.
    # It is not allowed to contain any other output!

    # Use # as a delimiter to ignore comments by matching against $1 field.
    # $0 (complete line with comments) is used to preserve comments in output.
    [[ -f $1 ]] && gawk -F'#' '
        # cur_menu_contents and result are local variables
        function parse_menu(    cur_menu_contents,    result) {

            cur_menu_contents=$1 "\n"

            while(getline > 0) {

                if ($1 ~ /^[[:space:]]*menu .*$/) {
                    cur_menu_contents=cur_menu_contents parse_menu()
                } else {

                    cur_menu_contents=cur_menu_contents $0 "\n" 
                
                    if($1 ~ /^[[:space:]]*endmenu[[:space:]]*$/) {
                        
                        # Is the current menu empty?
                        
                        cur_menu_contents_no_comments = gensub(/#[^\n]*\n/,"", "g", cur_menu_contents)
                        
                        result=(match(cur_menu_contents_no_comments, /menu [^\n]*\n[[:space:]]*endmenu/) )? "" : cur_menu_contents
                        return result
                    }
                }

            }
        }

        # Entry point
        {
            if ( $1 ~ /^[[:space:]]*menu .*$/) {
                printf parse_menu()
            } else {
                # Default rule: when not inside menu/endmenu, just print current line
                print
            }
        }
    ' $1 
}

function delete_unused_menu_entries()
{
    local filelist="${@}"

    for file in ${filelist}; do
        echo "delete_unused_menu_entries: analyzing ${file}..."
        print_file_without_unused_menu_entries ${file} > ${file}.tmp

        if [[ "$?" == "0" ]]; then
            mv ${file}.tmp ${file}
        else
            echo "awk: Error deleting unused menu entries."
            rm ${file}.tmp
            false
            return
        fi
    done
}

function backup_rules()
{
    cp -r rules rules.orig
}

function main()
{
    PTXDIST_DIR=$1
    PLATFORM=$2
    SELECTED_CONFIGS=$3
    WS=$4
    WORKAROUND_RULE_WHITELIST=$5

    local in_files=("${WS}/Kconfig")

    init_file_lists "${SELECTED_CONFIGS}"
#    backup_rules
    delete_unused_source_statements
    cleanup_select_statements
    delete_unused_rules

    mapfile -O "${#in_files[@]}" -t in_files < <(compgen -G "${WS}/rules/*.in")
    mapfile -O "${#in_files[@]}" -t in_files < <(compgen -G "${WS}/configs/${PLATFORM}/rules/*.in")

    delete_unused_menu_entries "${in_files[@]}" || exit 1
}

# Don't run main when the file is being sourced.
# Required for unit testing.
if [[ "$0" == "${BASH_SOURCE[0]}" ]]; then
    main "$@"
fi
