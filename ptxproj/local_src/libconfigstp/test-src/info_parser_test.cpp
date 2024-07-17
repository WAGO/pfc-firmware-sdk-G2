// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "info_parser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "stp.hpp"

namespace wago::stp::lib {

class info_parser_test : public testing::Test {
 public:
  ::std::string mstpctl_showbridge = R"([
    {
        "bridge": "br0",
        "enabled": "yes",
        "bridge-id": "8.000.00:30:DE:42:5D:E4",
        "designated-root": "8.000.00:30:DE:42:5D:E4",
        "regional-root": "8.000.00:30:DE:42:5D:E4",
        "root-port": "",
        "path-cost": "10",
        "internal-path-cost": "11",
        "max-age": "12",
        "bridge-max-age": "13",
        "forward-delay": "14",
        "bridge-forward-delay": "15",
        "tx-hold-count": "16",
        "max-hops": "17",
        "hello-time": "18",
        "ageing-time": "19",
        "force-protocol-version": "rstp",
        "time-since-topology-change": "794",
        "topology-change-count": "1",
        "topology-change": "no",
        "topology-change-port": "None",
        "last-topology-change-port": "ethX12"
    }
])";

  ::std::string mstpctl_showportdetails = R"([
    {
        "port": "ethX1",
        "bridge": "br0",
        "enabled": "yes",
        "role": "Designated",
        "port-id": "8.001",
        "state": "forwarding",
        "external-port-cost": "200000",
        "internal-port-cost": "200000",
        "admin-external-cost": "0",
        "admin-internal-cost": "0",
        "designated-root": "8.000.00:30:DE:42:5D:E4",
        "dsgn-external-cost": "0",
        "dsgn-regional-root": "8.000.00:30:DE:42:5D:E4",
        "dsgn-internal-cost": "0",
        "designated-bridge": "8.000.00:30:DE:42:5D:E4",
        "designated-port": "8.001",
        "admin-edge-port": "no",
        "auto-edge-port": "yes",
        "oper-edge-port": "yes",
        "topology-change-ack": "no",
        "point-to-point": "yes",
        "admin-point-to-point": "auto",
        "restricted-role": "no",
        "restricted-TCN": "no",
        "port-hello-time": "2",
        "disputed": "no",
        "bpdu-guard-port": "no",
        "bpdu-guard-error": "no",
        "bpdu-filter-port": "no",
        "network-port": "no",
        "ba-inconsistent": "no",
        "num-tx-bpdu": "286",
        "num-rx-bpdu": "0",
        "num-rx-bpdu-filtered": "0",
        "num-tx-tcn": "0",
        "num-rx-tcn": "0",
        "num-transition-fwd": "1",
        "num-transition-blk": "1",
        "received-bpdu": "no",
        "received-stp": "no",
        "received-rstp": "no",
        "received-tc-ack": "no",
        "received-tcn": "no",
        "send-rstp": "yes"
    },
    {
        "port": "ethX2",
        "bridge": "br0",
        "enabled": "no",
        "role": "Disabled",
        "port-id": "8.002",
        "state": "discarding",
        "external-port-cost": "200000001",
        "internal-port-cost": "200000000",
        "admin-external-cost": "0",
        "admin-internal-cost": "0",
        "designated-root": "8.000.00:30:DE:42:5D:E4",
        "dsgn-external-cost": "0",
        "dsgn-regional-root": "8.000.00:30:DE:42:5D:E4",
        "dsgn-internal-cost": "0",
        "designated-bridge": "8.000.00:30:DE:42:5D:E4",
        "designated-port": "0.000",
        "admin-edge-port": "yes",
        "auto-edge-port": "yes",
        "oper-edge-port": "no",
        "topology-change-ack": "no",
        "point-to-point": "no",
        "admin-point-to-point": "auto",
        "restricted-role": "yes",
        "restricted-TCN": "no",
        "port-hello-time": "2",
        "disputed": "no",
        "bpdu-guard-port": "yes",
        "bpdu-guard-error": "no",
        "bpdu-filter-port": "yes",
        "network-port": "no",
        "ba-inconsistent": "no",
        "num-tx-bpdu": "0",
        "num-rx-bpdu": "0",
        "num-rx-bpdu-filtered": "0",
        "num-tx-tcn": "0",
        "num-rx-tcn": "0",
        "num-transition-fwd": "0",
        "num-transition-blk": "1",
        "received-bpdu": "no",
        "received-stp": "no",
        "received-rstp": "no",
        "received-tc-ack": "no",
        "received-tcn": "no",
        "send-rstp": "yes"
    }
])";
};

TEST_F(info_parser_test, ParseBridgeInformations) {
  stp_info info;

  status s = parse_stp_info(mstpctl_showbridge, info);

  EXPECT_TRUE(s.ok());
  EXPECT_EQ("", s.to_string());

  EXPECT_EQ("br0", info.bridge);
  EXPECT_EQ(14, info.forward_delay);
  EXPECT_EQ(18, info.hello_time);
  EXPECT_EQ(12, info.max_age);
  EXPECT_EQ("8.000.00:30:DE:42:5D:E4", info.priority);
  EXPECT_EQ(protocol_version::RSTP, info.protocol);
  EXPECT_EQ(10, info.path_cost);
}

TEST_F(info_parser_test, ParsePortInformations) {
  stp_info info;
  status s = parse_stp_info(mstpctl_showportdetails, info);

  EXPECT_TRUE(s.ok());
  EXPECT_EQ("", s.to_string());

  ASSERT_EQ(2, info.ports.size());

  EXPECT_EQ("ethX1", info.ports[0].port);
  EXPECT_EQ("8.001", info.ports[0].priority);
  EXPECT_EQ(200000, info.ports[0].path_cost);
  EXPECT_EQ(false, info.ports[0].edge_port);
  EXPECT_EQ(false, info.ports[0].bpdu_guard);
  EXPECT_EQ(false, info.ports[0].bpdu_filter);
  EXPECT_EQ(false, info.ports[0].root_guard);
  EXPECT_EQ("Designated", info.ports[0].role);
  EXPECT_EQ("forwarding", info.ports[0].status);

  EXPECT_EQ("ethX2", info.ports[1].port);
  EXPECT_EQ("8.002", info.ports[1].priority);
  EXPECT_EQ(200000001, info.ports[1].path_cost);
  EXPECT_EQ(true, info.ports[1].edge_port);
  EXPECT_EQ(true, info.ports[1].bpdu_guard);
  EXPECT_EQ(true, info.ports[1].bpdu_filter);
  EXPECT_EQ(true, info.ports[1].root_guard);
  EXPECT_EQ("Disabled", info.ports[1].role);
  EXPECT_EQ("discarding", info.ports[1].status);

}
}  // namespace wago::stp::lib