//
// Created by areg1 on 6/24/2025.
//

#ifndef MYEXCEPTIONS_H
#define MYEXCEPTIONS_H

#include <exception>

class out_of_range : std::exception {
    const char* what() const noexcept override {
        return "list is empty";
    }
};

class key_doesnt_exist : std::exception {
    const char* what() const noexcept override {
        return "key does not exist";
    }
};

#endif //MYEXCEPTIONS_H
