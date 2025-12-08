#ifndef ARRAY_SORTER_HPP
#define ARRAY_SORTER_HPP

class Sorter {
public:
    virtual int* Sort(int* array, int size) = 0;
    virtual ~Sorter() = default;
};

#endif