/********************************************************************************
** Form generated from reading UI file 'transform_panel.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef TRANSFORM_PANEL_H
#define TRANSFORM_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "../widgets/spinbox.h"

namespace nifo {
namespace ui {

class Ui_transform_panel
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFormLayout *position;
    QLabel *label_x_pos;
    floating_point_spinbox *position_x;
    QLabel *label_y_pos;
    floating_point_spinbox *position_y;
    QLabel *label_z_pos;
    floating_point_spinbox *position_z;
    QFormLayout *rotate;
    QLabel *label_x_rot;
    floating_point_spinbox *rotate_x;
    QLabel *label_y_rot;
    floating_point_spinbox *rotate_y;
    QLabel *label_z_rot;
    floating_point_spinbox *rotate_z;
    QFormLayout *scale;
    QLabel *label_x_scl;
    floating_point_spinbox *scale_x;
    QLabel *label_y_scl;
    floating_point_spinbox *scale_y;
    QLabel *label_z_scl;
    floating_point_spinbox *scale_z;

    void setupUi(QWidget *nifo__ui__transform_panel)
    {
        if (nifo__ui__transform_panel->objectName().isEmpty())
            nifo__ui__transform_panel->setObjectName("nifo__ui__transform_panel");
        nifo__ui__transform_panel->resize(702, 445);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(nifo__ui__transform_panel->sizePolicy().hasHeightForWidth());
        nifo__ui__transform_panel->setSizePolicy(sizePolicy);
        layoutWidget = new QWidget(nifo__ui__transform_panel);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 40, 211, 301));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        position = new QFormLayout();
        position->setObjectName("position");
        position->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        position->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        position->setFormAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        position->setVerticalSpacing(0);
        label_x_pos = new QLabel(layoutWidget);
        label_x_pos->setObjectName("label_x_pos");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_x_pos->sizePolicy().hasHeightForWidth());
        label_x_pos->setSizePolicy(sizePolicy1);
        label_x_pos->setMinimumSize(QSize(0, 32));
        label_x_pos->setMaximumSize(QSize(16777215, 35));
        label_x_pos->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        position->setWidget(0, QFormLayout::LabelRole, label_x_pos);

        position_x = new floating_point_spinbox(layoutWidget);
        position_x->setObjectName("position_x");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(position_x->sizePolicy().hasHeightForWidth());
        position_x->setSizePolicy(sizePolicy2);
        position_x->setMinimumSize(QSize(0, 32));
        position_x->setMaximumSize(QSize(16777215, 35));

        position->setWidget(0, QFormLayout::FieldRole, position_x);

        label_y_pos = new QLabel(layoutWidget);
        label_y_pos->setObjectName("label_y_pos");
        sizePolicy1.setHeightForWidth(label_y_pos->sizePolicy().hasHeightForWidth());
        label_y_pos->setSizePolicy(sizePolicy1);
        label_y_pos->setMinimumSize(QSize(0, 32));
        label_y_pos->setMaximumSize(QSize(16777215, 35));
        label_y_pos->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        position->setWidget(1, QFormLayout::LabelRole, label_y_pos);

        position_y = new floating_point_spinbox(layoutWidget);
        position_y->setObjectName("position_y");
        sizePolicy2.setHeightForWidth(position_y->sizePolicy().hasHeightForWidth());
        position_y->setSizePolicy(sizePolicy2);
        position_y->setMinimumSize(QSize(0, 32));
        position_y->setMaximumSize(QSize(16777215, 35));

        position->setWidget(1, QFormLayout::FieldRole, position_y);

        label_z_pos = new QLabel(layoutWidget);
        label_z_pos->setObjectName("label_z_pos");
        sizePolicy1.setHeightForWidth(label_z_pos->sizePolicy().hasHeightForWidth());
        label_z_pos->setSizePolicy(sizePolicy1);
        label_z_pos->setMinimumSize(QSize(0, 32));
        label_z_pos->setMaximumSize(QSize(16777215, 35));
        label_z_pos->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        position->setWidget(2, QFormLayout::LabelRole, label_z_pos);

        position_z = new floating_point_spinbox(layoutWidget);
        position_z->setObjectName("position_z");
        sizePolicy2.setHeightForWidth(position_z->sizePolicy().hasHeightForWidth());
        position_z->setSizePolicy(sizePolicy2);
        position_z->setMinimumSize(QSize(0, 32));
        position_z->setMaximumSize(QSize(16777215, 35));

        position->setWidget(2, QFormLayout::FieldRole, position_z);


        verticalLayout->addLayout(position);

        rotate = new QFormLayout();
        rotate->setObjectName("rotate");
        rotate->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        rotate->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        rotate->setFormAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        rotate->setVerticalSpacing(0);
        label_x_rot = new QLabel(layoutWidget);
        label_x_rot->setObjectName("label_x_rot");
        sizePolicy1.setHeightForWidth(label_x_rot->sizePolicy().hasHeightForWidth());
        label_x_rot->setSizePolicy(sizePolicy1);
        label_x_rot->setMinimumSize(QSize(0, 32));
        label_x_rot->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(0, QFormLayout::LabelRole, label_x_rot);

        rotate_x = new floating_point_spinbox(layoutWidget);
        rotate_x->setObjectName("rotate_x");
        sizePolicy2.setHeightForWidth(rotate_x->sizePolicy().hasHeightForWidth());
        rotate_x->setSizePolicy(sizePolicy2);
        rotate_x->setMinimumSize(QSize(0, 32));
        rotate_x->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(0, QFormLayout::FieldRole, rotate_x);

        label_y_rot = new QLabel(layoutWidget);
        label_y_rot->setObjectName("label_y_rot");
        sizePolicy1.setHeightForWidth(label_y_rot->sizePolicy().hasHeightForWidth());
        label_y_rot->setSizePolicy(sizePolicy1);
        label_y_rot->setMinimumSize(QSize(0, 32));
        label_y_rot->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(1, QFormLayout::LabelRole, label_y_rot);

        rotate_y = new floating_point_spinbox(layoutWidget);
        rotate_y->setObjectName("rotate_y");
        sizePolicy2.setHeightForWidth(rotate_y->sizePolicy().hasHeightForWidth());
        rotate_y->setSizePolicy(sizePolicy2);
        rotate_y->setMinimumSize(QSize(0, 32));
        rotate_y->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(1, QFormLayout::FieldRole, rotate_y);

        label_z_rot = new QLabel(layoutWidget);
        label_z_rot->setObjectName("label_z_rot");
        sizePolicy1.setHeightForWidth(label_z_rot->sizePolicy().hasHeightForWidth());
        label_z_rot->setSizePolicy(sizePolicy1);
        label_z_rot->setMinimumSize(QSize(0, 32));
        label_z_rot->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(2, QFormLayout::LabelRole, label_z_rot);

        rotate_z = new floating_point_spinbox(layoutWidget);
        rotate_z->setObjectName("rotate_z");
        sizePolicy2.setHeightForWidth(rotate_z->sizePolicy().hasHeightForWidth());
        rotate_z->setSizePolicy(sizePolicy2);
        rotate_z->setMinimumSize(QSize(0, 32));
        rotate_z->setMaximumSize(QSize(16777215, 35));

        rotate->setWidget(2, QFormLayout::FieldRole, rotate_z);


        verticalLayout->addLayout(rotate);

        scale = new QFormLayout();
        scale->setObjectName("scale");
        scale->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        scale->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
        scale->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
        scale->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        scale->setFormAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        scale->setVerticalSpacing(0);
        label_x_scl = new QLabel(layoutWidget);
        label_x_scl->setObjectName("label_x_scl");
        sizePolicy1.setHeightForWidth(label_x_scl->sizePolicy().hasHeightForWidth());
        label_x_scl->setSizePolicy(sizePolicy1);
        label_x_scl->setMinimumSize(QSize(0, 32));
        label_x_scl->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(0, QFormLayout::LabelRole, label_x_scl);

        scale_x = new floating_point_spinbox(layoutWidget);
        scale_x->setObjectName("scale_x");
        sizePolicy2.setHeightForWidth(scale_x->sizePolicy().hasHeightForWidth());
        scale_x->setSizePolicy(sizePolicy2);
        scale_x->setMinimumSize(QSize(0, 32));
        scale_x->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(0, QFormLayout::FieldRole, scale_x);

        label_y_scl = new QLabel(layoutWidget);
        label_y_scl->setObjectName("label_y_scl");
        sizePolicy1.setHeightForWidth(label_y_scl->sizePolicy().hasHeightForWidth());
        label_y_scl->setSizePolicy(sizePolicy1);
        label_y_scl->setMinimumSize(QSize(0, 32));
        label_y_scl->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(1, QFormLayout::LabelRole, label_y_scl);

        scale_y = new floating_point_spinbox(layoutWidget);
        scale_y->setObjectName("scale_y");
        sizePolicy2.setHeightForWidth(scale_y->sizePolicy().hasHeightForWidth());
        scale_y->setSizePolicy(sizePolicy2);
        scale_y->setMinimumSize(QSize(0, 32));
        scale_y->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(1, QFormLayout::FieldRole, scale_y);

        label_z_scl = new QLabel(layoutWidget);
        label_z_scl->setObjectName("label_z_scl");
        sizePolicy1.setHeightForWidth(label_z_scl->sizePolicy().hasHeightForWidth());
        label_z_scl->setSizePolicy(sizePolicy1);
        label_z_scl->setMinimumSize(QSize(0, 32));
        label_z_scl->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(2, QFormLayout::LabelRole, label_z_scl);

        scale_z = new floating_point_spinbox(layoutWidget);
        scale_z->setObjectName("scale_z");
        sizePolicy2.setHeightForWidth(scale_z->sizePolicy().hasHeightForWidth());
        scale_z->setSizePolicy(sizePolicy2);
        scale_z->setMinimumSize(QSize(0, 32));
        scale_z->setMaximumSize(QSize(16777215, 35));

        scale->setWidget(2, QFormLayout::FieldRole, scale_z);


        verticalLayout->addLayout(scale);


        retranslateUi(nifo__ui__transform_panel);

        QMetaObject::connectSlotsByName(nifo__ui__transform_panel);
    } // setupUi

    void retranslateUi(QWidget *nifo__ui__transform_panel)
    {
        nifo__ui__transform_panel->setWindowTitle(QCoreApplication::translate("nifo::ui::transform_panel", "transform_panel", nullptr));
        label_x_pos->setText(QCoreApplication::translate("nifo::ui::transform_panel", "\344\275\215\347\275\256 X", nullptr));
        label_y_pos->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Y", nullptr));
        label_z_pos->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Z", nullptr));
        label_x_rot->setText(QCoreApplication::translate("nifo::ui::transform_panel", "\346\227\213\350\275\254 X", nullptr));
        label_y_rot->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Y", nullptr));
        label_z_rot->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Z", nullptr));
        label_x_scl->setText(QCoreApplication::translate("nifo::ui::transform_panel", "\347\274\251\346\224\276 X", nullptr));
        label_y_scl->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Y", nullptr));
        label_z_scl->setText(QCoreApplication::translate("nifo::ui::transform_panel", "Z", nullptr));
    } // retranslateUi

};

} // namespace ui
} // namespace nifo

namespace nifo {
namespace ui {
namespace Ui {
    class transform_panel: public Ui_transform_panel {};
} // namespace Ui
} // namespace ui
} // namespace nifo

#endif // TRANSFORM_PANEL_H
