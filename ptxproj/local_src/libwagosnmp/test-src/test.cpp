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

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
TEST(Tlv, size) {
  printf("sizeof netsnmp_variable_list: %lu\n", sizeof(netsnmp_variable_list));
  printf("sizeof tWagoSnmpTlv: %lu\n", sizeof(tWagoSnmpTlv));
  static_assert(sizeof(tWagoSnmpTlv) >= sizeof(netsnmp_variable_list), "tWagoSnmpTlv is too small to hold netsnmp_variable_list");
}

TEST(Tlv, callSo) {
  // setup

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_BAD_DATATYPE, libwagosnmp_Int32ToTlv(0, ASN_BOOLEAN, NULL));

  // teardown
}

TEST(Tlv, CallSoWithParameter) {
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0, sizeof(tlvData));
  int32_t input = 12;

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData));

  // teardown
}

TEST(Tlv, Uninitialized_DeathTest) {
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0xCC, sizeof(tlvData));
  int32_t input = 12;

  // test
  ASSERT_DEATH(libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData), "");

  // teardown
}

TEST(Tlv, ConversionInt) {
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0, sizeof(tlvData));
  int32_t input  = 12;
  int32_t output = 0;

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData));
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_TlvToInt32(&tlvData, &output));
  ASSERT_EQ(input, output);

  // teardown
}

TEST(Tlv, ConversionString) {
  // setup
  tWagoSnmpTlv tlvData;
  ::std::string input;
  ::std::array<char, WAGOSNMP_MAX_STR_LEN + 1> output{};
  memset(output.data(), 'x', output.size());

  for(int length = 1; length <= WAGOSNMP_MAX_STR_LEN; length++) {
    input.append(1, 'a');
    memset(&tlvData, 0, sizeof(tlvData));
    // test
    ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_StrToTlv(input.c_str(), ASN_OCTET_STR, &tlvData));
    ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_TlvToStr(&tlvData, output.data(), WAGOSNMP_MAX_STR_LEN));
    ASSERT_EQ(strnlen(output.data(), output.size()), length);
    ASSERT_STREQ(input.c_str(), output.data());
    libwagosnmp_TlvDeinit(&tlvData);
  }

  // teardown
}

struct SecAuthParams {
  SnmpAuhtenticationProtocol auth;
  SnmpPrivacyProtocol priv;
  tWagoSnmpSecLevel secLevel;
  oid* expectedAuthProto;
  int expectedAuthProtoLen;
  oid* expectedPrivProto;
  int expectedPrivProtoLen;
};

class SetAuthPrivTestFixture : public ::testing::TestWithParam<SecAuthParams> {
 protected:
  tWagoSnmpTranceiver transceiver;
  netsnmp_session session;
  ::std::string pass{"asdfASDF"};

  void SetUp() override {
    transceiver.sAuthPass  = const_cast<char*>(pass.c_str());
    transceiver.sPrivPass  = const_cast<char*>(pass.c_str());
    transceiver.szAuthPass = pass.size();
    transceiver.szPrivPass = pass.size();
  }
};

TEST_P(SetAuthPrivTestFixture, ShouldInitializeSessionSecAuthFields) {
  ::std::memset(&session, 0, sizeof(session));

  auto param = GetParam();

  transceiver.typAuthProt = param.auth;
  transceiver.typPrivProt = param.priv;
  transceiver.typSecLevel = param.secLevel;
  session.securityLevel   = param.secLevel;

  auto res = INTERNAL_SetAuthPriv(&transceiver, &session);

  EXPECT_EQ(res, WAGOSNMP_RETURN_OK);
  EXPECT_EQ(session.securityAuthProto, param.expectedAuthProto);
  EXPECT_EQ(session.securityPrivProto, param.expectedPrivProto);
  EXPECT_EQ(session.securityAuthProtoLen, param.expectedAuthProtoLen);
  EXPECT_EQ(session.securityPrivProtoLen, param.expectedPrivProtoLen);
}

INSTANTIATE_TEST_CASE_P(SecAuthTest, SetAuthPrivTestFixture,
                        ::testing::Values(SecAuthParams{AUTH_NONE, PRIVACY_NONE, SNMP_SEC_LEVEL_NOAUTH,
                                                        &usmNoAuthProtocol[0], USM_AUTH_PROTO_NOAUTH_LEN,
                                                        &usmNoPrivProtocol[0], USM_PRIV_PROTO_NOPRIV_LEN},
                                          SecAuthParams{AUTH_SHA512, PRIVACY_AES256, SNMP_SEC_LEVEL_AUTHPRIV,
                                                        &usmHMAC384SHA512AuthProtocol[0], USM_AUTH_PROTO_SHA_LEN,
                                                        &usmAES256PrivProtocol[0], 9},
                                          SecAuthParams{AUTH_SHA384, PRIVACY_AES256C, SNMP_SEC_LEVEL_AUTHPRIV,
                                                        &usmHMAC256SHA384AuthProtocol[0], USM_AUTH_PROTO_SHA_LEN,
                                                        &usmAES256CiscoPrivProtocol[0], 11}));

//---- End of source file ------------------------------------------------------
