#pragma once

#include "IDynamicIPClientFactory.hpp"

namespace netconf {

class DynamicIPClientFactory : public IDynamicIPClientFactory {
 public:
  DynamicIPClientFactory()          = default;
  ~DynamicIPClientFactory() override = default;

  DynamicIPClientFactory(const DynamicIPClientFactory &)             = delete;
  DynamicIPClientFactory &operator=(const DynamicIPClientFactory &)  = delete;
  DynamicIPClientFactory(const DynamicIPClientFactory &&)            = delete;
  DynamicIPClientFactory &operator=(const DynamicIPClientFactory &&) = delete;

  IDynamicIPClientPtr Create(DynamicIPType type, const Interface &interface, const ::std::string &hostname,
                             const ::std::string &vendorclass, const ::std::string &clientid) override;
};

} /* namespace netconf */
