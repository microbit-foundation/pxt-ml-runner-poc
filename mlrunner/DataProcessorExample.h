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

#include "DataProcessor.h"

class MlDataProcessorExample : public MlDataProcessor {
    private:

        float *accData;
        int accDataSize;
        int accDataIndex;

    public:

        MlDataProcessorExample(int samples) {
            accDataSize = samples * 3;
            accData = new float[accDataSize];
            accDataIndex = 0;
        }

        ~MlDataProcessorExample() {
            delete[] accData;
        }

        void recordAccData(int x, int y, int z) {
            accData[accDataIndex++] = x / 1000.0f;
            accData[accDataIndex++] = y / 1000.0f;
            accData[accDataIndex++] = z / 1000.0f;
            if (accDataIndex >= accDataSize) {
                accDataIndex = 0;
            }
        }

        bool isDataReady() {
            return accDataIndex == 0;
        }

        float* getModelInputData() {
            return accData;
        }
};
