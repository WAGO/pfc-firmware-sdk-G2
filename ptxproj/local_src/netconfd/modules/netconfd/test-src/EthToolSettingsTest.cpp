#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <linux/ethtool.h>

#include "EthTool.hpp"

namespace netconf {


TEST(EthToolSettings, GetLinkSettings) {
  EthToolSettings s{EthToolLinkSettings{}};
  EthToolLinkSettings& els = s.GetLinkSettings();
  els.s_.duplex = static_cast<uint8_t>(eth::Duplex::Full);
  els.s_.speed = 1337;
  els.s_.autoneg = AUTONEG_ENABLE;
  els.s_.port = PORT_FIBRE;
  els.s_.eth_tp_mdix = ETH_TP_MDI_AUTO;
  els.s_.link_mode_masks_nwords = 2;
  for(uint32_t i = 0; i < els.maxwords()*3; ++i){
    gsl::at(els.link_mode_masks_,static_cast<int>(i)) = i+1;
  }

  EXPECT_EQ(eth::Duplex::Full, s.GetDuplex());
  EXPECT_EQ(1337, s.GetSpeed());
  EXPECT_TRUE(s.IsAutonegEnabled());
  EXPECT_EQ(eth::MediaType::FIBRE, s.GetMediaType());
  EXPECT_EQ(eth::TpMdiType::Auto, s.GetMdixState());

  /* words 0 and 1 in link_mode_masks_ */
  EXPECT_EQ(2, s.GetSupported().size());
  EXPECT_EQ(1, s.GetSupported()[0]);
  EXPECT_EQ(2, s.GetSupported()[1]);

  /* words 2 and 3 in link_mode_masks_ */
  EXPECT_EQ(2, s.GetAutonegAdvertising().size());
  EXPECT_EQ(3, s.GetAutonegAdvertising()[0]);
  EXPECT_EQ(4, s.GetAutonegAdvertising()[1]);

  EXPECT_EQ(eth::InterfaceLinkState::Down, s.GetIfLinkState());
}


TEST(EthToolSettings, SetLinkSettings) {
  EthToolSettings s{EthToolLinkSettings{}};
  auto& els = s.GetLinkSettings();
  els.s_.duplex = static_cast<uint8_t>(eth::Duplex::Full);
  els.s_.link_mode_masks_nwords = 2; /* Fake ctor determined value */

  s.SetDuplex(eth::Duplex::Full);
  s.SetSpeed(1234);
  s.SetAutonegEnabled(true);
  s.SetMdixState(eth::TpMdiType::II);

  els = s.GetLinkSettings();

  EXPECT_EQ(DUPLEX_FULL, els.s_.duplex);
  EXPECT_EQ(1234, els.s_.speed);
  EXPECT_EQ(AUTONEG_ENABLE, els.s_.autoneg);
  EXPECT_EQ(ETH_TP_MDI, els.s_.eth_tp_mdix_ctrl);

  std::array<uint32_t, 2> advertising = {{13, 37}};
  s.SetAutonegAdvertising(advertising);
  EXPECT_EQ(13, els.link_mode_masks_[2]);
  EXPECT_EQ(37, els.link_mode_masks_[3]);

  std::array<uint32_t, 3> advertising_too_big = {{11, 22, 33}};
  s.SetAutonegAdvertising(advertising_too_big);
  EXPECT_EQ(11, els.link_mode_masks_[2]);
  EXPECT_EQ(22, els.link_mode_masks_[3]);
  EXPECT_EQ(0, els.link_mode_masks_[4]);

  std::array<uint32_t, 1> advertising_too_small = {{42}};
  s.SetAutonegAdvertising(advertising_too_small);
  EXPECT_EQ(42, els.link_mode_masks_[2]);
  EXPECT_EQ(22, els.link_mode_masks_[3]);

}

TEST(EthToolSettings, LinkModesAreSet){

  std::array<uint32_t,3> fullbitset{{ 1<<ETHTOOL_LINK_MODE_Autoneg_BIT , 1<<(ETHTOOL_LINK_MODE_10000baseER_Full_BIT%32),0}};


  EXPECT_TRUE(TestLinkModesBits(fullbitset, ETHTOOL_LINK_MODE_Autoneg_BIT));
  EXPECT_FALSE(TestLinkModesBits(fullbitset, ETHTOOL_LINK_MODE_TP_BIT));
  EXPECT_TRUE(TestLinkModesBits(fullbitset, ETHTOOL_LINK_MODE_10000baseER_Full_BIT));
  EXPECT_TRUE(TestLinkModesBits(fullbitset, ETHTOOL_LINK_MODE_Autoneg_BIT, ETHTOOL_LINK_MODE_10000baseER_Full_BIT));
  EXPECT_FALSE(TestLinkModesBits(fullbitset, 1337u));
  EXPECT_FALSE(TestLinkModesBits(fullbitset, ETHTOOL_LINK_MODE_Autoneg_BIT, 1337u));


}

} // namespace netconf



