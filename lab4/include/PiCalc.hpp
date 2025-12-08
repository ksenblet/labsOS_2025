#ifndef PI_CALCULATOR_HPP
#define PI_CALCULATOR_HPP

class PiCalc {
public:
    virtual float Calculate(int K) = 0;
    virtual ~PiCalc() = default;
};

#endif 