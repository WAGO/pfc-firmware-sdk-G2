#!/usr/bin/env bash
#shellcheck disable=SC1091,SC2034

# Globally disabled warnings:
# SC2034: foo appears unused. Verify it or export it.
#   local variables used in external functions trigger this warning (essential
#   technique for unit tests)
#
# SC1091: Not following: (error message here)
#   Shellcheck vim plugin cannot follow the sourced scripts
#

source ./unittest-common.sh

function_shift()
{
    local head=$1 
    shift 1
    local tail=( "$@" )

    echo "head: ${head} tail: ${tail[*]}"
}

test_function_arguments_shift()
{
    assertEquals "head: myfunc tail: a b cd e" "$(function_shift myfunc a b cd e)"
}

test_empty_array_definition()
{
    set -o nounset
    local -a myarray

    myarray+=""

    assertEquals "" "${myarray[@]}"
    set +o nounset
}

test_gawk_is_available()
{
    local prog=gawk

    #shellcheck disable=SC2016
    # Expressions do not expand in single quotes
    # The following is the expected way to pass a statement to shunit2 assertion
    assertTrue "${prog} not available. Install ${pkg:-it}!" '[[ -n "$(which ${prog})" ]]'
}

test_bc_is_available()
{
    local prog=bc

    #shellcheck disable=SC2016
    # Expressions do not expand in single quotes
    # The following is the expected way to pass a statement to shunit2 assertion
    assertTrue "${prog} not available. Install ${pkg:-it}!" '[[ -n "$(which ${prog})" ]]'
}

test_subshell_return_true()
{
    false
    ( true )
    local result=$?

    assertTrue "subshell return value should be 'true'"  "$result"
}

test_subshell_return_false()
{
    true
    ( false )
    local result=$?

    assertFalse "subshell return value should be 'true'"  "$result"
}

test_shunit_assert_in_subshell()
{
    ( assertTrue 0 )
    local result=$?

    assertTrue $result
}

test_shunit_assert_in_subshell_fail()
{
    ( assertTrue "Ignore this message." 1 )
    local result=$?

    assertFalse $result
}

. shunit2
