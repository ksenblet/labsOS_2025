#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

class Matrix {
public:
    Matrix();
    Matrix(int rows, int cols);
    
    int getRows() const;
    int getCols() const;
    int getValue(int row, int col) const;
    void setValue(int row, int col, int value);
    
    void print() const;
    void fillFromConsole();
    
    // медианный фильтр для одного элемента
    int applyMedianFilter(int row, int col, int windowSize, const Matrix& source) const;

private:
    std::vector<std::vector<int>> data;
    int rows;
    int cols;
};

#endif