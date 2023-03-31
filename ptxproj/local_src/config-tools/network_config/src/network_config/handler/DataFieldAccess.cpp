#include "DataFieldAccess.hpp"
#include "DataField.hpp"
#include "IPConfig.hpp"

namespace network_config {

template<>
DataFieldAccess<::netconf::InterfaceInformation>::DataFieldAccess()
    :
    field_map_ {
      { "label", DataField<::netconf::InterfaceInformation>{[](const auto &info){
        return info.GetInterface().GetLabel();
      }} },
      { "name", DataField<::netconf::InterfaceInformation>{[](const auto &info){
        return info.GetInterface().GetName();
      }} },
      { "type", DataField<::netconf::InterfaceInformation>{[](const auto &info){
        return ::netconf::ToString(info.GetType());
      }} },
      { "ip-ro", DataField<::netconf::InterfaceInformation>{ [](const auto &info){
        return info.IsIpConfigurationReadonly() ? "true" : "false";
      }} },
} {

}


} /* namespace network_config */
