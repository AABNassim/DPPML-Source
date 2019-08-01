//
// Created by Nassim AAB on 2019-07-23.
//

#ifndef DPPML_FHE_H
#define DPPML_FHE_H

struct FHE {
    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 2;
};

#endif //DPPML_FHE_H
