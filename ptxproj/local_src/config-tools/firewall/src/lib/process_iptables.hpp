//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_iptables.hpp
/// 
/// \brief Function processing iptables configuration file.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_PROCESS_IPTABLES_HPP_
#define WAGO_FIREWALL_PROCESS_IPTABLES_HPP_

#include <getopt.h>
#include <string>
#include <vector>
#include "xmlhlp.hpp"

namespace wago {
namespace firewall {
namespace iptables {

//------------------------------------------------------------------------------
/// Process iptables's configuration change request.
/// \param doc xml document to be operated on
/// \param cmd requested command (see help of firewall config-tool for description)
/// \param argv parameters of requested command
//------------------------------------------------------------------------------
void process(xmldoc &doc, const std::string &cmd, const std::vector<std::string> &argv);
void process_internal(xmldoc &doc, const std::string &cmd, const std::vector<std::string> &argv);

namespace impl {

std::string normalize_argv(const std::string &str);
char* const *c_argv(const std::vector<std::string> &packed);
void transform_argv(const std::vector<std::string> &argv,
                    const std::string short_options,
                    const option *long_options,
                    std::vector<std::string> &new_argv);

void set_echo_if(xmldoc &doc, const std::vector<std::string> &argv);
void rem_echo_if(xmldoc &doc, const std::vector<std::string> &argv);

void set_forward_link(xmldoc &doc, const std::vector<std::string> &argv);
void rem_forward_link(xmldoc &doc, const std::vector<std::string> &argv);

void set_masq(xmldoc &doc, const std::vector<std::string> &argv);
void rem_masq(xmldoc &doc, const std::vector<std::string> &argv);

void add_pfw(xmldoc &doc, const std::vector<std::string> &argv);
void upd_pfw(xmldoc &doc, const std::vector<std::string> &argv);
void rem_pfw(xmldoc &doc, const std::vector<std::string> &argv);

void set_open_if(xmldoc &doc, const std::vector<std::string> &argv);
void rem_open_if(xmldoc &doc, const std::vector<std::string> &argv);

void add_filter_kw(xmldoc &doc, const std::vector<std::string> &argv);
void add_filter(xmldoc &doc, const std::vector<std::string> &argv);
void add_filter2(xmldoc &doc, const std::vector<std::string> &argv);

void upd_filter_kw(xmldoc &doc, const std::vector<std::string> &argv);
void upd_filter(xmldoc &doc, const std::vector<std::string> &argv);
void upd_filter2(xmldoc &doc, const std::vector<std::string> &argv);

}  // namespace impl

}  // namespace iptables
}  // namespace firewall
}  // namespace wago

#endif // WAGO_FIREWALL_PROCESS_IPTABLES_HPP_
