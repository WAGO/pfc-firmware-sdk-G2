#!/usr/bin/env bash

#source ../delete-unused-patches.sh

rm(){
    echo "rm $*"
}

export -f rm

test_delete_unused_patches_acceptance_quilt()
{
    assertEquals \
        "$(< resources/delete_unused_patches_acceptance.out)" \
        "$(../delete-unused-patches.sh resources/logfile.fw9.1st-stage resources/sample_workspace_with_patches/ptxproj wago-pfcXXX)" 
}

test_delete_unused_patches_with_gnu_patch()
{
    assertEquals \
        "$(< resources/delete_unused_patches_patch_with_gnu_patch.out)" \
        "$(../delete-unused-patches.sh resources/logfile_patch_with_gnu_patch resources/sample_workspace_with_patches/ptxproj wago-pfcXXX)" 
}

test_keep_patches_directories_containing_only_autogen_sh()
{
    assertEquals \
        "$(< resources/keep_autogen_only_patch_dirs.out)" \
        "$(../delete-unused-patches.sh resources/logfile.f2fs-tools resources/sample_workspace_with_patches/ptxproj wago-pfcXXX)" 
}


. shunit2


