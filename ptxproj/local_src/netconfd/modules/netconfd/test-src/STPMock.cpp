#include "STPMock.hpp"

namespace wago::stp::lib {

using InstancePtr = ::std::unique_ptr<MockSTP, void (*)(MockSTP*)>;

static InstancePtr activeMock = {nullptr, []([[maybe_unused]] MockSTP* mock) {}};

MockSTP::MockSTP() {
  activeMock.reset(this);
}
MockSTP::~MockSTP() {
  activeMock.reset();
}

status configure(const stp_config &config) {
  if (activeMock) {
    return activeMock->configure(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

status get_persisted(stp_config &config) {
  if (activeMock) {
    return activeMock->get_persisted(config);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

status get_info(stp_info &info) {
  if (activeMock) {
    return activeMock->get_info(info);
  }
  throw std::runtime_error("No active mock! Instantiate MockIPConfig in your test");
}

}  // namespace wago::stp::lib
