//
// Created by root on 30/06/19.
//

#ifndef DPPML_MLSP_H
#define DPPML_MLSP_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "HEAAN.h"

using namespace std;



class MLSP {
public:
    // Client side
    int csp_sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    int csp_port = 8080;
    char *csp_ip = "127.0.0.1";

    // Server role for the workers
    int server_mlsp_fd;
    struct sockaddr_in mlsp_address;
    int port = 8100;

    // Cluster infos
    int nb_workers = 3;
    std::vector<int> workers_sockets;

    std::vector<struct sockaddr_in> addresses;

    int opt = 1;
    int addrlen = sizeof(struct sockaddr_in);


    // Crypto part
    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;

    SecretKey* secretKey;
    Scheme* scheme;

    // Serveer side
    /*int server_fd, new_socket;
    int port = 8080;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);*/


    MLSP();
    MLSP(Scheme* scheme1, SecretKey* secretKey1);

    void send_message();

    bool send_file(int sock, char *path);

    void test_send_file();

    void test_key_exchange();


    bool read_file(int sock, char *path);

    bool read_long(int sock, long *value);

    bool read_data(int sock, void *buf, int buflen);

    bool send_long(int sock, long value);

    bool send_data(int sock, void *buf, int buflen);

    void accept_workers_connections();

    void connect_to_csp();

    };
#endif //DPPML_MLSP_H
