/**
 * @brief Data Processor for the Model Example included in mlrunner.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * TODO: Need to double buffer this so that a model can be run while the
 * next data is being collected.
 */
#pragma once

#include "mlrunner/DataProcessor.h"

class PxtDataProcessor : public MlDataProcessor {
    private:

        float *accDataX;
        float *accDataY;
        float *accDataZ;
        float *outputData;
        int filterSize;
        int accSamples;
        int accDataIndex;

    public:
        PxtDataProcessor(int samples);
        ~PxtDataProcessor();
        void recordAccData(int x, int y, int z);
        bool isDataReady();
        float *getModelInputData();
};
