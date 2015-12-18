#include "GLDCustomButton.h"
#include "GLDIrregularForm.h"

#include <QPainter>

namespace GlodonMask
{
    GLDIrregularForm::GLDIrregularForm(QWidget *parent)
        : QWidget(parent)
        , m_pCustomBtn(nullptr)
        , m_irregularFormPm("")
        , m_xPosition(0)
        , m_yPosition(0)
    {
        setFlagAndAttribute();
    }

    GLDIrregularForm::GLDIrregularForm(const QString & irregularImgPath, const QString & btnImgPath, QWidget *parent)
        : QWidget(parent)
        , m_pCustomBtn(new GLDCustomButton(btnImgPath, this))
        , m_irregularFormPm("")
        , m_xPosition(0)
        , m_yPosition(0)
    {
        setFlagAndAttribute();

        loadPixmap(irregularImgPath);

        connect(m_pCustomBtn, &QPushButton::clicked, this, &GLDIrregularForm::irregularFormClicked);
    }

    void GLDIrregularForm::setFlagAndAttribute()
    {
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    }

    GLDIrregularForm::~GLDIrregularForm()
    {

    }

    QSize GLDIrregularForm::sizeHint() const
    {
        if (!m_irregularFormPm.isNull())
        {
            return m_irregularFormPm.size();
        }

        return QWidget::sizeHint();
    }

    void GLDIrregularForm::loadPixmap(const QString & pixmapPath)
    {
        QPixmap tempPixmap;
        tempPixmap.load(pixmapPath);
        setPixmap(tempPixmap);
    }

    void GLDIrregularForm::setPixmap(const QPixmap & pm)
    {
        m_irregularFormPm = pm;
        update();
    }

    void GLDIrregularForm::setBtnPos(QPushButton *pBtn, const int x, const int y)
    {
        Q_UNUSED(pBtn);

        m_xPosition = x;
        m_yPosition = y;
    }

    void GLDIrregularForm::paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);

        m_pCustomBtn->move(m_xPosition, m_yPosition);

        painter.drawPixmap(0, 0, m_irregularFormPm);

        QWidget::paintEvent(event);
    }
}

