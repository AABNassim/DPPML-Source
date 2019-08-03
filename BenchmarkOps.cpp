//
// Created by Nassim AAB on 2019-08-01.
//

#include "BenchmarkOps.h"

BenchmarkOps::BenchmarkOps() {
    auto y = std::time(nullptr);
    vector<string> func_names = {"encryption", "decryption", "addition",
                                 "batch_encryption", "batch_addition", "batch_decryption"};
    long t;

    auto tm = *std::localtime(&y);

    std::ostringstream oss;

    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 2;


    int l = 4;

    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    string folder_name = oss.str();
    auto dtpkc_file_name = "dtpkc.txt";
    auto fhe_file_name = "fhe.txt";
    auto config_file_name = "config.txt";

    BENCHBuildingBlocks benchBuildingBlocks;
    int keysizes[1] = {1024};
    int length = 1;
    string separator = ";";
    mpz_class pkey = benchBuildingBlocks.dtpkc.pkw;
    mpz_class skey;

    std::vector<long> v;
    int nb_slots = benchBuildingBlocks.dtpkc.nb_slots;
    int slot_size = benchBuildingBlocks.dtpkc.digits_per_slot;

    auto path = "../LOGS/MICRO_BENCH/" + folder_name + "/";

    ofstream dtpkc_log_file;
    ofstream fhe_log_file;
    ofstream config_log_file;

    if (mkdir(path.c_str(), 0777) == -1)
        cerr << "Error :  " << strerror(errno) << endl;
    else
        cout << "Directory created";

    cout << "Creating " << path + dtpkc_file_name << endl;
    dtpkc_log_file.open(path + dtpkc_file_name);
    fhe_log_file.open(path + fhe_file_name);
    config_log_file.open(path + config_file_name);


    config_log_file << " -------------------------- FHE -----------------------------" << endl;
    config_log_file << "slots = " << n << endl;
    config_log_file << "logp = " << logp << endl;
    config_log_file << "logq = " << logq << endl;
    config_log_file << "logN = " << logN << endl;
    config_log_file << "logQ = " << logQ << endl;

    config_log_file << " -------------------------- DTPKC -----------------------------" << endl;
    config_log_file << "key_size = " << 1024 << endl;
    config_log_file << "nb_slots" << nb_slots << endl;
    config_log_file << "slot_size" << slot_size << endl;

    config_log_file.close();


    dtpkc_log_file << "encryption, addition, decryption, batch_encryption, batch_addition, batch_decryption" << endl;
    fhe_log_file << "encryption, cst_addition, addition, subtraction, cst_multiplication, multiplication, left_rotation, right_rotation, decryption" << endl;



    for (int i = 0; i < nb_slots; ++i) {
        v.push_back(i * 111111111);
    }

    benchBuildingBlocks.set_iterations(10);
    benchBuildingBlocks.set_operands(10000000, 10);
    //file.open(file_name, std::ios_base::app);
    for (int i = 0; i < length; i++) {
        int keysize = keysizes[i];
        cout << "Key size : " << keysize << " bits." << endl;
        benchBuildingBlocks.set_keysize(keysize);
        benchBuildingBlocks.dtpkc.getKey(pkey, skey);
        for (auto const &func_name: func_names) {
            if (func_name == "encryption") {
                t = benchBuildingBlocks.bench_encryption();
            } else if (func_name == "decryption") {
                t = benchBuildingBlocks.bench_decryption();
            } else if (func_name == "partial_decryption0") {
                t = benchBuildingBlocks.bench_partial_decryption0();
            } else if (func_name == "partial_decryption1") {
                t = benchBuildingBlocks.bench_partial_decryption1();
            } else if (func_name == "comparison") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_comparison();
            } else if (func_name == "addition") {
                t = benchBuildingBlocks.bench_addition();
            } else if (func_name == "subtraction") {
                t = benchBuildingBlocks.bench_subtraction();
            } else if (func_name == "multiplication") {
                //cout << "issou" << endl;
                t = benchBuildingBlocks.bench_multiplication();
            } else if (func_name == "division") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_division();
            } else if (func_name == "keyswitch") {
                t = benchBuildingBlocks.bench_keyswitch();
            } else if (func_name == "hbound") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_hbound();
            } else if (func_name == "entropy") {
                cout << "issou" << endl;
                //t = benchBuildingBlocks.bench_entropy();
            } else if (func_name == "batch_encryption") {
                t = benchBuildingBlocks.bench_batch_encryption(v);
            } else if (func_name == "batch_decryption") {
                t = benchBuildingBlocks.bench_batch_decryption();
            } else if (func_name == "batch_addition") {
                t = benchBuildingBlocks.bench_batch_addition();
            } else if (func_name == "batch_encryption_partial_key") {
                t = benchBuildingBlocks.bench_batch_encryption_partial_key(v, pkey);
            } else if (func_name == "batch_decryption_partial_key") {
                t = benchBuildingBlocks.bench_batch_decryption_partial_key(skey);
            }
            dtpkc_log_file << t << ", ";
            cout << func_name << " took: " << t << "us." << endl;
            //time_size[keysize] = t;
            //file << func_name + separator + to_string(keysize) + separator + to_string(t) + "\n";
        }
        cout << "---------------------------------------------------------------------------------" << endl;
        //benchBuildingBlocks.dtpkc.serializeDtpkc();
    }



    // Construct and Generate Public Keys //
    srand(time(NULL));
    SetNumThreads(numThread);
    TimeUtils timeutils;
    Ring ring;
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);

    complex<double> *mvec1 = new complex<double>[n];
    complex<double> *mvec2 = new complex<double>[n];
    complex<double> *mvec3 = new complex<double>[n];

    double cst = 3;

    for (int i = 0; i < n; ++i) {
        complex<double> c;
        c.real(i);
        c.imag(0);
        mvec1[i] = c;
        mvec2[i] = c;
        mvec3[i] = c;
    }

    Ciphertext cipher1;
    Ciphertext cipher2;

    high_resolution_clock::time_point start = high_resolution_clock::now();
    scheme.encrypt(cipher1, mvec1, n, logp, logq);
    scheme.encrypt(cipher2, mvec2, n, logp, logq);
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    cout << "FHE encryption duration: " << duration / (1000 * 2) << "ms." << endl;

    auto encryption_duration = duration / 2.0;

    SerializationUtils serializationUtils;

    start = high_resolution_clock::now();

    serializationUtils.writeCiphertext(cipher1, "./cipher.txt");
    Ciphertext* cipher_deserialized = serializationUtils.readCiphertext("./cipher.txt");

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();

    cout << "Ser and Deser took: " << duration / 1000 << "ms." << endl;

    if (remove("./cipher.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    complex<double> *decrypted_deserialized = scheme.decrypt(secretKey, *cipher_deserialized);


    cout << "Deserialization result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << decrypted_deserialized[i] << ' ';
    }
    cout << " " << endl;


    Ciphertext cipherAdd;
    scheme.encrypt(cipherAdd, mvec2, n, logp, logq);

    start = high_resolution_clock::now();
    int k = 100;
    int iter = 2;

    for (int i = 0; i < k; i++) {
        scheme.add(cipherAdd, cipher1, cipher2);
    }

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE addition duration: " << duration / (1000 * k) << "ms." << endl;

    auto addition_duration = duration / k;

    Ciphertext cipherConstAdd;
    start = high_resolution_clock::now();

    for (int i = 0; i < k; i++) {
        scheme.addConst(cipherConstAdd, cipher1, cst, logp);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Constant addition duration: " << duration / (1000 * k) << "ms." << endl;

    auto cst_addition_duration = duration / k;

    start = high_resolution_clock::now();
    Ciphertext cipherSub;
    for (int i = 0; i < k; i++) {
        scheme.sub(cipherSub, cipher1, cipher2);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    auto subtraction_duration = duration / k;

    Ciphertext cipherConstMul;
    scheme.encrypt(cipherAdd, mvec2, n, logp, logq);

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.multByConst(cipherConstMul, cipher1, cst, logp);
        scheme.reScaleByAndEqual(cipherConstMul, logp);
    }

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE const multiplication duration: " << duration / (1000 * iter) << "ms." << endl;
    auto cst_multiplication_duration = duration / iter;

    Ciphertext cipherMul;
    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.mult(cipherMul, cipher1, cipher2);
        scheme.reScaleByAndEqual(cipherMul, logp);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE multiplication duration: " << duration / (1000 *iter) << "ms." << endl;
    auto multiplication_duration = duration / iter;

    Ciphertext cipherLeftRot;

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.leftRotateFast(cipherLeftRot, cipher1, 4);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Left rotate duration: " << duration / (1000 *iter) << "ms." << endl;
    auto left_rotation_duration = duration / iter;

    Ciphertext cipherRightRot;

    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        scheme.rightRotateFast(cipherRightRot, cipher1, 4);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Right rotate duration: " << duration / (1000 *iter) << "ms." << endl;
    auto right_rotation_duration = duration / iter;


    start = high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        complex<double> *dvec = scheme.decrypt(secretKey, cipher1);
    }
    end = high_resolution_clock::now();

    duration = duration_cast<microseconds>(end - start).count();
    cout << "FHE Decrypt duration: " << duration / (1000 *iter) << "ms." << endl;
    auto decryption_duration = duration / iter;


    fhe_log_file << encryption_duration << ", " << cst_addition_duration << ", " << addition_duration << ", " <<
                 subtraction_duration << ", " << cst_multiplication_duration << ", " << multiplication_duration << ", " <<
                 left_rotation_duration << ", " << right_rotation_duration << ", " << decryption_duration << endl;


    complex<double> *dvecAdd = scheme.decrypt(secretKey, cipherAdd);
    complex<double> *dvecMul = scheme.decrypt(secretKey, cipherMul);
    complex<double> *dvecConstAdd = scheme.decrypt(secretKey, cipherConstAdd);
    complex<double> *dvecConstMul = scheme.decrypt(secretKey, cipherConstMul);
    complex<double> *dvecLeftRot = scheme.decrypt(secretKey, cipherLeftRot);
    complex<double> *dvecRightRot = scheme.decrypt(secretKey, cipherRightRot);



    cout << "FHE Addition result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecAdd[i] << ' ';
    }
    cout << " " << endl;

    cout << "FHE Constant Addition result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecConstAdd[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Constant Multiplication result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecConstMul[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Multiplication result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecMul[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Left Rotation result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecLeftRot[i] << ' ';
    }
    cout << " " << endl;


    cout << "FHE Right Rotation result:" << endl;
    for (int i = 0; i < l; ++i) {
        cout << dvecRightRot[i] << ' ';
    }
    cout << " " << endl;

    dtpkc_log_file.close();
    fhe_log_file.close();
}
