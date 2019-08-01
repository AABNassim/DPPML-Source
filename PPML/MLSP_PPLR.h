//
// Created by Nassim AAB on 2019-07-27.
//

#ifndef DPPML_MLSP_LR_H
#define DPPML_MLSP_LR_H


#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "HEAAN.h"

#include <vector>
#include <Ciphertext.h>
#include <cstdlib>
#include <chrono>
#include "../ML/DatasetReader.h"
#include "../PPML/MLSP.h"
#include <cmath>

#include <iomanip>
#include <ctime>
#include <sstream>
#include "../ML/LogisticRegression.h"

#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>


#include "../CRYPTO/DTPKC.h"
#include "../CRYPTO/EvalAdd.h"

using namespace std::chrono;


class MLSP_PPLR {
public:
    //Meta
    bool log = true;

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
    int nb_workers = 4;
    std::vector<int> workers_sockets;

    std::vector<struct sockaddr_in> addresses;

    int opt = 1;
    int addrlen = sizeof(struct sockaddr_in);

    // DTPKC
    DTPKC dtpkc;

    // FHE Crypto part
    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 600; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;

    Ring ring;
    SecretKey secretKey;
    Scheme scheme;

    // Serveer side
    /*int server_fd, new_socket;
    int port = 8080;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);*/


    MLSP_PPLR();
    //MLSP_LR(Scheme* scheme1, SecretKey* secretKey1);

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

    void test_send_dtpkc_cipher();

        // ------------------------------------ Logistic Regression --------------------------------------


    long numThread = 5;
    double alpha = 1;
    int epochs = 30;
    int nb_slots = n;
    int nb_rows = 64;
    int nb_cols = 16;
    int log_nb_cols = 4;
    int log_nb_rows = 6;
    int d = 10;
    int class_number = 2;
    int sigmoid_degree = 7;
    int nb_training_ciphers = 2;
    int m = nb_rows * nb_training_ciphers;


    gmp_randstate_t randstate;
    mpz_class dtpkc_pkey, dtpkc_skey;
    int precision = 30;
    int nb_bits = 1024, error = 100;

    long dtpkc_scale_factor = pow(10, 6);


    string dataset_name = "Edin";
    string datasets_path = "../DATA/Datasets/";
    vector<Ciphertext> cipher_training_set;
    Ciphertext cipher_model;
    vector<double> sigmoid_coeffs_deg3 = {0.5, -1.20096, 0.81562};
    vector<double> sigmoid_coeffs_deg5 = {0.5, -1.53048, 2.3533056, -1.3511295};
    vector<double> sigmoid_coeffs_deg7 = {0.5, -1.73496, 4.19407, -5.43402, 2.50739};

    Ciphertext cipher_gadget_matrix;

    complex<double> *encoded_sigmoid_coeffs;
    Ciphertext cipher_sigmoid_coeffs;
    Ciphertext pp_sigmoid_deg3(Ciphertext cipher_x);
    Ciphertext pp_sigmoid_deg5(Ciphertext cipher_x);
    Ciphertext pp_sigmoid_deg7(Ciphertext cipher_x);
    Ciphertext pp_sigmoid(Ciphertext c, int degree);
    void test_pp_sigmoid(vector<double> x);
    double approx_sigmoid_deg3(double x);
    void pp_fit();
    void encrypt_dataset();
    Ciphertext pp_dot_product(Ciphertext cx, Ciphertext cy);
    void test_pp_dot_product(vector<double> x, vector<double> y);
    Ciphertext sum_slots(Ciphertext c, int start_slot, int end_slot);
    Ciphertext sum_slots_reversed(Ciphertext c, int start_slot, int end_slot);
    void test_sum_slots();
    Ciphertext refresh_cipher_unsecure(Ciphertext c);

    void pp_fit_local();
    Ciphertext refresh_cipher_local(Ciphertext c);

        /*void test_cryptosystem_switching();
        Ciphertext cryptosystem_switching_single(DTPKC::Cipher dtpkc_value);
        Ciphertext cryptosystem_switching_batch_optimized(vector<DTPKC::Cipher> dtpkc_values);
        void test_cryptosystem_switching_batch_optimized();

        Ciphertext refresh_cipher_unsecure(Ciphertext c);*/

    Ciphertext cryptosystem_switching_single(DTPKC::Cipher dtpkc_value);
    Ciphertext cryptosystem_switching_batch_naive(vector<DTPKC::Cipher> dtpkc_values);
    Ciphertext cryptosystem_switching_batch_optimized(vector<DTPKC::Cipher> dtpkc_values);

    void test_cryptosystem_switching_batch_optimized();
    void test_cryptosystem_switching_single();
    void test_cryptosystem_switching_batch_naive();


    Ciphertext refresh_cipher(Ciphertext c);
    void test_refresh_cipher();
    void test_refresh_cipher_unsecure();


    void send_dtpkc_cipher(DTPKC::Cipher dtpkc_value);

    void debug();

    Ciphertext* refresh_cipher_unsecure_modified(Ciphertext c);
    Ciphertext refresh_cipher_unsecure_old(Ciphertext c);

    Ciphertext refresh_cipher_old(Ciphertext c);

    void pp_fit_distributed();

    };

#endif //DPPML_MLSP_LR_H
