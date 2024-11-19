# -*-makefile-*-
#
# Copyright (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_MESALIB) += host-mesalib

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_MESALIB_CONF_TOOL	:= meson
HOST_MESALIB_CONF_OPT	:= \
	$(HOST_MESON_OPT) \
	-Dallow-kcmp=enabled \
	-Dandroid-libbacktrace=disabled \
	-Dandroid-stub=false \
	-Dbuild-aco-tests=false \
	-Dbuild-tests=false \
	-Dcustom-shader-replacement= \
	-Dd3d-drivers-path=/usr/lib/d3d \
	-Ddatasources=auto \
	-Ddraw-use-llvm=false \
	-Ddri-drivers-path=/usr/lib/dri \
	-Ddri-search-path=/usr/lib/dri \
	-Ddri3=disabled \
	-Degl=disabled \
	-Degl-lib-suffix= \
	-Degl-native-platform=auto \
	-Dexecmem=true \
	-Dexpat=disabled \
	-Dfreedreno-kmds= \
	-Dgallium-d3d10umd=false \
	-Dgallium-d3d12-video=disabled \
	-Dgallium-drivers=swrast \
	-Dgallium-extra-hud=false \
	-Dgallium-nine=false \
	-Dgallium-omx=disabled \
	-Dgallium-opencl=disabled \
	-Dgallium-rusticl=false \
	-Dgallium-va=disabled \
	-Dgallium-vdpau=disabled \
	-Dgallium-windows-dll-name=libgallium_wgl \
	-Dgallium-xa=disabled \
	-Dgbm=disabled \
	-Dgbm-backends-path= \
	-Dgles-lib-suffix= \
	-Dgles1=disabled \
	-Dgles2=disabled \
	-Dglvnd=false \
	-Dglvnd-vendor-name= \
	-Dglx=disabled \
	-Dglx-direct=false \
	-Dglx-read-only-text=false \
	-Dimagination-srv=false \
	-Dinstall-intel-gpu-tests=false \
	-Dintel-clc=disabled \
	-Dintel-xe-kmd=disabled \
	-Dlibunwind=disabled \
	-Dllvm=disabled \
	-Dlmsensors=disabled \
	-Dmicrosoft-clc=disabled \
	-Dmin-windows-version=8 \
	-Dmoltenvk-dir= \
	-Domx-libs-path=/usr/lib/dri \
	-Dopencl-spirv=false \
	-Dopengl=true \
	-Dosmesa=false \
	-Dperfetto=false \
	-Dplatform-sdk-version=25 \
	-Dplatforms= \
	-Dpower8=disabled \
	-Dradv-build-id='' \
	-Dselinux=false \
	-Dshader-cache=disabled \
	-Dshader-cache-default=true \
	-Dshader-cache-max-size=1G \
	-Dshared-glapi=enabled \
	-Dshared-llvm=disabled \
	-Dspirv-to-dxil=false \
	-Dsse2=true \
	-Dstatic-libclc=[] \
	-Dtools=glsl \
	-Dva-libs-path=/usr/lib/dri \
	-Dvalgrind=disabled \
	-Dvdpau-libs-path=/usr/lib/vdpau \
	-Dvideo-codecs=[] \
	-Dvmware-mks-stats=false \
	-Dvulkan-beta=false \
	-Dvulkan-drivers=[] \
	-Dvulkan-icd-dir=/etc/vulkan/icd.d \
	-Dvulkan-layers=[] \
	-Dxlib-lease=disabled \
	-Dxmlconfig=disabled \
	-Dzlib=enabled \
	-Dzstd=disabled

HOST_MESALIB_MAKE_OPT	:= \
	src/compiler/glsl/glsl_compiler

$(STATEDIR)/host-mesalib.install:
	@$(call targetinfo)
	install -D -m755 $(HOST_MESALIB_DIR)-build/src/compiler/glsl/glsl_compiler $(HOST_MESALIB_PKGDIR)/usr/bin/mesa/glsl_compiler
	@$(call touch)

# vim: syntax=make
