#ifndef GFontListEx_H
#define GFontListEx_H

#include <QFontComboBox>
#include <QAbstractItemDelegate>
#include "GLDWidget_Global.h"

class GLDFontListDelegate;

class GLDWIDGETSHARED_EXPORT GFontListEx : public QFontComboBox
{
    Q_OBJECT
public:
    explicit GFontListEx(QWidget *parent = 0);
    ~GFontListEx();

public:
    virtual void showPopup();
    void setHasBorder(bool bHasBorder = true);

protected:
    QColor m_highLightBackgroundColor;
    QColor m_highLightTextColor;

protected slots:
    void showFontFamily(QString family);

private:
    void setHightBackground(QColor color);
    void setHightText(QColor color);
    void setFontListItemDelegate(GLDFontListDelegate *delegate);
    void setHighLighColor(QColor background, QColor text);

private:
    GLDFontListDelegate *m_pFontDelegate;
};

class GLDWIDGETSHARED_EXPORT GLDFontListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit GLDFontListDelegate(QObject *parent = 0);

public:
    void setTTypeIconPath(const QString &path);  // 设置TrueType的图标路径
    void setBMapIconPath(const QString &path);  // 设置Bitmap图标的路径
    void setCMarkIconPath(const QString &path);  // 设置选中状态图标的路径

    // 控制是否显示选中状态图标的getter与setter
    bool isShowCheckMark();
    void setIsShowCheckMark(bool isShow = true);

    // 控制是否显示字体类型图标的getter与setter
    bool isShowFontTypeImage();
    void setIsShowFontTypeImage(bool isShow = true);

    // 控制显示时每个字体对应的效果图
    void setEnglishCaption(const QString &caption);
    void setZhCaption(const QString &caption);

    QString defaultFont();  // 取得默认字体
    void setDefaultFont(QString family);  // 设置默认字体

    QString selectedFamily() const;  // 取得组合框中选中的字体
    void setSelectedFamily(const QString &family);  // 根据combobox的设置选中的字体

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;  // paint each list item
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

private:
    void paintFontTypeImage(QRect &rect, QString fontFamilyName,
                            QPainter *painter, const QStyleOptionViewItem &option) const;
    void paintCheckStatusIcon(QRect &rect, QPainter *painter) const;

private:
    QString m_sTTypeIconPath;  // True Type图标的路径
    QString m_sBMapIconPath;  // Bitmpa类型图标的路径
    QString m_sCMarkIconPath;  // 选择状态图标的路径
    QIcon m_trueTypeIcon;
    QIcon m_bitmapIcon;
    QIcon m_checkMark;
    bool m_bShowTypeImageFlag;

    QString m_sEnglishCaption;
    QString m_sZhCaption;
    QString m_sDefaultFont;

    QString m_sSelectedFamily;
    bool m_bShowCheckMark;
};
#endif // GFontListEx_H
