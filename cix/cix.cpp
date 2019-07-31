// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog(cout);
struct cix_exit : public exception {};

unordered_map<string, cix_command> command_map{
    {"exit", cix_command::EXIT},
    {"get", cix_command::GET},
    {"help", cix_command::HELP},
    {"ls"  , cix_command::LS},
    {"put"  , cix_command::PUT},
    {"rm"  , cix_command::RM}
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
    cout << help;
}

void cix_get(client_socket& server, const char arg[FILENAME_SIZE]) {
    cix_header header;
    header.command = cix_command::GET;
    strcpy(header.filename, arg);
    outlog << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    outlog << "received header " << header << endl;
    if (header.command != cix_command::FILEOUT) {
        outlog << "sent GET, server did not return FILEOUT" << endl;
        outlog << "server returned " << header << endl;
    }
    else {
        char buffer[header.nbytes + 1];
        recv_packet(server, buffer, header.nbytes);
        outlog << "received " << header.nbytes << " bytes" << endl;
        buffer[header.nbytes] = '\0';
        cout << buffer;
    }
}

void cix_ls(client_socket& server) {
    cix_header header;
    header.command = cix_command::LS;
    outlog << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    outlog << "received header " << header << endl;
    if (header.command != cix_command::LSOUT) {
        outlog << "sent LS, server did not return LSOUT" << endl;
        outlog << "server returned " << header << endl;
    }
    else {
        char buffer[header.nbytes + 1];
        recv_packet(server, buffer, header.nbytes);
        outlog << "received " << header.nbytes << " bytes" << endl;
        buffer[header.nbytes] = '\0';
        cout << buffer;
        //auto buffer = make_unique<char[]>(header.nbytes + 1);
        //recv_packet(server, buffer.get(), header.nbytes);
        //outlog << "received " << header.nbytes << " bytes" << endl;
        //buffer[header.nbytes] = '\0';
        //cout << buffer.get();
    }
}


void usage() {
    cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
    throw cix_exit();
}

int main(int argc, char** argv) {
    outlog.execname(basename(argv[0]));
    outlog << "starting" << endl;
    vector<string> args(&argv[1], &argv[argc]);
    if (args.size() > 2) usage();
    string host = get_cix_server_host(args, 0);
    in_port_t port = get_cix_server_port(args, 1);
    outlog << to_string(hostinfo()) << endl;
    try {
        outlog << "connecting to " << host << " port " << port << endl;
        client_socket server(host, port);
        outlog << "connected to " << to_string(server) << endl;
        for (;;) {
            string command;
            string arg;
            cin >> command >> arg;
            if (cin.eof()) throw cix_exit();
            outlog << "command " << command << endl;
            const auto& itor = command_map.find(command);
            cix_command cmd = itor == command_map.end()
                ? cix_command::ERROR : itor->second;
            switch (cmd) {
            case cix_command::EXIT:
                throw cix_exit();
                break;
            case cix_command::HELP:
                cix_help();
                break;
            case cix_command::LS:
                cix_ls(server);
                break;
            case cix_command::GET:
                cix_get(server, arg.c_str());
                break;
            default:
                outlog << command << ": invalid command" << endl;
                break;
            }
        }
    }
    catch (socket_error& error) {
        outlog << error.what() << endl;
    }
    catch (cix_exit& error) {
        outlog << "caught cix_exit" << endl;
    }
    outlog << "finishing" << endl;
    return 0;
}

