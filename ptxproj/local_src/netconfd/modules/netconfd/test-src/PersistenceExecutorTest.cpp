// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     PersistenceProviderTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "CommonTestDependencies.hpp"

#include <regex>

#include "TypesHelper.hpp"
#include "BackupRestoreFake.hpp"
#include "PersistenceExecutor.hpp"
#include "FileEditorFake.hpp"

#include "MockIFileEditor.hpp"
#include "MockIBackupRestore.hpp"

#include "DipSwitchFake.hpp"

using testing::Return;
using testing::_;
using testing::SetArgReferee;
using testing::ContainerEq;
using testing::Eq;
using testing::ElementsAre;
using testing::DoAll;



namespace netconf
{
  class APersistenceExecutor: public testing::Test
  {
    public:

      testing::NiceMock<MockIFileEditor> mock_file_editor_;
      MockIBackupRestore mock_backup_restore_;
      DipSwitchFake dip_switch_fake_;
      BackupRestoreFake legacy_restore_fake_;

      InterfaceConfigs interface_configs;
      BridgeConfig persisted_bridge_config;

      IPConfigs persisted_ip_configs;
      Interfaces persisted_interfaces;

      const ::std::string base_path_ = ".";
      const ::std::string path_persistence_file_ = "./netconfd.json";
      const ::std::string path_interface_config_file_ = "./netconfd_interface_config.json";

      ::std::unique_ptr<PersistenceExecutor> persistence_executor_;

      void SetUp() override
      {

        ON_CALL(mock_file_editor_, Read(path_persistence_file_, _)).WillByDefault(
            DoAll(SetArgReferee<1>(persistence_file_content), Return(Status{ })));
        ON_CALL(mock_file_editor_, Read(path_interface_config_file_, _)).WillByDefault(
            DoAll(SetArgReferee<1>(interface_config_file_content_), Return(Status{ })));

        persisted_ip_configs = {{"br0", IPSource::STATIC, "192.168.2.17", "255.255.255.0"}, {
            "br1", IPSource::DHCP, "172.29.233.17", "255.255.0.0"}};
        persisted_bridge_config = {{Interface::CreateBridge("br0"), {Interface::CreatePort("X1")}}, {Interface::CreateBridge("br1"), {Interface::CreatePort("X2")}}};
        persisted_interfaces = Interfaces{Interface::CreateDummy("dummy1"), Interface::CreateDummy("dummy2")};

        interface_configs.clear();
        persistence_executor_ =
            ::std::make_unique < PersistenceExecutor > (base_path_, mock_file_editor_, mock_backup_restore_, legacy_restore_fake_, dip_switch_fake_);
      }

      ::std::string persistence_file_content =
          R"({
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "interfaces": [
      {
        "name": "dummy1",
        "type": "dummy"
      },
      {
        "name": "dummy2",
        "type": "dummy"
      }
    ],
    "ip-config" : {
      "br0" : {
        "source" : "static",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0"
      },
    "br1" : {
        "source" : "dhcp",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0"
      }
    }
}
)";

      ::std::string interface_config_file_content_ =
          R"([
          {"autonegotiation":"on","device":"X1","duplex":"full","speed":100,"state":"up","maclearning":"on"},
          {"autonegotiation":"off","device":"X2","duplex":"half","speed":10,"state":"up","maclearning":"on"}
         ])";

      ::std::string backup_network_data_content =
          R"({
      "bridge-config" : {
        "br0": ["X1"],
        "br1": ["X2"]
      },
      "interfaces": [
        {
          "name": "dummy1",
          "type": "dummy"
        },
        {
          "name": "dummy2",
          "type": "dummy"
        }
      ],
      "ip-config" : {
        "br0" : {
          "source" : "static",
          "ipaddr" : "192.168.2.17",
          "netmask" : "255.255.255.0"
        },
      "br1" : {
          "source" : "dhcp",
          "ipaddr" : "172.29.233.17",
          "netmask" : "255.255.0.0"
        }
      },
      "interface-config":{"autonegotiation":"on","device":"X1","duplex":"full","speed":100,"state":"up","maclearning":"on"}
  })";

      ::std::string backup_dipswitch_data_content =
          R"({"dip-ip-config":{"ipaddr":"192.168.9.0","netmask":"255.255.0.0"}})";
  };


  TEST_F(APersistenceExecutor, PersistsABridgeConfig)
  {

    BridgeConfig bridge_config = {{Interface::CreateBridge("br0"), {Interface::CreatePort("X1")}}, {Interface::CreateBridge("br1"), {Interface::CreatePort("X2")}}};
    IPConfigs ip_configs = { };
    ::std::string config_str = R"({"br0": ["X1"]})";

    Status status = persistence_executor_->Write(bridge_config);
    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
  }

  TEST_F(APersistenceExecutor, PersistsIPConfigs)
  {

    IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"}};
    ::std::string config_str = R"({"br0": ["X1"]})";

    Status status = persistence_executor_->Write(ip_configs);
    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
  }

  TEST_F(APersistenceExecutor, ReadsABridgeConfigFromPersistence)
  {
    BridgeConfig config_read;
    Status status = persistence_executor_->Read(config_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    EXPECT_TRUE(IsEqual(persisted_bridge_config, config_read));
  }

  TEST_F(APersistenceExecutor, ReadsIPConfigsFromPersistence)
  {
    IPConfigs config_read;
    Status status = persistence_executor_->Read(config_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    EXPECT_EQ(persisted_ip_configs, config_read);
  }

  TEST_F(APersistenceExecutor, ReadsConfigurationsFromPersistence)
  {

    BridgeConfig bridge_config_read;
    IPConfigs ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Read(bridge_config_read, ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    EXPECT_TRUE(IsEqual(persisted_bridge_config, bridge_config_read));
    EXPECT_TRUE(IsEqual(persisted_ip_configs, ip_config_read));

    EXPECT_THAT(interfaces_read, Eq(persisted_interfaces));
  }

  TEST_F(APersistenceExecutor, BackupsAConfig)
  {

    ::std::string config_str =
        R"("{"bridge-config":{},"interface-config":{"autonegotiation":"on","device":"X1","duplex":"full","speed":100,"state":"up"},"ip-config":{}}")";

    EXPECT_CALL(mock_backup_restore_, Backup(path_persistence_file_,_,_,_)).WillOnce(Return(Status(StatusCode::OK)));

    Status status = persistence_executor_->Backup(path_persistence_file_,"");

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
  }

  TEST_F(APersistenceExecutor, RestoresAConfig)
  {

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_network_data_content), SetArgReferee<2>(backup_dipswitch_data_content),
              SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());

    EXPECT_THAT(bridge_config_read, ContainerEq(BridgeConfig{{Interface::CreateBridge("br0"), {Interface::CreatePort("X1")}}, {Interface::CreateBridge("br1"), {Interface::CreatePort("X2")}}}));
    EXPECT_THAT(interface_configs, ElementsAre(InterfaceConfig{Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100,
                                                               Duplex::FULL, MacLearning::ON}));
    EXPECT_THAT(dip_ip_config_read,DipSwitchIpConfig("192.168.9.0","255.255.0.0"));

    EXPECT_THAT(interfaces_read, Eq(persisted_interfaces));
  }

  TEST_F(APersistenceExecutor, RestoresAConfigWithoutInterfacesContent)
  {

    ::std::string backup_network_data_content_without_interfaces =
              R"({
          "bridge-config" : {
            "br0": ["X1"],
            "br1": ["X2"]
          },
          "ip-config" : {
            "br0" : {
              "source" : "static",
              "ipaddr" : "192.168.2.17",
              "netmask" : "255.255.255.0"
            },
          "br1" : {
              "source" : "dhcp",
              "ipaddr" : "172.29.233.17",
              "netmask" : "255.255.0.0"
            }
          },
          "interface-config":{"autonegotiation":"on","device":"X1","duplex":"full","speed":100,"state":"up","maclearning":"on"}
      })";

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_network_data_content_without_interfaces), SetArgReferee<2>(backup_dipswitch_data_content),
              SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());

    EXPECT_THAT(bridge_config_read, ContainerEq(BridgeConfig{{Interface::CreateBridge("br0"), {Interface::CreatePort("X1")}}, {Interface::CreateBridge("br1"), {Interface::CreatePort("X2")}}}));
    EXPECT_THAT(interface_configs, ElementsAre(InterfaceConfig{Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100,
                                                               Duplex::FULL, MacLearning::ON}));
    EXPECT_THAT(dip_ip_config_read,DipSwitchIpConfig("192.168.9.0","255.255.0.0"));

    EXPECT_TRUE(interfaces_read.empty());
  }

  TEST_F(APersistenceExecutor, RestoresAConfigWithoutDipSwitchContent)
  {

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_network_data_content), SetArgReferee<2>(""),
              SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());

    EXPECT_THAT(bridge_config_read, ContainerEq(BridgeConfig{{Interface::CreateBridge("br0"), {Interface::CreatePort("X1")}}, {Interface::CreateBridge("br1"), {Interface::CreatePort("X2")}}}));
    EXPECT_THAT(interface_configs, ElementsAre(InterfaceConfig{Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100,
                                                               Duplex::FULL, MacLearning::ON}));
    EXPECT_THAT(interfaces_read, Eq(persisted_interfaces));
  }

  TEST_F(APersistenceExecutor, RestoresAnInterfaceConfigWithoutMacLearningContent)
  {
    auto backup_content_without_mac_learning = ::std::regex_replace(backup_network_data_content,
                                                          ::std::regex(::std::string { R"(",maclearning":"on")" }),
                                                          "");

    EXPECT_CALL(mock_backup_restore_, Restore(path_interface_config_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_content_without_mac_learning), SetArgReferee<2>(""),
              SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_interface_config_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    EXPECT_THAT(interface_configs, ElementsAre(InterfaceConfig{Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100,
      Duplex::FULL, MacLearning::ON}));
    EXPECT_THAT(interfaces_read, Eq(persisted_interfaces));
  }

  TEST_F(APersistenceExecutor, RestoresAnInterfaceConfigWithMacLearningOff)
  {
    auto backup_content_with_mac_learning_off = ::std::regex_replace(backup_network_data_content,
                                                          ::std::regex(::std::string{R"("maclearning":"on")"}),
                                                          ::std::string{R"("maclearning":"off")"} );

    EXPECT_CALL(mock_backup_restore_, Restore(path_interface_config_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_content_with_mac_learning_off), SetArgReferee<2>(""),
              SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_interface_config_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
    EXPECT_THAT(interface_configs, ElementsAre(InterfaceConfig{Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100,
      Duplex::FULL, MacLearning::OFF}));
    EXPECT_THAT(interfaces_read, Eq(persisted_interfaces));
  }

  TEST_F(APersistenceExecutor, TriesToRestoreAConfigInCaseRestoreFailed)
  {

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(""), SetArgReferee<2>(""), SetArgReferee<3>(1), Return(Status(StatusCode::FILE_READ))));
    legacy_restore_fake_.restore_status = Status(StatusCode::FILE_READ);

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    EXPECT_EQ(StatusCode::FILE_READ, status.GetStatusCode());
    EXPECT_TRUE(bridge_config_read.empty());
    EXPECT_TRUE(ip_configs_read.empty());
    EXPECT_TRUE(interfaces_read.empty());
  }

  TEST_F(APersistenceExecutor, TriesToRestoreABridgeConfigInCasePersitenceWriteFailes)
  {

    ::std::string config_str = "XXX";

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_network_data_content), SetArgReferee<2>(backup_dipswitch_data_content),SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    EXPECT_CALL(mock_file_editor_, WriteAndReplace(path_persistence_file_, _)).WillOnce(
        Return(Status{StatusCode::FILE_WRITE}));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    EXPECT_EQ(StatusCode::FILE_WRITE, status.GetStatusCode());
    EXPECT_TRUE(bridge_config_read.empty());
    EXPECT_TRUE(ip_configs_read.empty());
    EXPECT_TRUE(interfaces_read.empty());
  }

  TEST_F(APersistenceExecutor, TriesToRestoreABridgeConfigInCaseInterfaceConfigWriteFailes)
  {

    ::std::string config_str = "XXX";

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(backup_network_data_content), SetArgReferee<2>(backup_dipswitch_data_content),SetArgReferee<3>(1), Return(Status(StatusCode::OK))));

    EXPECT_CALL(mock_file_editor_, WriteAndReplace(path_persistence_file_, _)).WillOnce(Return(Status{StatusCode::OK}));
    EXPECT_CALL(mock_file_editor_, WriteAndReplace(path_interface_config_file_, _)).WillOnce(Return(Status{
        StatusCode::FILE_WRITE}));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    EXPECT_EQ(StatusCode::FILE_WRITE, status.GetStatusCode());
    EXPECT_TRUE(bridge_config_read.empty());
    EXPECT_TRUE(ip_configs_read.empty());
    EXPECT_TRUE(interfaces_read.empty());
  }

  TEST_F(APersistenceExecutor, RestoreZeroVersionAndFails)
  {

    ::std::string config_str = "XXX";
    ::std::uint32_t version = 0;

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(config_str), SetArgReferee<2>(version), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    EXPECT_EQ(StatusCode::BACKUP_VERSION, status.GetStatusCode());
    EXPECT_TRUE(bridge_config_read.empty());
    EXPECT_TRUE(ip_configs_read.empty());
    EXPECT_TRUE(interfaces_read.empty());
  }

  TEST_F(APersistenceExecutor, TriesToRestoreAHigherVersionAndFails)
  {

    ::std::string config_str = "XXX";
    ::std::uint32_t higher_version = 42;

    EXPECT_CALL(mock_backup_restore_, Restore(path_persistence_file_,_,_,_)).WillOnce(
        DoAll(SetArgReferee<1>(config_str), SetArgReferee<2>(higher_version), Return(Status(StatusCode::OK))));

    BridgeConfig bridge_config_read;
    IPConfigs ip_configs_read;
    DipSwitchIpConfig dip_ip_config_read;
    Interfaces interfaces_read;
    Status status = persistence_executor_->Restore(path_persistence_file_, bridge_config_read, ip_configs_read,
                                                   interface_configs, dip_ip_config_read, interfaces_read);

    EXPECT_EQ(StatusCode::BACKUP_VERSION, status.GetStatusCode());
    EXPECT_TRUE(bridge_config_read.empty());
    EXPECT_TRUE(ip_configs_read.empty());
    EXPECT_TRUE(interfaces_read.empty());
  }

} /* namespace netconf */
