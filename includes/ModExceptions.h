#ifndef MODEXCEPTIONS_H
#define MODEXCEPTIONS_H
#include <stdexcept>
#include <string>
class UnknownModType : public std::exception{
    std::string name;
public:
    UnknownModType(std::string name): name("Неизвестный тип мода у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};

class MissingModType : public std::exception{
    std::string name;
public:
    MissingModType(std::string name): name("Отсутствует тип мода у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};

class MissingName : public std::exception{
    std::string name;
public:
    MissingName(std::string name): name("Отсутствует название мода у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};


class MissingVersions : public std::exception{
    std::string name;
public:
    MissingVersions(std::string name): name("Отсутствуют версии у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};

class MissingVersionName : public std::exception{
    std::string name;
public:
    MissingVersionName(std::string name): name("Отсутствуют название версии у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};

class MissingVersionURL : public std::exception{
    std::string name;
public:
    MissingVersionURL(std::string name): name("Отсутствуют ссылка для скачивания у версии у " + name){
    }
    const char* what() const override {
        return name.c_str();
    }
};

#endif // MODEXCEPTIONS_H
