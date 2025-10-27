#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "matrix.h"

Matrix::Matrix() : rows(0), cols(0) {}

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
    data.resize(rows, std::vector<int>(cols, 0));
}

int Matrix::getRows() const { 
    return rows; 
}

int Matrix::getCols() const { 
    return cols; 
}

int Matrix::getValue(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return data[row][col];
    }
    return 0;
}

void Matrix::setValue(int row, int col, int value) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        data[row][col] = value;
    }
}

void Matrix::print() const {
    std::cout << "Matrix " << rows << "x" << cols << ":" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << data[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

void Matrix::fillFromConsole() {
    std::cout << "input the number of rows: ";
    std::cin >> rows;
    std::cout << "input the number of cols: ";
    std::cin >> cols;
    
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("input positive values for rows and cols for matrix");
    }
    
    data.resize(rows, std::vector<int>(cols));
    
    std::cout << "Input the matrix elements separated by a space:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cin >> data[i][j];
        }
    }
}

int Matrix::applyMedianFilter(int row, int col, int windowSize, const Matrix& source) const {
    if (windowSize % 2 == 0) {
        throw std::invalid_argument("Window size must be odd(for example(1, 3, 5, etc))");
    }
    
    int halfWindow = windowSize / 2;
    std::vector<int> windowValues;
    
    // Собираем значения из окна
    for (int i = -halfWindow; i <= halfWindow; ++i) {
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            int currentRow = row + i;
            int currentCol = col + j;
            
            if (currentRow >= 0 && currentRow < source.getRows() && 
                currentCol >= 0 && currentCol < source.getCols()) {
                windowValues.push_back(source.getValue(currentRow, currentCol));
            }
        }
    }
    
    // Сортируем и находим медиану
    if (windowValues.empty()) {
        return 0;
    }
    
    std::sort(windowValues.begin(), windowValues.end());
    return windowValues[windowValues.size() / 2];
}