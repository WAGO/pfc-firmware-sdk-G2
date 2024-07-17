
#include <gmock/gmock.h>

#include <stp.hpp>

namespace wago::stp::lib {

struct MockSTP {
 public:
  MockSTP();
  ~MockSTP();

  MOCK_METHOD1(configure, status(const stp_config &config));
  MOCK_METHOD1(get_persisted, status(stp_config &config));
  MOCK_METHOD1(get_info, status(stp_info &info));
};

}  // namespace wago::stp::lib
