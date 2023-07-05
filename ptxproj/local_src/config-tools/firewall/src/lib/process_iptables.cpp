//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_iptables.cpp
/// 
/// \brief Function processing iptables configuration file.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <algorithm>
#include <getopt.h>
#include <glib.h>

#include "process_iptables.hpp"

#include "error.hpp"
#include "process.hpp"
#include "regex.hpp"
#include "interface_mapping_provider.hpp"
#include <libxml/parser.h>


namespace wago {
namespace firewall {
namespace iptables {

namespace impl{

namespace {


void set_climits(xmldoc &doc, const std::vector<std::string> &argv) {
  if (5 != argv.size())
    throw invalid_param_error();

  const std::string &total(argv[0]);
  const std::string &limit(argv[1]);
  const std::string &burst(argv[2]);
  const std::string &tcp(argv[3]);
  const std::string &udp(argv[4]);

  if (!is_match_opt(regex::rex_conn_count, total) || !is_match_opt(regex::rex_limit, limit)
      || !is_match_opt(regex::rex_burst, burst) || !is_match_opt(regex::rex_limit, tcp)
      || !is_match_opt(regex::rex_limit, udp) || ("-" == limit && "-" != burst))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode climits(get_node(ctx, "/f:firewall/f:ipv4/f:climits"));

  updrem_attribute(climits, "total", total);
  updrem_attribute(climits, "limit", limit);
  updrem_attribute(climits, "burst", burst);
  updrem_attribute(climits, "tcp", tcp);
  updrem_attribute(climits, "udp", udp);
}

void set_echo(xmldoc &doc, const std::vector<std::string> &argv) {
  if (4 != argv.size())
    throw invalid_param_error();

  const std::string &policy(argv[0]);
  const std::string &limit(argv[1]);
  const std::string &burst(argv[2]);
  const std::string &bprot(argv[3]);

  if (!is_match_std(regex::rex_policy, policy) || !is_match_opt(regex::rex_limit, limit)
      || !is_match_opt(regex::rex_burst, burst) || ("-" == limit && "-" != burst)
      || !is_match_std(regex::rex_onoff, bprot))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode echo(get_node(ctx, "/f:firewall/f:ipv4/f:echo"));

  updrem_attribute(echo, "policy", policy);
  updrem_attribute(echo, "limit", limit);
  updrem_attribute(echo, "burst", burst);
  update_attribute(echo, "broadcast_protection", bprot);
}

void set_forward(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size())
    throw invalid_param_error();

  const std::string &state(argv[0]);

  if (!is_match_std(regex::rex_onoff, state))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode forward(get_node(ctx, "/f:firewall/f:ipv4/f:forward"));

  update_attribute(forward, "state", state);
}


void rem_filter(xmldoc &doc, const std::vector<std::string> &argv) {
  remove(doc, argv, "/f:firewall/f:ipv4/f:input/f:filter/f:rule");
}

} // anonymous namespace

std::string normalize_argv(const std::string & str) {
  std::vector<char> allowed_chars = { ' ', '-', '_' };
  char *tmp;
  std::string normalized;

  if(str == "-")
    return "-";

  tmp = g_uri_unescape_string(str.c_str(), nullptr);

  if(tmp != nullptr)
  {
    normalized = tmp;
    free(tmp);
  }

  // keep in sync with "filter_tag" type in "/etc/firewall/patterns.xsd"
  normalized.erase(std::remove_if(normalized.begin(),
                                  normalized.end(),
                                  [allowed_chars](unsigned char c) {
                                    return !std::isalnum(c) && std::find(allowed_chars.begin(), allowed_chars.end(), c) == allowed_chars.end();
                                  }),
                   normalized.end());

  return normalized;
}

char* const *c_argv(const std::vector<std::string> &packed) {
  // + 1 for dummy
  std::vector<char *> unpacked(packed.size() + 1);

  // put dummy in front as getopt starts parsing at index 1
  char* dummy = new char[5];
  strcpy(dummy, "abce");
  unpacked.front() = dummy;

  std::transform(packed.begin(), packed.end(),
                 unpacked.begin() + 1,
                 [] (const std::string & arg) { char * clone = new char [arg.length() + 1];
                                                strcpy(clone, arg.c_str());
                                                return clone; }
                );

  char** clone = new char*[unpacked.size()];
  copy(unpacked.begin(), unpacked.end(), clone);
  return clone;
}

void transform_argv(const std::vector<std::string> &argv, const std::string short_options, const option *long_options, std::vector<std::string> &new_argv) {
  // add + 1 for dummy which is added by "c_argv"
  int argc = static_cast<int>(argv.size() + 1);

  // getopt: do not print error messages
  opterr = 0;

  // initialise all options with "-" and overwrite them
  // with values as specified by keyword args
  std::fill(new_argv.begin(), new_argv.end(), "-");

  // argv needs to be unpacked, i.e. vector to [] and
  // elements from string() to c_str, to be usable in
  // "getopt_long"
  auto unpacked = c_argv(argv);

  while (1) {
    int optindex = 1;
    std::vector<std::string>::size_type new_argv_index;

    int optreturn = getopt_long(argc, unpacked, short_options.c_str(), long_options, &optindex);

    // getopt returns -1 if all arguments parsed
    if (optreturn == -1) {
      break;
    }

    if(optreturn == '?') {
      // error/unknown option
      throw invalid_param_error("transform_argv::getopt");
    }

    // overwrite default value ("-") in "new_argv"
    new_argv_index = static_cast<std::vector<std::string>::size_type>(optindex);
    new_argv[new_argv_index] = std::string(optarg);
  } // while(1)

  for(auto i = 0; i < argc; ++i) {
    delete unpacked[i];
  }

  delete unpacked;
}

void set_echo_if(xmldoc &doc, const std::vector<std::string> &argv) {
  if (4 != argv.size())
    throw invalid_param_error();

  const std::string &policy(argv[0]);
  const std::string &ifname(argv[1]);
  const std::string &limit(argv[2]);
  const std::string &burst(argv[3]);

  InterfaceMappingProvider mapping;
  ::std::string corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_policy, policy) || !is_match_std(regex::rex_ifname, corresponding_interface)
      || !is_match_opt(regex::rex_limit, limit) || !is_match_opt(regex::rex_burst, burst)
      || ("-" == limit && "-" != burst))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode request(get_node(ctx, "/f:firewall/f:ipv4/f:echo/f:request[@if='" + corresponding_interface + "']", false));

  if (request.is_empty()) {
    xmlnode echo(get_node(ctx, "/f:firewall/f:ipv4/f:echo"));
    request = append_node(echo, "request");

    append_attribute(request, "policy", policy);
    append_attribute(request, "if", corresponding_interface);
    if ("-" != limit)
      append_attribute(request, "limit", limit);
    if ("-" != burst)
      append_attribute(request, "burst", burst);
  } else {
    update_attribute(request, "policy", policy);
    updrem_attribute(request, "limit", limit);
    updrem_attribute(request, "burst", burst);
  }
}

void rem_echo_if(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size()) {
    throw invalid_param_error();
  }

  const std::string &ifname(argv[0]);
  InterfaceMappingProvider mapping;
  ::std::string corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_ifname, corresponding_interface)) {
    throw invalid_param_error();
  }

  xmlctx ctx(get_ctx(doc));
  xmlnode request(get_node(ctx, "/f:firewall/f:ipv4/f:echo/f:request[@if='" + corresponding_interface + "']", false));

  if (!request.is_empty()) {
    remove_node(std::move(request));
  }
}

void set_forward_link(xmldoc &doc, const std::vector<std::string> &argv) {
  if (3 != argv.size())
    throw invalid_param_error();

  const std::string &state(argv[0]);
  const std::string &if1(argv[1]);
  const std::string &if2(argv[2]);

  InterfaceMappingProvider mapping;
  auto corresponding_interfaces = mapping.get_interface(if1, if2);

  if (!is_match_std(regex::rex_onoff, state) || !is_match_std(regex::rex_ifname, corresponding_interfaces.first)
      || !is_match_std(regex::rex_ifname, corresponding_interfaces.second))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode link;

  link = get_node(
      ctx,
      "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + corresponding_interfaces.first + "' and @if2='"
          + corresponding_interfaces.second + "']",
      false);
  if (link.is_empty())
    link = get_node(
        ctx,
        "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + corresponding_interfaces.second + "' and @if2='"
            + corresponding_interfaces.first + "']",
        false);

  if (link.is_empty()) {
    xmlnode fw(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:fw"));
    link = append_node(fw, "link");

    append_attribute(link, "state", state);
    append_attribute(link, "if1", corresponding_interfaces.first);
    append_attribute(link, "if2", corresponding_interfaces.second);
  } else
    update_attribute(link, "state", state);
}

void rem_forward_link(xmldoc &doc, const std::vector<std::string> &argv) {
  if (2 != argv.size())
    throw invalid_param_error();

  const std::string &if1(argv[0]);
  const std::string &if2(argv[1]);

  InterfaceMappingProvider mapping;
  auto corresponding_interfaces = mapping.get_interface(if1, if2);

  if (!is_match_std(regex::rex_ifname, corresponding_interfaces.first)
      || !is_match_std(regex::rex_ifname, corresponding_interfaces.second))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode link;

  link = get_node(
      ctx,
      "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + corresponding_interfaces.first + "' and @if2='"
          + corresponding_interfaces.second + "']",
      false);
  if (link.is_empty())
    link = get_node(
        ctx,
        "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + corresponding_interfaces.second + "' and @if2='"
            + corresponding_interfaces.first + "']",
        false);

  if (!link.is_empty())
    remove_node(std::move(link));
}

void set_masq(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size())
    throw invalid_param_error();

  const std::string &ifname(argv[0]);
  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_ifname, corresponding_interface))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode masq(
      get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade[@if='" + corresponding_interface + "']", false));

  if (masq.is_empty()) {
    xmlnode snat(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:snat"));
    masq = append_node(snat, "masquerade");

    append_attribute(masq, "if", corresponding_interface);
  }
}

void rem_masq(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size())
    throw invalid_param_error();

  const std::string &ifname(argv[0]);
  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_ifname, corresponding_interface))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  if ("all" == argv[0]) {
    int const count = get_node_count(ctx, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade");

    // Delete all nodes.
    for (int i = count; i > 0; i--) {
      std::vector<std::string> arg_index(1, std::to_string(i));
      remove(doc, arg_index, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade");
    }
  } else {
    xmlnode masq(
        get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade[@if='" + corresponding_interface + "']",
                 false));

    if (!masq.is_empty())
      remove_node(std::move(masq));
  }
}

void add_pfw(xmldoc &doc, const std::vector<std::string> &argv) {
  if (7 != argv.size())
    throw invalid_param_error();

  const std::string &state(argv[0]);
  const std::string &ifname(argv[1]);
  const std::string &proto(argv[2]);
  const std::string &dst_ip(argv[3]);
  const std::string &dst_port(argv[4]);
  const std::string &fw_ip(argv[5]);
  const std::string &fw_port(argv[6]);

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_onoff, state) || !is_match_opt(regex::rex_ifname, corresponding_interface)
      || !is_match_std(regex::rex_tcpudp, proto) || !is_match_opt(regex::rex_ip4, dst_ip)
      || !is_match_opt(regex::rex_port_range, dst_port) || !is_match_opt(regex::rex_ip4, fw_ip)
      || !is_match_opt(regex::rex_port_range, fw_port) || ("-" == dst_ip && "-" == dst_port)
      || ("-" == fw_ip && "-" == fw_port))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode filter(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat"));
  int count;

  // Prevent adding duplicate entries.
  {
    std::string const ifname_proto_xpath =
        ("-" != corresponding_interface) ?
            ("/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port[@if='" + corresponding_interface + "' and @proto='" + proto) :
            "";
    std::string const dst_ip_xpath = ("-" != dst_ip) ? ("' and @dst_ip='" + dst_ip) : "";
    std::string const dst_port_xpath = ("-" != dst_port) ? ("' and @dst_port='" + dst_port) : "";
    std::string const fw_ip_xpath = ("-" != fw_ip) ? ("' and @fw_ip='" + fw_ip) : "";
    std::string const fw_port_xpath = ("-" != fw_port) ? ("' and @fw_port='" + fw_port) : "";

    count = get_node_count(ctx,
                           ifname_proto_xpath + dst_ip_xpath + dst_port_xpath + fw_ip_xpath + fw_port_xpath + "']");
  }

  if (0 == count) {
    xmlnode rule(append_node(filter, "fw_port"));

    append_attribute(rule, "state", state);
    if ("-" != corresponding_interface)
      append_attribute(rule, "if", corresponding_interface);
    append_attribute(rule, "proto", proto);
    if ("-" != dst_ip)
      append_attribute(rule, "dst_ip", dst_ip);
    if ("-" != dst_port)
      append_attribute(rule, "dst_port", dst_port);
    if ("-" != fw_ip)
      append_attribute(rule, "fw_ip", fw_ip);
    if ("-" != fw_port)
      append_attribute(rule, "fw_port", fw_port);
  }
}

void upd_pfw(xmldoc &doc, const std::vector<std::string> &argv) {
  if (8 != argv.size())
    throw invalid_param_error();

  const std::string &index(argv[0]);
  const std::string &state(argv[1]);
  const std::string &ifname(argv[2]);
  const std::string &proto(argv[3]);
  const std::string &dst_ip(argv[4]);
  const std::string &dst_port(argv[5]);
  const std::string &fw_ip(argv[6]);
  const std::string &fw_port(argv[7]);

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_number, index) || !is_match_std(regex::rex_onoff, state)
      || !is_match_opt(regex::rex_ifname, corresponding_interface) || !is_match_std(regex::rex_tcpudp, proto)
      || !is_match_opt(regex::rex_ip4, dst_ip) || !is_match_opt(regex::rex_port_range, dst_port)
      || !is_match_opt(regex::rex_ip4, fw_ip) || !is_match_opt(regex::rex_port_range, fw_port)
      || ("-" == dst_ip && "-" == dst_port) || ("-" == fw_ip && "-" == fw_port))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode fwp(get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port[" + index + "]", false));


  if (fwp.is_empty())
    return;

  update_attribute(fwp, "state", state);
  updrem_attribute(fwp, "if", corresponding_interface);
  update_attribute(fwp, "proto", proto);
  updrem_attribute(fwp, "dst_ip", dst_ip);
  updrem_attribute(fwp, "dst_port", dst_port);
  updrem_attribute(fwp, "fw_ip", fw_ip);
  updrem_attribute(fwp, "fw_port", fw_port);
}

void rem_pfw(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size())
    throw invalid_param_error();

  if ("all" == argv[0]) {
    xmlctx ctx(get_ctx(doc));
    int const count = get_node_count(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");

    // Delete all nodes.
    for (int i = count; i > 0; i--) {
      std::vector<std::string> arg_index(1, std::to_string(i));
      remove(doc, arg_index, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");
    }
  } else {
    remove(doc, argv, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port");
  }
}

void set_open_if(xmldoc &doc, const std::vector<std::string> &argv) {
  if (2 != argv.size())
    throw invalid_param_error();

  const std::string &state(argv[0]);
  const std::string &ifname(argv[1]);

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_onoff, state) || !is_match_std(regex::rex_ifname, corresponding_interface))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode interface;

  interface = get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open/f:interface[@if='" + corresponding_interface + "']",
                       false);

  if (interface.is_empty()) {
    xmlnode open(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open"));
    interface = append_node(open, "interface");

    append_attribute(interface, "state", state);
    append_attribute(interface, "if", corresponding_interface);
  } else
    update_attribute(interface, "state", state);
}

void rem_open_if(xmldoc &doc, const std::vector<std::string> &argv) {
  if (1 != argv.size())
    throw invalid_param_error();

  const std::string &ifname(argv[0]);

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_ifname, corresponding_interface))
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));
  xmlnode interface;

  interface = get_node(ctx, "/f:firewall/f:ipv4/f:input/f:open/f:interface[@if='" + corresponding_interface + "']", false);

  if (!interface.is_empty())
    remove_node(std::move(interface));
}

void add_filter_kw(xmldoc &doc, const std::vector<std::string> &argv) {
  // we want to stick to the old API, thus transform argv
  // from keyword args to positional args which can then
  // be fed into old API
  auto new_argv =  std::vector<std::string>(11);

  static std::string short_options = "s:i:p:o:m:r:d:a:t:l:g:";
  static option long_options[] =
  {
    {"state",           required_argument, nullptr, 's'},
    {"interface",       required_argument, nullptr, 'i'},
    {"protocol",        required_argument, nullptr, 'p'},
    {"sourceip",        required_argument, nullptr, 'o'},
    {"sourcemask",      required_argument, nullptr, 'm'},
    {"sourceport",      required_argument, nullptr, 'r'},
    {"destinationip",   required_argument, nullptr, 'd'},
    {"destinationmask", required_argument, nullptr, 'a'},
    {"destinationport", required_argument, nullptr, 't'},
    {"policy",          required_argument, nullptr, 'l'},
    {"tag",             required_argument, nullptr, 'g'},
    {0, 0, 0, 0}
  };

  // be aware of following behaviour of getopt:
  //   The default is to permute the contents of argv while scanning it
  //   so that eventually all the non-options are at the end. This
  //   allows options to be given in any order, even with programs that
  //   were not written to expect this.

  transform_argv(argv, short_options, long_options, new_argv);

  add_filter2(doc, new_argv);
}

void add_filter(xmldoc &doc, const std::vector<std::string> &argv) {
  if (10 != argv.size())
    throw invalid_param_error("add_filter");

  auto new_argv = argv;
  new_argv.push_back("-");

  add_filter2(doc, new_argv);
}

void add_filter2(xmldoc &doc, const std::vector<std::string> &argv) {
  if(11 != argv.size())
    throw invalid_param_error("add_filter");

  const std::string &state(argv[0]);
  const std::string &ifname(argv[1]);
  const std::string &proto(argv[2]);
  const std::string &src_ip(argv[3]);
  const std::string &src_mask(argv[4]);
  const std::string &src_port(argv[5]);
  const std::string &dst_ip(argv[6]);
  const std::string &dst_mask(argv[7]);
  const std::string &dst_port(argv[8]);
  const std::string &policy(argv[9]);
  const std::string tag(normalize_argv(argv[10]));

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_onoff, state) || !is_match_opt(regex::rex_ifname, corresponding_interface)
      || !is_match_std(regex::rex_tcpudp, proto) || !is_match_opt(regex::rex_ip4, src_ip)
      || !is_match_opt(regex::rex_ip4mask, src_mask) || !is_match_opt(regex::rex_port_range, src_port)
      || !is_match_opt(regex::rex_ip4, dst_ip) || !is_match_opt(regex::rex_ip4mask, dst_mask)
      || !is_match_opt(regex::rex_port_range, dst_port) || !is_match_std(regex::rex_policy, policy)
      || ("-" == proto && ("-" != src_port || "-" != dst_port)) || ("-" == src_ip && "-" != src_mask)
      || ("-" == dst_ip && "-" != dst_mask))
    throw invalid_param_error();

  // "normalize_argv" eliminates all unwanted characters,
  // thus we only need to constrain it to length
  //
  // note: keep in sync with "filter_tag" type in "/etc/firewall/patterns.xsd"
  if(tag.length() < 1 || tag.length() > 42)
    throw invalid_param_error();

  if ("-" == corresponding_interface && "-" == src_ip && "-" == src_port && "-" == dst_ip && "-" == dst_port)
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode filter(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:filter"));
  xmlnode rule(append_node(filter, "rule"));

  append_attribute(rule, "state", state);
  if ("-" != corresponding_interface)
    append_attribute(rule, "if", corresponding_interface);
  if ("-" != proto)
    append_attribute(rule, "proto", proto);
  if ("-" != src_ip)
    append_attribute(rule, "src_ip", src_ip);
  if ("-" != src_mask)
    append_attribute(rule, "src_mask", src_mask);
  if ("-" != src_port)
    append_attribute(rule, "src_port", src_port);
  if ("-" != dst_ip)
    append_attribute(rule, "dst_ip", dst_ip);
  if ("-" != dst_mask)
    append_attribute(rule, "dst_mask", dst_mask);
  if ("-" != dst_port)
    append_attribute(rule, "dst_port", dst_port);
  append_attribute(rule, "policy", policy);
  if ("-" != tag)
    append_attribute(rule, "tag", tag);
}

void upd_filter_kw(xmldoc &doc, const std::vector<std::string> &argv) {
  // we want to stick to the old API, thus transform argv
  // from keyword args to positional args which can then
  // be fed into old API
  auto new_argv =  std::vector<std::string>(11);

  static std::string short_options = "n:s:i:p:o:m:r:d:a:t:g:";
  static option long_options[] =
  {
    {"index",           required_argument, nullptr, 'n'},
    {"state",           required_argument, nullptr, 's'},
    {"interface",       required_argument, nullptr, 'i'},
    {"protocol",        required_argument, nullptr, 'p'},
    {"sourceip",        required_argument, nullptr, 'o'},
    {"sourcemask",      required_argument, nullptr, 'm'},
    {"sourceport",      required_argument, nullptr, 'r'},
    {"destinationip",   required_argument, nullptr, 'd'},
    {"destinationmask", required_argument, nullptr, 'a'},
    {"destinationport", required_argument, nullptr, 't'},
    {"tag",             required_argument, nullptr, 'g'},
    {0, 0, 0, 0}
  };

  // be aware of following behaviour of getopt:
  //   The default is to permute the contents of argv while scanning it
  //   so that eventually all the non-options are at the end. This
  //   allows options to be given in any order, even with programs that
  //   were not written to expect this.

  transform_argv(argv, short_options, long_options, new_argv);

  upd_filter2(doc, new_argv);
}

void upd_filter(xmldoc &doc, const std::vector<std::string> &argv) {
  if (10 != argv.size())
    throw invalid_param_error();

  auto new_argv = argv;
  new_argv.push_back("-");

  upd_filter2(doc, new_argv);
}

void upd_filter2(xmldoc &doc, const std::vector<std::string> &argv) {
  if (11 != argv.size())
    throw invalid_param_error("upd_filter");

  const std::string &index(argv[0]);
  const std::string &state(argv[1]);
  const std::string &ifname(argv[2]);
  const std::string &proto(argv[3]);
  const std::string &src_ip(argv[4]);
  const std::string &src_mask(argv[5]);
  const std::string &src_port(argv[6]);
  const std::string &dst_ip(argv[7]);
  const std::string &dst_mask(argv[8]);
  const std::string &dst_port(argv[9]);
  const std::string tag(normalize_argv(argv[10]));

  InterfaceMappingProvider mapping;
  auto corresponding_interface = mapping.get_interface(ifname);

  if (!is_match_std(regex::rex_number, index) || !is_match_std(regex::rex_onoff, state)
      || !is_match_opt(regex::rex_ifname, corresponding_interface) || !is_match_std(regex::rex_tcpudp, proto)
      || !is_match_opt(regex::rex_ip4, src_ip) || !is_match_opt(regex::rex_ip4mask, src_mask)
      || !is_match_opt(regex::rex_port_range, src_port) || !is_match_opt(regex::rex_ip4, dst_ip)
      || !is_match_opt(regex::rex_ip4mask, dst_mask) || !is_match_opt(regex::rex_port_range, dst_port)
      || ("-" == proto && ("-" != src_port || "-" != dst_port)) || ("-" == src_ip && "-" != src_mask)
      || ("-" == dst_ip && "-" != dst_mask))
    throw invalid_param_error();

  // "normalize_argv" eliminates all non alpha characters,
  // thus we only need to constrain it to length
  //
  // note: keep in sync with "/etc/firewall/patterns.xsd" - "filter_tag" type
  if(tag.length() < 1 || tag.length() > 42)
    throw invalid_param_error();

  if ("-" == corresponding_interface && "-" == src_ip && "-" == src_port && "-" == dst_ip && "-" == dst_port)
    throw invalid_param_error();

  xmlctx ctx(get_ctx(doc));

  xmlnode rule(get_node(ctx, "/f:firewall/f:ipv4/f:input/f:filter/f:rule[" + index + "]", false));

  if (rule.is_empty())
    return;

  update_attribute(rule, "state", state);
  updrem_attribute(rule, "if", corresponding_interface);
  updrem_attribute(rule, "proto", proto);
  updrem_attribute(rule, "src_ip", src_ip);
  updrem_attribute(rule, "src_mask", src_mask);
  updrem_attribute(rule, "src_port", src_port);
  updrem_attribute(rule, "dst_ip", dst_ip);
  updrem_attribute(rule, "dst_mask", dst_mask);
  updrem_attribute(rule, "dst_port", dst_port);
  updrem_attribute(rule, "tag", tag);
}

} // namespace impl

void process(xmldoc &doc, const std::string &cmd, const std::vector<std::string> &argv) {
  if ("--set-climits" == cmd)
    impl::set_climits(doc, argv);
  else if ("--set-echo" == cmd)
    impl::set_echo(doc, argv);
  else if ("--set-echo-if" == cmd)
    impl::set_echo_if(doc, argv);
  else if ("--rem-echo-if" == cmd)
    impl::rem_echo_if(doc, argv);
  else if ("--set-forward" == cmd)
    impl::set_forward(doc, argv);
  else if ("--set-forward-link" == cmd)
    impl::set_forward_link(doc, argv);
  else if ("--rem-forward-link" == cmd)
    impl::rem_forward_link(doc, argv);
  else if ("--set-masq" == cmd)
    impl::set_masq(doc, argv);
  else if ("--rem-masq" == cmd)
    impl::rem_masq(doc, argv);
  else if ("--add-pfw" == cmd)
    impl::add_pfw(doc, argv);
  else if ("--upd-pfw" == cmd)
    impl::upd_pfw(doc, argv);
  else if ("--rem-pfw" == cmd)
    impl::rem_pfw(doc, argv);
  else if ("--set-open-if" == cmd)
    impl::set_open_if(doc, argv);
  else if ("--rem-open-if" == cmd)
    impl::rem_open_if(doc, argv);
  else if ("--add-filter2" == cmd)
    impl::add_filter_kw(doc, argv);
  else if ("--add-filter" == cmd)
    impl::add_filter(doc, argv);
  else if ("--upd-filter2" == cmd)
    impl::upd_filter_kw(doc, argv);
  else if ("--upd-filter" == cmd)
    impl::upd_filter(doc, argv);
  else if ("--rem-filter" == cmd)
    impl::rem_filter(doc, argv);
  else
    throw invalid_param_error();
}

void process_internal(xmldoc &doc, const std::string &cmd, const std::vector<std::string> &argv) {
  if ("--internal-add-filter2" == cmd)
    impl::add_filter2(doc, argv);
  else
    process(doc, cmd, argv);
}

}  // namespace iptables
}  // namespace firewall
}  // namespace wago

