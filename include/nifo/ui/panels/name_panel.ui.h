/********************************************************************************
** Form generated from reading UI file 'name_panel.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef NAME_PANEL_H
#define NAME_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

namespace nifo {
namespace ui {

class Ui_name_panel
{
public:
    QWidget *layoutWidget;
    QFormLayout *position;
    QLabel *label_x_pos;
    QLineEdit *name;

    void setupUi(QWidget *nifo__ui__name_panel)
    {
        if (nifo__ui__name_panel->objectName().isEmpty())
            nifo__ui__name_panel->setObjectName("nifo__ui__name_panel");
        nifo__ui__name_panel->resize(533, 444);
        nifo__ui__name_panel->setMinimumSize(QSize(0, 32));
        layoutWidget = new QWidget(nifo__ui__name_panel);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 10, 209, 101));
        position = new QFormLayout(layoutWidget);
        position->setObjectName("position");
        position->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        position->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        position->setFormAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        position->setVerticalSpacing(0);
        position->setContentsMargins(0, 0, 0, 0);
        label_x_pos = new QLabel(layoutWidget);
        label_x_pos->setObjectName("label_x_pos");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_x_pos->sizePolicy().hasHeightForWidth());
        label_x_pos->setSizePolicy(sizePolicy);
        label_x_pos->setMinimumSize(QSize(0, 32));
        label_x_pos->setMaximumSize(QSize(16777215, 35));
        label_x_pos->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        position->setWidget(0, QFormLayout::LabelRole, label_x_pos);

        name = new QLineEdit(layoutWidget);
        name->setObjectName("name");
        sizePolicy.setHeightForWidth(name->sizePolicy().hasHeightForWidth());
        name->setSizePolicy(sizePolicy);
        name->setMinimumSize(QSize(0, 32));
        name->setMaximumSize(QSize(16777215, 35));

        position->setWidget(0, QFormLayout::FieldRole, name);


        retranslateUi(nifo__ui__name_panel);

        QMetaObject::connectSlotsByName(nifo__ui__name_panel);
    } // setupUi

    void retranslateUi(QWidget *nifo__ui__name_panel)
    {
        nifo__ui__name_panel->setWindowTitle(QCoreApplication::translate("nifo::ui::name_panel", "name", nullptr));
        label_x_pos->setText(QCoreApplication::translate("nifo::ui::name_panel", "\345\220\215\347\247\260", nullptr));
    } // retranslateUi

};

} // namespace ui
} // namespace nifo

namespace nifo {
namespace ui {
namespace Ui {
    class name_panel: public Ui_name_panel {};
} // namespace Ui
} // namespace ui
} // namespace nifo

#endif // NAME_PANEL_H
