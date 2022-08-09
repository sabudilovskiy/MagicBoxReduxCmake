#ifndef MYTABEXCEPTIONS_H
#define MYTABEXCEPTIONS_H
#include <stdexcept>
#include <string>
class MissingType : public std::exception{
    std::string name_elem;
public:
    MissingType(const std::string& name_elem):name_elem("Отсуствует тип у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};

class MissingTitle : public std::exception{
    std::string name_elem;
public:
    MissingTitle(const std::string& name_elem):name_elem("Отсуствует заголовок у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};

class MissingTab : public std::exception{
    std::string name_elem;
public:
    MissingTab(const std::string& name_elem):name_elem("Отсуствует название вкладки  у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};

class MissingMin : public std::exception{
    std::string name_elem;
public:
    MissingMin(const std::string& name_elem):name_elem("Отсуствует наименьшее значение  у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};
class MissingMax : public std::exception{
    std::string name_elem;
public:
    MissingMax(const std::string& name_elem):name_elem("Отсуствует наибольшее значение  у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};
class MissingDefault : public std::exception{
    std::string name_elem;
public:
    MissingDefault(const std::string& name_elem):name_elem("Отсуствует стандартное значение  у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};

class MissingStep : public std::exception{
    std::string name_elem;
public:
    MissingStep(const std::string& name_elem):name_elem("Отсуствует шаг у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};
class MissingAvailableValues : public std::exception{
    std::string name_elem;
public:
    MissingAvailableValues(const std::string& name_elem):name_elem("Отсуствует доступные значения у " + name_elem){
    }
    const char* what() const override{
        return name_elem.c_str();
    }
};

#endif // MYTABEXCEPTIONS_H
