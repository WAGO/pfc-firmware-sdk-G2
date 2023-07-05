//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev$
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>
#include <cstdbool>

#include <glib.h>

#include "../ct_error_handling.h"

extern "C"
{
#include "../libnet/ct_libnet.h"
}

TEST_GROUP(ct_libnet_netmask_convert)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST_GROUP(ct_libnet_calc_broadcast)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

// in case we decide to replace asserts with proper checks
IGNORE_TEST(ct_libnet_netmask_convert, ct_libnet_test_asserts)
{
    char value[16];
    value[0] = '\0';

    // is prevented by an assert
    int status = ct_libnet_netmask_to_classic(NULL, value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);
    
    status = ct_libnet_netmask_to_classic(value, NULL, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    status = ct_libnet_netmask_to_classic(value, value, 0);
    LONGS_EQUAL(INVALID_PARAMETER, status);

    strcpy(value, "a");
    status = ct_libnet_netmask_to_classic("24", value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);
    value[0] = '\0';

    status = ct_libnet_netmask_to_cidr(NULL, value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);
    
    status = ct_libnet_netmask_to_cidr(value, NULL, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    status = ct_libnet_netmask_to_cidr(value, value, 0);
    LONGS_EQUAL(INVALID_PARAMETER, status);

    strcpy(value, "a");
    status = ct_libnet_netmask_to_cidr("255.255.255.0", value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);
    value[0] = '\0';

}

const char* classicNetmasks[] = 
{
    "0.0.0.0",

   "128.0.0.0",
   "192.0.0.0",
   "224.0.0.0",
   "240.0.0.0",
   "248.0.0.0", 
   "252.0.0.0", 
   "254.0.0.0", 
   "255.0.0.0", 
   "255.128.0.0", 
   "255.192.0.0",  
   "255.224.0.0", 
   "255.240.0.0",
   "255.248.0.0",
   "255.252.0.0",
   "255.254.0.0",
   "255.255.0.0",
   "255.255.128.0",
   "255.255.192.0",
   "255.255.224.0",
   "255.255.240.0",
   "255.255.248.0",
   "255.255.252.0",
   "255.255.254.0",
   "255.255.255.0",
   "255.255.255.128",
   "255.255.255.192",
   "255.255.255.224",
   "255.255.255.240",
   "255.255.255.248",
   "255.255.255.252",
   "255.255.255.254",
   "255.255.255.255",
   NULL
};

const char* cidrNetmasks[] = 
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30",
    "31",
    "32",
    NULL
};

TEST(ct_libnet_netmask_convert, ct_libnet_convert_test)
{
    int status = SUCCESS;

    char value[16];
    value[0] = '\0';

    int index = 0;

    // test illegal parameters
    status = ct_libnet_netmask_to_classic("355.255.255.0", value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    status = ct_libnet_netmask_to_cidr("33", value, sizeof(value));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    // test legal parameters
    while(NULL != classicNetmasks[index])
    {

        // classic -> classic
        value[0] = '\0';
        status = ct_libnet_netmask_to_classic(classicNetmasks[index], value, sizeof(value));
        LONGS_EQUAL(SUCCESS, status);
        STRCMP_EQUAL(classicNetmasks[index], value);

        // cidr -> classic
        value[0] = '\0';
        status = ct_libnet_netmask_to_classic(cidrNetmasks[index], value, sizeof(value));
        LONGS_EQUAL(SUCCESS, status);
        STRCMP_EQUAL(classicNetmasks[index], value);

        // cidr -> cidr
        value[0] = '\0';
        status = ct_libnet_netmask_to_cidr(cidrNetmasks[index], value, sizeof(value));
        LONGS_EQUAL(SUCCESS, status);
        STRCMP_EQUAL(cidrNetmasks[index], value);

        // classic -> cidr
        value[0] = '\0';
        status = ct_libnet_netmask_to_cidr(classicNetmasks[index], value, sizeof(value));
        LONGS_EQUAL(SUCCESS, status);
        STRCMP_EQUAL(cidrNetmasks[index], value);

        ++index;
    }
        value[0] = '\0';

        // test stripping: cidr values are auto-stripped by strtol; classic values have to be stripped
        // by the caller
        status = ct_libnet_netmask_to_classic(" 12 ", value, sizeof(value));
        LONGS_EQUAL(SUCCESS, status);
        STRCMP_EQUAL("255.240.0.0", value);

        value[0] = '\0';
        status = ct_libnet_netmask_to_cidr(" 255.255.254.0  ", value, sizeof(value));
        LONGS_EQUAL(INVALID_PARAMETER, status);

}


TEST(ct_libnet_calc_broadcast, ct_libnet_calc_broadcast)
{
    int status = SUCCESS;

    char broadcast[strlen("255.255.255.255") + 1];

    // empty ip address
    status = ct_libnet_calculate_broadcast("", "255.255.255.0", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    // invalid ip address
    status = ct_libnet_calculate_broadcast("10.11.12.300", "255.255.255.0", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    // empty netmask
    status = ct_libnet_calculate_broadcast("10.11.12.13", "", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    // invalid netmasks
    status = ct_libnet_calculate_broadcast("10.11.12.13", "0.255.255.255", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);
    
    status = ct_libnet_calculate_broadcast("10.11.12.13", "255.254.255.0", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    status = ct_libnet_calculate_broadcast("10.11.12.13", "255.0.255.255", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    status = ct_libnet_calculate_broadcast("10.11.12.13", "255.0.255", broadcast, sizeof(broadcast));
    LONGS_EQUAL(INVALID_PARAMETER, status);

    // CIDR netmasks
    status = ct_libnet_calculate_broadcast("10.11.12.13", "24", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("10.11.12.255", broadcast);

    status = ct_libnet_calculate_broadcast("10.11.12.13", "27", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("10.11.12.31", broadcast);

    status = ct_libnet_calculate_broadcast("10.11.12.130", "27", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("10.11.12.159", broadcast);


    // classic netmasks
    status = ct_libnet_calculate_broadcast("10.11.12.13", "255.255.255.0", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status); 
    STRCMP_EQUAL("10.11.12.255", broadcast);

    status = ct_libnet_calculate_broadcast("10.11.12.13", "255.255.255.128", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status); 
    STRCMP_EQUAL("10.11.12.127", broadcast);

    status = ct_libnet_calculate_broadcast("10.11.12.130", "255.255.255.128", broadcast, sizeof(broadcast));
    LONGS_EQUAL(SUCCESS, status); 
    STRCMP_EQUAL("10.11.12.255", broadcast);

}
