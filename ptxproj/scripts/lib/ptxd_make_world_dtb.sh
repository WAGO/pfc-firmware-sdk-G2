#!/bin/bash
#
# Copyright (C) 2020 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

ptxd_make_dtb() {
    local dtc dts tmp_dts dtb deps tmp_deps

    if [ "$(basename "${dts_dts}")" != "${dts_dts}" ]; then
	ptxd_bailout "'${dts_dts}' must not include directories!" \
	    "Use <PKG>_DTS_PATH to specify the search path."
    fi

    if ! ptxd_in_path pkg_dts_path "${dts_dts}"; then
	ptxd_bailout "Device-tree '${dts_dts}' not found in '${pkg_dts_path}'."
    fi
    dts="${ptxd_reply}"
    dtb="${dtb_dir}/${dts_dts/%.dts/.dtb}"

    dtc="${pkg_build_dir}/scripts/dtc/dtc"
    if [ ! -x "${dtc}" ]; then
	dtc=dtc
    fi

    tmp_dts="${ptx_state_dir}/${pkg_label}.$(basename "${dts}").tmp"

    deps="${ptx_state_dir}/${pkg_label}.$(basename "${dts}").deps"
    tmp_deps="${PTXDIST_TEMPDIR}/${pkg_label}.$(basename "${dts}").deps"

    exec 2>&${PTXDIST_FD_LOGERR}

    echo "CPP $(ptxd_print_path "${dts}")" &&
    ptxd_eval \
	cpp \
	-Wp,-MMD,${tmp_deps} \
	-Wp,-MT,${dtb_deps_target} \
	-nostdinc \
	-I"$(dirname "${dts}")" \
	-I${pkg_dir}/arch/${pkg_arch}/boot/dts \
	-I${pkg_dir}/arch/${pkg_arch}/boot/dts/include \
	-I${pkg_dir}/drivers/of/testcase-data \
	-I${pkg_dir}/include \
	-undef -D__DTS__ -x assembler-with-cpp \
	-o ${tmp_dts} \
	${dts} &&

    sed -e "\;^ ${pkg_dir}[^ ]*;d" \
	-e 's;^ \([^ \]*\); $(wildcard \1);' "${tmp_deps}" > "${deps}" &&
    # empty line in case all dependencies were removed
    echo >> "${deps}" &&

    mkdir -p "$(dirname "${dtb}")" &&
    echo "DTC $(ptxd_print_path "${dtb}")" &&
    ptxd_eval \
	"${dtc}" \
	$(ptxd_get_ptxconf PTXCONF_DTC_EXTRA_ARGS) \
	-i "$(dirname "${dts}")" \
	-i "${pkg_dir}/arch/${pkg_arch}/boot/dts" \
	-d "${tmp_deps}" \
	-I dts -O dtb -b 0 \
	-o "${dtb}" "${tmp_dts}" &&

    awk "{ \
	    s = \"\"; \
	    for (i = 2; i <= NF; i++) { \
		if (\$i != \"${tmp_dts}\")
		    s = s \" \$(wildcard \" \$i \")\"; \
	    }; \
	    if (s != \""\"") {
		printf \"${dtb_deps_target}:\" s;  \
		print \"\"; \
	    } \
	}" "${tmp_deps}" >> "${deps}" ||

    ptxd_bailout "Unable to generate dtb file."
}
export -f ptxd_make_dtb


ptxd_make_world_dtb() {
    local dtb_deps_target dtb_dir

    ptxd_make_world_init || break

    dtb_deps_target="${ptx_state_dir}/${pkg_stamp}"
    dtb_dir="${pkg_pkg_dir}/boot"

    echo -e "\nBuilding Device trees..."

    for dts_dts in ${pkg_dts}; do
	ptxd_make_dtb || break
    done
}
export -f ptxd_make_world_dtb
