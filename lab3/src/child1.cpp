#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#include "systemCall.h"

int main(int argc, char* argv[]) {
    const char* input_mmf_name = argc > 1 ? argv[1] : "parent_to_child1";
    const char* output_mmf_name = argc > 2 ? argv[2] : "child1_to_child2";
    
    mmfT input_mmf, output_mmf;
    input_mmf.handle = INVALID_MMF_HANDLE; input_mmf.data = nullptr;
    output_mmf.handle = INVALID_MMF_HANDLE; output_mmf.data = nullptr;
    
    if (!MMFOpen(&input_mmf, input_mmf_name, 1024) ||
        !MMFOpen(&output_mmf, output_mmf_name, 1024)) {
        std::cerr << "Child1: Failed to open MMF" << std::endl;
        return 1;
    }
    
    while (true) {
        std::string data = ReadFromMMF(input_mmf);
        
        if (!data.empty()) {
            std::string processed_data = data;
            std::transform(processed_data.begin(), processed_data.end(), 
                          processed_data.begin(), 
                          [](unsigned char c){ return std::tolower(c); });
            
            if (WriteToMMF(output_mmf, processed_data)) {
                ClearMMF(input_mmf);
            }
        }
    }
    
    return 0;
}