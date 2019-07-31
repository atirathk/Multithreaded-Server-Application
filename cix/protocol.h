// $Id: protocol.h,v 1.7 2019-05-15 15:26:36-07 - - $

#ifndef __PROTOCOL__H__
#define __PROTOCOL__H__

#include <cstdint>
#include <cstring>
#include <iostream>
using namespace std;

#include "sockets.h"

enum class cix_command : uint8_t {
    ERROR = 0, EXIT, GET, HELP, LS, PUT, RM, FILEOUT, LSOUT, ACK, NAK,
};
constexpr size_t FILENAME_SIZE = 59;
constexpr size_t HEADER_SIZE = 64;
struct cix_header {
    uint32_t nbytes{};
    cix_command command{ cix_command::ERROR };
    char filename[FILENAME_SIZE]{};
};
//cix_command::ERROR: return "ERROR";
//cix_command::EXIT: return "EXIT";
//void cix_command::GET();

//cix_command::HELP: return "HELP";
//cix_command::LS: return "LS";
//cix_command::PUT: return "PUT";
//cix_command::RM: return "RM";
//cix_command::FILEOUT: return "FILEOUT";
//cix_command::LSOUT: return "LSOUT";
//cix_command::ACK;
//cix_command::NAK;
static_assert (sizeof(cix_header) == HEADER_SIZE, "running static_assert");

void send_packet(base_socket& socket,
    const void* buffer, size_t bufsize);

void recv_packet(base_socket& socket, void* buffer, size_t bufsize);

ostream& operator<< (ostream& out, const cix_header& header);

string get_cix_server_host(const vector<string>& args, size_t index);

in_port_t get_cix_server_port(const vector<string>& args,
    size_t index);

#endif

