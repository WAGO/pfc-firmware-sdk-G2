//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test.cpp
///
///  \brief    Simple tests to show googletest working in PTXdist packet.
///
///  \author   Jens Breitenstein : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>

#include <cstring>
#include <string>
#include <array>

extern "C" {
#include "wagosnmp_API.h"
// It should not be necessary to include internal header, but in order to use libwagosnmp_Int32ToTlv,
// some definitions from asn are required.
#include "wagosnmp_internal.h"
}

using namespace testing;

constexpr char iod[] = "1.2.3.4.5.6.7.8.1.2.3.4.5.6.3.2.3.4";

static void init(tWagoSnmpTranceiver *trcvr, tWagoSnmpTrcvType trcvType, tWagoSnmpTlv *tlv) {
    memset(trcvr, 0, sizeof(*trcvr));
    trcvr->version = SNMP_VERSION_2c;
    trcvr->retries = SNMP_DEFAULT_RETRIES;
    trcvr->timeout_us = 1000;
    trcvr->trcvType = trcvType;
    trcvr->sHost = const_cast<char*>("127.0.0.2");
    trcvr->szHost = strlen(trcvr->sHost);
    trcvr->sCommunity = const_cast<char*>("public");
    trcvr->szCommunity = strlen(trcvr->sCommunity);
    trcvr->sOID = const_cast<char*>(iod);
    trcvr->szOID = strlen(trcvr->sOID);
    trcvr->sInformOID = const_cast<char*>(iod);
    trcvr->szInformOID = strlen(trcvr->sInformOID);
    trcvr->typData = tlv;
}

TEST(WAT36097, SendInformWithTLVStringMemoryAlloc) {
    tWagoSnmpTlv req;
    tWagoSnmpTranceiver trcvr;
    memset(&req, 0, sizeof(req));

    init(&trcvr, WAGOSNMP_TRCV_INFORM, &req);


    std::string inform_data = "inform_data";
    // data longer than 40 bytes triggers memory allocation in net-snmp.
    // upto 40 bytes are stored netsnmp_variable_list->buf directly.
    inform_data.append(42, 'a');

    auto ret = libwagosnmp_StrToTlv(inform_data.c_str(), ASN_OCTET_STR, &req);
    ASSERT_THAT(ret, Eq(WAGOSNMP_RETURN_OK));

    ret = libwagosnmp_Tranceive(&trcvr);

    libwagosnmp_NullToTlv(&req);
    libwagosnmp_NullToTlv(&req);
}

//---- End of source file ------------------------------------------------------
