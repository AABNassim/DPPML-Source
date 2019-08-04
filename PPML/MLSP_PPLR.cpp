//
// Created by Nassim AAB on 2019-07-27.
//

#include "MLSP_PPLR.h"

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

MLSP_PPLR::MLSP_PPLR() : secretKey(ring), scheme(secretKey, ring, false) {

    SetNumThreads(numThread);
    // Deserialize DTPKC

    /*dtpkc.deserializeDtpkc("./");

    DTPKC::Cipher dtpkc_value;

    long val = 2;
    mpz_class mpz_value;

    mpz_value.set_str(std::to_string(val), 10);
    dtpkc_pkey = dtpkc.pkw;
    dtpkc_value = dtpkc.enc(mpz_value, dtpkc.pkw);

    mpz_class mpz_value_decrypted = dtpkc.Sdec(dtpkc_value);
    long decrypted_value = mpz_value_decrypted.get_si();

    cout << "Bongo Bongo ? :" << decrypted_value << endl;*/

    // First, we need to establish a connection between the MLSP and the CSP
    //accept_workers_connections();
    // Then, proceed to the key exchange
    //test_key_exchange();

    // Once we have the keys, initialize the model

    /*encoded_sigmoid_coeffs = new complex<double>[n]; // TODO : Useless ?
    for (int i = 0; i < sigmoid_degree; i++) {
        complex<double> c;
        c.imag(0);
        c.real(sigmoid_coeffs_deg3[i]);
        encoded_sigmoid_coeffs[i] = c;
    }
    scheme.encrypt(cipher_sigmoid_coeffs, encoded_sigmoid_coeffs, n, logp, logq);*/

    // Model initialization
    complex<double> *encoded_model = new complex<double>[n];
    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_model[i] = c;
    }

    /*for (int i = 0; i < d; i++) {
        complex<double> c;
        c.imag(0);
        c.real(fRand(-4, 4));
        encoded_model[i] = c;
    }*/
    scheme.encrypt(cipher_model, encoded_model, n, logp, logq);


    //Ciphertext yatangakli = refresh_cipher_unsecure(cipher_model);

    // Toy Matrix creation
    complex<double> *encoded_gadget_matrix = new complex<double>[n];
    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_gadget_matrix[i] = c;
    }
    for (int i = 0; i < nb_rows; i++) {
        complex<double> c;
        c.imag(0);
        c.real(1);
        encoded_gadget_matrix[i * nb_cols] = c;
    }
    scheme.encrypt(cipher_gadget_matrix, encoded_gadget_matrix, n, logp, logq);    //TODO : what's happening ?

    //accept_workers_connections();
    // Test the refresh procedure
}

/*MLSP_LR::MLSP_LR(Scheme* scheme1, SecretKey* secretKey1) {
    scheme = scheme1;
    secretKey = secretKey1;
}*/

void MLSP_PPLR::connect_to_csp() {
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

void MLSP_PPLR::accept_workers_connections() {

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
    /*if (read_long(workers_sockets[1], &fifty_three)) {
        cout << "Got : " << fifty_three << endl;
    } else {
        cout << "Did not find the second number" << endl;
    }
    if (read_long(workers_sockets[2], &twenty_two)) {
        cout << "Got : " << twenty_two << endl;
    } else {
        cout << "Did not find the third number" << endl;
    }*/
}

void MLSP_PPLR::send_message() {

    send(csp_sock , hello, strlen(hello) , 0 );

    printf("Hello message sent\n");
    valread = read(csp_sock , buffer, 1024);
    printf("%s\n", buffer);
}

bool MLSP_PPLR::send_data(int sock, void *buf, int buflen)
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


bool MLSP_PPLR::send_long(int sock, long value)
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


bool MLSP_PPLR::send_file(int sock, char* path)
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


bool MLSP_PPLR::read_data(int sock, void *buf, int buflen)
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

bool MLSP_PPLR::read_long(int sock, long *value)
{
    if (!read_data(sock, value, sizeof(value)))
        return false;
    *value = ntohl(*value);
    return true;
}


bool MLSP_PPLR::read_file(int sock, char* path)
{
    FILE *f = fopen(path, "wb");
    long filesize;
    if (!read_long(sock, &filesize))
        return false;
    if (filesize > 0)
    {
        cout << "Got file of size : " << filesize << endl;
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


void MLSP_PPLR::test_send_file() {
    send_file(csp_sock, "./cipher.txt");
}

void MLSP_PPLR::test_key_exchange() {
    return;
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

        //scheme.read_encryption_key();
        Ciphertext test_cipher;
        complex<double> *mvec1 = new complex<double>[n];

        for (int i = 0; i < n; ++i) {
            complex<double> c;
            c.real(i);
            c.imag(0);
            mvec1[i] = c;
        }
        scheme.encrypt(test_cipher, mvec1, n, logp, logq);

        // Serialize cipher
        SerializationUtils::writeCiphertext(test_cipher, "test_ciphertext.txt");
        send_file(csp_sock, "test_ciphertext.txt");


        // Multiplication
        read_file(csp_sock, "serkey/MULTIPLICATION_KEY.txt");

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

        scheme.encrypt(cipher_a, mvec_a, n, logp, logq);
        scheme.encrypt(cipher_b, mvec_b, n, logp, logq);

        scheme.mult(cipher_mult, cipher_a, cipher_b);
        scheme.reScaleByAndEqual(cipher_mult, logp);

        SerializationUtils::writeCiphertext(cipher_mult, "cipher_mult.txt");

        if (send_file(csp_sock, "cipher_mult.txt")) {
            cout << "Sent the mult cipheer" << endl;
        }
        else {
            cout << "ERROR, could not send mult cipher" << endl;
        }

        // Get left rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            //cout << "Rot " << i << endl;
            long idx = 1 << i;
            string filename = "serkey/ROTATION_" + to_string(idx) + ".txt";
            char* path = (char*) filename.c_str();
            read_file(csp_sock, path);
        }
        cout << "Received the left rotation keys" << endl;
        //scheme.read_left_rotation_key();

        // Test the rotation
        Ciphertext cipher_left_rot;
        scheme.leftRotateFast(cipher_left_rot, cipher_a, 1);
        SerializationUtils::writeCiphertext(cipher_left_rot, "cipher_rot.txt");

        // send the cipher
        if (send_file(csp_sock, "cipher_rot.txt")) {
            cout << "Sent the rotation cipher" << endl;
        }


        // Get right rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            //cout << "Rot " << i << endl;
            long idx = Nh - (1 << i);
            string filename = "serkey/ROTATION_" + to_string(idx) + ".txt";
            char* path = (char*) filename.c_str();
            read_file(csp_sock, path);
        }
        cout << "Received the right rotation keys" << endl;
        //scheme.read_right_rotation_key();

        // Test right rotation
        Ciphertext cipher_right_rot;
        scheme.rightRotateFast(cipher_right_rot, cipher_a, 4);
        SerializationUtils::writeCiphertext(cipher_right_rot, "cipher_right_rot.txt");

        // send the cipher
        if (send_file(csp_sock, "cipher_right_rot.txt")) {
            cout << "Sent the right rotation cipher" << endl;
        }
    }
}

// --------------------------------------------- LOGISTIC REGRESSION --------------------------------------------
void MLSP_PPLR::encrypt_dataset() {
    DatasetReader *datasetReader = new DatasetReader(datasets_path + dataset_name + "/", class_number, d);
    complex<double> *vectorized_batch = new complex<double>[nb_slots];
    vector<Record*> training_data = datasetReader->read_all_train_records();

    for (int i = 0; i < m; i++) {
        training_data[i]->print();
    }

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        vectorized_batch[i] = c;
    }

    for (int i = 0; i < nb_training_ciphers; i++) {
        Ciphertext cipher_training_batch;
        vectorized_batch = new complex<double>[nb_slots];
        for (int j = 0; j < nb_rows; j++) {
            if (i * nb_rows + j < m) {
                Record *rcd = training_data[i * nb_rows + j];
                for (int k = 0; k < d; k++) {
                    complex<double> c;
                    int label;
                    if (rcd->label == 0) {
                        label = -1;
                    } else {
                        label = 1;
                    }
                    c.imag(0);
                    c.real(rcd->values[k] * label);
                    vectorized_batch[j * nb_cols + k] = c;
                }
            } else {
                for (int k = 0; k < d; k++) {
                    complex<double> c;
                    c.imag(0);
                    c.real(0);
                    vectorized_batch[j * nb_cols + k] = c;
                }
            }
        }
        scheme.encrypt(cipher_training_batch, vectorized_batch, n, logp, logq);
        cipher_training_set.push_back(cipher_training_batch);
    }
}

void MLSP_PPLR::debug() {

    complex<double> *mvec1 = new complex<double>[n];
    complex<double> *mvec2 = new complex<double>[n];
    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec1[i] = c;
    }

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec2[i] = c;
    }

    Ciphertext cipher1, cipher2, product;
    scheme.encrypt(cipher1, mvec1, n, logp, logq);
    scheme.encrypt(cipher2, mvec2, n, logp, logq);

    Ciphertext cipher_sig = pp_sigmoid_deg3(cipher1);

    complex<double> * sig = scheme.decrypt(secretKey, cipher_sig);

    cout << "sig:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << sig[i] << ' ';
    }
    cout << " " << endl;

    scheme.mult(product, cipher1, cipher2);
    scheme.reScaleByAndEqual(product, logp);

    SerializationUtils::writeCiphertext(cipher1, "cipher1.txt");
    if (send_file(csp_sock, "cipher1.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(cipher2, "cipher2.txt");
    if (send_file(csp_sock, "cipher2.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(product, "product.txt");
    if (send_file(csp_sock, "product.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    SerializationUtils::writeCiphertext(cipher_sig, "cipher_sig.txt");
    if (send_file(csp_sock, "cipher_sig.txt")) {
        cout << "Sent the cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }


    read_file(csp_sock, "recrypted_cipher1.txt");
    Ciphertext* recrypted_cipher1 = SerializationUtils::readCiphertext("recrypted_cipher1.txt");

    read_file(csp_sock, "recrypted_cipher2.txt");
    Ciphertext* recrypted_cipher2 = SerializationUtils::readCiphertext("recrypted_cipher2.txt");

    read_file(csp_sock, "recrypted_product.txt");
    Ciphertext* recrypted_product = SerializationUtils::readCiphertext("recrypted_product.txt");

    read_file(csp_sock, "cipher_rot.txt");
    Ciphertext* cipher_rot = SerializationUtils::readCiphertext("cipher_rot.txt");


    complex<double> * plaintext_cipher1 = scheme.decrypt(secretKey, *recrypted_cipher1);

    cout << "Plaintext value of recrypted cipher1:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_cipher1[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * plaintext_cipher2 = scheme.decrypt(secretKey, *recrypted_cipher2);

    cout << "Plaintext value of recrypted cipher2:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << plaintext_cipher2[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * prod = scheme.decrypt(secretKey, *recrypted_product);

    cout << "Plaintext value of recrypted prod:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << prod[i] << ' ';
    }
    cout << " " << endl;

    complex<double> * rot = scheme.decrypt(secretKey, *cipher_rot);

    cout << "Plaintext value of rot:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << rot[i] << ' ';
    }
    cout << " " << endl;
}

void MLSP_PPLR::pp_fit() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    string folder_name = oss.str();
    auto config_file_name = "CONFIG.txt";
    auto grads_file_name = "GRADS.txt";
    auto weights_file_name = "WEIGHTS.txt";
    auto duration_file_name = "DURATION.txt";

    auto path = "../LOGS/" + dataset_name + "/PPLR/CENTRALISED/" + folder_name + "/";

    ofstream config_log_file;
    ofstream grads_log_file;
    ofstream weights_log_file;
    ofstream duration_log_file;

    if (mkdir(path.c_str(), 0777) == -1)
        cerr << "Error :  " << strerror(errno) << endl;
    else
        cout << "Directory created";

    grads_log_file.open(path + grads_file_name);
    weights_log_file.open(path + weights_file_name);
    config_log_file.open(path + config_file_name);
    duration_log_file.open(path + duration_file_name);


    config_log_file << " -------------------------- Dataset -----------------------------" << endl;
    config_log_file << "dataset = " << dataset_name << endl;
    config_log_file << "m = " << m << endl;
    config_log_file << "d = " << d << endl;
    config_log_file << "classes = " << class_number << endl;


    config_log_file << " -------------------------- Logistic Regression -----------------------------" << endl;
    config_log_file << "epochs = " << epochs << endl;
    config_log_file << "alpha = " << alpha << endl;
    config_log_file << "sigmoid_degree = " << sigmoid_degree << endl;

    config_log_file << " -------------------------- Crypto -----------------------------" << endl;
    config_log_file << "slots = " << n << endl;
    config_log_file << "nb_training_ciphers = " << nb_training_ciphers << endl;
    config_log_file << "logp = " << logp << endl;
    config_log_file << "logq = " << logq << endl;
    config_log_file << "logN = " << logN << endl;
    config_log_file << "logQ = " << logQ << endl;
    config_log_file << "nb_threads = " << numThread << endl;


    config_log_file.close();

    high_resolution_clock::time_point start_timer = high_resolution_clock::now();
    high_resolution_clock::time_point end_timer = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

    for (int e = 0; e < epochs; e++) {
        start_timer = high_resolution_clock::now();
        Ciphertext cipher_grad;
        complex<double> *encoded_grad = new complex<double>[n];
        for (int i = 0; i < n; i++) {
            complex<double> c;
            c.imag(0);
            c.real(0);
            encoded_grad[i] = c;
        }
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 4 * logp - sigmoid_degree * logp);

        for (int i = 0; i < nb_training_ciphers; i++) {
            Ciphertext cipher_product;

            scheme.mult(cipher_product, cipher_model, cipher_training_set[i]);   // TODO : modify
            scheme.reScaleByAndEqual(cipher_product, logp);

            Ciphertext cipher_dot_product = sum_slots(cipher_product, 0, log_nb_cols);

            scheme.multAndEqual(cipher_dot_product, cipher_gadget_matrix);

            scheme.reScaleByAndEqual(cipher_dot_product, logp);

            Ciphertext cipher_dot_product_duplicated = sum_slots_reversed(cipher_dot_product, 0, log_nb_cols);

            Ciphertext cipher_sig = pp_sigmoid(cipher_dot_product_duplicated, sigmoid_degree);

            scheme.multAndEqual(cipher_sig, cipher_training_set[i]); // TODO : modify
            scheme.reScaleByAndEqual(cipher_sig, logp);

            Ciphertext cipher_partial_grad = sum_slots(cipher_sig, log_nb_cols, log_nb_rows + log_nb_cols);

            scheme.addAndEqual(cipher_grad, cipher_partial_grad);
        }

        scheme.multByConstAndEqual(cipher_grad, alpha / m, logp);
        scheme.reScaleByAndEqual(cipher_grad, logp);            //TODO: factor out

        cout << "Gradient n : " << e << endl;

        Ciphertext refreshed_grad = refresh_cipher(cipher_grad);
        scheme.addAndEqual(cipher_model, refreshed_grad);

        end_timer = high_resolution_clock::now();
        auto epoch_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

        duration_log_file << to_string(epoch_duration) << endl;
        total_duration += epoch_duration;

        // TODO : BE CAREFUL
        if (false) {
            SerializationUtils::writeCiphertext(cipher_model, "cipher_weights.txt");
            if (send_file(csp_sock, "cipher_weights.txt")) {
                cout << "Sent the cipheer" << endl;
            }
            else {
                cout << "ERROR, could not send the cipher" << endl;
            }
        }
        if (logging) {
            complex<double> * grad = scheme.decrypt(secretKey, cipher_grad);
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    grads_log_file << grad[j * nb_cols + k].real() << ", ";
                }
                grads_log_file << "" << endl;
            }
            cout << " " << endl;

            complex<double> * weights = scheme.decrypt(secretKey, cipher_model);

            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    weights_log_file << weights[j * nb_cols + k].real() << ", ";
                }
                weights_log_file << "" << endl;
            }
            cout << " " << endl;
        }

    }
    cout << "The hole training process took : " << total_duration / 1000.0 << "s" << endl;
    grads_log_file.close();
    weights_log_file.close();
    duration_log_file.close();
}

Ciphertext MLSP_PPLR::pp_sigmoid(Ciphertext c, int degree) {
    if (degree == 3) {
        Ciphertext cipher_sig = pp_sigmoid_deg3(c);
        return cipher_sig;
    } else if (degree == 5) {
        Ciphertext cipher_sig = pp_sigmoid_deg5(c);
        return cipher_sig;
    }
    Ciphertext cipher_sig = pp_sigmoid_deg7(c);
    return cipher_sig;
}

void MLSP_PPLR::pp_fit_local() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    string folder_name = oss.str();
    auto config_file_name = "CONFIG.txt";
    auto grads_file_name = "GRADS.txt";
    auto weights_file_name = "WEIGHTS.txt";
    auto duration_file_name = "DURATION.txt";
    auto loss_file_name = "LOSS.txt";

    auto path = "../LOGS/" + dataset_name + "/PPLR/LOCAL/" + folder_name + "/";

    ofstream config_log_file;
    ofstream grads_log_file;
    ofstream weights_log_file;
    ofstream duration_log_file;
    ofstream loss_log_file;

    if (mkdir(path.c_str(), 0777) == -1)
        cerr << "Error :  " << strerror(errno) << endl;
    else
        cout << "Directory created";

    grads_log_file.open(path + grads_file_name);
    weights_log_file.open(path + weights_file_name);
    config_log_file.open(path + config_file_name);
    duration_log_file.open(path + duration_file_name);
    loss_log_file.open(path + loss_file_name);


    config_log_file << " -------------------------- Dataset -----------------------------" << endl;
    config_log_file << "dataset = " << dataset_name << endl;
    config_log_file << "m = " << m << endl;
    config_log_file << "d = " << d << endl;
    config_log_file << "classes = " << class_number << endl;


    config_log_file << " -------------------------- Logistic Regression -----------------------------" << endl;
    config_log_file << "epochs = " << epochs << endl;
    config_log_file << "alpha = " << alpha << endl;
    config_log_file << "sigmoid_degree = " << sigmoid_degree << endl;

    config_log_file << " -------------------------- Crypto -----------------------------" << endl;
    config_log_file << "slots = " << n << endl;
    config_log_file << "nb_training_ciphers = " << nb_training_ciphers << endl;
    config_log_file << "logp = " << logp << endl;
    config_log_file << "logq = " << logq << endl;
    config_log_file << "logN = " << logN << endl;
    config_log_file << "logQ = " << logQ << endl;
    config_log_file << "nb_threads = " << numThread << endl;

    config_log_file.close();

    high_resolution_clock::time_point start_timer = high_resolution_clock::now();
    high_resolution_clock::time_point end_timer = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

    for (int e = 0; e < epochs; e++) {
        start_timer = high_resolution_clock::now();
        Ciphertext cipher_grad;
        complex<double> *encoded_grad = new complex<double>[n];
        for (int i = 0; i < n; i++) {
            complex<double> c;
            c.imag(0);
            c.real(0);
            encoded_grad[i] = c;
        }
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 4 * logp - sigmoid_degree * logp);

        for (int i = 0; i < nb_training_ciphers; i++) {
            Ciphertext cipher_product;

            scheme.mult(cipher_product, cipher_model, cipher_training_set[i]);   // TODO : modify
            scheme.reScaleByAndEqual(cipher_product, logp);

            Ciphertext cipher_dot_product = sum_slots(cipher_product, 0, log_nb_cols);

            scheme.multAndEqual(cipher_dot_product, cipher_gadget_matrix);

            scheme.reScaleByAndEqual(cipher_dot_product, logp);

            Ciphertext cipher_dot_product_duplicated = sum_slots_reversed(cipher_dot_product, 0, log_nb_cols);

            Ciphertext cipher_sig = pp_sigmoid(cipher_dot_product_duplicated, sigmoid_degree);

            scheme.multAndEqual(cipher_sig, cipher_training_set[i]); // TODO : modify
            scheme.reScaleByAndEqual(cipher_sig, logp);

            Ciphertext cipher_partial_grad = sum_slots(cipher_sig, log_nb_cols, log_nb_rows + log_nb_cols);
            scheme.addAndEqual(cipher_grad, cipher_partial_grad);
        }

        scheme.multByConstAndEqual(cipher_grad, alpha / m, logp);
        scheme.reScaleByAndEqual(cipher_grad, logp);            //TODO: factor out

        cout << "Gradient n : " << e << endl;

        Ciphertext refreshed_grad = refresh_cipher_local(cipher_grad);
        scheme.addAndEqual(cipher_model, refreshed_grad);

        end_timer = high_resolution_clock::now();
        auto epoch_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

        duration_log_file << to_string(epoch_duration) << endl;
        total_duration += epoch_duration;

        if (logging) {
            complex<double> * grad = scheme.decrypt(secretKey, cipher_grad);
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    grads_log_file << grad[j * nb_cols + k].real() << ", ";
                }
                grads_log_file << "" << endl;
            }
            cout << " " << endl;

            complex<double> * weights = scheme.decrypt(secretKey, cipher_model);

            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    weights_log_file << weights[j * nb_cols + k].real() << ", ";
                }
                weights_log_file << "" << endl;
            }
            cout << " " << endl;

            // Check the loss
            vector<double> w(d);
            for (int k = 0; k < d; k++) {
                w[k] = weights[k].real();
            }
            loss_log_file << loss(w) << endl;
        }

    }
    cout << "The hole training process took : " << total_duration / 1000.0 << "s" << endl;

    grads_log_file.close();
    weights_log_file.close();
    duration_log_file.close();
    loss_log_file.close();
}

double MLSP_PPLR::loss(vector <double> w) {
    double loss = 0.0;
    DatasetReader *datasetReader = new DatasetReader(datasets_path + dataset_name + "/", class_number, d); // Hum
    vector<Record*> train_records = datasetReader->read_all_train_records();
    for (int i = 0; i < m; i++) {
        Record* rcd = train_records[i];
        double wx = 0.0;
        for (int j = 0; j < d; j++) {
            wx += w[j] * rcd->values[j];
        }
        double h = LogisticRegression::sigmoid(wx);
        int y = rcd->label;
        loss += -y * log(h) - (1 - y) * (1 - h);
    }
    loss /= m;
    return loss;
}

void MLSP_PPLR::pp_fit_distributed() {

    string updated_model_filename =  "updated_model.txt";
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    string folder_name = oss.str();
    auto config_file_name = "CONFIG.txt";
    auto grads_file_name = "GRADS.txt";
    auto weights_file_name = "WEIGHTS.txt";
    auto duration_file_name = "DURATION.txt";

    auto path = "../LOGS/" + dataset_name + "/PPLR/DISTRIBUTED/" + folder_name + "/";

    ofstream config_log_file;
    ofstream grads_log_file;
    ofstream weights_log_file;
    ofstream duration_log_file;

    if (mkdir(path.c_str(), 0777) == -1)
        cerr << "Error :  " << strerror(errno) << endl;
    else
        cout << "Directory created";

    grads_log_file.open(path + grads_file_name);
    weights_log_file.open(path + weights_file_name);
    config_log_file.open(path + config_file_name);
    duration_log_file.open(path + duration_file_name);


    config_log_file << " -------------------------- Dataset -----------------------------" << endl;
    config_log_file << "dataset = " << dataset_name << endl;
    config_log_file << "m = " << m << endl;
    config_log_file << "d = " << d << endl;
    config_log_file << "classes = " << class_number << endl;


    config_log_file << " -------------------------- Logistic Regression -----------------------------" << endl;
    config_log_file << "epochs = " << epochs << endl;
    config_log_file << "alpha = " << alpha << endl;
    config_log_file << "sigmoid_degree = " << sigmoid_degree << endl;

    config_log_file << " -------------------------- Crypto -----------------------------" << endl;
    config_log_file << "slots = " << n << endl;
    config_log_file << "nb_training_ciphers = " << nb_training_ciphers << endl;
    config_log_file << "logp = " << logp << endl;
    config_log_file << "logq = " << logq << endl;
    config_log_file << "logN = " << logN << endl;
    config_log_file << "logQ = " << logQ << endl;
    config_log_file << "nb_threads = " << numThread << endl;


    config_log_file.close();

    high_resolution_clock::time_point start_timer = high_resolution_clock::now();
    high_resolution_clock::time_point end_timer = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

    for (int e = 0; e < epochs; e++) {
        start_timer = high_resolution_clock::now();
        Ciphertext cipher_grad;
        complex<double> *encoded_grad = new complex<double>[n];
        for (int i = 0; i < n; i++) {
            complex<double> c;
            c.imag(0);
            c.real(0);
            encoded_grad[i] = c;
        }
        scheme.encrypt(cipher_grad, encoded_grad, n, logp, logq - 4 * logp - sigmoid_degree * logp);
        cout << "Start waiting for the gradients" << endl;

        for (int i = 0; i < nb_workers; i++) {
            cout << "Waiting for grad " << i << endl;

            string grad_filename = "worker" + to_string(i + 1) + "_grad.txt";

            if (read_file(workers_sockets[i], (char *) grad_filename.c_str())) {
                cout << "Got grad " << i << " from socket " << workers_sockets[i] << endl;
            } else {
                cout << "Error while reading grad " << i << " from socket " << workers_sockets[i] << endl;
            }
        }

        for (int i = 0; i < nb_workers; i++) {
            string grad_filename = "worker" + to_string(i + 1) + "_grad.txt";
            Ciphertext *cipher_partial_grad = SerializationUtils::readCiphertext((char *) grad_filename.c_str());
            scheme.addAndEqual(cipher_grad, *cipher_partial_grad);
        }

        scheme.multByConstAndEqual(cipher_grad, alpha /  m, logp);
        scheme.reScaleByAndEqual(cipher_grad, logp);            //TODO: factor out

        cout << "Gradient n : " << e << endl;

        Ciphertext refreshed_grad = refresh_cipher(cipher_grad);
        scheme.addAndEqual(cipher_model, refreshed_grad);

        end_timer = high_resolution_clock::now();
        auto epoch_duration = duration_cast<milliseconds>(end_timer - start_timer).count();

        duration_log_file << to_string(epoch_duration) << endl;
        total_duration += epoch_duration;

        SerializationUtils::writeCiphertext(cipher_model, (char *) updated_model_filename.c_str());


        for (int i = 0; i < nb_workers; i++) {
            if (send_file(workers_sockets[i], (char *) updated_model_filename.c_str())) {
                cout << "Sent the model" << endl;
            } else {
                cout << "ERROR while sending the cipher" << endl;
            }
        }

        if (logging) {
            complex<double> * model = scheme.decrypt(secretKey, cipher_model);
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < d; k++) {
                    cout << model[j * nb_cols + k].real() << ", ";
                    weights_log_file << model[j * nb_cols + k].real() << ", ";
                }
                weights_log_file << "" << endl;
            }
            cout << " " << endl;
        }
    }

    for (int i = 0; i < nb_workers; i++) {
        string grad_filename = "worker" + to_string(i + 1) + "_grad.txt";
        if (remove(grad_filename.c_str()) != 0)
            perror("Error deleting file");
        else
            puts("File successfully deleted");
    }

    if (remove(updated_model_filename.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    cout << "The hole training process took : " << total_duration / 1000.0 << "s" << endl;

    weights_log_file.close();
    grads_log_file.close();
    duration_log_file.close();
}

Ciphertext MLSP_PPLR::refresh_cipher_local(Ciphertext c) {
    complex<double> * randomness = EvaluatorUtils::randomComplexArray(n);
    Ciphertext encrypted_randomness;
    Ciphertext encrypted_randomness_down;
    scheme.encrypt(encrypted_randomness, randomness, n, logp, logq);
    scheme.modDownBy(encrypted_randomness_down, encrypted_randomness, logq - c.logq);

    Ciphertext blinded_cipher;
    scheme.add(blinded_cipher, c, encrypted_randomness_down);

    complex<double> * blinded_plaintext = scheme.decrypt(secretKey, blinded_cipher);
    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, blinded_plaintext, n, logp, logq);

    scheme.subAndEqual(refreshed_c, encrypted_randomness);
    return refreshed_c;
}

Ciphertext MLSP_PPLR::refresh_cipher_unsecure_old(Ciphertext c) {
    SerializationUtils::writeCiphertext(c, "cipher_to_refresh.txt");
    if (send_file(csp_sock, "cipher_to_refresh.txt")) {
        cout << "Sent the cipheer" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }
    read_file(csp_sock, "refreshed_cipher.txt");
    Ciphertext* refreshed_c = SerializationUtils::readCiphertext("refreshed_cipher.txt");

    /*if (remove("cipher_to_refresh.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    if (remove("refreshed_cipher.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");*/

    return *refreshed_c;
}

Ciphertext MLSP_PPLR::refresh_cipher_unsecure(Ciphertext c) {
    SerializationUtils::writeCiphertext(c, "cipher_to_refresh.txt");
    if (send_file(csp_sock, "cipher_to_refresh.txt")) {
        cout << "Sent the cipheer" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }
    read_file(csp_sock, "refreshed_cipher.txt");
    Ciphertext* cipher_to_refresh = SerializationUtils::readCiphertext("refreshed_cipher.txt");

    complex<double> * plaintext = scheme.decrypt(secretKey, *cipher_to_refresh);

    cout << "Plaintext value of the cipher to refresh:" << endl;
    for (int i = 0; i < d; ++i) {
        cout << plaintext[i] << ' ';
    }
    cout << " " << endl;

    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, plaintext, n, logp, logq);

    if (remove("cipher_to_refresh.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    if (remove("refreshed_cipher.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    return refreshed_c;
}

Ciphertext* MLSP_PPLR::refresh_cipher_unsecure_modified(Ciphertext c) {
    SerializationUtils::writeCiphertext(c, "cipher_to_refresh.txt");
    if (send_file(csp_sock, "cipher_to_refresh.txt")) {
        cout << "Sent the cipheer" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }
    read_file(csp_sock, "refreshed_cipher.txt");
    Ciphertext* refreshed_c = SerializationUtils::readCiphertext("refreshed_cipher.txt");

    if (remove("cipher_to_refresh.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    if (remove("refreshed_cipher.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    return refreshed_c;
}


void MLSP_PPLR::test_refresh_cipher_unsecure() {
    Ciphertext test_cipher;
    complex<double> *mvec1 = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec1[i] = c;
    }
    scheme.encrypt(test_cipher, mvec1, n, logp, logq);
    Ciphertext refreshed_c = refresh_cipher_unsecure(test_cipher);
}


Ciphertext MLSP_PPLR::refresh_cipher_old(Ciphertext c) {
    complex<double> * randomness = EvaluatorUtils::randomComplexArray(n);
    Ciphertext encrypted_randomness;
    Ciphertext encrypted_randomness_down;
    scheme.encrypt(encrypted_randomness, randomness, n, logp, logq);
    scheme.modDownBy(encrypted_randomness_down, encrypted_randomness, logq - c.logq);

    Ciphertext blinded_cipher;
    scheme.add(blinded_cipher, c, encrypted_randomness_down);

    SerializationUtils::writeCiphertext(blinded_cipher, "cipher_to_refresh.txt");
    if (send_file(csp_sock, "cipher_to_refresh.txt")) {
        cout << "Sent the blinded cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    read_file(csp_sock, "refreshed_cipher.txt");
    Ciphertext* refreshed_blinded_c = SerializationUtils::readCiphertext("refreshed_cipher.txt");

    Ciphertext refreshed_c;
    scheme.sub(refreshed_c, *refreshed_blinded_c, encrypted_randomness);

    return refreshed_c;
}


Ciphertext MLSP_PPLR::refresh_cipher(Ciphertext c) {
    complex<double> * randomness = EvaluatorUtils::randomComplexArray(n);
    Ciphertext encrypted_randomness;
    Ciphertext encrypted_randomness_down;
    scheme.encrypt(encrypted_randomness, randomness, n, logp, logq);
    scheme.modDownBy(encrypted_randomness_down, encrypted_randomness, logq - c.logq);

    Ciphertext blinded_cipher;
    scheme.add(blinded_cipher, c, encrypted_randomness_down);

    SerializationUtils::writeCiphertext(blinded_cipher, "cipher_to_refresh.txt");
    if (send_file(csp_sock, "cipher_to_refresh.txt")) {
        cout << "Sent the blinded cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }

    read_file(csp_sock, "refreshed_cipher.txt");
    Ciphertext* not_refreshed_blinded_c = SerializationUtils::readCiphertext("refreshed_cipher.txt");

    complex<double> * plaintext = scheme.decrypt(secretKey, *not_refreshed_blinded_c);

    Ciphertext refreshed_c;
    scheme.encrypt(refreshed_c, plaintext, n, logp, logq);
    scheme.subAndEqual(refreshed_c, encrypted_randomness);

    if (remove("cipher_to_refresh.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    if (remove("refreshed_cipher.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    return refreshed_c;
}


void MLSP_PPLR::test_refresh_cipher() {
    Ciphertext test_cipher;
    complex<double> *mvec1 = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec1[i] = c;
    }
    scheme.encrypt(test_cipher, mvec1, n, logp, logq);
    Ciphertext refreshed_c = refresh_cipher(test_cipher);
    SerializationUtils::writeCiphertext(refreshed_c, "check_cipher.txt");

    if (send_file(csp_sock, "check_cipher.txt")) {
        cout << "Sent the check cipher" << endl;
    }
    else {
        cout << "ERROR, could not send the cipher" << endl;
    }
}


Ciphertext MLSP_PPLR::sum_slots(Ciphertext c, int start_slot, int end_slot) {
    Ciphertext cipher_sum = c;
    for (int i = start_slot; i < end_slot; i++) {
        Ciphertext cipher_rot;
        //cout << "Rotating by : " << (int) pow(2, i) << endl;
        scheme.leftRotateFast(cipher_rot, cipher_sum, (int) pow(2, i));/// TODO: Consider iterative update
        //complex<double> * decrypted_sum = scheme.decrypt(secretKey, cipher_sum);
        scheme.addAndEqual(cipher_sum, cipher_rot);
    }
    return cipher_sum;
}

Ciphertext MLSP_PPLR::sum_slots_reversed(Ciphertext c, int start_slot, int end_slot) {
    Ciphertext cipher_sum = c;
    for (int i = start_slot; i < end_slot; i++) {
        Ciphertext cipher_rot;
        //cout << "Rotating by : " << (int) pow(2, i) << endl;
        scheme.rightRotateFast(cipher_rot, cipher_sum, (int) pow(2, i));   // TODO: Consider iterative update
        //complex<double> * decrypted_sum = scheme.decrypt(secretKey, cipher_sum);
        scheme.addAndEqual(cipher_sum, cipher_rot);
    }
    return cipher_sum;
}


void MLSP_PPLR::test_sum_slots() {
    complex<double> *encoded_x;
    vector<double> x{1, 1, 1, 1};
    Ciphertext cipher_x;
    long nb = x.size();
    encoded_x = new complex<double>[n];

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_x[i] = c;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < nb_cols; j++) {
            complex<double> c;
            c.imag(0);
            c.real(i);
            encoded_x[i * nb_cols + j] = c;
        }
    }

    scheme.encrypt(cipher_x, encoded_x, n, logp, logq);

    Ciphertext cipher_sum = sum_slots(cipher_x, 0, log_nb_cols);
    scheme.multAndEqual(cipher_sum, cipher_gadget_matrix);
    scheme.reScaleByAndEqual(cipher_sum, logp);

    Ciphertext duplicated = sum_slots_reversed(cipher_sum, 0, log_nb_cols);

    complex<double> * decrypted_sum = scheme.decrypt(secretKey, duplicated);
    cout << " " << endl;

    cout << "Decrypted sums:" << endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < nb_cols; j++) {
            cout << decrypted_sum[i * nb_cols + j] << ", ";
        }
        cout << " " << endl;
    }
    cout << " " << endl;
}

Ciphertext MLSP_PPLR::pp_sigmoid_deg3(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;

    //complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);


    scheme.mult(cipher_x_cube, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);


    scheme.mult(cipher_x_cube, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg3[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    scheme.multByConst(cipher_sig, cipher_x_cube, sigmoid_coeffs_deg3[2], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg3[0], logp);

    //eussou = scheme.decrypt(secretKey, cipher_sig);

    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);



    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, 2 * logp);

    scheme.add(cipher_result, cipher_sig, cipher_ax);

    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << endl;

    return cipher_result;
}


Ciphertext MLSP_PPLR::pp_sigmoid_deg5(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_square;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;
    Ciphertext cipher_x_four;
    Ciphertext cipher_x_five;

    //complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);


    scheme.mult(cipher_square, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_square, logp);


    scheme.mult(cipher_x_cube, cipher_square, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    /*cout << " CUBE " << endl;
    complex<double> *decrypted_x_cube = scheme.decrypt(secretKey, cipher_x_cube);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_cube[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.mult(cipher_x_four, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_four, logp);

    /*cout << " FOUR " << endl;
    complex<double> *decrypted_x_four = scheme.decrypt(secretKey, cipher_x_four);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_four[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.mult(cipher_x_five, cipher_x_four, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    /*cout << " FIVE " << endl;
    complex<double> *decrypted_x_five = scheme.decrypt(secretKey, cipher_x_five);
    for (int i = 0; i < n; i++) {
        cout << decrypted_x_five[i] << ", ";
    }
    cout << " " << endl;*/

    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg5[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    scheme.multByConst(cipher_ax_cube, cipher_x_cube, sigmoid_coeffs_deg5[2], logp);
    scheme.reScaleByAndEqual(cipher_ax_cube, logp);

    scheme.multByConst(cipher_sig, cipher_x_five, sigmoid_coeffs_deg5[3], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg5[0], logp);

    //eussou = scheme.decrypt(secretKey, cipher_sig);

    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);



    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, cipher_ax.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_cube, cipher_ax_cube.logq - cipher_sig.logq);
    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << " Cipher AX CUBE : " << cipher_ax_cube.logq << endl;

    scheme.add(cipher_result, cipher_sig, cipher_ax_cube);
    scheme.addAndEqual(cipher_result, cipher_ax);


    return cipher_result;
}

Ciphertext MLSP_PPLR::pp_sigmoid_deg7(Ciphertext cipher_x) {
    Ciphertext cipher_sig;
    Ciphertext cipher_square;
    Ciphertext cipher_x_cube;
    Ciphertext cipher_x_five;
    Ciphertext cipher_x_seven;
    Ciphertext cipher_ax;
    Ciphertext cipher_ax_cube;
    Ciphertext cipher_ax_five;

    //complex<double> *eussou = scheme.decrypt(secretKey, cipher_x);

    scheme.multByConstAndEqual(cipher_x, 0.125, logp);
    scheme.reScaleByAndEqual(cipher_x, logp);

    scheme.mult(cipher_square, cipher_x, cipher_x);
    scheme.reScaleByAndEqual(cipher_square, logp);


    scheme.mult(cipher_x_cube, cipher_square, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_cube, logp);

    scheme.mult(cipher_x_five, cipher_x_cube, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    scheme.multAndEqual(cipher_x_five, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_five, logp);

    scheme.mult(cipher_x_seven, cipher_x_five, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_seven, logp);

    scheme.multAndEqual(cipher_x_seven, cipher_x);
    scheme.reScaleByAndEqual(cipher_x_seven, logp);

    scheme.multByConst(cipher_ax, cipher_x, sigmoid_coeffs_deg7[1], logp);
    scheme.reScaleByAndEqual(cipher_ax, logp);

    scheme.multByConst(cipher_ax_cube, cipher_x_cube, sigmoid_coeffs_deg7[2], logp);
    scheme.reScaleByAndEqual(cipher_ax_cube, logp);

    scheme.multByConst(cipher_ax_five, cipher_x_five, sigmoid_coeffs_deg7[3], logp);
    scheme.reScaleByAndEqual(cipher_ax_five, logp);

    scheme.multByConst(cipher_sig, cipher_x_seven, sigmoid_coeffs_deg7[4], logp);
    scheme.reScaleByAndEqual(cipher_sig, logp);

    scheme.addConstAndEqual(cipher_sig, sigmoid_coeffs_deg7[0], logp);

    //eussou = scheme.decrypt(secretKey, cipher_sig);

    //scheme.encrypt(cipher_sig, eussou, n, logp, logq);

    Ciphertext cipher_result;
    scheme.modDownByAndEqual(cipher_ax, cipher_ax.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_cube, cipher_ax_cube.logq - cipher_sig.logq);
    scheme.modDownByAndEqual(cipher_ax_five, cipher_ax_five.logq - cipher_sig.logq);

    scheme.add(cipher_result, cipher_sig, cipher_ax_five);
    scheme.addAndEqual(cipher_result, cipher_ax_cube);
    scheme.addAndEqual(cipher_result, cipher_ax);

    cout << "Cipher Sig : " << cipher_sig.logq << " Cipher AX : " << cipher_ax.logq << endl;

    return cipher_result;
}


double MLSP_PPLR::approx_sigmoid_deg3(double x) {
    x = x / 8.0;
    return sigmoid_coeffs_deg3[0] + sigmoid_coeffs_deg3[1] * x + sigmoid_coeffs_deg3[2] * x * x * x;
}

void MLSP_PPLR::test_pp_sigmoid(vector<double> x) {
    complex<double> *encoded_x;
    Ciphertext cipher_x;
    long nb = x.size();
    encoded_x = new complex<double>[n];

    for (int i = 0; i < n; i++) {
        complex<double> c;
        c.imag(0);
        c.real(0);
        encoded_x[i] = c;
    }

    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(x[i]);
        encoded_x[i] = c;
    }

    scheme.encrypt(cipher_x, encoded_x, n, logp, logq);
    Ciphertext cipher_sig = pp_sigmoid(cipher_x, 7);

    complex<double> *decrypted_encoded_sigmoids = scheme.decrypt(secretKey, cipher_sig);
    vector<double> decrypted_sig(nb);
    cout << "Values after decryption: " << endl;
    for (int i = 0; i < nb; i++) {
        decrypted_sig[i] = decrypted_encoded_sigmoids[i].real();
        cout << decrypted_sig[i] << ", ";
    }
    cout << " " << endl;

    cout << "Original Sigmoid Values" << endl;
    for (int i = 0; i < nb; i++) {
        double plain_sig = LogisticRegression::sigmoid(x[i]);
        cout << plain_sig << ", ";
    }
    cout << " " << endl;
    cout << "Plaintext Sigmoid Approx" << endl;
    for (int i = 0; i < nb; i++) {
        double approx_sig = approx_sigmoid_deg3(x[i]);
        cout << approx_sig << ", ";
    }
}

Ciphertext MLSP_PPLR::pp_dot_product(Ciphertext cx, Ciphertext cy) {
    Ciphertext cipher_prod;
    scheme.mult(cipher_prod, cx, cy);
    scheme.reScaleByAndEqual(cipher_prod, logp);

    complex<double> *decrypted_product = scheme.decrypt(secretKey, cipher_prod);
    for (int i = 0; i < n; i++) {
        cout << decrypted_product[i] << ", ";
    }
    cout << " " << endl;

    Ciphertext dot_product = sum_slots(cipher_prod, 0, 10);

    complex<double> *decrypted_dot_product = scheme.decrypt(secretKey, dot_product);
    for (int i = 0; i < n; i++) {
        cout << decrypted_dot_product[i] << ", ";
    }
    cout << " " << endl;
    return dot_product;
}


void MLSP_PPLR::test_pp_dot_product(vector<double> x, vector<double> y) {
    Ciphertext cx, cy;
    complex<double> *encoded_x, *encoded_y;

    long nb = x.size();
    encoded_x = new complex<double>[n];
    encoded_y = new complex<double>[n];
    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(x[i]);
        encoded_x[i] = c;
    }

    for (int i = 0; i < nb; i++) {
        complex<double> c;
        c.imag(0);
        c.real(y[i]);
        encoded_y[i] = c;
    }

    scheme.encrypt(cx, encoded_x, n, logp, logq);
    scheme.encrypt(cy, encoded_y, n, logp, logq);

    Ciphertext cipher_dot_product = pp_dot_product(cx, cy);

    complex<double> *decrypted_dot_product = scheme.decrypt(secretKey, cipher_dot_product);
    for (int i = 0; i < n; i++) {
        cout << decrypted_dot_product[i] << ", ";
    }
    cout << " " << endl;
}

void MLSP_PPLR::send_dtpkc_cipher(DTPKC::Cipher dtpkc_value) {
    auto string_T1 = dtpkc_value.T1.get_str();
    auto string_T2 = dtpkc_value.T2.get_str();

    string comma = ",";

    string str_cipher = string_T1 + comma + string_T2 + comma;

    //cout << str_cipher << endl;

    //unsigned char *buf = new unsigned char[str_cipher.length() + 1];
    unsigned char *buf = new unsigned char[2048];
    //cout << "Size : " << str_cipher.length() + 1 << endl;
    //memset(buf, 0, (str_cipher.length() + 1) * sizeof(unsigned char));
    memset(buf, 0, 2048);

    strcpy((char *)buf , str_cipher.c_str());

    cout << buf << endl;

    if (send_data(csp_sock, buf, 2048)) {
        cout << "Sent the DTPKC Cipher" << endl;
    } else {
        cout << "Error while sending the issou" << endl;
    }
    //const int length = string_T1.length() + string_T2.length() + 2;
}

void MLSP_PPLR::test_send_dtpkc_cipher() {
    long val = 2;
    mpz_class mpz_value;

    mpz_value.set_str(std::to_string(val), 10);
    DTPKC::Cipher dtpkc_value = dtpkc.enc(mpz_value, dtpkc.pkw);
    send_dtpkc_cipher(dtpkc_value);
}

Ciphertext MLSP_PPLR::cryptosystem_switching_single(DTPKC::Cipher dtpkc_value) {
    DTPKC::Cipher dtpkc_randomness;
    DTPKC::Cipher dtpkc_value_blinded;

    long randomness = rand();
    double rescaled_randomness = (double) randomness / dtpkc_scale_factor;

    mpz_class mpz_randomness;
    mpz_randomness.set_str(std::to_string(randomness), 10);
    mpz_class mpz_value;

    mpz_randomness.set_str(std::to_string(randomness), 10);
    dtpkc_randomness = dtpkc.enc(mpz_randomness, dtpkc_pkey);

    EvalAdd add(dtpkc_value, dtpkc_randomness);

    dtpkc_value_blinded = add.EvalAdd_U1();

    send_dtpkc_cipher(dtpkc_value_blinded);

    read_file(csp_sock, "blinded_cipher.txt");
    Ciphertext* fhe_blinded_cipher = SerializationUtils::readCiphertext("blinded_cipher.txt");

    Ciphertext fhe_value, fhe_rescaled_randomness;
    complex<double> *encoded_randomness = new complex<double>[n];

    {
        complex<double> c;
        c.real(rescaled_randomness);
        c.imag(0);
        encoded_randomness[0] = c;
    }

    for (int i = 1; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
    }

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);
    cout << " " << endl;
    scheme.sub(fhe_value, *fhe_blinded_cipher, fhe_rescaled_randomness);

    return fhe_value;
}

void MLSP_PPLR::test_cryptosystem_switching_single() {
    DTPKC::Cipher dtpkc_value;

    double val = 1.11111;
    long val_scaled = (long) (val * dtpkc_scale_factor);
    mpz_class mpz_value;
    mpz_value.set_str(std::to_string(val_scaled), 10);
    dtpkc_value = dtpkc.enc(mpz_value, dtpkc_pkey);

    chrono::high_resolution_clock::time_point start_key_exchange = chrono::high_resolution_clock::now();

    Ciphertext fhe_value = cryptosystem_switching_single(dtpkc_value);

    SerializationUtils::writeCiphertext(fhe_value, "fhe_cipher.txt");
    if (send_file(csp_sock, "fhe_cipher.txt")) {
        cout << "Sent the fhe cipher" << endl;
    } else {
        cout << "Error while sending the fhe cipher" << endl;
    }
}

Ciphertext MLSP_PPLR::cryptosystem_switching_batch_naive(vector<DTPKC::Cipher> dtpkc_values) {
    long nb_vals = dtpkc_values.size();

    Ciphertext fhe_value, fhe_rescaled_randomness;
    complex<double> *encoded_randomness = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
    }

    for (int i = 0; i < nb_vals; i++) {
        DTPKC::Cipher dtpkc_randomness;
        DTPKC::Cipher dtpkc_value = dtpkc_values[i];
        DTPKC::Cipher dtpkc_value_blinded;

        long randomness = rand();
        mpz_class mpz_randomness;
        mpz_randomness.set_str(std::to_string(randomness), 10);
        mpz_class mpz_value;


        mpz_randomness.set_str(std::to_string(randomness), 10);
        dtpkc_randomness = dtpkc.enc(mpz_randomness, dtpkc_pkey);

        EvalAdd add(dtpkc_value, dtpkc_randomness);

        dtpkc_value_blinded = add.EvalAdd_U1();
        double rescaled_randomness = (double) randomness / dtpkc_scale_factor;

        {
            complex<double> c;
            c.real(rescaled_randomness);
            c.imag(0);
            encoded_randomness[i] = c;
        }

        send_dtpkc_cipher(dtpkc_value_blinded);
    }

    read_file(csp_sock, "blinded_cipher.txt");
    Ciphertext* fhe_blinded_cipher = SerializationUtils::readCiphertext("blinded_cipher.txt");

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);
    scheme.sub(fhe_value, *fhe_blinded_cipher, fhe_rescaled_randomness);

    return fhe_value;
}


void MLSP_PPLR::test_cryptosystem_switching_batch_naive() {
    vector<double> values{1.5, 1.411, 1.1111, 2.55555};
    int nb_values = values.size();
    vector<long> scaled_values(nb_values);

    for (int i = 0; i < nb_values; i++) {
        scaled_values[i] = (long) (values[i] * dtpkc_scale_factor);
    }

    vector<DTPKC::Cipher> dtpkc_values;
    for (int i = 0; i < nb_values; i++) {
        long scaled_value = scaled_values[i];
        mpz_class mpz_value;
        mpz_value.set_str(std::to_string(scaled_value), 10);
        DTPKC::Cipher dtpkc_value = dtpkc.enc(mpz_value, dtpkc_pkey);
        dtpkc_values.push_back(dtpkc_value);
    }

    Ciphertext fhe_value = cryptosystem_switching_batch_naive(dtpkc_values);

    SerializationUtils::writeCiphertext(fhe_value, "fhe_cipher.txt");
    if (send_file(csp_sock, "fhe_cipher.txt")) {
        cout << "Sent the fhe cipher" << endl;
    } else {
        cout << "Error while sending the fhe cipher" << endl;
    }
}


Ciphertext MLSP_PPLR::cryptosystem_switching_batch_optimized(vector<DTPKC::Cipher> dtpkc_values) {
    long nb_vals = dtpkc_values.size();
    int dtpkc_nb_slots = dtpkc.nb_slots;

    Ciphertext fhe_value, fhe_rescaled_randomness;
    complex<double> *encoded_randomness = new complex<double>[n];

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(0);
        c.imag(0);
        encoded_randomness[i] = c;
    }

    for (int i = 0; i < nb_vals; i++) {
        DTPKC::Cipher dtpkc_randomness;
        DTPKC::Cipher dtpkc_value = dtpkc_values[i];
        DTPKC::Cipher dtpkc_value_blinded;

        vector<long> randomness_vec(dtpkc_nb_slots);
        vector<double> rescaled_randomness_vec(dtpkc_nb_slots);
        for (int j = 0; j < dtpkc_nb_slots; j++) {
            randomness_vec[j] = rand();
            rescaled_randomness_vec[j] = ((double) randomness_vec[j]) / dtpkc_scale_factor;
        }

        dtpkc_randomness = dtpkc.enc_batch(randomness_vec, dtpkc_pkey);

        mpz_class mpz_value;

        EvalAdd add(dtpkc_value, dtpkc_randomness);

        dtpkc_value_blinded = add.EvalAdd_U1();



        for (int j = 0; j < dtpkc_nb_slots; j++) {
            {
                complex<double> c;
                c.real(rescaled_randomness_vec[j]);
                c.imag(0);
                encoded_randomness[i * dtpkc_nb_slots + j] = c;
            }
        }
        send_dtpkc_cipher(dtpkc_value_blinded);
    }

    read_file(csp_sock, "blinded_cipher.txt");
    Ciphertext* fhe_blinded_cipher = SerializationUtils::readCiphertext("blinded_cipher.txt");

    scheme.encrypt(fhe_rescaled_randomness, encoded_randomness, n, logp, logq);
    scheme.sub(fhe_value, *fhe_blinded_cipher, fhe_rescaled_randomness);

    return fhe_value;
}


void MLSP_PPLR::test_cryptosystem_switching_batch_optimized() {
    vector<double> values{1.5, 1.411, 1.1111, 2.55555, 2.66666, 4, 2.3333, 6.66666, 8.9999999};
    int nb_values = values.size();
    int nb_ciphers = 1;
    vector<long> scaled_values(nb_values);
    int dtpkc_nb_slots = dtpkc.nb_slots;

    for (int i = 0; i < nb_values; i++) {
        scaled_values[i] = (long) (values[i] * dtpkc_scale_factor);
    }

    vector<DTPKC::Cipher> dtpkc_values;
    for (int i = 0; i < nb_ciphers; i++) {
        vector<long> scaled_slot(dtpkc_nb_slots);
        for (int j = 0; j < dtpkc_nb_slots; j++) {
            scaled_slot[j] = scaled_values[i * dtpkc_nb_slots + j];
        }
        DTPKC::Cipher dtpkc_value = dtpkc.enc_batch(scaled_slot, dtpkc_pkey);
        dtpkc_values.push_back(dtpkc_value);
    }

    Ciphertext fhe_value = cryptosystem_switching_batch_optimized(dtpkc_values);

    SerializationUtils::writeCiphertext(fhe_value, "fhe_cipher.txt");
    if (send_file(csp_sock, "fhe_cipher.txt")) {
        cout << "Sent the fhe cipher" << endl;
    } else {
        cout << "Error while sending the fhe cipher" << endl;
    }

}