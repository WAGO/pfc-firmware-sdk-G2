// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"
#include <linux/in.h>
#include "Socket.hpp"

static int closedfd = 0;

extern "C" {

int socket([[maybe_unused]]int __domain, [[maybe_unused]] int __type, [[maybe_unused]] int __protocol) {
  return 42;
}

int close(int fd) {
  closedfd = fd;
  return 0;
}

}

namespace netconf {

TEST(SocketTest, CreateDestroy) {

  {
    Socket s { AF_INET, SOCK_DGRAM, IPPROTO_IP };
    EXPECT_EQ(s.fd(), 42);
  }
  EXPECT_EQ(closedfd, 42);

}

TEST(SocketTest, Copy) {

  Socket orig { AF_INET, SOCK_DGRAM, IPPROTO_IP };
  ASSERT_EQ(orig.fd(), 42);

  Socket copy {orig};  //NOLINT(performance-unnecessary-copy-initialization) do not check because it is necessary for the test case

  EXPECT_EQ(orig.fd(), 42);
  EXPECT_EQ(copy.fd(), 42);

  Socket assigned;
  EXPECT_EQ(assigned.fd(), -1);
  assigned = orig;

  EXPECT_EQ(assigned.fd(), 42);
  EXPECT_EQ(copy.fd(), 42);

}

TEST(SocketTest, Move) {

  Socket orig { AF_INET, SOCK_DGRAM, IPPROTO_IP };
  ASSERT_EQ(orig.fd(), 42);

  Socket moved {::std::move(orig)};

  EXPECT_EQ(orig.fd(), -1);  //NOLINT do not check because it is necessary for the test case
  EXPECT_EQ(moved.fd(), 42);

}

}
