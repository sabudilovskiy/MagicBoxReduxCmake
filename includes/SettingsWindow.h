#ifndef SDKSETTINGS_H
#define SDKSETTINGS_H

#include "xmlnode.h"
#include <unordered_map>
#include "MyTab.h"
namespace Ui {
class SettingsWindow;
}

class UnknownTypeSettings : public std::exception{

};
class UnknownTab : public std::exception{

};

class SettingsWindow : public QWidget
{
    Q_OBJECT
    std::vector<QSpinBox*> spin_boxes;
    std::vector<QCheckBox*> check_boxes;
    std::vector<QTextEdit*> text_edits;
    std::vector<QDoubleSpinBox*> double_spin_boxes;
    std::vector<QComboBox*> combo_boxes;
    std::unordered_map<std::wstring, MyTab> tabs;
    std::wstring path_fl;
    std::wstring path_file;
public:
    explicit SettingsWindow(std::wstring path_fl, std::wstring path_file, QWidget *parent = nullptr);
    ~SettingsWindow();
    void add_elem(const std::wstring& name, XMLNode& node);
    MyTab* add_tab(std::wstring name, XMLNode& node);
    MyTab* find_tab(const std::wstring& key);
signals:
    void resaved_file();
private slots:
    void on_load_button_clicked();
    void on_save_button_clicked();
private:
    Ui::SettingsWindow *ui;
};

#endif // SDKSETTINGS_H
