#include <windows.h>

#include "DynamicLoader.hpp"

DynamicLoader::DynamicLoader() : dllHandle(nullptr), 
                                createPi(nullptr),
                                createSort(nullptr),
                                deletePi(nullptr),
                                deleteSort(nullptr) {}  // ← ИНИЦИАЛИЗИРОВАТЬ НУЛЯМИ

DynamicLoader::~DynamicLoader() {
    unload();
}

bool DynamicLoader::load(const std::string& dllPath) {
    unload();
    
    dllHandle = LoadLibraryA(dllPath.c_str());
    if (!dllHandle) {
        return false;
    }
    
    // Получаем адреса функций
    createPi = (PiCalc*(*)())GetProcAddress((HMODULE)dllHandle, "CreatePiCalculator");
    createSort = (Sorter*(*)())GetProcAddress((HMODULE)dllHandle, "CreateArraySorter");
    deletePi = (void(*)(PiCalc*))GetProcAddress((HMODULE)dllHandle, "DeletePiCalculator");
    deleteSort = (void(*)(Sorter*))GetProcAddress((HMODULE)dllHandle, "DeleteArraySorter");
    
    if (!createPi || !createSort || !deletePi || !deleteSort) {
        FreeLibrary((HMODULE)dllHandle);
        dllHandle = nullptr;
        return false;
    }
    
    currentLib = dllPath;
    return true;
}

void DynamicLoader::unload() {
    if (dllHandle) {
        FreeLibrary((HMODULE)dllHandle);
        dllHandle = nullptr;
    }
    currentLib.clear();
    
    // Обнуляем указатели на функции
    createPi = nullptr;
    createSort = nullptr;
    deletePi = nullptr;
    deleteSort = nullptr;
}

bool DynamicLoader::switchLib() {
    if (currentLib.find("lib1") != std::string::npos) {
        return load("lib2.dll");
    } else {
        return load("lib1.dll");
    }
}

PiCalc* DynamicLoader::getPiCalculator() {
    if (createPi) {
        return createPi();
    }
    return nullptr;
}

Sorter* DynamicLoader::getArraySorter() {
    if (createSort) {
        return createSort();
    }
    return nullptr;
}

void DynamicLoader::freePiCalculator(PiCalc* p) {
    if (deletePi && p) {
        deletePi(p);
    }
}

void DynamicLoader::freeArraySorter(Sorter* p) {
    if (deleteSort && p) {
        deleteSort(p);
    }
}

std::string DynamicLoader::currentLibrary() const {
    return currentLib;
}

bool DynamicLoader::isLoaded() const {
    return dllHandle != nullptr;
}