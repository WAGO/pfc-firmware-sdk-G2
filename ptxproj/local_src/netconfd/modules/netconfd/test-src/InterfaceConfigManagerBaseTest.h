#pragma once

#include "BridgeUtil.hpp"
#include "InterfaceConfigManager.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include "MacAddress.hpp"
#include <list>

static ::std::uint32_t if_index_counter = 0;
namespace netconf {

class InterfaceConfigManagerBaseTest {
 public:

  class FakeEthernetInterface : public IEthernetInterface {
   public:

    FakeEthernetInterface(::std::string name, InterfaceConfigManagerBaseTest& upper)
        : mac{0},
          name_ { ::std::move(name) },
          upper_ { upper },
          state_ {eth::DeviceState::Down},
          link_state_{eth::InterfaceLinkState::Down},
          if_index_{++if_index_counter},
          autoneg_{eth::Autoneg::Off},
          speed_{0},
          duplex_{eth::Duplex::Half},
          committed_{false}
          {
            upper_.EthernetInterfaceCreated(this);
    }
    ~FakeEthernetInterface() override {
      upper_.EthernetInterfaceDeleted(this);
    }
    Status UpdateConfig() override { return {}; }
    ::std::string const& GetName() const { return name_;}
    MacAddress GetMac() const override { return MacAddress{mac};}
    bool GetAutonegSupport() const override { return true;}
    bool GetAutonegEnabled() const override {return autoneg_ == eth::Autoneg::On;}
    gsl::span<const ::std::uint32_t> GetSupportedLinkModes() const override {
      return supported_link_modes_;
    }
    eth::MediaType GetMediaType() const override {return eth::MediaType::TP;}
    eth::DeviceState GetState() const override {return state_;}
    eth::InterfaceLinkState GetLinkState() const override {return link_state_;}
    ::std::uint32_t GetInterfaceIndex() const override {return if_index_;}
    ::std::uint32_t GetMTU() const override {return 1500;}
    uint32_t GetSpeed() const override {return speed_;}
    eth::Duplex GetDuplex() const override {return duplex_;}
    Status Commit() override {committed_ = true; return {}; }
    void SetAutoneg(eth::Autoneg autoneg) override {autoneg_ = autoneg;}
    void SetState(eth::DeviceState state) override {state_ = state;}
    void SetSpeed(::std::uint32_t speed) override {speed_ = speed;}
    void SetDuplex(eth::Duplex duplex) override {duplex_ = duplex;}

    uint8_t mac[6];
    std::array<uint32_t,1> supported_link_modes_ {{63}}; //link modes 10/100/1000 full/half
    ::std::string name_;
    InterfaceConfigManagerBaseTest& upper_;
    eth::DeviceState state_;
    eth::InterfaceLinkState link_state_;
    ::std::uint32_t if_index_;
    eth::Autoneg autoneg_;
    ::std::uint32_t speed_;
    eth::Duplex duplex_;
    bool committed_;

   private :

  };

  class FakeEthernetInterfaceFactory : public IEthernetInterfaceFactory {
   public:

    explicit FakeEthernetInterfaceFactory(InterfaceConfigManagerBaseTest& upper) : upper_{upper} {

    }

    ::std::unique_ptr<IEthernetInterface> getEthernetInterface(Interface name) override {
      return ::std::make_unique<FakeEthernetInterface>(name.GetName(), upper_);
    }

    InterfaceConfigManagerBaseTest& upper_;
  };

  InterfaceConfigManagerBaseTest() = default;

  void EthernetInterfaceCreated(FakeEthernetInterface* itf) {
    created_ethernet_interfaces.push_back(itf);
  }
  void EthernetInterfaceDeleted(FakeEthernetInterface* itf) {
    created_ethernet_interfaces.remove(itf);
  }

  ::std::list<FakeEthernetInterface*> created_ethernet_interfaces;

  FakeEthernetInterface* FindCreatedEthernetInterface(::std::string name)
  {
    auto name_matches = [&name](FakeEthernetInterface* itf) {
      return name == itf->name_;
    };

    auto found_itf = std::find_if(created_ethernet_interfaces.begin(), created_ethernet_interfaces.end() , name_matches);
    if(found_itf != created_ethernet_interfaces.end()){
      return *found_itf;
    }
    return nullptr;
  }

};

} /* namespace netconf */
