# BSP Scripts

This directory contains scripts to prepare the PTXdist project for shipment as SDK.

## BSP build

The BSP build consists of two build stages:
1. Regular build
2. BSP build stage

The regular build is used to create binary artifacts of the closed source packages.
The BSP build stage removed the sources of all closed source and unused open source
packages and runs a build using the previously created binary artifacts. This verifies
that the SDK is able to create a valid firmware image.

### Tasks of the BSP build stage

- remove internal sources (`wago_intern`)
- remove unselected configurations
- remove unselected packages
  - remove PTXdist rule files
  - remove sources
  - remove patches
- build images with only selected OSS packages and binaries of closed source packages available
- verify that the build result corresponds with the first build stage  
  _(make sure, the 2nd build stage installs exactly the same packages as the 1st stage)_

## Build server interface

The BSP build is controlled by the build server. The build server is responsible for

- starting the regular build (1st build stage)
- preparing the BSP build (before 2nd build stage)
- starting the BSP build stage (2nd build stage)

### Prepare BSP build stage

The following steps are performed by the build server, before the BSP build stage is invoked:

- provide PTXdist sources in `ptxdist` directory
- provide toolchain archive in a file named `toolchain/{BSP_TARGET_ARCH}.tar.gz`
- provide toolchain source archive in a file named `toolchain/{BSP_TARGET_ARCH}-src.tar.gz`
- provide previously built PTXdist project in `bsp` directory
- provide all `.sh` files from `bsp/ptxproj.2013.03.0/scripts/wago/bsp-scripts` in the root of the working directory

Where `{BSP_TARGET_ARCH}` is a environment variable, provided by the build server (see below).

### Start BSP build stage

After preparation as described above, the script [create-bsp-main.sh](create-bsp-main.sh) is
invoked. No command line arguments are used since all configuration is done using environment variables.

### Environment Variables

| Variable | Value | Description |
| -------- | ----- | ----------- |
| __SCRIPT_DIR | &lt;unset&gt; | only used for unit tests |
| BSP_SCRIPT_ENABLE_DEBUG | &lt;unset&gt; | only used for local debugging |
| BSP_TARGET_ARCH | arm-linux-gnueabihf | name of the toolchain |
| BSP_TARGET_PLATFORM | wago-pfcXXX | name of the seleceted platform |
| BSP_TOOLCHAIN_NAME | LINARO.Toolchain-2022.08-wago.1 | target name of the toolchain |
| BSP_SELECT_PTXDISTCONFIG | ptxconfig_pfc_g2 | name of the PTXdist config to select |
| BSP_SELECT_PTXDISTPLATFORMCONFIG | platformconfig | name of the PTXdist platform config to select |
| BSP_WORKAROUND_RULE_WHITELIST | rules/wago-config-007-separate-config-tools.in | colon-separated list of rule file to keep |
| BSP_OUTPUT_DIR | /workspace/bsp_output | output directory of BSP artifacts (SDK) |
| BSP_SW_VERSION | 040500 | firmware version |
| BSP_SW_INDEX | 00 | release index |
| BSP_SW_REVISION | 6399f7b65f87603ae99bc1487568843f29b9c5f3 | revision |
| BSP_PTXPROJ_VERSION | &lt;unset&gt; | unused |
| BSP_MAIN_FILENAME | &lt;unset&gt; | name of the BSP archive |
| BSP_MAIN_FILENAME_SUFFIX | &lt;unset&gt; | suffix of the BSP archive |

### Removed Variables

The following variables were used in the past and are not used anymore.

- `BSP_SKIP_TOOLCHAIN_FETCH_SRC`    
  Value: 1  
  Ancient BSP builds used to fetch the sources of the used toolchain in order to repackage them.
  This feature is not used anymore. The code is removed from BSP build. The build server may set
  this value in order to support ancient versions.
- `BSP_PLATFORM_EXTRA_ENABLE`  
  Value: &lt;unset&gt;  
  Contains a colon-separated list of PTXdist packages to enable in the platform configuration.
  Was never used in production.
- `BSP_PLATFORM_EXTRA_DISABLE`  
  Value: IMAGE_SD_DOWNGRADE:IMAGE_BOOT_DOWNGRADE  
  Contains a colon-separated list of PTXdist packages to disable in the platform configuration.
  Was used to disable internal packages, such as downgrade images. The build server may set
  this value in order to support ancient version.s
- `BSP_PTXCONF_EXTRA_ENABLE`  
  Value: &lt;unset&gt;  
  Contains a colon-separated list of PTXdist packages to enable in the PTXdist configuration.
  Was never used in production.
- `BSP_PTXCONF_EXTRA_DISABLE`  
  Value: &lt;unset&gt;  
  Contains a colon-separated list of PTXdist packages to disable in the PTXdist configuration.
  Was never used in production.
- `BSP_FW_IMAGE_NAME`  
  Value: &lt;unset&gt;  
  Purpose and value are unknown.

## BSP build process

The [create-bsp-main.sh](create-bsp-main.sh) is responisble to
- start [vm-main.sh](vm-main.sh) to build the firmware
- publish the created artifacts (SDK, toolchain, ptxdist) to `{BSP_OUTPUT_DIR}` directory

So, the main work is done in [vm-main.sh](vm-main.sh):

- pre_cleanup
- preserve_resources
- prepare_workspace
- remove_unused_files
- ptxdist_fix_config
- run_sanity_checks
- build_binaries_version
- verify_build
- print_stats
- create_distribution_files
- bsp_set_release_index_to_99   
- cleanup

### pre_cleanup

- remove internal files
  - wproject, .project files, .gitignore, ...
  - jenkinsfile
- remove inactive platforms
- remoove `wago_intern`
- remove inactive configurations
- rename project directory  to `ptxproj`
- remove inactive `ptxproj` directories

### preserve_resources

Copy `ptxconfig` and `platformconfig` to `RES`-directory (e.g. `/tmp/shared`).

### prepare_workspace

- setup ptxdist and toolchain
- modify `ptxconfig` and `platformconfig` (extra keys)

### remove_unused_files

- remove unused rules
- remove unused patches

### ptxdist_fix_config

- run `ptxdist oldconfig` to make shure all selected packages are enabled

### run_sanity_checks

Make sure, patches directory is not empty.

There is only one check and the comment says something about an error in delete-unused-patches.sh so that will be the original reason to check it but the requirement for an non-empty patches folder is something specific to the current state of the pfc project.

### build_binaries_version

Call `ptxdist go` in a loop, scan errors for missing files,
recover them and build image.

### verify_build

- verify that installed packages match 1st build stage
- verify that applied patches match 1st build stage
- verify that platformconfig is not different to 1st build stage
- verify that ptxconfig is not different to 1st build stage

### print_stats

Print some statisics, e.g. provided sourcen and installed packages.

### create_distribution_files

Run `make dist`.

### bsp_set_release_index_to_99

Set release index `99` in `/etc/REVISIONS`.

### cleanup

Remove temporary files.

## Run unit tests

Unit tests should be run in chroot to make sure all required tools are available (e.g. `bats`).

```
cd tests
./run_all.sh
```

On success, the script will print `✓ All OK` and return with exit code 0.
On failure, the script will print the failed test cases and return with a diffent exit code.

Note that `run_all.sh` runs all the shell scripts and bats tests in the test directory. Each script can also be executed separately.
