// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <chrono>
#include <fstream>
#include <thread>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <glib-2.0/glib.h>

#ifndef OPCUACONFIGTOOL_VERSION
#error "config_opcua version is not defined"
#endif

#define XSTR(value) STR(value)
#define STR(value) #value

#define TEST_APP_VERSION XSTR(OPCUACONFIGTOOL_VERSION)

#define LOGLEVEL_ERROR "32"
#define LOGLEVEL_WARNING "48"
#define LOGLEVEL_INFO "60"
#define LOGLEVEL_DEBUG "4031"

namespace
{

int exec_command(const std::string & command, gchar ** std_out, gchar ** std_err)
{
  int result = -1;
  gchar * cmd_out = nullptr;
  gchar * cmd_err = nullptr;
  gint exit_status;
  GError * error = nullptr;

  gboolean cmd_result = g_spawn_command_line_sync(command.c_str(), &cmd_out, &cmd_err, &exit_status, &error);

  if (cmd_result)
  {
    if (cmd_out != nullptr)
    {
      *std_out = cmd_out;
      cmd_out = nullptr;

      if (WIFEXITED(exit_status))
      {
        result = WEXITSTATUS(exit_status);
      }
    }
    if (cmd_err != nullptr)
    {
      *std_err = cmd_err;
      cmd_err = nullptr;
    }
    if (error)
    {
      g_free(error);
    }
  }
  return result;
}

int call_config_tool(const std::string & args, gchar ** std_out, gchar ** std_err)
{
  return exec_command(std::string("./config_opcua.elf ") + args, std_out, std_err);
}

void reset_cfg(bool clear = false)
{
  std::string text;
  if (clear)
  {
    text = "";
  }
  else
  {
    text = "\
[CmpOPCUAProviderIecVarAccess]\n\
CustomNodeName=\n\
[CmpOPCUAStack]\n\
TraceLevel=48\n\
[CmpOPCUAServer]\n\
SECURITY.Activation=ACTIVATED\n\
SECURITY.UserAuthentication=ENABLED\n\
SECURITY.CommunicationPolicy=POLICY_AES128SHA256RSAOAEP\n\
SECURITY.CommunicationMode=ALL\n\
SECURITY.AllowUserPasswordOnPlaintext=YES\n\
SECURITY.ApplicationName=\n\
SECURITY.CompanyOrOrganizationName=\n\
SECURITY.City=\n\
SECURITY.State=\n\
SECURITY.Country=\n";
  }

  std::ofstream cfg_file("/etc/codesys3.d/CODESYSControl_User.cfg");
  cfg_file << text;
}

int set_in_cfg_file(const std::string & key, const std::string & value)
{
  gchar * std_out;
  gchar * std_err;

  std::string command = "sed -i \"s/";
  command += key + "=.*/";
  command += key + "=" + value + "/g\" /etc/codesys3.d/CODESYSControl_User.cfg";

  int result = exec_command(command, &std_out, &std_err);

  if (std_out != nullptr)
  {
    g_free(std_out);
  }
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  return result;
}

std::string get_from_cfg_file(const std::string & key)
{
  gchar * std_out;
  gchar * std_err;

  std::string result = "";
  if(exec_command("cat /etc/codesys3.d/CODESYSControl_User.cfg", &std_out, &std_err) == EXIT_SUCCESS && std_out != nullptr)
  {
    std::string str_out = std_out;
    size_t pos = str_out.find(key);
    if (pos != std::string::npos)
    {
      size_t pos_n = str_out.find("\n", pos);
      if (pos_n != std::string::npos)
      {
        result = str_out.substr(pos, pos_n - pos);
      }
    }
  }

  if (std_out != nullptr)
  {
    g_free(std_out);
  }
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  return result;
}

std::string get_test_typelabel_descr()
{
  gchar * std_out;
  gchar * std_err;
  std::string result = "";

  exec_command("/etc/config-tools/get_typelabel_value DESCR", &std_out, &std_err);

  if (std_out != nullptr)
  {
    result = std_out;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.cend());

    g_free(std_out);
  }
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  return result;
}

} // namespace

TEST(config_Target, version)
{
  std::string expected = std::string(TEST_APP_VERSION) + "\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-v", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("--version", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, help)
{
  const std::string expected = "\n\
* Command line interface tool to change OPC UA configuration parameters *\n\
Usage: config_opcua [OPTION]\n\
   or: config_opcua [OPTION]... PARAMETER\n\
\n\
Options:\n\
  -h, --help                  Print this help.\n\
  -v, --version               Print the version of this config tool.\n\
\n\
  -g, --get [PARAMETER]       Get the json formatted value(s) of either one specific parameter or all, if no parameter is given.\n\
  -o, --onlyvalue             Removes the full json output from --get and just returns the value (only applicable when getting specific parameters).\n\
  -s, --set PARAMETER=VALUE   Set the value of a parameter.\n\
  -r, --restart               Restart the runtime.\n\
\n\
Known values for PARAMETER:\n\
  allowPlaintextPassword\n\
  anonymousAccess\n\
  certAppName\n\
  certCity\n\
  certCompany\n\
  certCountry\n\
  certState\n\
  customCtrlConfigName\n\
  enabled\n\
  logLevel\n\
  securityModes\n\
  securityPolicyMin\n\
\n\
";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-h", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("--help", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, invalidArg)
{
  const std::string expected_error = "./config_opcua.elf: unrecognized option '--xyzunknown'\nUnknown option. Use --help to get more information.\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("--xyzunknown", &std_out, &std_err), EXIT_FAILURE);
  ASSERT_TRUE(std_err != nullptr);
  ASSERT_STREQ(std_err, expected_error.c_str());

  g_free(std_err);
  if (std_out != nullptr)
  {
    g_free(std_out);
  }
}

TEST(config_Target, getInvalidParam)
{
  const std::string expected_error = "Unknown parameter for option --get.\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g xyzunknown", &std_out, &std_err), EXIT_FAILURE);
  ASSERT_TRUE(std_err != nullptr);
  ASSERT_STREQ(std_err, expected_error.c_str());

  g_free(std_err);
  if (std_out != nullptr)
  {
    g_free(std_out);
  }
}

TEST(config_Target, setInvalidParam)
{
  const std::string expected_error = "Unknown parameter for option --set.\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-s xyzunknown=123", &std_out, &std_err), EXIT_FAILURE);
  ASSERT_TRUE(std_err != nullptr);
  ASSERT_STREQ(std_err, expected_error.c_str());

  g_free(std_err);
  if (std_out != nullptr)
  {
    g_free(std_out);
  }
}

TEST(config_Target, setMultiple)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key1 = "SECURITY.Activation";
  std::string expected1 = key1 + "=DEACTIVATED";
  const std::string key2 = "CustomNodeName";
  std::string expected2 = key2 + "=hello";

  ASSERT_EQ(call_config_tool("-s enabled=false -s customCtrlConfigName=hello", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key1), expected1);
  ASSERT_EQ(get_from_cfg_file(key2), expected2);
}

TEST(config_Target, getAndSet)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key1 = "SECURITY.Activation";
  std::string expected1 = key1 + "=DEACTIVATED";
  const std::string key2 = "CustomNodeName";
  std::string expected2 = key2 + "=hello";

  ASSERT_EQ(call_config_tool("-g enabled -s customCtrlConfigName=hello", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "{\"enabled\":true}\n");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_NE(get_from_cfg_file(key1), expected1);
  ASSERT_EQ(get_from_cfg_file(key2), expected2);
}

TEST(config_Target, getAll)
{
  reset_cfg();

  const std::string expected_ctrl_config_name = "mynodename";
  const std::string expected_log_level = LOGLEVEL_DEBUG;
  const std::string expected_enabled = "DEACTIVATED";
  const std::string expected_anon_access = "ENFORCED";
  const std::string expected_sec_policy = "POLICY_BASIC256SHA256";
  const std::string expected_sec_mode = "MIN_SIGNED";
  const std::string expected_plain_password = "NO";
  const std::string expected_cert_name = "bla";
  const std::string expected_cert_company = "comp";
  const std::string expected_cert_city = "blub";
  const std::string expected_cert_state = "123";
  const std::string expected_cert_country = "germany";

  ASSERT_EQ(set_in_cfg_file("CustomNodeName", expected_ctrl_config_name), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("TraceLevel", expected_log_level), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.Activation", expected_enabled), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.UserAuthentication", expected_anon_access), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationPolicy", expected_sec_policy), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationMode", expected_sec_mode), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.AllowUserPasswordOnPlaintext", expected_plain_password), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.ApplicationName", expected_cert_name), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.CompanyOrOrganizationName", expected_cert_company), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.City", expected_cert_city), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.State", expected_cert_state), EXIT_SUCCESS);
  ASSERT_EQ(set_in_cfg_file("SECURITY.Country", expected_cert_country), EXIT_SUCCESS);

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);

  std::string result = std_out;

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  nlohmann::json j = nlohmann::json::parse(result);
  ASSERT_TRUE(j.is_object());
  ASSERT_NE(j.size(), 0);
  for (auto & it : j.items())
  {
    if (it.key() == "enabled")
    {
      EXPECT_TRUE(it.value().is_boolean());
      EXPECT_EQ(it.value().get<bool>(), false);
    }
    else if (it.key() == "customCtrlConfigName")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_ctrl_config_name);
    }
    else if (it.key() == "logLevel")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), "debug");
    }
    else if (it.key() == "securityPolicyMin")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), "Basic256Sha256");
    }
    else if (it.key() == "securityModes")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), "Sign+SignEncrypt");
    }
    else if (it.key() == "allowPlaintextPassword")
    {
      EXPECT_TRUE(it.value().is_boolean());
      EXPECT_EQ(it.value().get<bool>(), false);
    }
    else if (it.key() == "anonymousAccess")
    {
      EXPECT_TRUE(it.value().is_boolean());
      EXPECT_EQ(it.value().get<bool>(), false);
    }
    else if (it.key() == "certAppName")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_cert_name);
    }
    else if (it.key() == "certCompany")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_cert_company);
    }
    else if (it.key() == "certCity")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_cert_city);
    }
    else if (it.key() == "certState")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_cert_state);
    }
    else if (it.key() == "certCountry")
    {
      EXPECT_TRUE(it.value().is_string());
      EXPECT_EQ(it.value().get<std::string>(), expected_cert_country);
    }
    else
    {
      EXPECT_TRUE(false) << "unknown / untested key encountered: " << it.key();
    }
  }
}

TEST(config_Target, getRawValue)
{
  reset_cfg();

  std::string expected = "true\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g enabled -o", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "\"warning\"\n";

  ASSERT_EQ(call_config_tool("--onlyvalue -g logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getEnabled)
{
  reset_cfg();

  std::string expected = "{\"enabled\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g enabled", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("--get enabled", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"enabled\":false}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.Activation", "DEACTIVATED"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get enabled", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getEnabledDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"enabled\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g enabled", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setEnabled)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.Activation";
  std::string expected = key + "=ACTIVATED";

  ASSERT_EQ(call_config_tool("-s enabled=true", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=DEACTIVATED";

  ASSERT_EQ(call_config_tool("-s enabled=false", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, setEnabledInvalid)
{
  reset_cfg();

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.Activation";
  const std::string expected = key + "=DEACTIVATED";

  ASSERT_EQ(call_config_tool("-s enabled=justno", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCtrlConfigurationName)
{
  reset_cfg();

  std::string expected = "{\"customCtrlConfigName\":\"" + get_test_typelabel_descr() + "\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g customCtrlConfigName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("--get customCtrlConfigName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"customCtrlConfigName\":\"MyNewName\"}\n";
  ASSERT_EQ(set_in_cfg_file("CustomNodeName", "MyNewName"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get customCtrlConfigName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCtrlConfigurationNameDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"customCtrlConfigName\":\"" + get_test_typelabel_descr() + "\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g customCtrlConfigName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCtrlConfigurationName)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "CustomNodeName";
  std::string expected = key + "=whatever i want";

  ASSERT_EQ(call_config_tool("-s customCtrlConfigName=\"whatever i want\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=WAGO";

  ASSERT_EQ(call_config_tool("-s customCtrlConfigName=WAGO", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=";

  ASSERT_EQ(call_config_tool("-s customCtrlConfigName=", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=Escaped Sequence here";

  ASSERT_EQ(call_config_tool("-s customCtrlConfigName=Escaped%20Sequence%20here", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getLogLevel)
{
  reset_cfg();

  std::string expected = "{\"logLevel\":\"warning\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("--get logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"logLevel\":\"error\"}\n";
  ASSERT_EQ(set_in_cfg_file("TraceLevel", LOGLEVEL_ERROR), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"logLevel\":\"info\"}\n";
  ASSERT_EQ(set_in_cfg_file("TraceLevel", LOGLEVEL_INFO), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"logLevel\":\"debug\"}\n";
  ASSERT_EQ(set_in_cfg_file("TraceLevel", LOGLEVEL_DEBUG), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getLogLevelDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"logLevel\":\"warning\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g logLevel", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setLogLevel)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "TraceLevel";
  std::string expected = key + "=" + LOGLEVEL_ERROR;

  ASSERT_EQ(call_config_tool("-s logLevel=error", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=" + LOGLEVEL_WARNING;

  ASSERT_EQ(call_config_tool("-s logLevel=warning", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=" + LOGLEVEL_INFO;

  ASSERT_EQ(call_config_tool("-s logLevel=info", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=" + LOGLEVEL_DEBUG;

  ASSERT_EQ(call_config_tool("-s logLevel=debug", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, setLogLevelInvalid)
{
  reset_cfg();

  gchar * std_out;
  gchar * std_err;

  const std::string key = "TraceLevel";
  const std::string expected = key + "=" + LOGLEVEL_WARNING;

  set_in_cfg_file(key, LOGLEVEL_ERROR);

  ASSERT_EQ(call_config_tool("-s logLevel=justno", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCertAppName)
{
  reset_cfg();

  std::string expected = "{\"certAppName\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certAppName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"certAppName\":\"that\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.ApplicationName", "that"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get certAppName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCertAppNameDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"certAppName\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certAppName", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCertAppName)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.ApplicationName";
  std::string expected = key + "=and another name";

  ASSERT_EQ(call_config_tool("-s certAppName=\"and another name\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCertCompany)
{
  reset_cfg();

  std::string expected = "{\"certCompany\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCompany", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"certCompany\":\"that\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CompanyOrOrganizationName", "that"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get certCompany", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCertCompanyDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"certCompany\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCompany", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCertCompany)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.CompanyOrOrganizationName";
  std::string expected = key + "=once again another name";

  ASSERT_EQ(call_config_tool("-s certCompany=\"once again another name\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCertCity)
{
  reset_cfg();

  std::string expected = "{\"certCity\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCity", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"certCity\":\"this\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.City", "this"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get certCity", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCertCityDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"certCity\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCity", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCertCity)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.City";
  std::string expected = key + "=just a name";

  ASSERT_EQ(call_config_tool("-s certCity=\"just a name\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCertState)
{
  reset_cfg();

  std::string expected = "{\"certState\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certState", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"certState\":\"test\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.State", "test"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get certState", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCertStateDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"certState\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certState", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCertState)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.State";
  std::string expected = key + "=some state";

  ASSERT_EQ(call_config_tool("-s certState=\"some state\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getCertCountry)
{
  reset_cfg();

  std::string expected = "{\"certCountry\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCountry", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"certCountry\":\"abc\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.Country", "abc"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get certCountry", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getCertCountryDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"certCountry\":\"\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g certCountry", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setCertCountry)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.Country";
  std::string expected = key + "=earth";

  ASSERT_EQ(call_config_tool("-s certCountry=\"earth\"", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getSecPolicies)
{
  reset_cfg();

  std::string expected = "{\"securityPolicyMin\":\"Aes128Sha256RsaOaep\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g securityPolicyMin", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"securityPolicyMin\":\"Basic256Sha256\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationPolicy", "POLICY_BASIC256SHA256"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get securityPolicyMin", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"securityPolicyMin\":\"Aes256Sha256RsaPss\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationPolicy", "POLICY_AES256SHA256RSAPSS"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get securityPolicyMin", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getSecPoliciesDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"securityPolicyMin\":\"Aes128Sha256RsaOaep\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g securityPolicyMin", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setSecPolicies)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.CommunicationPolicy";
  std::string expected = key + "=POLICY_BASIC256SHA256";

  ASSERT_EQ(call_config_tool("-s securityPolicyMin=Basic256Sha256", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=POLICY_AES256SHA256RSAPSS";

  ASSERT_EQ(call_config_tool("-s securityPolicyMin=Aes256Sha256RsaPss", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=POLICY_AES128SHA256RSAOAEP";

  ASSERT_EQ(call_config_tool("-s securityPolicyMin=Aes128Sha256RsaOaep", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, setSecPoliciesInvalid)
{
  reset_cfg();

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.CommunicationPolicy";
  const std::string expected = key + "=POLICY_AES128SHA256RSAOAEP";

  ASSERT_EQ(call_config_tool("-s securityPolicyMin=justno", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getSecModes)
{
  reset_cfg();

  std::string expected = "{\"securityModes\":\"None+Sign+SignEncrypt\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g securityModes", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"securityModes\":\"None\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationMode", "ONLY_PLAINTEXT"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get securityModes", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"securityModes\":\"Sign+SignEncrypt\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationMode", "MIN_SIGNED"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get securityModes", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"securityModes\":\"SignEncrypt\"}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.CommunicationMode", "SIGNED_AND_ENCRYPTED"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get securityModes", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getSecModesDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"securityModes\":\"None+Sign+SignEncrypt\"}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g securityModes", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setSecModes)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.CommunicationMode";
  std::string expected = key + "=SIGNED_AND_ENCRYPTED";

  ASSERT_EQ(call_config_tool("-s securityModes=SignEncrypt", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=MIN_SIGNED";

  ASSERT_EQ(call_config_tool("-s securityModes=Sign+SignEncrypt", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=ALL";

  ASSERT_EQ(call_config_tool("-s securityModes=None+Sign+SignEncrypt", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=ONLY_PLAINTEXT";

  ASSERT_EQ(call_config_tool("-s securityModes=None", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, setSecModesInvalid)
{
  reset_cfg();

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.CommunicationMode";
  const std::string expected = key + "=ALL";

  ASSERT_EQ(call_config_tool("-s securityModes=justno", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getAllowPlaintextPass)
{
  reset_cfg();

  std::string expected = "{\"allowPlaintextPassword\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g allowPlaintextPassword", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"allowPlaintextPassword\":false}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.AllowUserPasswordOnPlaintext", "NO"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get allowPlaintextPassword", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getAllowPlaintextPassDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"allowPlaintextPassword\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g allowPlaintextPassword", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setAllowPlaintextPass)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.AllowUserPasswordOnPlaintext";
  std::string expected = key + "=NO";

  ASSERT_EQ(call_config_tool("-s allowPlaintextPassword=false", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=YES";

  ASSERT_EQ(call_config_tool("-s allowPlaintextPassword=true", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, getAnonymousAccess)
{
  reset_cfg();

  std::string expected = "{\"anonymousAccess\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g anonymousAccess", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  expected = "{\"anonymousAccess\":false}\n";
  ASSERT_EQ(set_in_cfg_file("SECURITY.UserAuthentication", "ENFORCED"), EXIT_SUCCESS);

  ASSERT_EQ(call_config_tool("--get anonymousAccess", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, getAnonymousAccessDefault)
{
  reset_cfg(true);

  const std::string expected = "{\"anonymousAccess\":true}\n";

  gchar * std_out;
  gchar * std_err;

  ASSERT_EQ(call_config_tool("-g anonymousAccess", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, expected.c_str());

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}

TEST(config_Target, setAnonymousAccess)
{
  reset_cfg(true);

  gchar * std_out;
  gchar * std_err;

  const std::string key = "SECURITY.UserAuthentication";
  std::string expected = key + "=ENFORCED";

  ASSERT_EQ(call_config_tool("-s anonymousAccess=false", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);

  expected = key + "=ENABLED";

  ASSERT_EQ(call_config_tool("-s anonymousAccess=true", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STREQ(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(get_from_cfg_file(key), expected);
}

TEST(config_Target, restartRuntime)
{
  gchar * std_out;
  gchar * std_err;

  const std::string cmd_codesys_pid = "pidof /usr/bin/codesys3";

  ASSERT_EQ(exec_command("/etc/init.d/runtime stop", &std_out, &std_err), EXIT_SUCCESS);
  if (std_out != nullptr)
  {
    g_free(std_out);
  }
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(exec_command(cmd_codesys_pid, &std_out, &std_err), EXIT_FAILURE);
  if (std_out != nullptr)
  {
    g_free(std_out);
  }
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  ASSERT_EQ(call_config_tool("-r", &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STRNE(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));

  ASSERT_EQ(exec_command(cmd_codesys_pid, &std_out, &std_err), EXIT_SUCCESS);
  ASSERT_TRUE(std_out != nullptr);
  ASSERT_STRNE(std_out, "");

  g_free(std_out);
  if (std_err != nullptr)
  {
    g_free(std_err);
  }
}
