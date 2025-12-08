#include <iostream>
#include <string>
#include <sstream>

#include "DynamicLoader.hpp"

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    DynamicLoader manager;

    if (!manager.load("lib1.dll")) {
        std::cout << "Cannot load lib1.dll, trying lib2.dll..." << std::endl;
        if (!manager.load("lib2.dll")) {
            std::cout << "Cannot load any library!" << std::endl;
            return 1;
        }
    }
    
    std::cout << "   Program 2 Dynamic Loading" << std::endl;
    std::cout << "Current library: " << manager.currentLibrary() << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  0 - Switch library" << std::endl;
    std::cout << "  1 K - Calculate Pi" << std::endl;
    std::cout << "  2 n a1 a2 ... an - Sort array" << std::endl;
    std::cout << "  'exit' for exit" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        if (line == "exit") break;
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "0") {
            if (manager.switchLib()) {
                std::cout << "Switched to: " << manager.currentLibrary() << std::endl;
            } else {
                std::cout << "Switch failed!" << std::endl;
            }
        }
        
        else if (cmd == "1") {
            int K;
            if (iss >> K && K > 0) {
                PiCalc* calc = manager.getPiCalculator();
                if (calc) {
                    float pi = calc->Calculate(K);  // Теперь работает!
                    std::cout << "Pi(" << K << ") = " << pi << std::endl;
                    manager.freePiCalculator(calc);
                } else {
                    std::cout << "Failed to create PiCalculator" << std::endl;
                }
            } else {
                std::cout << "Invalid K value" << std::endl;
            }
        }
        
        else if (cmd == "2") {
            int n;
            if (iss >> n && n > 0) {
                int* arr = new int[n];
                bool ok = true;
                
                for (int i = 0; i < n; i++) {
                    if (!(iss >> arr[i])) {
                        std::cout << "Need " << n << " numbers!" << std::endl;
                        delete[] arr;
                        arr = nullptr;
                        ok = false;
                        break;
                    }
                }
                
                if (ok && arr) {
                    Sorter* sorter = manager.getArraySorter();
                    if (sorter) {
                        std::cout << "Before: ";
                        printArray(arr, n);
                        
                        sorter->Sort(arr, n);
                        
                        std::cout << "After:  ";
                        printArray(arr, n);
                        
                        manager.freeArraySorter(sorter);
                    } else {
                        std::cout << "Failed to create Sorter" << std::endl;
                    }
                    delete[] arr;
                }
            } else {
                std::cout << "Invalid array size" << std::endl;
            }
        }
        
        else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }
    
    return 0;
}