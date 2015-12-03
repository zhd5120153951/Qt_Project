#include <QRect>
#include <QSpinBox>
#include <QPushButton>
#include <QStyleOption>
#include <QStyleOptionButton>
#include "GLDEllipsis.h"
#include "GLDStylePaintUtils.h"

void GLDStylePaintUtils::initComboBoxOpt(QStyleOptionComboBox &opt, const QRect &rect)
{
    QStyleOptionComboBox optTmp;

    opt.editable = true;
    opt.frame = optTmp.frame;
    opt.subControls = (QStyle::SubControls)5;//opt.subControls;
    opt.activeSubControls = optTmp.activeSubControls;
    opt.state = optTmp.state;
    opt.state |= QStyle::State_Enabled;
    opt.rect = rect;
}

void GLDStylePaintUtils::initPushButtonOpt(QStyleOptionButton &opt, const QRect &rect, const QString &btnText)
{
    static QPushButton s_btn;
    QPushButton *pBtn = &s_btn;
    {
        opt.initFrom(pBtn);
        opt.features = QStyleOptionButton::None;
        opt.text = btnText;
        opt.fontMetrics = s_btn.fontMetrics();
        opt.direction = Qt::LeftToRight;
    }
    QRect hostRect = rect;
    int nTargetBtnWidth = qMax(c_MinButtonEditBtnWidth, opt.fontMetrics.width(" ..."));
    opt.rect = QRect(hostRect.right() - nTargetBtnWidth + 1,
        hostRect.top() - 1, nTargetBtnWidth, hostRect.height() + 2);
    opt.state |= QStyle::State_Enabled;
}

void GLDStylePaintUtils::initSpinBoxOpt(QStyleOptionSpinBox &opt, const QRect &rect)
{
    static QSpinBox s_pinBox;
    QWidget *target = &s_pinBox;
    opt.initFrom(target);
    opt.activeSubControls = QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown;
    opt.subControls |= QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown;
    opt.state |= QStyle::State_Enabled;
    opt.stepEnabled = QAbstractSpinBox::StepDownEnabled | QAbstractSpinBox::StepUpEnabled;
    opt.frame = false;//target->frameSize()
    opt.rect = rect;
}

GLDStylePaintUtils::GLDStylePaintUtils()
{
}
