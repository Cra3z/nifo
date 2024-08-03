/********************************************************************************
** Form generated from reading UI file 'point_light_panel.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef POINT_LIGHT_PANEL_H
#define POINT_LIGHT_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

namespace nifo {
namespace ui {

class Ui_point_light_panel
{
public:
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *label_diffuse;
    QSlider *diffuse_strength;
    QLabel *label_color;
    QPushButton *color;
    QLabel *label_specular;
    QSlider *specular_strength;
    QLabel *label_power;
    QSlider *power;

    void setupUi(QWidget *nifo__ui__point_light_panel)
    {
        if (nifo__ui__point_light_panel->objectName().isEmpty())
            nifo__ui__point_light_panel->setObjectName("nifo__ui__point_light_panel");
        nifo__ui__point_light_panel->resize(386, 321);
        layoutWidget = new QWidget(nifo__ui__point_light_panel);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(100, 40, 128, 148));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_diffuse = new QLabel(layoutWidget);
        label_diffuse->setObjectName("label_diffuse");
        label_diffuse->setMinimumSize(QSize(36, 32));
        label_diffuse->setMaximumSize(QSize(16777215, 35));
        label_diffuse->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_diffuse);

        diffuse_strength = new QSlider(layoutWidget);
        diffuse_strength->setObjectName("diffuse_strength");
        diffuse_strength->setMinimumSize(QSize(0, 32));
        diffuse_strength->setMaximumSize(QSize(16777215, 35));
        diffuse_strength->setTracking(false);
        diffuse_strength->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(1, QFormLayout::FieldRole, diffuse_strength);

        label_color = new QLabel(layoutWidget);
        label_color->setObjectName("label_color");
        label_color->setMinimumSize(QSize(36, 32));
        label_color->setMaximumSize(QSize(16777215, 35));
        label_color->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::LabelRole, label_color);

        color = new QPushButton(layoutWidget);
        color->setObjectName("color");
        color->setMinimumSize(QSize(0, 32));
        color->setMaximumSize(QSize(16777215, 35));

        formLayout->setWidget(3, QFormLayout::FieldRole, color);

        label_specular = new QLabel(layoutWidget);
        label_specular->setObjectName("label_specular");
        label_specular->setMinimumSize(QSize(36, 32));
        label_specular->setMaximumSize(QSize(16777215, 35));
        label_specular->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_specular);

        specular_strength = new QSlider(layoutWidget);
        specular_strength->setObjectName("specular_strength");
        specular_strength->setMinimumSize(QSize(0, 32));
        specular_strength->setMaximumSize(QSize(16777215, 35));
        specular_strength->setTracking(false);
        specular_strength->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(2, QFormLayout::FieldRole, specular_strength);

        label_power = new QLabel(layoutWidget);
        label_power->setObjectName("label_power");
        label_power->setMinimumSize(QSize(36, 32));
        label_power->setMaximumSize(QSize(16777215, 35));
        label_power->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_power);

        power = new QSlider(layoutWidget);
        power->setObjectName("power");
        power->setMinimumSize(QSize(0, 32));
        power->setMaximumSize(QSize(16777215, 35));
        power->setValue(10);
        power->setTracking(false);
        power->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(0, QFormLayout::FieldRole, power);


        retranslateUi(nifo__ui__point_light_panel);

        QMetaObject::connectSlotsByName(nifo__ui__point_light_panel);
    } // setupUi

    void retranslateUi(QWidget *nifo__ui__point_light_panel)
    {
        nifo__ui__point_light_panel->setWindowTitle(QCoreApplication::translate("nifo::ui::point_light_panel", "point_light_panel", nullptr));
        label_diffuse->setText(QCoreApplication::translate("nifo::ui::point_light_panel", "\346\274\253\345\260\204", nullptr));
        label_color->setText(QCoreApplication::translate("nifo::ui::point_light_panel", "\351\242\234\350\211\262", nullptr));
        color->setText(QString());
        label_specular->setText(QCoreApplication::translate("nifo::ui::point_light_panel", "\345\217\215\345\205\211", nullptr));
        label_power->setText(QCoreApplication::translate("nifo::ui::point_light_panel", "\350\203\275\351\207\217", nullptr));
    } // retranslateUi

};

} // namespace ui
} // namespace nifo

namespace nifo {
namespace ui {
namespace Ui {
    class point_light_panel: public Ui_point_light_panel {};
} // namespace Ui
} // namespace ui
} // namespace nifo

#endif // POINT_LIGHT_PANEL_H
