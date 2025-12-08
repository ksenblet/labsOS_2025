#include <iostream>
#include <string>

#include "systemCall.h"

int main() {
    mmfT mmf1, mmf2, mmf3;
    process child1, child2;
    
    // инициализация 
    mmf1.handle = INVALID_MMF_HANDLE; mmf1.data = nullptr;
    mmf2.handle = INVALID_MMF_HANDLE; mmf2.data = nullptr;
    mmf3.handle = INVALID_MMF_HANDLE; mmf3.data = nullptr;
    
    std::cout << "Creating MMF and processes..." << std::endl;
    
    // создаем общую память
    const size_t MMF_SIZE = 1024;
    if (!MMFCreate(&mmf1, "parent_to_child1", MMF_SIZE) ||
        !MMFCreate(&mmf2, "child1_to_child2", MMF_SIZE) ||
        !MMFCreate(&mmf3, "child2_to_parent", MMF_SIZE)) {
        std::cerr << "Failed to create MMF" << std::endl;
        return 1;
    }
    
#ifdef _WIN32
    child1 = ProcessCreateWithMMF("child1.exe", "parent_to_child1", "child1_to_child2");
    child2 = ProcessCreateWithMMF("child2.exe", "child1_to_child2", "child2_to_parent");
#else
    child1 = ProcessCreateWithMMF("./child1", "parent_to_child1", "child1_to_child2");
    child2 = ProcessCreateWithMMF("./child2", "child1_to_child2", "child2_to_parent");
#endif
    
    if (!child1.is_valid || !child2.is_valid) {
        std::cerr << "Failed to create child processes" << std::endl;
        MMFClose(&mmf1);
        MMFClose(&mmf2);
        MMFClose(&mmf3);
        return 1;
    }

#ifdef _WIN32
    Sleep(500); 
#else
    usleep(500000); 
#endif
    
    std::cout << "Ready. Enter strings (empty line to exit):" << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            break;
        }
        
        // jтправляем данные в child1
        if (WriteToMMF(mmf1, input)) {
            std::cout << "Sent to child1: " << input << std::endl;
            
            // ждем результат от child2
            bool data_ready = false;
            int attempts = 0;
            while (!data_ready && attempts < 100) {
                std::string result = ReadFromMMF(mmf3);
                if (!result.empty()) {
                    std::cout << "Result: " << result << std::endl;
                    ClearMMF(mmf3); // очищаем для следующей итерации
                    data_ready = true;
                }
                attempts++;
            }
            
            if (!data_ready) {
                std::cerr << "Timeout waiting for result" << std::endl;
            }
            ClearMMF(mmf1);
        } else {
            std::cerr << "Failed to send data" << std::endl;
        }
    }

    if (child1.is_valid) {
        ProcessTerminate(&child1);
    }
    if (child2.is_valid) {
        ProcessTerminate(&child2);
    }

    MMFClose(&mmf1);
    MMFClose(&mmf2);
    MMFClose(&mmf3);
    
    std::cout << "Program finished" << std::endl;
    return 0;
}