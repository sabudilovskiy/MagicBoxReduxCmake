#ifndef MYTAB_H
#define MYTAB_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QCheckBox>
#include "xmlnode.h"


class MyTab{
    QWidget* tab;
    QVBoxLayout* base_layout;
    QScrollArea* scroll_area;
    QWidget* scroll_area_contents;
    QFormLayout* form_layout;
    int count;
protected:
    void add_label(const std::wstring& name, XMLNode& node);
public:
    MyTab(std::wstring name);
    QWidget* get_tab();
    QSpinBox* add_integer(const std::wstring& name, XMLNode& node);
    QTextEdit* add_string(const std::wstring& name, XMLNode& node);
    QCheckBox* add_boolean(const std::wstring& name, XMLNode& node);
    QDoubleSpinBox* add_double(const std::wstring& name, XMLNode& node);
    void add_space(const std::wstring& name, XMLNode& node);
};

#endif // MYTAB_H
