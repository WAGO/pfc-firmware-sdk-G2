__UNITTESTS_NOCOLOR=${UNITTESTS_NOCOLOR:-""}
__UNITTESTS_VERBOSE=${UNITTESTS_VERBOSE:-""}
__UNITTESTS_ABORT_ON_ERROR=${UNITTESTS_ABORT_ON_ERROR:-""}

if [[ "$__UNITTESTS_NOCOLOR" == "" ]]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    NOCOLOR='\033[0m'
else
    RED=""
    GREEN=""
    NOCOLOR=""
fi

MAX_LINELEN=80

function print_test_name
{
    local line="Running test \"$1\""
    local linelen=${#line}
    
    local pump
    pump=$(head -c $((MAX_LINELEN - linelen - 8)) < /dev/zero | tr '\0' '.')
    line=${line}${pump}

    printf "%s" "$line"
}

function print_success
{
    # shellcheck disable=SC2059
    printf ${GREEN}"[SUCCESS]"${NOCOLOR}"\n"
}

function print_failure
{
    printf ${RED}"[FAILURE]"${NOCOLOR}"\n"
}

function print_error
{
    printf ${RED}"[ERROR]"${NOCOLOR}"\n"
}

function print_diff
{
    local leftfile=$1
    local rightfile=$2
    if [[ "${__UNITTESTS_VERBOSE}" != "" ]]; then
        echo "${leftfile}                      ${rightfile}"
        echo
        echo "===Result: =========================================================== Expected: =============="
        diff -y "${leftfile}" "${rightfile}"
        echo "==============================================================================================="
    fi

}

function expect_true
{
    __EXPECTED_VALUE=0
    _expect "$@"
    __EXPECTED_VALUE=
}

function expect_false
{
    __EXPECTED_VALUE=1
    _expect "$@"
    __EXPECTED_VALUE=
}

# NOT TO BE CALLED DIRECTLY
# Use expect_true/expect_false
function _expect
{
    local func=$1

    shift 1   

    local -a funcArgs

    # workaround for "unset variable array"
    if [[ -n "$*" ]]; then
        funcArgs=("$@")
    else
        funcArgs+=("")
    fi

    local testname=${FUNCNAME[2]}

    print_test_name "${testname}"

    ${func} "${funcArgs[@]}"  > "run/${testname}.out" 2>&1

    if [[ "$?" == "${__EXPECTED_VALUE}" ]]; then
        print_success
    else
        print_failure
    fi

    rm -f "run/${testname}.out"
}

function runtest
{
    local func=$1

    shift 1   

    local -a funcArgs
    
    # workaround for "unset variable array"
    if [[ -n "$*" ]]; then
        funcArgs=( "$@" )
    else
        funcArgs+=("")
    fi

    local testname=${FUNCNAME[1]}

    print_test_name "${testname}"

    rm -rf run/*

    if ! ${func} "${funcArgs[@]}" > "run/${testname}.out" 2>&1; then
        print_error

        if [[ "${__UNITTESTS_ABORT_ON_ERROR}" != "" ]]; then
            exit 0
        fi

        return
    fi

    if diff -q "run/${testname}.out" "resources/${testname}.out" > /dev/null; then
        print_success
    else
        print_failure
        print_diff "run/${testname}.out" "resources/${testname}.out"
    
        if [[ "${__UNITTESTS_ABORT_ON_ERROR}" != "" ]]; then
            exit 0
        fi
    fi
}
