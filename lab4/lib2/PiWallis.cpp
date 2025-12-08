#include "../include/PiCalc.hpp"


class WallisPi : public PiCalc {
public:
    float Calculate(int K) override {
        float pi = 1.0f;
        for (int n = 1; n <= K; n++) {
            float numerator = 4.0f * n * n;
            float denominator = 4.0f * n * n - 1.0f;
            pi *= numerator / denominator;
        }
        return 2.0f * pi;
    }
};

extern "C" __declspec(dllexport) PiCalc* CreatePiCalculator() {
    return new WallisPi();
}

extern "C" __declspec(dllexport) void DeletePiCalculator(PiCalc* p) {
    delete p;
}