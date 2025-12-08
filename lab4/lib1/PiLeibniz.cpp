#include "../include/PiCalc.hpp"


class LeibnizPi : public PiCalc {
public:
    float Calculate(int K) override {
        float result = 0.0f;
        for (int n = 0; n < K; n++) {
            float term = 1.0f / (2 * n + 1);
            if (n % 2 == 0) result += term;
            else result -= term;
        }
        return 4.0f * result;
    }
};


extern "C" __declspec(dllexport) PiCalc* CreatePiCalculator() {
    return new LeibnizPi();
}

extern "C" __declspec(dllexport) void DeletePiCalculator(PiCalc* p) {
    delete p;
}