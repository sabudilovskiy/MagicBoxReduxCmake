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
#include <QComboBox>
#include "xmlnode.h"


class MyTab{
    QWidget* tab;
    QVBoxLayout* base_layout;
    QScrollArea* scroll_area;
    QWidget* scroll_area_contents;
    QFormLayout* form_layout;
    int count;
    std::vector<std::function<void()>> buttons;
protected:
    void add_label(const std::wstring& name, XMLNode& node);
public:
    MyTab(std::wstring name);
    QWidget* get_tab();
    QVBoxLayout *get_base_layout() const;
    QScrollArea *get_scroll_area() const;
    QWidget *get_scroll_area_contents() const;
    QFormLayout *get_form_layout() const;
    int get_count() const;
    const std::vector<std::function<void()>> &get_buttons() const;
    QSpinBox* add_integer(const std::wstring& name, XMLNode& node);
    QTextEdit* add_string(const std::wstring& name, XMLNode& node);
    QCheckBox* add_boolean(const std::wstring& name, XMLNode& node);
    QDoubleSpinBox* add_double(const std::wstring& name, XMLNode& node);
    QTextEdit* add_path_folder(const std::wstring& name, XMLNode& node);
    QTextEdit* add_path_file(const std::wstring& name, XMLNode& node);
    QComboBox* add_enum(const std::wstring& name, XMLNode& node);
    void add_space(const std::wstring& name, XMLNode& node);
};

#endif // MYTAB_H
