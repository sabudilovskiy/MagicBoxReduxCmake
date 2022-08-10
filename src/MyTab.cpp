#include "MyTab.h"
#include "MyTabExceptions.h"
#include <QToolButton>
#include <QFileDialog>
void MyTab::add_label(const std::wstring &name, XMLNode &node){
    auto type_node = node.get_child(L"type");
    if (!type_node) throw MissingType(QString::fromStdWString(name).toStdString());
    std::wstring type = type_node->get_value();

    auto title_node = node.get_child(L"title");
    if (!title_node) throw MissingTitle(QString::fromStdWString(name).toStdString());
    std::wstring title = title_node->get_value();

    QString qname = QString::fromStdWString(name);
    QLabel* label = new QLabel(scroll_area_contents);
    label->setObjectName(qname + "_label");
    label->setText(QString::fromStdWString(title));
    form_layout->setWidget(count, QFormLayout::LabelRole, label);
}

MyTab::MyTab(std::wstring name) : count(0){
    tab = new QWidget();
    tab->setObjectName(QString::fromStdWString(name));
    base_layout = new QVBoxLayout(tab);
    base_layout->setObjectName(QString::fromStdWString(name + L"_layout"));
    scroll_area = new QScrollArea(tab);
    scroll_area->setObjectName(QString::fromStdWString(name + L"scroll_area_"));
    scroll_area->setWidgetResizable(true);
    scroll_area_contents = new QWidget();
    scroll_area_contents->setObjectName(QString::fromStdWString(name + L"scroll_area_contents_" ));;
    scroll_area_contents->setGeometry(QRect(0,0,437,296));
    form_layout = new QFormLayout(scroll_area_contents);
    form_layout->setObjectName(QString::fromStdWString(name + L"form_layout_"));
    scroll_area->setWidget(scroll_area_contents);
    base_layout->addWidget(scroll_area);
}

QWidget *MyTab::get_tab()
{
    return tab;
}

QSpinBox *MyTab::add_integer(const std::wstring &name, XMLNode &node){
    add_label(name, node);
    QString qname = QString::fromStdWString(name);

    auto min_node = node.get_child(L"min");
    if (!min_node) throw MissingMin(QString::fromStdWString(name).toStdString());
    int min_value = std::stoi(min_node->get_value());

    auto max_node = node.get_child(L"max");
    if (!max_node) throw MissingMax(QString::fromStdWString(name).toStdString());
    int max_value = std::stoi(max_node->get_value());

    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    int default_value = std::stoi(default_node->get_value());

    QSpinBox* field = new QSpinBox(scroll_area_contents);
    field->setObjectName(qname);
    field->setMinimum(min_value);
    field->setMaximum(max_value);
    field->setValue(default_value);
    form_layout->setWidget(count, QFormLayout::FieldRole, field);
    count++;
    return field;
}

QCheckBox *MyTab::add_boolean(const std::wstring &name, XMLNode &node){
    add_label(name, node);
    QString qname = QString::fromStdWString(name);
    QCheckBox* field = new QCheckBox(scroll_area_contents);
    field->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    field->setObjectName(qname);
    form_layout->setWidget(count, QFormLayout::FieldRole, field);
    count++;
    return field;
}
QDoubleSpinBox *MyTab::add_double(const std::wstring &name, XMLNode &node){
    add_label(name, node);
    QString qname = QString::fromStdWString(name);

    auto min_node = node.get_child(L"min");
    if (!min_node) throw MissingMin(QString::fromStdWString(name).toStdString());
    double min_value = std::stod(min_node->get_value());

    auto max_node = node.get_child(L"max");
    if (!max_node) throw MissingMax(QString::fromStdWString(name).toStdString());
    double max_value = std::stod(max_node->get_value());

    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    double default_value = std::stod(default_node->get_value());

    auto step_node = node.get_child(L"step");
    if (!step_node) throw MissingStep(QString::fromStdWString(name).toStdString());
    double step_value = std::stod(step_node->get_value());

    QDoubleSpinBox* field = new QDoubleSpinBox(scroll_area_contents);

    field->setObjectName(qname);
    field->setMinimum(min_value);
    field->setMaximum(max_value);
    field->setValue(default_value);
    field->setObjectName(qname);
    field->setSingleStep(step_value);

    form_layout->setWidget(count, QFormLayout::FieldRole, field);
    count++;
    return field;
}

void MyTab::add_space(const std::wstring &name, XMLNode &node){
    add_label(name, node);
    QSpacerItem* field = new QSpacerItem(40,20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    form_layout->setItem(count, QFormLayout::FieldRole, field);
    count++;
}
QTextEdit *MyTab::add_string(const std::wstring &name, XMLNode &node){
    add_label(name, node);
    QString qname = QString::fromStdWString(name);
    QTextEdit* field = new QTextEdit(scroll_area_contents);
    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    QString default_value = QString::fromStdWString(default_node->get_value());
    field->setText(default_value);
    field->setObjectName(QString::fromStdWString(name));
    field->setMinimumHeight(24);
    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setLineWrapMode(QTextEdit::FixedPixelWidth);
    field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    field->setObjectName(qname);
    form_layout->setWidget(count, QFormLayout::FieldRole, field);
    count++;
    return field;
}


QTextEdit *MyTab::add_path_folder(const std::wstring &name, XMLNode &node) {
    add_label(name, node);
    QHBoxLayout* layout = new QHBoxLayout();
    QTextEdit* field = new QTextEdit(scroll_area_contents);
    field->setObjectName(QString::fromStdWString(name));
    field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setLineWrapMode(QTextEdit::NoWrap);
    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    QString default_value = QString::fromStdWString(default_node->get_value());
    field->setText(default_value);
    QToolButton* tool_button = new QToolButton(scroll_area_contents);
    tool_button->setText("...");
    layout->addWidget(field);
    layout->addWidget(tool_button);
    auto it = buttons.emplace_back([field, this]() -> void {
        field->setText(QFileDialog::getExistingDirectory(this->scroll_area_contents,"Выберите папку"));
    });
    form_layout->setLayout(count, QFormLayout::FieldRole, layout);
    QObject::connect(tool_button,  &QToolButton::clicked, it);
    count++;
    return field;
}

QTextEdit *MyTab::add_path_file(const std::wstring &name, XMLNode &node) {
    add_label(name, node);
    QHBoxLayout* layout = new QHBoxLayout();
    QTextEdit* field = new QTextEdit(scroll_area_contents);
    field->setObjectName(QString::fromStdWString(name));
    field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    field->setLineWrapMode(QTextEdit::NoWrap);
    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    QString default_value = QString::fromStdWString(default_node->get_value());
    field->setText(default_value);

    QToolButton* tool_button = new QToolButton(scroll_area_contents);
    tool_button->setText("...");
    layout->addWidget(field);
    layout->addWidget(tool_button);

    auto it = buttons.emplace_back([field, this]() -> void {
        field->setText(QFileDialog::getOpenFileName(this->scroll_area_contents,"Выберите файл"));
    });

    form_layout->setLayout(count, QFormLayout::FieldRole, layout);
    QObject::connect(tool_button,  &QToolButton::clicked, it);
    count++;
    return field;
}

QComboBox *MyTab::add_enum(const std::wstring &name, XMLNode &node) {
    add_label(name, node);
    QString qname = QString::fromStdWString(name);
    QComboBox* field = new QComboBox(scroll_area_contents);
    auto available_values = node.get_child(L"availableValues");
    if (!available_values) throw MissingAvailableValues(QString::fromStdWString(name).toStdString());
    for (auto& value : *available_values){
        field->addItem(QString::fromStdWString(value.second->get_value()));
    }
    auto default_node = node.get_child(L"default");
    if (!default_node) throw MissingDefault(QString::fromStdWString(name).toStdString());
    QString default_value = QString::fromStdWString(default_node->get_value());
    field->setObjectName(QString::fromStdWString(name));
    field->setMinimumHeight(24);
    field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    field->setObjectName(qname);
    field->setCurrentText(default_value);
    form_layout->setWidget(count, QFormLayout::FieldRole, field);
    count++;
    return field;
}

