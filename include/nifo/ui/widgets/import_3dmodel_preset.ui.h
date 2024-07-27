/********************************************************************************
** Form generated from reading UI file 'import_3dmodel_preset.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef IMPORT_3DMODEL_PRESET_H
#define IMPORT_3DMODEL_PRESET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "spinbox.h"

namespace nifo {
namespace ui {

class Ui_import_3dmodel_preset
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_forward;
    QComboBox *forward_axis;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_up;
    QComboBox *up_axis;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_scale;
    nifo::ui::floating_point_spinbox *scale;
    QRadioButton *flip_uv;

    void setupUi(QWidget *nifo__ui__import_3dmodel_preset)
    {
        if (nifo__ui__import_3dmodel_preset->objectName().isEmpty())
            nifo__ui__import_3dmodel_preset->setObjectName("nifo__ui__import_3dmodel_preset");
        nifo__ui__import_3dmodel_preset->resize(653, 463);
        layoutWidget = new QWidget(nifo__ui__import_3dmodel_preset);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(0, 10, 231, 241));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_forward = new QLabel(layoutWidget);
        label_forward->setObjectName("label_forward");
        label_forward->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout->addWidget(label_forward);

        forward_axis = new QComboBox(layoutWidget);
        forward_axis->addItem(QString());
        forward_axis->addItem(QString());
        forward_axis->addItem(QString());
        forward_axis->addItem(QString());
        forward_axis->addItem(QString());
        forward_axis->addItem(QString());
        forward_axis->setObjectName("forward_axis");

        horizontalLayout->addWidget(forward_axis);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_up = new QLabel(layoutWidget);
        label_up->setObjectName("label_up");
        label_up->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_2->addWidget(label_up);

        up_axis = new QComboBox(layoutWidget);
        up_axis->addItem(QString());
        up_axis->addItem(QString());
        up_axis->addItem(QString());
        up_axis->addItem(QString());
        up_axis->addItem(QString());
        up_axis->addItem(QString());
        up_axis->setObjectName("up_axis");

        horizontalLayout_2->addWidget(up_axis);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_scale = new QLabel(layoutWidget);
        label_scale->setObjectName("label_scale");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_scale->sizePolicy().hasHeightForWidth());
        label_scale->setSizePolicy(sizePolicy);
        label_scale->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_3->addWidget(label_scale);

        scale = new nifo::ui::floating_point_spinbox(layoutWidget);
        scale->setObjectName("scale");
        sizePolicy.setHeightForWidth(scale->sizePolicy().hasHeightForWidth());
        scale->setSizePolicy(sizePolicy);
        scale->setMinimumSize(QSize(0, 0));

        horizontalLayout_3->addWidget(scale);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 2);

        verticalLayout->addLayout(horizontalLayout_3);

        flip_uv = new QRadioButton(layoutWidget);
        flip_uv->setObjectName("flip_uv");
        flip_uv->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        flip_uv->setChecked(true);

        verticalLayout->addWidget(flip_uv);

#if QT_CONFIG(shortcut)
        label_forward->setBuddy(forward_axis);
        label_up->setBuddy(up_axis);
#endif // QT_CONFIG(shortcut)

        retranslateUi(nifo__ui__import_3dmodel_preset);

        forward_axis->setCurrentIndex(4);
        up_axis->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(nifo__ui__import_3dmodel_preset);
    } // setupUi

    void retranslateUi(QWidget *nifo__ui__import_3dmodel_preset)
    {
        nifo__ui__import_3dmodel_preset->setWindowTitle(QString());
        label_forward->setText(QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "\345\211\215\345\220\221\350\275\264", nullptr));
        forward_axis->setItemText(0, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+X", nullptr));
        forward_axis->setItemText(1, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-X", nullptr));
        forward_axis->setItemText(2, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+Y", nullptr));
        forward_axis->setItemText(3, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-Y", nullptr));
        forward_axis->setItemText(4, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+Z", nullptr));
        forward_axis->setItemText(5, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-Z", nullptr));

        label_up->setText(QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "\345\220\221\344\270\212\345\235\220\346\240\207\350\275\264", nullptr));
        up_axis->setItemText(0, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+X", nullptr));
        up_axis->setItemText(1, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-X", nullptr));
        up_axis->setItemText(2, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+Y", nullptr));
        up_axis->setItemText(3, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-Y", nullptr));
        up_axis->setItemText(4, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "+Z", nullptr));
        up_axis->setItemText(5, QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "-Z", nullptr));

        label_scale->setText(QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "\347\274\251\346\224\276", nullptr));
#if QT_CONFIG(whatsthis)
        flip_uv->setWhatsThis(QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "\346\262\277y\350\275\264\347\277\273\350\275\254UV\345\235\220\346\240\207", nullptr));
#endif // QT_CONFIG(whatsthis)
        flip_uv->setText(QCoreApplication::translate("nifo::ui::import_3dmodel_preset", "UV\347\277\273\350\275\254", nullptr));
    } // retranslateUi

};

} // namespace ui
} // namespace nifo

namespace nifo {
namespace ui {
namespace Ui {
    class import_3dmodel_preset: public Ui_import_3dmodel_preset {};
} // namespace Ui
} // namespace ui
} // namespace nifo

#endif // IMPORT_3DMODEL_PRESET_H
