//
// Created by Nassim AAB on 2019-08-01.
//

#ifndef DPPML_BENCHMARKOPS_H
#define DPPML_BENCHMARKOPS_H

#include "HEAAN.h"
#include "BENCHS/BENCHBuildingBlocks.h"
#include <gmp.h>
#include <vector>
#include "ML/DatasetReader.h"
#include "ML/LogisticRegression.h"
#include "PPML/PPLogisticRegression.h"
#include "CRYPTO/DTPKC.h"
#include "CRYPTO/EvalAdd.h"

#include "PPML/MLSP.h"
#include "PPML/CSP.h"

#include <NTL/fileio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std::chrono;

class BenchmarkOps {
public:
    BenchmarkOps();
};


#endif //DPPML_BENCHMARKOPS_H
