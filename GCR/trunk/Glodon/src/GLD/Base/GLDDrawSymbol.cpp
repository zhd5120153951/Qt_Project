#include "GLDDrawSymbol.h"
#include "GLDStrUtils.h"
#include "GLDGlobal.h"

#include <QFontMetrics>
#include <QBitmap>
#include <math.h>

GlodonCustomDrawSymbol *GlodonMeterDrawSymbolFactory::create()
{
    return new GlodonMeterDrawSymbol();
}

GlodonCustomDrawSymbol *GlodonDiaDrawSymbolFactory::create()
{
    return new GlodonDiaDrawSymbol();
}

bool GlodonMeterDrawSymbol::canHandle(const QString &text)
{
    return includeM2M3(text) || includeSharp(text);
}

int GlodonMeterDrawSymbol::calcDrawLineLength(QPainter *painter, const QString &text, int &width)
{
    int result = 0;
    QString strSubStr;
    int nx, nWidth, nIndex, nLength;
    nx = width;
    nWidth = 0;
    nIndex = 0;
    nLength = text.length();
    while (nIndex < nLength)
    {
        if ((text.at(nIndex) == QChar::LineFeed))
        {
            result = nIndex + 1;
            width -= nx;
            return result;
        }
        strSubStr = text.at(nIndex);
        nIndex++;
//        if (nIndex < nLength)
//        {
//            strSubStr = strSubStr + text.at(nIndex);
//            nIndex ++;
//        }
        if (isTextM2M3(strSubStr))
        {
            nWidth = calcM2M3Length(painter);
        }
        else
        {
            nWidth = painter->fontMetrics().width(strSubStr);
        }
        if (nWidth <= nx)
        {
            nx -= nWidth;
        }
        else
            break;
    }
    width -= nx;
    if (nWidth < nx)
    {
        result = nLength;
    }
    else
    {
        nIndex--;
        result = nIndex - 1;
    }
    return result;
}

int GlodonMeterDrawSymbol::calcDrawTextLength(QPainter *painter, const QString &text, int width, int height)
{
    return painter->fontMetrics().width(text);
    G_UNUSED(height);
    G_UNUSED(width);
}

int GlodonMeterDrawSymbol::calcM2M3Length(QPainter *painter)
{
    int nSize, result;
    nSize = painter->font().pointSize();
    result = painter->fontMetrics().width(c_MeterSymbol);
    QFont tempFont = painter->font();
    tempFont.setPointSize(ceil(c_SuperScale * nSize));
    painter->setFont(tempFont);
    result = result + painter->fontMetrics().width(c_SuperSymbol);
    tempFont = painter->font();
    tempFont.setPointSize(nSize);

    painter->setFont(tempFont);
    return result;
}

void GlodonMeterDrawSymbol::drawText(QPainter *painter, int scale, const QString &text,
                                     Qt::Alignment align, QRect &rect)
{
    QString strText;
    int nHeight = 0;
    strText = replaceSymbolText(text);
    interDrawText(painter, strText, align, rect.left(), rect.top(), rect.right() - rect.left(), nHeight);
    G_UNUSED(scale);
}

bool GlodonMeterDrawSymbol::isTextM2M3(const QString &text)
{
    return sameText(text, c_SquareReplace) || sameText(text, c_CubeReplace);
}

bool GlodonMeterDrawSymbol::isTextSharp(const QString &text)
{
    return text == c_SharpReplace;
}

QString GlodonMeterDrawSymbol::replaceSymbolText(const QString &text)
{
    QString result = text;
    result.replace("m2", c_SquareReplace);
    result.replace("M2", c_SquareReplace);
    result.replace("m3", c_CubeReplace);
    result.replace("M3", c_CubeReplace);
    return result;
}

void GlodonMeterDrawSymbol::drawLine(QPainter *painter, const QString &text, int x, int y, int &width, int &height)
{
    int nWidth;
    int nHeight;
    width = 0;
    height = 0;
    int nx = x;
    int ny = y;
    int nIndex= 0;
    int nLength = text.length();
    QString strSubStr;
    while (nIndex < nLength)
    {
        strSubStr = text.at(nIndex);
        nIndex++;
//        if (nIndex < nLength)
//        {
//            strSubStr = strSubStr + text.at(nIndex);
//            nIndex ++;
//        }
        if (isTextM2M3(strSubStr))
        {
            drawM2M3(painter, nx, ny, strSubStr);
            nWidth = calcM2M3Length(painter);
        }
        else if (isTextSharp(strSubStr))
        {
            drawSharp(painter, nx, ny, strSubStr);
        }
        else
        {
            int nyPosition = ny + painter->fontMetrics().boundingRect(strSubStr).height();
            painter->drawText(nx, nyPosition, strSubStr);
            nWidth = painter->fontMetrics().width(strSubStr);
        }
        nHeight = painter->fontMetrics().boundingRect(strSubStr).height();//ACavas.TextHeight(strSubStr);
        nx += nWidth;
        height = qMax(height, nHeight);
    }
    width = nx - x;
}

void GlodonMeterDrawSymbol::drawM2M3(QPainter *painter, int x, int y, const QString &symbol)
{
    int nx, nSize;
    QString strSuper;
    int nyPosition = y + painter->fontMetrics().boundingRect(symbol).height();
    if (sameText(symbol, c_SquareReplace))
    {
        strSuper = c_SquareSymbol;
    }
    else if (sameText(symbol, c_CubeReplace))
    {
        strSuper = c_CubeSymbol;
    }
    else
    {
        painter->drawText(x, nyPosition, symbol);
    }
    nyPosition = y + painter->fontMetrics().boundingRect(c_MeterSymbol).height();
    painter->drawText(x, nyPosition, c_MeterSymbol);
    nx = x + painter->fontMetrics().width(c_MeterSymbol);
    QFont tempFont = painter->font();
    nSize = painter->font().pointSize();
    tempFont.setPointSize(c_SharpScale * nSize);
    painter->save();
    painter->setFont(tempFont);
    nyPosition = y + painter->fontMetrics().boundingRect(strSuper).height();
    painter->drawText(nx, nyPosition, strSuper);
    painter->restore();
}

void GlodonMeterDrawSymbol::drawSharp(QPainter *painter, int x, int y, const QString &symbol)
{
    int nSize;
    QString strSuper;
    int nyPosition = y + painter->fontMetrics().boundingRect(symbol).height();
    if (sameText(symbol, c_SharpReplace))
    {
        strSuper = c_SharpReplace;
    }
    else
    {

        painter->drawText(x, nyPosition, symbol);
    }
    nSize = painter->font().pointSize();
    QFont tempFont = painter->font();
    tempFont.setPointSize(c_SharpScale * nSize);
    painter->save();
    painter->setFont(tempFont);
    nyPosition = y + painter->fontMetrics().boundingRect(strSuper).height();
    painter->drawText(x, nyPosition, strSuper);
    tempFont.setPointSize(nSize);
    painter->setFont(tempFont);
    painter->restore();
}

void GlodonMeterDrawSymbol::interDrawText(QPainter *painter, const QString &text,
                                          Qt::Alignment align, int x, int y, int width, int &height)
{
    QString strText, strSubStr;
    int nx, ny, nWidth, nHeight, nSize;
    height = 0;
    strText = text;
    nx = x;
    ny = y;
    nWidth = width;
    nHeight = 0;
    nSize = calcDrawLineLength(painter, strText, nWidth);
    while (nSize != 0)
    {
        if (align & Qt::AlignLeft)
        {
            nx = x;
        }
        else if (align & Qt::AlignRight)
        {
            nx = x + (width - nWidth);
        }
        else
        {
            nx = x + (width - nWidth) / 2;
        }
        ny += nHeight;
        strSubStr = strText.mid(0, nSize);
        drawLine(painter, trimRight(strSubStr), nx, ny, nWidth, nHeight);;
        height += nHeight;
        strText.remove(0, nSize);
        nWidth = width;
        nSize = calcDrawLineLength(painter, strText, nWidth);
    }
}

bool GlodonDiaDrawSymbol::canHandle(const QString &text)
{
    return includeDiaSymbol(text);
}

int GlodonDiaDrawSymbol::getContentWidth(QPainter *painter, int scale, const QString &text)
{
    GObjectList<GlodonSymbolPos *> *list = createIndexOfDiameterSymbolList(text);
    // 计算文字和钢筋符号的宽度
    int nWidth = 0;
    int nIndex = 0;
    while (nIndex<text.size())
    {
        bool bSymbol = false;
        for (int nSymbol=0; nSymbol<list->count(); ++nSymbol)
        {
            if (nIndex == list->at(nSymbol)->pos)
            {
                bSymbol = true;
                QString strResource = c_DiameterSymbols[nSymbol].resourceName;
                QPixmap pixmap(":/icons/" + strResource);
                nWidth = nWidth + pixmap.width() * scale / 100;
                nIndex = list->at(nSymbol)->pos + c_DiameterSymbols[list->at(nSymbol)->symbolPosIndex].symbol.length();
            }
        }
        if (!bSymbol)
        {
            nWidth = nWidth + painter->fontMetrics().width(text.at(nIndex));
            nIndex++;
        }
    }
    freeAndNil(list);
    return nWidth;
}

int GlodonDiaDrawSymbol::getContentHeight(QPainter *painter, int scale, const QString &text)
{
    GObjectList<GlodonSymbolPos *> *list = createIndexOfDiameterSymbolList(text);
    // 计算文字和钢筋符号的宽度
    int nSymBolHeight = 0;
    int nTextHeight = 0;
    int nIndex = 0;
    while (nIndex<text.size())
    {
        bool bSymbol = false;
        for (int nSymbol=0; nSymbol<list->count(); ++nSymbol)
        {
            if (nIndex == list->at(nSymbol)->pos)
            {
                bSymbol = true;
                QString strResource = c_DiameterSymbols[nSymbol].resourceName;
                QPixmap pixmap(":/icons/" + strResource);
                if (pixmap.height() * scale / 100 > nSymBolHeight)
                {
                    nSymBolHeight = pixmap.height() * scale / 100;
                }
                nIndex = list->at(nSymbol)->pos + c_DiameterSymbols[list->at(nSymbol)->symbolPosIndex].symbol.length();
            }
        }
        if (nIndex < text.length())
        {
            if (painter->fontMetrics().width(text.at(nIndex)) > nTextHeight)
            {
                nTextHeight = painter->fontMetrics().height();
            }
        }
        nIndex++;
    }
    freeAndNil(list);
    return (nSymBolHeight > nTextHeight) ? nSymBolHeight : nTextHeight;
}

void GlodonDiaDrawSymbol::draw(QPainter *painter, int scale, const QString &text,
                               Qt::Alignment align, QRect &rect)
{
    int nPos;
    QRect cRect;
    QString strText;
    GObjectList<GlodonSymbolPos *> *list = createIndexOfDiameterSymbolList(text);

    try
    {
        cRect = rect;
        nPos = 0;
        int nContentWidth = getContentWidth(painter, scale, text);
        int nContentHeight = getContentHeight(painter, scale, text);

        // 根据对齐方式设置开始画的位置
        // 先做横向判断
        if (align & Qt::AlignLeft)
        {
            // DO NOthing
        }
        else if (align & Qt::AlignRight)
        {
            cRect.setLeft(rect.right() - nContentWidth);
        }
        else if (align & Qt::AlignHCenter)
        {
            cRect.setLeft((rect.left() + rect.right() - nContentWidth) / 2);
        }
        // 再做纵向判断
        if (align & Qt::AlignTop)
        {
            // DO NOthing
        }
        else if (align & Qt::AlignBottom)
        {
            cRect.setTop(rect.bottom() - nContentHeight);
        }
        else if (align & Qt::AlignVCenter)
        {
            cRect.setTop((rect.bottom() + rect.top() - nContentHeight) / 2);
            cRect.setBottom((rect.bottom() + rect.top() + nContentHeight) / 2);
        }

        // 开始画文本和钢筋符号的图片
        for (int i = 0; i < list->count(); ++i)
        {
            if (nPos <= list->at(i)->pos)
            {
                strText = copy(text, nPos, list->at(i)->pos - nPos);
                cRect.setRight(rect.right());
                innerDrawText(painter, align, cRect, strText);
                cRect.setLeft(cRect.left() + painter->fontMetrics().width(strText));
                nPos = list->at(i)->pos + c_DiameterSymbols[list->at(i)->symbolPosIndex].symbol.length();
                drawBitMap(painter, scale, align, cRect, list->at(i)->symbolPosIndex);
            }
        }
        if (nPos <= text.length())
        {
            strText = copy(text, nPos, MaxInt);
            cRect.setRight(rect.right());
            innerDrawText(painter, align, cRect, strText);
        }
    }
    catch (...)
    {
        freeAndNil(list);
        throw;
    }
    freeAndNil(list);
}

void GlodonDiaDrawSymbol::drawText(QPainter *painter, int scale, const QString &text,
                                   Qt::Alignment align, QRect &rect)
{
    draw(painter, scale, text, align, rect);
}

void GlodonDiaDrawSymbol::draw1Symbol(QPainter *painter, int scale, const QString &text,
                                      Qt::Alignment align, QRect &rect, int index)
{
    QRect cRect;
    QString strResource, strText;
    strResource = c_DiameterSymbols[index].resourceName;
    QPixmap pixmap(":/icons/" + strResource);
    pixmap.setMask(pixmap.createMaskFromColor(Qt::white));
    if (!pixmap.isNull())
    {
        cRect = rect;
        int nContentWidth = getContentWidth(painter, scale, text);
        if (align & Qt::AlignLeft)
        {
            // DO NOthing
        }
        else if (align & Qt::AlignRight)
        {
            cRect.setLeft(rect.right() - nContentWidth);
        }
        else
        {
            cRect.setLeft((rect.left() + rect.right() - nContentWidth) / 2);
        }

        cRect.setRight(cRect.left() + pixmap.width() * scale / 100);
        cRect.setTop((cRect.top() + cRect.bottom() - pixmap.height()) / 2);
        cRect.setBottom(cRect.top() + pixmap.height() * scale / 100);
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->drawPixmap(cRect, pixmap);
    }
    strText = copy(text, c_DiameterSymbols[index].symbol.length(), MaxInt);
    if (strText != "")
    {
        if (align & Qt::AlignRight)
        {
            cRect.setRight(cRect.left());
            cRect.setLeft(rect.left());
        }
        else
        {
            cRect.setLeft(cRect.right());
            cRect.setRight(rect.right());
        }
        cRect.setTop((cRect.top() + cRect.bottom() - pixmap.height()) / 2);
        cRect.setBottom(rect.bottom());
        painter->drawText(cRect, strText);
    }
}

void GlodonDiaDrawSymbol::drawBitMap(QPainter *painter, int scale, Qt::Alignment align, QRect &rect, int index)
{
    QString strResource = c_DiameterSymbols[index].resourceName;
    QPixmap pixmap(":/icons/" + strResource);
    pixmap.setMask(pixmap.createMaskFromColor(Qt::white));

    QRect symBolRect = rect;

    if (pixmap.isNull())
    {
        return;
    }
    else
    {
        symBolRect.setRight(symBolRect.left() + pixmap.width() * scale / 100);
        symBolRect.setBottom(symBolRect.top() + pixmap.height() * scale / 100);
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->drawPixmap(symBolRect, pixmap);
        rect.setLeft(symBolRect.right());
    }
    G_UNUSED(align);
}

void GlodonDiaDrawSymbol::innerDrawText(QPainter *painter, Qt::Alignment align, QRect &rect, const QString &text)
{
    if (text != "")
    {
        painter->drawText(rect, text);
    }
    G_UNUSED(align);
}

GObjectList<GlodonSymbolPos *> *createIndexOfDiameterSymbolList(const QString &text)
{
    int nIndex, nPos;
    GlodonSymbolPos *pSymbolPos = NULL;
    GObjectList<GlodonSymbolPos *> *list = new GObjectList<GlodonSymbolPos *>();
    for (int i = 0; i < int(sizeof(c_DiameterSymbols) / sizeof(c_DiameterSymbols[0])); ++i)
    {
        nIndex = 0;
        nPos = pos(c_DiameterSymbols[i].symbol, text, nIndex);//rPosEx(c_DiameterSymbols[i].symbol, text, nIndex);

        while (nPos > -1)
        {
            pSymbolPos = new GlodonSymbolPos;
            pSymbolPos->pos = nPos;
            pSymbolPos->symbolPosIndex = i;
            list->push_back(pSymbolPos);
            nIndex = nPos + c_DiameterSymbols[i].symbol.length() - 1;
            nPos = pos(c_DiameterSymbols[i].symbol, text, nIndex);//rPosEx(c_DiameterSymbols[i].symbol, text, nIndex);
        }
    }
    if (list->count() > 0)
    {
        qSort(list->list().begin(), list->list().end(), compare);
    }
    return list;
}

bool compare(GlodonSymbolPos *item1, GlodonSymbolPos *item2)
{
    if (item1->pos > item2->pos)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void drawDiaSymbol(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect)
{
    GlodonDiaDrawSymbol diaDrawSymbol;
    diaDrawSymbol.drawText(painter, scale, text, align, rect);
}

bool includeM2M3(const QString &text)
{
    return text.contains("m2") || text.contains("M2") || text.contains("m3") || text.contains("M3");
}

void drawTextMx(QPainter *painter, const QString &text, Qt::Alignment align, QRect &rect)
{
    GlodonMeterDrawSymbol cDrawSymbol;
    cDrawSymbol.drawText(painter, 100, text, align, rect);
}

bool includeSharp(const QString &text)
{
    return text.contains("#");
}

bool includeDiaSymbol(const QString &text)
{
    GObjectList<GlodonSymbolPos *> *list = NULL;
    bool result;
    try
    {
        list = createIndexOfDiameterSymbolList(text);
        result = list->count() > 0;
    }
    catch (...)
    {
        freeAndNil(list);
        throw;
    }
    freeAndNil(list);
    return result;
}

GlodonCustomDrawSymbol::GlodonCustomDrawSymbol(QObject *parent)
{
    G_UNUSED(parent);
}

#ifdef _FONTSYMBOL_
GlodonSymbols::GlodonSymbols()
{
    initSymbols(c_DiameterSymbols,
                sizeof(c_DiameterSymbols)/sizeof(GlodonDiameterSymbolRec));
}

QString GlodonSymbols::aliasFromDisplaySymbol(const QString &displaySymbol)
{
    if (displaySymbol.isEmpty())
    {
        return "";
    }

    QString alias = m_symbols.key(displaySymbol);
    return m_symbols.contains(alias) ? alias : displaySymbol;
}

QString GlodonSymbols::displaySymbolFromAlias(const QString &alias)
{
    if (alias.isEmpty())
    {
        return "";
    }

    return m_symbols.contains(alias) ? m_symbols.value(alias) : alias;
}

QMap<QString, QString> GlodonSymbols::allSymbols() const
{
    return m_symbols;
}

QString GlodonSymbols::aliasesFromDisplaySymbols(const QString &displaySymbols)
{
    QStringList displaySymbolList;
    QString aliases;
    // 将钢筋符号串拆分放到list中
    for (int i = 0; i < displaySymbols.length(); ++i)
    {
        displaySymbolList.push_back(displaySymbols.mid(i, 1));
    }
    // 再将钢筋符号的别名加到串中
    foreach (QString subSymbol, displaySymbolList)
    {
        aliases += aliasFromDisplaySymbol(subSymbol);
    }
    return aliases;
}

QString GlodonSymbols::displaySymbolsFromAliases(const QString &aliases)
{
    GObjectList<GlodonSymbolPos *> *list = createIndexOfDiameterSymbolList(aliases);
    int nPos = 0;
    // 用于截取每一位符号的别名
    QString strText;
    QString displaySymbols;
    for (int i = 0; i < list->count(); ++i)
    {
        if (nPos <= list->at(i)->pos)
        {
            strText = copy(aliases, nPos, list->at(i)->pos - nPos);
            nPos = list->at(i)->pos;
            // 转为用于显示的钢筋符号， 并连接在钢筋符号串中
            displaySymbols += displaySymbolFromAlias(strText);
        }
    }
    // 获取最后一个可识别的钢筋符号的长度
    int lastSymbolLength = 0;
    for (int i = 0; i < int(sizeof(c_DiameterSymbols) / sizeof(c_DiameterSymbols[0])); ++i)
    {
        if (aliases.indexOf(c_DiameterSymbols[i].symbol, nPos) > -1)
        {
            lastSymbolLength = c_DiameterSymbols[i].symbol.length();
        }
    }

    if (nPos <= aliases.length())
    {
        // 最后一个可识别的钢筋符号加到符号串中
        strText = copy(aliases, nPos, lastSymbolLength);
        displaySymbols += displaySymbolFromAlias(strText);
        // 钢筋符号串之后的内容原样加到符号串中
        strText = copy(aliases, nPos + lastSymbolLength, MaxInt);
        displaySymbols += strText;
    }
    return displaySymbols;
}

void GlodonSymbols::initSymbols(const GlodonDiameterSymbolRec array[], size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        GlodonDiameterSymbolRec symbol = array[i];
        m_symbols.insert(symbol.symbol, symbol.resourceName);
    }
}

#endif

