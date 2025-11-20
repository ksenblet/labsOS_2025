#include <iostream>
#include <string>
#include <cctype>

#include "systemCall.h"

int main(int argc, char* argv[]) {
    const char* input_mmf_name = argc > 1 ? argv[1] : "child1_to_child2";
    const char* output_mmf_name = argc > 2 ? argv[2] : "child2_to_parent";
    
    mmfT input_mmf, output_mmf;
    input_mmf.handle = INVALID_MMF_HANDLE; input_mmf.data = nullptr;
    output_mmf.handle = INVALID_MMF_HANDLE; output_mmf.data = nullptr;
    
    if (!MMFOpen(&input_mmf, input_mmf_name, 1024) ||
        !MMFOpen(&output_mmf, output_mmf_name, 1024)) {
        std::cerr << "Child2: Failed to open MMF" << std::endl;
        return 1;
    }

    while (true) {
        std::string data = ReadFromMMF(input_mmf);
        
        if (!data.empty()) {
            std::string result;
            bool prev_space = false;
            
            for (char c : data) {
                if (std::isspace(c)) {
                    if (!prev_space) {
                        result += c;
                        prev_space = true;
                    }
                } else {
                    result += c;
                    prev_space = false;
                }
            }
            
            if (WriteToMMF(output_mmf, result)) {
                ClearMMF(input_mmf);
            }
        }
    }
    
    return 0;
}