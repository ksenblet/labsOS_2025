#pragma once

#include <string>

#include "../include/PiCalc.hpp"
#include "../include/Sorter.hpp"    


class DynamicLoader {
private:
    void* dllHandle;
    std::string currentLib;
    
    // Указатели на функции из DLL
    PiCalc* (*createPi)();
    Sorter* (*createSort)();
    void (*deletePi)(PiCalc*);
    void (*deleteSort)(Sorter*);
    
public:
    DynamicLoader();
    ~DynamicLoader();
    
    bool load(const std::string& dllPath);
    void unload();
    bool switchLib();  // переключение lib1 lib2
    
    PiCalc* getPiCalculator();
    Sorter* getArraySorter();
    void freePiCalculator(PiCalc* p);
    void freeArraySorter(Sorter* p);
    
    std::string currentLibrary() const;
    bool isLoaded() const;
};