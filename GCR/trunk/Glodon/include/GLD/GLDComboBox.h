#ifndef GLDCOMBOBOX_H
#define GLDCOMBOBOX_H

#include <QComboBox>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include "GLDWidget_Global.h"

class GLDWIDGETSHARED_EXPORT GLDCustomComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
public:
    explicit GLDCustomComboBox(QWidget *parent = 0);
    virtual void showPopup();
    virtual void hidePopup();

    /**
     * @brief ComboBox中的Edit是否能编辑
     * @return
     */
    bool isReadOnly();
    void setReadOnly(bool value);

    bool hasSelectedText();

    /**
     * @brief 在处于readOnly时，edit中的内容按Backspace键或则Delete键是否能删除内容
     *        只有ComboBox的readOnly为true时，才能开启该功能，否则按照正常编辑操作处理
     * @return
     */
    bool canDelete();
    void setCanDelete(bool value);

    void setCurrentIndexAndEmitSignal(int index);
    virtual void resizeContent();
    /**
     * @brief setAlignment 设置下拉框选择后编辑框中文字的对齐方式
     */
    void setAlignment(Qt::Alignment align);

Q_SIGNALS:
    void onSetCurrentIndex(int index);
    void cursorPositionChanged();
    void selectionChanged();

public Q_SLOTS:
    void cut();
    void paste();
    void copy();
    void deleteSelectedText();

private slots:
    void onExpandOrCollapsed(const QModelIndex &index);
    void doCursorPositionChanged(int, int);
    void doSelectionChanged();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void updateEditFieldGeometry();

private:
    bool m_isReadOnly;
};

class GLDWIDGETSHARED_EXPORT GLDCustomComboBoxEx : public GLDCustomComboBox
{
    Q_OBJECT
public:
    explicit GLDCustomComboBoxEx(QWidget *parent = 0);

public:
    void setHasBorder(bool hasBorder = false);
    //对输入文本后的弹出框进行样式修改
    void setFilterPopupStyleSheet();

protected:
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *target, QEvent *e);
    void keyPressEvent(QKeyEvent *e);
private:
    bool m_hasBorder;
    bool m_isShow;
};

class GLDWIDGETSHARED_EXPORT GLDLineWidthComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit GLDLineWidthComboBox(QWidget *parent);

    inline int minLineWidth() { return m_minLineWidth; }
    void setMinLineWidth(int minLineWidth);

    inline int maxLineWidth() { return m_maxLineWidth; }
    void setMaxLineWidth(int maxLineWidth);

    inline int currentLineWidth() { return m_currentLineWidth; }
    void setCurrentLineWidth(int lineWidth);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void onCurrentIndexChanged(int index);

private:
    void initLineWidthIcon();
    QWidget *m_parent;
    QSize m_parentSize;
    int m_minLineWidth;
    int m_maxLineWidth;
    int m_currentLineWidth;
    bool m_inAddItem;
};

class GLDWIDGETSHARED_EXPORT GLDComboBoxLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit GLDComboBoxLineEdit(QWidget *parent = 0);

    bool canDelete();
    void setCanDelete(bool value);

public slots:
    void deleteSelectedText();

protected:
    void keyPressEvent(QKeyEvent *keyEvent);

private:
    bool m_canDelete;
};

class GLDWIDGETSHARED_EXPORT FilterPopupItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FilterPopupItemDelegate(QObject *parent = 0);
    virtual ~FilterPopupItemDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
};

#endif // GLDCOMBOBOX_H
