#include <gtest/gtest.h>

#include "OutputFactory.hpp"
#include <string>
#include <typeinfo>

namespace network_config {

using namespace ::std::string_literals;
using ::boost::program_options::variables_map;
using ::boost::program_options::variable_value;

class OutputFactoryTest : public ::testing::Test {
 public:
  variables_map map;

  template<typename T>
  void PutToVM(::std::string key, T &&arg) {
    map.emplace(key, variable_value { std::forward < T > (arg), false });
  }

  void PutToVM(::std::string key) {
    map.emplace(key, variable_value { });
  }

};

TEST_F(OutputFactoryTest, NullOutput) {

  PutToVM("quiet"s);

  auto output = OutputFactory::Create(map);
  auto output_ptr = output.get();
  EXPECT_EQ(typeid(NullOutput), typeid(*output_ptr));
}

TEST_F(OutputFactoryTest, FilenameOutput) {

  PutToVM("error-msg-dst"s, "<ignored>"s);

  auto output = OutputFactory::Create(map);
  auto output_ptr = output.get();
  EXPECT_EQ(typeid(FileOutput), typeid(*output_ptr));
}

TEST_F(OutputFactoryTest, OutputToLastError) {

  PutToVM("write-last-error"s);

  auto output = OutputFactory::Create(map);
  auto output_ptr = output.get();
  EXPECT_EQ(typeid(FileOutput), typeid(*output_ptr));
}

TEST_F(OutputFactoryTest, StderrOutput) {

  auto output = OutputFactory::Create(map);
  auto output_ptr = output.get();
  EXPECT_EQ(typeid(StderrOutput), typeid(*output_ptr));
}

TEST_F(OutputFactoryTest, QuietIsPrefered) {

  PutToVM("error-msg-dst"s, "bla"s);
  PutToVM("quiet"s);

  auto output = OutputFactory::Create(map);
  auto output_ptr = output.get();
  EXPECT_EQ(typeid(NullOutput), typeid(*output_ptr));
}

}  // namespace network_config
