#include <iostream>
#include <string>
#include <sstream>

#include "../include/PiCalc.hpp"
#include "../include/Sorter.hpp"


// oбъявления функций из DLL 
extern "C" {
    PiCalc* CreatePiCalculator();
    Sorter* CreateArraySorter();
    void DeletePiCalculator(PiCalc*);
    void DeleteArraySorter(Sorter*);
}

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    PiCalc* piCalc = CreatePiCalculator();
    Sorter* sorter = CreateArraySorter();
    
    std::cout << "Program 1 Static Linking " << std::endl;
    std::cout << "0 - Exit" << std::endl;
    std::cout << "1 K - Calculate Pi" << std::endl;
    std::cout << "2 n a1 a2 ... an - Sort array" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "0") break;
        
        if (cmd == "1") {
            int K;
            if (iss >> K && K > 0) {
                float pi = piCalc->Calculate(K);
                std::cout << "Pi(" << K << ") = " << pi << std::endl;
            }
        }
        
        else if (cmd == "2") {
            int n;
            if (iss >> n && n > 0) {
                int* array = new int[n];
                
                for (int i = 0; i < n; i++) {
                    if (!(iss >> array[i])) {
                        std::cout << "Error" << std::endl;
                        delete[] array;
                        array = nullptr;
                        break;
                    }
                }
                
                if (array) {
                    std::cout << "Before: ";
                    printArray(array, n);
                    
                    sorter->Sort(array, n);
                    
                    std::cout << "After:  ";
                    printArray(array, n);
                    
                    delete[] array;
                }
            }
        }
    }
    
    DeletePiCalculator(piCalc);
    DeleteArraySorter(sorter);
    
    return 0;
}