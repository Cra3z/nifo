/********************************************************************************
** Form generated from reading UI file 'spot_light_panel.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SPOT_LIGHT_PANEL_H
#define SPOT_LIGHT_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "../widgets/spinbox.h"

namespace nifo {
namespace ui {

class Ui_spot_light_panel
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
    QLabel *label_outer_cutoff;
    nifo::ui::floating_point_spinbox *outer_cutoff;
    QLabel *label;
    QSlider *inner_div_outer;

    void setupUi(QWidget *nifo__ui__spot_light_panel)
    {
        if (nifo__ui__spot_light_panel->objectName().isEmpty())
            nifo__ui__spot_light_panel->setObjectName("nifo__ui__spot_light_panel");
        nifo__ui__spot_light_panel->resize(386, 321);
        layoutWidget = new QWidget(nifo__ui__spot_light_panel);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(100, 40, 171, 186));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_diffuse = new QLabel(layoutWidget);
        label_diffuse->setObjectName("label_diffuse");
        label_diffuse->setMinimumSize(QSize(36, 32));
        label_diffuse->setMaximumSize(QSize(16777215, 35));
        label_diffuse->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_diffuse);

        diffuse_strength = new QSlider(layoutWidget);
        diffuse_strength->setObjectName("diffuse_strength");
        diffuse_strength->setMinimumSize(QSize(0, 32));
        diffuse_strength->setMaximumSize(QSize(16777215, 35));
        diffuse_strength->setTracking(false);
        diffuse_strength->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(0, QFormLayout::FieldRole, diffuse_strength);

        label_color = new QLabel(layoutWidget);
        label_color->setObjectName("label_color");
        label_color->setMinimumSize(QSize(36, 32));
        label_color->setMaximumSize(QSize(16777215, 35));
        label_color->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(4, QFormLayout::LabelRole, label_color);

        color = new QPushButton(layoutWidget);
        color->setObjectName("color");
        color->setMinimumSize(QSize(0, 32));
        color->setMaximumSize(QSize(16777215, 35));

        formLayout->setWidget(4, QFormLayout::FieldRole, color);

        label_specular = new QLabel(layoutWidget);
        label_specular->setObjectName("label_specular");
        label_specular->setMinimumSize(QSize(36, 32));
        label_specular->setMaximumSize(QSize(16777215, 35));
        label_specular->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_specular);

        specular_strength = new QSlider(layoutWidget);
        specular_strength->setObjectName("specular_strength");
        specular_strength->setMinimumSize(QSize(0, 32));
        specular_strength->setMaximumSize(QSize(16777215, 35));
        specular_strength->setTracking(false);
        specular_strength->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(1, QFormLayout::FieldRole, specular_strength);

        label_outer_cutoff = new QLabel(layoutWidget);
        label_outer_cutoff->setObjectName("label_outer_cutoff");
        label_outer_cutoff->setMinimumSize(QSize(36, 32));
        label_outer_cutoff->setMaximumSize(QSize(16777215, 35));
        label_outer_cutoff->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_outer_cutoff);

        outer_cutoff = new nifo::ui::floating_point_spinbox(layoutWidget);
        outer_cutoff->setObjectName("outer_cutoff");

        formLayout->setWidget(2, QFormLayout::FieldRole, outer_cutoff);

        label = new QLabel(layoutWidget);
        label->setObjectName("label");
        label->setMinimumSize(QSize(36, 32));
        label->setMaximumSize(QSize(16777215, 35));
        label->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::LabelRole, label);

        inner_div_outer = new QSlider(layoutWidget);
        inner_div_outer->setObjectName("inner_div_outer");
        inner_div_outer->setMinimumSize(QSize(0, 32));
        inner_div_outer->setMaximumSize(QSize(16777215, 35));
        inner_div_outer->setOrientation(Qt::Orientation::Horizontal);

        formLayout->setWidget(3, QFormLayout::FieldRole, inner_div_outer);


        retranslateUi(nifo__ui__spot_light_panel);

        QMetaObject::connectSlotsByName(nifo__ui__spot_light_panel);
    } // setupUi

    void retranslateUi(QWidget *nifo__ui__spot_light_panel)
    {
        nifo__ui__spot_light_panel->setWindowTitle(QCoreApplication::translate("nifo::ui::spot_light_panel", "spot_light_panel", nullptr));
        label_diffuse->setText(QCoreApplication::translate("nifo::ui::spot_light_panel", "\346\274\253\345\260\204", nullptr));
        label_color->setText(QCoreApplication::translate("nifo::ui::spot_light_panel", "\351\242\234\350\211\262", nullptr));
        color->setText(QString());
        label_specular->setText(QCoreApplication::translate("nifo::ui::spot_light_panel", "\345\217\215\345\205\211", nullptr));
        label_outer_cutoff->setText(QCoreApplication::translate("nifo::ui::spot_light_panel", "\345\244\226\345\210\207\345\205\211\350\247\222", nullptr));
        label->setText(QCoreApplication::translate("nifo::ui::spot_light_panel", "\345\206\205\345\244\226\345\210\207\345\205\211\350\247\222\344\271\213\346\257\224", nullptr));
    } // retranslateUi

};

} // namespace ui
} // namespace nifo

namespace nifo {
namespace ui {
namespace Ui {
    class spot_light_panel: public Ui_spot_light_panel {};
} // namespace Ui
} // namespace ui
} // namespace nifo

#endif // SPOT_LIGHT_PANEL_H
