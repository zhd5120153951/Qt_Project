#include "GLDFontList.h"
#include <QFontDatabase>
#include <QFontComboBox>
#include <QAbstractItemView>
#include <QFont>
#include <QPainter>

GFontList::GFontList(QWidget *parent) : QFontComboBox(parent),
    m_bFirstPopup(0)
{
    setItemDelegate(new GlodonFontFamilyDelegate(this));
}

GFontList::~GFontList()
{

}

void GFontList::showPopup()
{
    QFontComboBox::showPopup();
    if (view() && view()->parentWidget())
    {
        QRect rect = this->view()->parentWidget()->geometry();
        int ncomboBoxEditorWidth = this->geometry().width();
        QFrame* popupViewContainer = dynamic_cast<QFrame*>(view()->parentWidget());
        if (popupViewContainer)
        {
            if (0 == m_bFirstPopup)
            {
                m_bFirstPopup = 1;
                if (popupViewContainer->minimumWidth() > ncomboBoxEditorWidth)
                {
                    popupViewContainer->setMinimumWidth(ncomboBoxEditorWidth);
                }

                this->view()->resize(ncomboBoxEditorWidth, rect.height());
                this->view()->update();
                popupViewContainer->resize(ncomboBoxEditorWidth, rect.height());
                GFontList::showPopup();
            }
        }
    }
    if (m_bFirstPopup > 0)
    {
        m_bFirstPopup = 0;
    }
}

static QFontDatabase::WritingSystem writingSystemForFont(const QFont &font, bool *hasLatin)
{
    QList<QFontDatabase::WritingSystem> writingSystems = QFontDatabase().writingSystems(font.family());

    // this just confuses the algorithm below. Vietnamese is Latin with lots of special chars
    writingSystems.removeOne(QFontDatabase::Vietnamese);
    *hasLatin = writingSystems.removeOne(QFontDatabase::Latin);

    if (writingSystems.isEmpty())
        return QFontDatabase::Any;

    QFontDatabase::WritingSystem system = writingSystems.last();

    if (!*hasLatin) {
        // we need to show something
        return system;
    }

    if (writingSystems.count() == 1 && system > QFontDatabase::Cyrillic)
        return system;

    if (writingSystems.count() <= 2 && system > QFontDatabase::Armenian && system < QFontDatabase::Vietnamese)
        return system;

    if (writingSystems.count() <= 5 && system >= QFontDatabase::SimplifiedChinese && system <= QFontDatabase::Korean)
        return system;

    return QFontDatabase::Any;
}

GlodonFontFamilyDelegate::GlodonFontFamilyDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    truetype = QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/fonttruetype-16.png"));
    bitmap = QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/fontbitmap-16.png"));
    writingSystem = QFontDatabase::SimplifiedChinese;
}

void GlodonFontFamilyDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
    font.setPointSize(10/*QFontInfo(font).pointSize() * 3 / 2*/);
    QFont font2 = font;
    font2.setFamily(text);

    bool hasLatin;
    QFontDatabase::WritingSystem system = writingSystemForFont(font2, &hasLatin);
    if (hasLatin)
        font = font2;

    QRect r = option.rect;

    if (option.state & QStyle::State_Selected) {
        painter->save();
        painter->setBrush(option.palette.highlight());
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->setPen(QPen(option.palette.highlightedText(), 0));
        painter->restore();
    }

    const QIcon *icon = &bitmap;
    if (QFontDatabase().isSmoothlyScalable(text)) {
        icon = &truetype;
    }
    QSize actualSize = icon->actualSize(r.size());

    icon->paint(painter, r, Qt::AlignLeft|Qt::AlignVCenter);
    if (option.direction == Qt::RightToLeft)
        r.setRight(r.right() - actualSize.width() - 4);
    else
        r.setLeft(r.left() + actualSize.width() + 4);

    QFont old = painter->font();
    painter->setFont(font);

    // If the ascent of the font is larger than the height of the rect,
    // we will clip the text, so it's better to align the tight bounding rect in this case
    // This is specifically for fonts where the ascent is very large compared to
    // the descent, like certain of the Stix family.
    QFontMetricsF fontMetrics(font);
    if (fontMetrics.ascent() > r.height()) {
        QRectF tbr = fontMetrics.tightBoundingRect(text);
        painter->drawText(r.x(), r.y() + (r.height() + tbr.height()) / 2.0, text);
    } else {
        painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, text);
    }

    if (writingSystem != QFontDatabase::Any)
        system = writingSystem;

//    if (system != QFontDatabase::Any) {
//        int w = painter->fontMetrics().width(text + QLatin1String("  "));
//        painter->setFont(font2);
//        QString sample = QFontDatabase().writingSystemSample(system);
//        if (option.direction == Qt::RightToLeft)
//            r.setRight(r.right() - w);
//        else
//            r.setLeft(r.left() + w);
//        painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, sample);
//    }
    painter->setFont(old);

    if (option.state & QStyle::State_Selected)
        painter->restore();

}

QSize GlodonFontFamilyDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
//     font.setFamily(text);
    font.setPointSize(QFontInfo(font).pointSize() * 3/2);
    QFontMetrics fontMetrics(font);
    return QSize(fontMetrics.width(text), fontMetrics.height());
}

