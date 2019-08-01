//
// Created by root on 30/06/19.
//

#include "MLSP.h"


MLSP::MLSP() {


}

MLSP::MLSP(Scheme* scheme1, SecretKey* secretKey1) {
    scheme = scheme1;
    secretKey = secretKey1;
}

void MLSP::connect_to_csp() {
    // Initiate connection with csp
    if ((csp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(csp_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, csp_ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(csp_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
}

void MLSP::accept_workers_connections() {

    if ((server_mlsp_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

// Forcefully attaching socket to the port
    if (setsockopt(server_mlsp_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    mlsp_address.sin_family = AF_INET;
    mlsp_address.sin_addr.s_addr = INADDR_ANY;
    mlsp_address.sin_port = htons(port);

// Forcefully attaching socket to the port
    if (::bind(server_mlsp_fd, (struct sockaddr *) &mlsp_address,
             sizeof(mlsp_address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_mlsp_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nb_workers; i++) {
        int worker_socket;
        if ((worker_socket = accept(server_mlsp_fd, (struct sockaddr *) &mlsp_address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        workers_sockets.push_back(worker_socket);
        cout << "Socket n " << i << " = " << worker_socket << endl;
    }
    cout << "Setup the sockets" << endl;
    long forty_two;
    long fifty_three;
    long twenty_two;
    if (read_long(workers_sockets[0], &forty_two)) {
        cout << "Got : " << forty_two << endl;
    } else {
        cout << "Did not find the first number" << endl;
    }
    if (read_long(workers_sockets[1], &fifty_three)) {
        cout << "Got : " << fifty_three << endl;
    } else {
        cout << "Did not find the second number" << endl;
    }
    if (read_long(workers_sockets[2], &twenty_two)) {
        cout << "Got : " << twenty_two << endl;
    } else {
        cout << "Did not find the third number" << endl;
    }
}

void MLSP::send_message() {

    send(csp_sock , hello, strlen(hello) , 0 );

    printf("Hello message sent\n");
    valread = read(csp_sock , buffer, 1024);
    printf("%s\n", buffer);
}

bool MLSP::send_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = send(sock, pbuf, buflen, 0);

        pbuf += num;
        buflen -= num;
    }

    return true;
}


bool MLSP::send_long(int sock, long value)
{
    value = htonl(value);
    return send_data(sock, &value, sizeof(value));
}

/*long min(long a, long b) {
    if (a < b) {
        return a;
    }
    return b;
}*/


bool MLSP::send_file(int sock, char* path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    rewind(f);
    if (filesize == EOF)
        return false;
    if (!send_long(sock, filesize))
        return false;
    if (filesize > 0)
    {
        char buffer[1024];
        do
        {
            size_t num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            num = fread(buffer, 1, num, f);
            if (num < 1)
                return false;
            if (!send_data(sock, buffer, num))
                return false;
            filesize -= num;
        }
        while (filesize > 0);
    }
    return true;
}


bool MLSP::read_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = recv(sock, pbuf, buflen, 0);
        if (num == 0)
            return false;

        pbuf += num;
        buflen -= num;
    }

    return true;
}

bool MLSP::read_long(int sock, long *value)
{
    if (!read_data(sock, value, sizeof(value)))
        return false;
    *value = ntohl(*value);
    return true;
}


bool MLSP::read_file(int sock, char* path)
{
    FILE *f = fopen(path, "wb");
    long filesize;
    if (!read_long(sock, &filesize))
        return false;
    if (filesize > 0)
    {
        char buffer[1024];
        do
        {
            int num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            if (!read_data(sock, buffer, num))
                return false;
            int offset = 0;
            do
            {
                size_t written = fwrite(&buffer[offset], 1, num-offset, f);
                if (written < 1)
                    return false;
                offset += written;
            }
            while (offset < num);
            filesize -= num;
        }
        while (filesize > 0);
    }
    return true;
}


void MLSP::test_send_file() {
    send_file(csp_sock, "./cipher.txt");
}

void MLSP::test_key_exchange() {
    // Send signal
    if (send_long(csp_sock, 42)) {
        // Need to receive the keys

        // Encryption key
        char* path = "serkey/ENCRYPTION_KEY.txt";
        read_file(csp_sock, path);

        cout << "Got the encryption key !" << endl;

        // Read the encryption key

        /*// Construct and Generate Public Keys //
        srand(42);
        SetNumThreads(numThread);
        Ring ring;
        SecretKey secretKey(ring);
        Scheme scheme(secretKey, ring);*/

        //scheme->read_encryption_key();
        Ciphertext test_cipher;
        complex<double> *mvec1 = new complex<double>[n];

        for (int i = 0; i < n; ++i) {
            complex<double> c;
            c.real(i);
            c.imag(0);
            mvec1[i] = c;
        }
        scheme->encrypt(test_cipher, mvec1, n, logp, logq);

        complex<double> *decrypted = scheme->decrypt(*secretKey, test_cipher);

        cout.precision(10);
        cout << "Decrypted values:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << decrypted[i] << ' ';
        }
        cout << " " << endl;

        //Ciphertext test_cipher_read;
        //SerializationUtils::readCiphertextModified(test_cipher_read, "cipher_test.txt");

        // Serialize cipher
        SerializationUtils::writeCiphertext(test_cipher, "test_ciphertext.txt");
        send_file(csp_sock, "test_ciphertext.txt");


        // Multiplication
        read_file(csp_sock, "serkey/MULTIPLICATION_KEY.txt");
        //scheme->read_multiplication_key();

        Ciphertext cipher_a;
        Ciphertext cipher_b;
        Ciphertext cipher_mult;

        complex<double> *mvec_a = new complex<double>[n];
        complex<double> *mvec_b = new complex<double>[n];

        for (int i = 0; i < n; ++i) {
            complex<double> c;
            c.real(i);
            c.imag(0);
            mvec_a[i] = c;
            mvec_b[i] = c;
        }

        scheme->encrypt(cipher_a, mvec_a, n, logp, logq);
        scheme->encrypt(cipher_b, mvec_b, n, logp, logq);

        scheme->mult(cipher_mult, cipher_a, cipher_b);
        scheme->reScaleByAndEqual(cipher_mult, logp);

        SerializationUtils::writeCiphertext(cipher_mult, "cipher_mult.txt");

        if (send_file(csp_sock, "cipher_mult.txt")) {
            cout << "Sent the mult cipheer" << endl;
        }
        else {
            cout << "ERROR, could not send mult cipher" << endl;
        }

        // Get left rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            cout << "Rot " << i << endl;
            long idx = 1 << i;
            string filename = "serkey/ROTATION_" + to_string(idx) + ".txt";
            char* path = (char*) filename.c_str();
            read_file(csp_sock, path);
        }
        cout << "Received the rotation keys" << endl;
        //scheme->read_left_rotation_key();

        // Test the rotation
        Ciphertext cipher_left_rot;
        scheme->leftRotateFast(cipher_left_rot, cipher_a, 4);
        SerializationUtils::writeCiphertext(cipher_left_rot, "cipher_rot.txt");

        // send the cipher
        if (send_file(csp_sock, "cipher_rot.txt")) {
            cout << "Sent the rotation cipher" << endl;
        }


        // Get right rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            cout << "Rot " << i << endl;
            long idx = Nh - (1 << i);
            string filename = "serkey/ROTATION_" + to_string(idx) + ".txt";
            char* path = (char*) filename.c_str();
            read_file(csp_sock, path);
        }
        cout << "Received the rotation keys" << endl;
        //scheme->read_right_rotation_key();

        // Test right rotation
        Ciphertext cipher_right_rot;
        scheme->rightRotateFast(cipher_right_rot, cipher_a, 4);
        SerializationUtils::writeCiphertext(cipher_right_rot, "cipher_right_rot.txt");

        // send the cipher
        if (send_file(csp_sock, "cipher_right_rot.txt")) {
            cout << "Sent the right rotation cipher" << endl;
        }
    }

}