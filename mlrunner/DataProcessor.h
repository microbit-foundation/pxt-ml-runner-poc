#pragma once

class MlDataProcessor {
    public:
        virtual void recordAccData(int x, int y, int z) = 0;
        virtual bool isDataReady() = 0;
        virtual float* getModelInputData() = 0;
};
