// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <stdint.h> //NOLINT(modernize-deprecated-headers) header is used for c projects
#include <stddef.h> //NOLINT(modernize-deprecated-headers) header is used for c projects

#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

int NetconfGetDipSwitchConfig(uint32_t* ip_address, uint32_t* netmask, uint8_t* value);

int NetconfSetDipSwitchConfig(uint32_t ip_address, uint32_t netmask);


#ifdef __cplusplus
}
#endif
