#!/bin/bash

source ./unittest-common.sh

source ../delete-unused-rules.sh

mkdir -p run tmp

function test_non_empty_menu()
{
    local infile=non_empty_menu.in

    cp resources/${infile} tmp/

    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_empty_menu()
{
    local infile=empty_menu.in

    cp resources/${infile} tmp/
    
    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_nested_menus1()
{
    local infile=nested_menus1.in

    cp resources/${infile} tmp/
    
    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_comments()
{
    local infile=comments.in

    cp resources/${infile} tmp/
    
    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_comments_within_empty_menu()
{
    local infile=comments1.in

    cp resources/${infile} tmp/
    
    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_comments_within_non_empty_menu()
{
    local infile=comments2.in

    cp resources/${infile} tmp/
    
    runtest print_file_without_unused_menu_entries tmp/${infile}
    
    rm tmp/*
}

function test_delete_unused_menu_entries_error()
{
    print_test_name ${FUNCNAME[0]}
    delete_unused_menu_entries "FileThatDoesNotExist" >& /dev/null
    
    if [[ "$?" == "0" ]]; then
        print_failure
    else
        print_success
    fi
}

function test_delete_unused_menu_entries()
{
    local infile=Kconfig

    print_test_name ${FUNCNAME[0]}

    rm -f tmp/*
    cp resources/${infile} tmp/

    delete_unused_menu_entries "tmp/${infile}" >& /dev/null
    
    if [[ "$?" != "0" ]]; then
        print_failure
    else
        # Check result

        if diff -q tmp/${infile} resources/${FUNCNAME[0]}.out > /dev/null; then
            print_success
        else
            print_failure
            print_diff tmp/${infile} resources/${FUNCNAME[0]}.out 
        fi
    fi
    
    rm -f tmp/*
}

function test_delete_unused_menu_entries_multiple_files()
{
    local infile0=Kconfig
    local infile1=elrest_specific.in

    print_test_name ${FUNCNAME[0]}

    rm -f tmp/*
    cp resources/${infile0} tmp/
    cp resources/${infile1} tmp/

    # delete_unused_menu_entries changes files in-place.
    # When the file that would not be changed is not processed at all, the test would succeed.
    # To prevent this, we look at the logging output of the function.
    delete_unused_menu_entries tmp/${infile0} tmp/${infile1} > tmp/logging.out 
    
    if [[ "$?" != "0" ]]; then
        print_failure
    else
        # Check result

        local success="yes"
        for infile in logging.out ${infile0} ${infile1}; do
            if ! diff -q tmp/${infile} resources/${FUNCNAME[0]}_${infile}.out > /dev/null; then
                print_failure
                print_diff tmp/${infile} resources/${FUNCNAME[0]}_${infile}.out
                success="no"
                break
            fi
        done
    fi
    
    if [[ "${success}" == "yes" ]]; then
        print_success
    fi

    rm -f tmp/*
}

#FIXME: obsolete: reimplemented in BATS (./delete-unused-rules-tests.bats::"Obtain keys in use from one config file" {
function test_init_file_lists_base()
{
    PLATFORM=wago-pfcXXX
    SELECTED_CONFIG=selected_ptxconfig
    WS=resources/sample_workspace0
    WORKAROUND_RULE_WHITELIST=""

    print_test_name ${FUNCNAME[0]}
    
    pushd "${WS}" > /dev/null
    init_file_lists "${SELECTED_CONFIG}" > /dev/null
    popd > /dev/null
    echo "${!used_rules_array[@]}" > tmp/"${FUNCNAME}.out"

    # Check result

    local success="yes"   
    if ! diff -q "tmp/${FUNCNAME[0]}.out" "resources/${FUNCNAME[0]}.out" > /dev/null; then
        print_failure
        print_diff "tmp/${FUNCNAME[0]}.out" "resources/${FUNCNAME[0]}_${infile}.out"
        success="no"
    fi

    if [[ "${success}" == "yes" ]]; then
        print_success
    fi

    rm -f tmp/*
}

function test_init_file_lists_whitelist_base()
{
    PLATFORM=wago-pfcXXX
    SELECTED_CONFIG=selected_ptxconfig
    WS=resources/sample_workspace0
    WORKAROUND_RULE_WHITELIST="rules/foobar.in:baz.in"

    print_test_name ${FUNCNAME[0]}
    
    pushd "${WS}" > /dev/null
    init_file_lists "${SELECTED_CONFIG}" > /dev/null
    popd > /dev/null
    IFS=$'\n' sorted=($(sort <<<"${!used_rules_array[*]}")) ; unset IFS
    echo "${sorted[@]}" > tmp/"${FUNCNAME[0]}.out"

    # Check result

    local success="yes"   
    if ! diff -q "tmp/${FUNCNAME[0]}.out" "resources/${FUNCNAME[0]}.out" > /dev/null; then
        print_failure
        print_diff "tmp/${FUNCNAME[0]}.out" "resources/${FUNCNAME[0]}.out"
        success="no"
    fi

    if [[ "${success}" == "yes" ]]; then
        print_success
    fi

    rm -f tmp/*
}

test_non_empty_menu
test_empty_menu
test_nested_menus1
test_comments
test_comments_within_empty_menu
test_comments_within_non_empty_menu
test_delete_unused_menu_entries_error
test_delete_unused_menu_entries
test_delete_unused_menu_entries_multiple_files
test_init_file_lists_base
test_init_file_lists_whitelist_base
