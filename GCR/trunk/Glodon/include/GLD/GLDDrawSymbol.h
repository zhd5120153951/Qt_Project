#ifndef GLDDRAWSYMBOL_H
#define GLDDRAWSYMBOL_H
#include <QString>
#include <QRect>
#include <QPainter>
#include "GLDString.h"
#include "GLDObjectList.h"
#include "GLDStrUtils.h"
#include "GLDTableView_Global.h"

struct GlodonDiameterSymbolRec
{
    GString symbol;
    GString resourceName;
};

struct GlodonSymbolPos
{
    int pos;
    int symbolPosIndex;
};

const  GString c_MeterSymbol        = "m";

const  GString c_SquareSymbol       = "2";
const  GString c_CubeSymbol         = "3";

const  GString c_SquareReplace      = TRANS_STRING("弍");//这两个字符是为了替换m2,m3做标识用，不用翻译
const  GString c_CubeReplace        = TRANS_STRING("弎");
const  GString c_SharpReplace       = "#";

const  GString c_SuperSymbol        = "3";
const  double c_SuperScale          = 0.5;
const  double c_SharpScale          = 0.7;

#ifndef _FONTSYMBOL_
    #ifdef __APPLE__
    const GlodonDiameterSymbolRec c_DiameterSymbols [] =
    {
          {TRANS_STRING("Φ16"), "SDIABFE"},
          {TRANS_STRING("Φ15"), "SDIAEEE"},
          {TRANS_STRING("Φ13"), "SDIABE"},
          {TRANS_STRING("Φ14"), "SDIACE"},
          {TRANS_STRING("Φ17"), "SDIACFE"},
          {TRANS_STRING("Φ18"), "SDIAEFE"},
          {TRANS_STRING("ΦA"), "SDIAA"},
          {TRANS_STRING("ΦB"), "SDIAB"},
          {TRANS_STRING("Φ2"), "SDIABF"},
          {TRANS_STRING("ΦC"), "SDIAC"},
          {TRANS_STRING("Φ3"), "SDIACF"},
          {TRANS_STRING("ΦD"), "SDIAD"},
          {TRANS_STRING("ΦE"), "SDIAE"},
          {TRANS_STRING("Φ5"), "SDIAEF"},
          {TRANS_STRING("ΦG"), "SDIAG"},
          {TRANS_STRING("ΦL"), "SDIAL"},
          {TRANS_STRING("ΦN"), "SDIAN"},
          {TRANS_STRING("ΦY"), "SDIAY"}
    };
    #else
    const GlodonDiameterSymbolRec c_DiameterSymbols [] =
    {
          {GString::fromLocal8Bit("Φ16"), "SDIABFE"},
          {GString::fromLocal8Bit("Φ15"), "SDIAEEE"},
          {GString::fromLocal8Bit("Φ13"), "SDIABE"},
          {GString::fromLocal8Bit("Φ14"), "SDIACE"},
          {GString::fromLocal8Bit("Φ17"), "SDIACFE"},
          {GString::fromLocal8Bit("Φ18"), "SDIAEFE"},
          {GString::fromLocal8Bit("ΦA"), "SDIAA"},
          {GString::fromLocal8Bit("ΦB"), "SDIAB"},
          {GString::fromLocal8Bit("Φ2"), "SDIABF"},
          {GString::fromLocal8Bit("ΦC"), "SDIAC"},
          {GString::fromLocal8Bit("Φ3"), "SDIACF"},
          {GString::fromLocal8Bit("ΦD"), "SDIAD"},
          {GString::fromLocal8Bit("ΦE"), "SDIAE"},
          {GString::fromLocal8Bit("Φ5"), "SDIAEF"},
          {GString::fromLocal8Bit("ΦG"), "SDIAG"},
          {GString::fromLocal8Bit("ΦL"), "SDIAL"},
          {GString::fromLocal8Bit("ΦN"), "SDIAN"},
          {GString::fromLocal8Bit("ΦY"), "SDIAY"}
    };
    #endif
#else
    #ifdef __APPLE__
    const GlodonDiameterSymbolRec c_DiameterSymbols [] =
    {
          {TRANS_STRING("Φ16"), TRANS_STRING("")},
          {TRANS_STRING("Φ15"), TRANS_STRING("")},
          {TRANS_STRING("Φ13"), TRANS_STRING("")},
          {TRANS_STRING("Φ14"), TRANS_STRING("")},
          {TRANS_STRING("Φ17"), TRANS_STRING("")},
          {TRANS_STRING("Φ18"), TRANS_STRING("")},
          {TRANS_STRING("ΦA"), TRANS_STRING("")},
          {TRANS_STRING("ΦB"), TRANS_STRING("")},
          {TRANS_STRING("Φ2"), TRANS_STRING("")},
          {TRANS_STRING("ΦC"), TRANS_STRING("")},
          {TRANS_STRING("Φ3"), TRANS_STRING("")},
          {TRANS_STRING("ΦD"), TRANS_STRING("")},
          {TRANS_STRING("ΦE"), TRANS_STRING("")},
          {TRANS_STRING("Φ5"), TRANS_STRING("")},
          {TRANS_STRING("ΦG"), TRANS_STRING("")},
          {TRANS_STRING("ΦL"), TRANS_STRING("")},
          {TRANS_STRING("ΦN"), TRANS_STRING("")},
          {TRANS_STRING("ΦY"), TRANS_STRING("")}
    };
    #else
    const GlodonDiameterSymbolRec c_DiameterSymbols [] =
    {
          {GString::fromLocal8Bit("Φ16"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ15"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ13"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ14"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ17"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ18"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦA"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦB"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ2"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦC"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ3"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦD"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦE"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("Φ5"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦG"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦL"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦN"), GString::fromLocal8Bit("")},
          {GString::fromLocal8Bit("ΦY"), GString::fromLocal8Bit("")}
    };
    #endif
#endif

class GlodonCustomDrawSymbol;
class GlodonMeterDrawSymbol;
class GlodonDiaDrawSymbol;
class GlodonCustomDrawSymbolFactory;
class GlodonMeterDrawSymbolFactory;
class GlodonDiaDrawSymbolFactory;

class GlodonCustomDrawSymbolFactory
{
public:
    virtual ~GlodonCustomDrawSymbolFactory() = 0;

    virtual GlodonCustomDrawSymbol* create() = 0;
};

class GlodonMeterDrawSymbolFactory : public GlodonCustomDrawSymbolFactory
{
public:
    virtual ~GlodonMeterDrawSymbolFactory() = 0;
    GlodonCustomDrawSymbol* create();
};

class GlodonDiaDrawSymbolFactory : public GlodonCustomDrawSymbolFactory
{
public:
    virtual ~GlodonDiaDrawSymbolFactory() = 0;
    GlodonCustomDrawSymbol* create();
};

class GLDTABLEVIEWSHARED_EXPORT GlodonCustomDrawSymbol
{
public:
    GlodonCustomDrawSymbol(QObject *parent = 0);
    bool virtual canHandle(const QString &text) = 0;

protected:
    virtual void drawText(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect) = 0;
};

class GlodonDrawSymbolManager
{
public:
    GlodonDrawSymbolManager();
    ~GlodonDrawSymbolManager();

public:
    int indexOfDrawSymbolClass(const QString &text);
    void addDrawSymbol();

    void drawText(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect, bool &handle);
};

class GLDTABLEVIEWSHARED_EXPORT GlodonMeterDrawSymbol : public GlodonCustomDrawSymbol
{
public:
    bool canHandle(const QString &text);
protected:
    /**
     * @brief 计算在指定宽度下能输出字串的长度，同时返回实际占用的宽度
     * @param painter
     * @param text
     * @param width
     * @param height
     * @return
     */
    int calcDrawLineLength(QPainter *painter, const QString &text, int &width);
    /**
     * @brief 计算在制定范围内能输出的字串的长度
     * @param painter
     * @param text
     * @param width
     * @param height
     * @return
     */
    int calcDrawTextLength(QPainter *painter, const QString &text, int width, int height);
    int calcM2M3Length(QPainter *painter);

public:
    void drawText(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect);

private:
    bool isTextM2M3(const QString &text);
    bool isTextSharp(const QString &text);
    QString replaceSymbolText(const QString &text);
    void drawLine(QPainter *painter, const QString &text, int x, int y, int &width, int &height);
    void drawM2M3(QPainter *painter, int x, int y, const QString & symbol);
    void drawSharp(QPainter *painter, int x, int y, const QString &symbol);
    void interDrawText(QPainter *painter, const QString &text, Qt::Alignment align, int x, int y, int width, int &height);
};

class GLDTABLEVIEWSHARED_EXPORT GlodonDiaDrawSymbol : public GlodonCustomDrawSymbol
{
public:
    bool canHandle(const QString &text);

public:
    void draw(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect);
    void drawText(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect);

private:
    void draw1Symbol(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect, int index);
    void drawBitMap(QPainter *painter, int scale, Qt::Alignment align, QRect &rect, int index);
    void innerDrawText(QPainter *painter, Qt::Alignment align, QRect &rect, const QString &text);
	int getContentWidth(QPainter *painter, int scale, const QString &text);
	int getContentHeight(QPainter *painter, int scale, const QString &text);
};

//  function  DrawSymbolManager TDrawSymbolManager;

GLDTABLEVIEWSHARED_EXPORT bool includeM2M3(const QString &text);
GLDTABLEVIEWSHARED_EXPORT void drawTextMx(QPainter *painter, const QString &text, Qt::Alignment align, QRect &rect);
GLDTABLEVIEWSHARED_EXPORT bool includeSharp(const QString &text);
GLDTABLEVIEWSHARED_EXPORT bool includeDiaSymbol(const QString &text);

GLDTABLEVIEWSHARED_EXPORT void drawDiaSymbol(QPainter *painter, int scale, const QString &text, Qt::Alignment align, QRect &rect);
GObjectList<GlodonSymbolPos *> *createIndexOfDiameterSymbolList(const QString &text);
GLDTABLEVIEWSHARED_EXPORT bool compare(GlodonSymbolPos *item1, GlodonSymbolPos *item2);

/**
 * @brief The GlodonSymbols class 钢筋符号类
 */

#ifdef _FONTSYMBOL_
class GLDTABLEVIEWSHARED_EXPORT GlodonSymbols
{
public:
    GlodonSymbols();
    ~GlodonSymbols(){}

public:
    /**
     * @brief aliasFromDisplaySymbol 如果displaySymbol属于钢筋符号库，则返回其别名，否则返回displaySymbol
     * @param displaySymbol
     * @return
     */
    QString aliasFromDisplaySymbol(const QString &displaySymbol);
    /**
     * @brief displaySymbolFromGiveSymbol 如果alias属于钢筋符号库，择返回用于显示的钢筋符号字符串，否则返回alias
     * @param giveSymbol
     * @return
     */
    QString displaySymbolFromAlias(const QString &alias);

    /**
     * @brief allSymbols 返回所有的钢筋符号
     * @return QMap<QString, QString> QMap中第一个参数为钢筋符号别名，同GlodonDiameterSymbolRec结构体中的symbol
     *                                QMap中第二个参数为钢筋符号的显示字符串， 主要用于显示
     */
    QMap<QString, QString> allSymbols() const;

    /**
     * @brief aliasesFromDisplaySymbols 从显示的钢筋符号串(多个钢筋符号)转为别名符号串(多个)
     * @param displaySymbols
     * @return
     */
    QString aliasesFromDisplaySymbols(const QString &displaySymbols);

    /**
     * @brief displaySymbolsFromAliases 从别名符号串(多个)转为显示的钢筋符号串(多个)
     * @param aliases
     * @return
     */
    QString displaySymbolsFromAliases(const QString &aliases);

private:
    void initSymbols(const GlodonDiameterSymbolRec array[], size_t size);
    QMap<QString, QString> m_symbols; // 钢筋符号列表， 第一个参数为钢筋符号别名，后一个参数为需要显示的钢筋符号
};
#endif

#endif // GLDDRAWSYMBOL_H
