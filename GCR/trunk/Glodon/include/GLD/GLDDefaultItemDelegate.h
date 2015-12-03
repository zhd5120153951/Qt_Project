/*!
 *@file glodondefaultitemdelegate.h
 *@brief {默认编辑方式}
 *提供了TableView需要的默认编辑方式，如需要扩展，则一律从此派生
 *@author Gaosy
 *@date 2012.9.7
 *@remarks {remarks}
 *Copyright (c) 1998-2012 Glodon Corporation
 */

#ifndef GLDDEFAULTITEMDELEGATE_H
#define GLDDEFAULTITEMDELEGATE_H

#include <QItemDelegate>
#include "GLDComboBox.h"
#include "GLDEnum.h"

#include "GLDDateTimeEdit.h"
#include "GLDMathUtils.h"
#include "GLDTableView_Global.h"

class QComboBox;
class GlodonAbstractItemView;
class GlodonAbstractItemViewPrivate;
class GLDTableView;
class GLDTableViewPrivate;
class GLDAbstractButtonEdit;
class GlodonDefaultItemDelegatePrivate;
class GLDWindowComboBoxEx;
class GLDLineWidthComboBoxEx;

/*!
 *@class: GlodonDefaultItemDelegate
 *@brief {Grid默认编辑方式，所有需要扩展编辑方式的，都需要从这里派生}
 *@author Gaosy
 *@date 2012.9.10
 */
class GLDTABLEVIEWSHARED_EXPORT GlodonDefaultItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit GlodonDefaultItemDelegate(QObject *parent = 0);

public:
    // 派生类需要覆盖的方法
    virtual bool editable(const QModelIndex &index) const;
    virtual GEditStyle editStyle(const QModelIndex &index, bool &readOnly) const;
    virtual void initEllipsisButtonEdit(GLDAbstractButtonEdit *ellipsis, const QModelIndex &index) const;
    virtual void initComboBox(QComboBox *comboBox, const QModelIndex &index) const;
    virtual void initWindowComboBox(GLDWindowComboBoxEx *comboBox, const QModelIndex &index) const;
    virtual void initLineWidthComboBox(GLDLineWidthComboBoxEx *comboBox, const QModelIndex &index) const;
    virtual void initSpinBox(QAbstractSpinBox *spinBox, const QModelIndex &index);
    virtual void initDateTimeEdit(GLDDateTimeEdit *dateTimeEdit, const QModelIndex &index);

    virtual void itemViewSelected(QComboBox *comboBox, const QModelIndex &index, const QModelIndex &itemIndex) const;
public:
    /*!
     *@enum: GDataType
     *@brief {数据类型，当前仅为HTML超链接提供支持}
     *@author Gaosy
     *@date 2012.9.10
     */
    enum GDataType
    {
        GTypeNormal,
        GTypeHTML,
        GRichText,
        GDiaSymbol,
        GMeterSymbol
    };

    /**
     * @brief 图片显示的模式
     * @see QImage::scaled
     */
    enum GAspectRatioMode
    {
        IgnoreAspectRatio,  //忽略长宽比（只为充满）
        KeepAspectRatio,    //保持比例缩放(图片完全展示前提下尽可能地缩放)
        KeepAspectRatioByExpanding,//保持比例缩放(图片只能有一个维度超过容纳的尺寸)
        KeepAspectPixel    //保持原大小
    };

    /*!
     *格子的绘制
     *@param[in]  painter
     *@param[in]  option
     *@param[in]  index
     *@return 无
     *@see 参见QItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
     *                              const QModelIndex &index) const;
     */
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(
            const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *返回编辑方式需要的控件
     *@param[in]  parent
     *@param[in]  option
     *@param[in]  index
     *@return QWidget*
     */
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    /*!
     *进入编辑状态时，初始化控件的值
     *@param[in]  editor  格子编辑需要的控件
     *@param[in]  index
     *@return 无
     */
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    /*!
     *退出编辑状态时，将编辑控件中的值提交
     *@param[in]  editor  格子编辑需要的控件
     *@param[in]  model   TableView的model
     *@param[in]  index
     *@return 无
     */
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;

    /*!
     *设置编辑控件的位置及Geometry信息
     *@param[in]  editor  格子编辑需要的控件
     *@param[in]  model   TableView的model
     *@param[in]  index
     *@return 无
     */
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

    virtual QVariant currentEditorData(const QModelIndex &index, QWidget *editor);

    QModelIndex dataIndex(const QModelIndex &index) const;
    QModelIndex treeIndex(const QModelIndex &index) const;

    static GEditStyle editStyleByVariantType(QVariant::Type vType);

    inline QWidget *curEditor()
    {
        return m_curEditor;
    }

    /**
     * 设置编辑方式esDropDown是否选择值立即退出编辑状态: 默认值true
     * @brief setComboBoxConvenient
     * @param convenient
     */
    inline bool isComboBoxConvenient() const
    {
        return m_comboBoxConvenient;
    }
    inline void setComboBoxConvenient(bool convenient)
    {
        m_comboBoxConvenient = convenient;
    }

    void setIsTextEllipse(bool value);
    inline bool isTextElided() const
    {
        return m_isTextEllipse;
    }
    /**
     * @brief 判断当前tableView是否处于编辑状态
     * @return
     */
    bool isEditing();
    inline void setCurrTreeEditting(const QModelIndex &value)
    {
        m_currTreeEditing = value;
    }
    inline QModelIndex currTreeEditting()
    {
        return m_currTreeEditing;
    }    /*!
     *绘制格子背景
     *@param[in]  painter
     *@param[in]  option
     *@param[in]  index
     *@return 无
     *@see 参见QItemDelegate::drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
     */
    virtual void drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     * \brief 设置文本绘制时的折行方式
     * \param wrap
     */
    inline void setTextWrapMode(QTextOption::WrapMode wrap)
    {
        m_wrapMode = wrap;
    }
    inline QTextOption::WrapMode textWrapMode()
    {
        return m_wrapMode;
    }

protected:
    QRect textLayoutBounds(const QStyleOptionViewItem &option) const;
    bool isBoolCell(const QModelIndex &index) const;

    bool eventFilter(QObject *object, QEvent *event);

    template<typename T>
    bool commitIfCanElseSelectAll(QEvent *event, QWidget *editor, bool &state);

    bool commitDataAndCloseEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint = NoHint);

    /*!
     * \brief cursorMoveTextEndByEndKey,使用Ctrl + End键来将光标位置移动到最后
     * \param editor
     */
    void cursorMoveTextEndByEndKey(QWidget *editor);
    /*!
     * \brief cursorMoveTextEnd，用于将鼠标光标在editor中移向最后一个位置
     * \param editor
     * \return
     */
    bool cursorMoveTextEnd(QWidget *editor);
    /*!
     * \brief cursorPosInsertANewLine
     * \param editor
     * \return true:move to a new line successful, false:cursor can`t movable control,or moving failed
     */
    bool cursorPosInsertANewLine(QWidget *editor);
    QStyleOptionViewItem setOptions(const QModelIndex &index,
                                    const QStyleOptionViewItem &option) const;
Q_SIGNALS:
    void onQueryImageAspectRatioMode(const QModelIndex &index, GlodonDefaultItemDelegate::GAspectRatioMode &ratioMode) const;
    void onQueryFloatOrDoubleViewFormat(const QModelIndex &index, QString &text) const;
    void onQueryIndexDataType(const QModelIndex &index, GlodonDefaultItemDelegate::GDataType &dataType) const;
    void onCommitDataAndCloseEditor();
    void commitData(QWidget *editor, bool &canCloseEditor);
    void closeEditor(QWidget *editor, bool &canCloseEditor, QAbstractItemDelegate::EndEditHint hint = NoHint);

protected:
    /**
     * 设置传入的editor中的文本被全选
     * @brief setTextAllSelected
     * @param editor
     */
    bool setTextAllSelected(QWidget *editor);
    /**
     * 失去焦点退出编辑状态: 默认值false
     * @brief hideEditOnExit
     * @return
     */
    inline bool closeEditorOnFocusOut() const
    {
        return m_bCloseEditorOnFocusOut;
    }
    inline void setCloseEditorOnFocusOut(bool value, bool ignoreActiveWindowFocusReason = false)
    {
        m_bCloseEditorOnFocusOut = value;
        m_bIgnoreActiveWindowFocusReason = ignoreActiveWindowFocusReason;
    }

    /*!
     * \brief 设置是否使用comboBox的自动提示功能，默认为使用
     * \return
     */
    inline bool useComboBoxCompleter() const
    {
        return m_bUseComboBoxCompleter;
    }
    inline void setUseComboBoxCompleter(bool value)
    {
        m_bUseComboBoxCompleter = value;
    }
    /**
     * 选中单元格使用混色
     * @brief useBlendColor
     * @return
     */
    inline bool useBlendColor() const
    {
        return m_bUseBlendColor;
    }
    inline void setUseBlendColor(bool value)
    {
        m_bUseBlendColor = value;
    }
    /**
     * 选中单元格的背景颜色
     * @brief useBlendColor
     * @return
     */
    inline QColor selectedCellBackgroundColor() const
    {
        return m_oSelectedCellBackgroundColor;
    }
    inline void setSelectedCellBackgroundColor(QColor value)
    {
        m_oSelectedCellBackgroundColor = value;
    }
    /**
     * @brief 判断tableview是否处于回车跳格状态
     * @param value
     */
    inline bool isEnterJump() const
    {
        return m_enterJump;
    }
    inline void setIsEnterJump(bool value)
    {
        m_enterJump = value;
    }

    inline bool isEnterJumpPro() const
    {
        return m_enterJumpPro;
    }
    inline void setIsEnterJumpPro(bool value)
    {
        m_enterJumpPro = value;
    }
    /*!
     * \brief setIncludeLeading设置显示文字时是否包含行间距
     * \param value
     */
    inline bool isIncludeLeading() const
    {
        return m_includingLeading;
    }
    inline void setIncludeLeading(bool value)
    {
        m_includingLeading = value;
    }
    /*!
     *绘制格子焦点状态
     *@param[in]  painter
     *@param[in]  option
     *@param[in]  rect
     *@return 无
     *@see 参见QItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
     */
    virtual void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

    inline void setCurEditor(QWidget *value)
    {
        m_curEditor = value;
    }

    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                     const QRect &rect, const QString &text, const QModelIndex &index) const;

private:
    void paintOther(QPainter *painter,
                    const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
    void paintDouble(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;
    void paintImg(QPainter *painter,
                  const QStyleOptionViewItem &option,
                  const QModelIndex &index) const;
    void paintBool(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    void paintColor(QPainter *painter,
                    const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
    void paintGType(QPainter *painter,
                    const QStyleOptionViewItem &option,
                    const QModelIndex &index, GDataType type) const;

    bool setTreeViewModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    bool setDropDownWindowModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    bool setMonthCalendarModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index, QVariant::Type vType) const;

    QWidget *doCreateEditor(QWidget *parent,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index);

    void createLineEditEditor(QWidget *parent, bool bReadOnly);
    void createTreeViewEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly);
    void createDropDownWindowEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly, GEditStyle eEditStyle);
    void createMonthCalendarEditor(QWidget *parent, QModelIndex &dIndex, QVariant::Type vType);
    void createColorListEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly);
    void createFontListEditor(QWidget *parent);
    void createLineWidthEditor(QModelIndex &dIndex, QWidget *parent);

    template<class T>
    void createEllipsisEditor(QWidget *parent, QModelIndex &dIndex, bool bReadOnly)
    {
        T *ellipsis = new T(parent);

        m_curEditor = ellipsis;

        initEllipsisButtonEdit(ellipsis, dIndex);
        ellipsis->setEditable(!bReadOnly);
        ellipsis->setHasBorder(false);
    }

    /**
     * @brief esSimple，esUpDown这两种编辑方式创建的都是QSpinBox类似控件
     * @param parent
     * @param option
     * @param index
     * @param vType 数据类型
     * @param isUpDown 是否是增量设置框
     * @param numberRightAlign
     * @return
     */
    void createSpinBox(QWidget *parent,  const QModelIndex &index, const QStyleOptionViewItem &option,
                       QVariant::Type vType, bool bReadOnly, bool &numberRightAlign);


    void drawEditStyleDraw(QPainter *painter, QStyleOptionViewItem &option, QRect &rect) const;
    /**
     * @brief 与QItemDelegate中的drawDisplay一样，去掉了从style中获取margin部分
     * @param painter
     * @param option
     * @param rect
     * @param text
     */
    void doDrawDisplay(
            QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect,
            const QString &text, const QModelIndex &index) const;
    /**
     * @brief 根据role，画对角线和反对角线
     * @param painter
     * @param rect
     * @param index
     */
    void drawDiagonal(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

    /**
     * @brief 根据显示区域行高，计算显示文本的行数
     * @param textRect  显示区域
     */
    int calculateLineCount(const QRect &textRect) const;

    /**
     * @brief text不包含换行，根据行高及option处理text，适应显示区域
     * @param[out]  textLineCount   text显示的行数
     * @param[in]   rectLineCount   显示区域能够显示的行数
     * @param[in]   textRect        显示区域
     * @param[in]   text            显示区域的字串
     * @param[in]   option          其它的格式(是否显示...等)
     */
    QString elidedText(int &textLineCount, int rectLineCount, const QRect &textRect, const QString &text, const QStyleOptionViewItem &option) const;
    /**
     * @brief text不包含换行，根据行高及option处理text，适应显示区域
     * @param[in] textRect  显示区域
     * @param[in] text      显示区域的字串
     * @param[in] option    其它的格式(是否显示...等)
     */
    QString elidedTextWithoutLineFeed(const QRect &textRect, const QString &text, const QStyleOptionViewItem &option) const;
    /**
     * @brief text包含换行，根据行高及option处理text，适应显示区域
     * @param[in] textRect  显示区域
     * @param[in] text      显示区域的字串
     * @param[in] option    其它的格式(是否显示...等)
     */
    QString elidedTextWithLineFeed(const QRect &textRect, const QString &text, const QStyleOptionViewItem &option) const;

    inline bool checkSetModelData() const
    {
        return m_checkSetModelData;
    }
    inline void setCheckSetModelData(bool value)
    {
        m_checkSetModelData = value;
    }
    /**
     * @brief 根据给定的缩放因子缩放Delegate画TableView的内容时的字体
     * @param index
     * @param font
     * @param factor
     * @return
     */
    QFont zoomFont(QModelIndex index, QFont font, double factor) const;
    /**
     * @brief 给ComboBox增加QCompleter
     * @param comboBox
     * @param index
     */
    void initComboBoxCompleter(QComboBox *comboBox, QModelIndex index);

    /*!
     * \brief 根据编辑方式，获取对应编辑方式所占区域
     * \param index
     * \param rect
     * \return
     */
    QRect editStyleRect(QModelIndex index, QRect rect);
    bool isInSubControlRect(QModelIndex index, QPoint pos);
    void clickSubControl(QModelIndex index);

    void adjustDecorationAlignment(const QModelIndex &index, QStyleOptionViewItem &opt) const;

private:
    bool filterKeyPress(QWidget *editor, QEvent *event);
    bool filterReturnKeyPress(QWidget *editor, QKeyEvent *keyEvent);
    bool filterRightKeyPress(QWidget *editor, QKeyEvent *keyEvent);
    bool filterUpKeyPress(QWidget *editor, QKeyEvent *keyEvent);
    // 重构filterKeyPress
    void dealWithEscapeKeyPress(QWidget *editor);
    bool dealWithTabKeyPress(QWidget *editor);
    bool dealWithBackTabPress(QWidget *editor);
    template<typename T>
    QVariant compareDataByType(QVariant oldData, T newData) const;
    QVariant compareDataByType(QVariant oldData, double newData) const;
    // 重构EventFilter
    bool dealWithLoseFocus(QEvent *event, QWidget *pEditor);
    bool dealWithShortCutOverride(QEvent *event);

    // 重构自动行高
    QRect calcRectByData(
            const QStyleOptionViewItem &option, const QModelIndex &index, int role) const;
    QRect textRectangle(
            const QRect &rect, const QFont &font, const QString &text) const;

    void innerDoLayout(
            const QStyleOptionViewItem &option, QRect *checkRect, QRect *pixmapRect, QRect *textRect,
            bool hint) const;
    void adjustRectByTextMargin(const QModelIndex &index, const QStyleOptionViewItem &opt, bool isIncludeMargin, QRect &rect) const;

private Q_SLOTS:
    void doComboBoxActivated(int index);

private:
    Q_DECLARE_PRIVATE(GlodonDefaultItemDelegate)

    mutable QModelIndex m_currTreeEditing;
    QWidget *m_curEditor;
    GlodonAbstractItemView *m_pTable;
    mutable int m_oComboBoxCurIndex;
    static Qt::Alignment s_originalDisplayAlignmentForCurrentItem; //用于保持displayAlignment
    double m_factor;//缩放因子
    QEvent *m_closeEditEvent;//导致退出编辑状态的event

    bool m_comboBoxConvenient;//combo立即展开下拉
    bool m_bCloseEditorOnFocusOut;
    bool m_inCommitData;
    bool m_inSetModelData; //setModelData时不再调用setEditorData，否则会初始化多次
    bool m_checkSetModelData;//setModelData是否成功
    static bool s_digitCurrentItem;
    bool m_bUseBlendColor;
    bool m_includingLeading; //draw文字时是否包含leading(行距)
    bool m_enterJump;
    bool m_enterJumpPro;
    bool m_isTextEllipse;
    bool m_bRepeatCommit; //当开启失去焦点退出编辑状态，通过抛异常的方式校验数据不合法时，会出现重复提交现象
    bool m_bCanHideEidtOnExit;
    bool m_bIgnoreActiveWindowFocusReason;
    bool m_bUseComboBoxCompleter;//设置是否使用comboBox的自动提示功能
    QTextOption::WrapMode m_wrapMode;

    QColor m_oSelectedCellBackgroundColor; //选择区域的背景颜色
    mutable QVariant m_oDisplayData;

    friend class GlodonAbstractItemView;
    friend class GlodonAbstractItemViewPrivate;
    friend class GLDTableView;
    friend class GLDTableViewPrivate;
};

#endif // GLDDEFAULTITEMDELEGATE_H
