#include "GLDImageEditor.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>

#include "GLDStrUtils.h"
#include "GLDStrings.h"

GImageEditor::GImageEditor(QWidget *parent) :
    QWidget(parent)
{
    m_frame = new QFrame(this);
    m_frame->move(this->pos());
    m_frame->resize(this->frameSize());
    m_frame->setStyleSheet("QFrame {background-color: rgb(255,255,255);color: rgb(0,0,0);}");

    m_button = new QPushButton("...", m_frame);
    m_label = new QLabel(m_frame);
    m_label->setAttribute(Qt::WA_TranslucentBackground, true);

    setFocusProxy(m_label);
    setFocusPolicy(Qt::StrongFocus);

    m_label->move(m_frame->x(),m_frame->y());
    m_label->resize(m_frame->frameSize().width() - m_button->fontMetrics().width("..."),
                    m_frame->frameSize().height());
    m_button->move(m_frame->x() + m_frame->frameSize().width() - m_button->fontMetrics().width("...") - 2,
                   m_frame->y());
    m_button->resize(m_button->fontMetrics().width("...") + 2,
                     m_frame->frameSize().height());
    connect(m_button, SIGNAL(clicked()), this, SLOT(editorButtonClicked()));
}

GImageEditor::~GImageEditor()
{
    if (m_button != NULL)
    {
        delete m_button;
    }
    if (m_label != NULL)
    {
        delete m_label;
    }
    if (m_frame != NULL)
    {
        delete m_frame;
    }
}

void GImageEditor::paintEvent(QPaintEvent *)
{
    m_frame->resize(this->size());

    m_label->resize(m_frame->frameSize().width() - m_button->fontMetrics().width("...") - 2,
                    m_frame->frameSize().height());

    m_button->move(m_label->x() + m_frame->frameSize().width() - m_button->fontMetrics().width("...") - 2,
                   m_label->y());

    m_button->resize(m_button->fontMetrics().width("...") + 2,
                     m_frame->frameSize().height());
}

void GImageEditor::setImage(QImage img)
{
    QImage result = img.scaled(m_label->width(), m_label->height(), Qt::KeepAspectRatio);
    m_label->setPixmap(QPixmap::fromImage(result));
}

QImage GImageEditor::image()
{
    const QPixmap *pixmap = m_label->pixmap();
    QImage img = pixmap->toImage();
    return img;
}

void GImageEditor::editorButtonClicked()
{
    QString strFileName = QFileDialog::getOpenFileName(this, getGLDi18nStr(g_SelectImage), "",
                                                    "Images (*.png *.xpm *.jpg *.jpeg *.gif *.bmp *.xbm)");
    QImage img;
    if (img.load(strFileName))
    {
        QImage result = img.scaled(m_label->width(), m_label->height(), Qt::KeepAspectRatio);
        setImage(result);
    }
}
