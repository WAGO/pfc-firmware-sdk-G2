#!/usr/bin/env bats

# Old behaviour: only consider keys defined in ptxconfig
@test "Obtain keys in use from one config file" {
    source ../delete-unused-rules.sh
    
    local PLATFORM=wago-pfcXXX
    local SELECTED_CONFIGS=selected_ptxconfig
    local WS=resources/sample_workspace0

    pushd "${WS}" > /dev/null
    run init_file_lists "${SELECTED_CONFIGS}"
    popd > /dev/null

    [[ "${lines[2]}" =~ "rules/foo.in is in use: it contains an enabled key PTXCONF_FOO" ]]
    [[ "${lines[3]}" =~ "rules/bar.in is in use: it contains an enabled key PTXCONF_BAR" ]]
}

# Use case: some packages (i.e. host-libkmod) are set in platformconfig
@test "Obtain keys in use from multiple config files" {
    source ../delete-unused-rules.sh
    
    local PLATFORM=wago-pfcXXX
    local SELECTED_CONFIGS=selected_ptxconfig:selected_platformconfig
    local WS=resources/sample_workspace0

    pushd "${WS}" > /dev/null
    run init_file_lists "${SELECTED_CONFIGS}"
    popd > /dev/null

    [[ "${lines[2]}" =~ "rules/foo.in is in use: it contains an enabled key PTXCONF_FOO" ]]
    [[ "${lines[3]}" =~ "rules/bar.in is in use: it contains an enabled key PTXCONF_BAR" ]]
    [[ "${lines[4]}" =~ "rules/platformconfig-foo.in is in use: it contains an enabled key PTXCONF_PLATFORMCONFIG_FOO" ]]
}


