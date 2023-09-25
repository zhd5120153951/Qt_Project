/*!
 * \file GLDRichTextEdit.h
 *
 * \author xg
 * \date ���� 2014
 *
 * 
 */
#ifndef GLDRICHTEXTEDIT_H
#define GLDRICHTEXTEDIT_H

#include <QEvent>
#include <QFrame>
#include <QMenu>
#include <QScrollBar>
#include <QScrollArea>
#include <QWidget>
#include <QResizeEvent>

class GLDRichTextEditPrivate;
class GLDRichEditScrollArea;
class GLDRichTextEditor;

static bool g_bResize = true;

class GLDRichTextEdit : public QWidget
{
    Q_OBJECT

public:
    enum Align
    {
        Left,
        Center,
        Right,
        Justify
    };

    // �ı��е�����
    friend class TextRowIterator;
    class TextRowIterator
    {
        friend class GLDRichTextEdit;
    public:
        TextRowIterator();
        explicit TextRowIterator(GLDRichTextEdit * edit, int row);
        ~TextRowIterator();

        bool isVaild() const;

        QString getText(int cell) const;
        void setText(int cell, const QString & text);
        bool hasCell() const;
        int cellCount() const;

        TextRowIterator next() const;

        int row() const
        {
            return m_row;
        }

        bool operator==(const TextRowIterator & o)
        {
            return (m_row == o.m_row);
        }

        bool operator!=(const TextRowIterator & o)
        {
            return (m_row != o.m_row);
        }

        bool operator<=(const TextRowIterator & o)
        {
            return (m_row <= o.m_row);
        }

        int operator++(int)
        {
            return m_row++;
        }

        int operator++()
        {
            return ++m_row;
        }

        int operator--(int)
        {
            return m_row--;
        }

        int operator--()
        {
            return --m_row;
        }

    private:
        GLDRichTextEdit *m_edit;
        int              m_row;
    };

public:
    GLDRichTextEdit(QWidget *parent);
    ~GLDRichTextEdit();

    void alwaysHideScrollBar();
    int contentHeight();
    int contentWidth();

    bool canUndo();
    bool canRedo();

    void open(const QString &fileName);
    int save(); // ����0��ʾ�ɹ�
    void saveAs(const QString &fileName, bool userSaveAsFileName);

    /**
     * @brief saveToBuf ���浽��������
     * @param byteArray ��������
     * @return 1--�ɹ� 0--ʧ��
     */
    int saveToBuf(QByteArray &byteArray);

    /**
     * @brief readFromBuf �������������ļ�
     * @param byteArray ��������
     * @return 1--�ɹ� 0--ʧ��
     */
    int readFromBuf(QByteArray &byteArray);


    bool isModify() const;
    void setModify(bool modified);

    void setDefaultFont(const QFont &fnt, int size, const QColor &clr);
    QString getSelectText() const;

	QString getAllText() const;

    const QString &getFileName() const;

    QFont selectionCharFont(bool bMousePress) const;
    QColor selectionCharFontColor() const;
    QColor selectionCharBackgroundColor() const;

    QPoint getTextCursor() const;
    void setTextCursor(const QPoint &pos);

    int getCurrentRow() const;
    void moveTextCursorNext();

    /*!
    * @brief    �������� [1, n]������
    * @return   �������һ���е�һ��û�����ݷ���0
    */
    int getLineCount() const;

    /*!
     * @brief    �е�����
     * @param    [i]textRowIndex �ı��к�
     * @return   GLDRichTextEditRow * ����������
     * ע��      ����Ե�ǰ��
     */
    TextRowIterator getRowIterator(_In_ int textRowIndex);

     /*!
      * @brief    ��ȡ�����ı���ʼλ�úͽ��� [start, end)
      * @param    [i]start �������������ʼλ��
      * @param    [i]end   ����������Ľ���λ��
      * @param    [i]textRowIndex �ı�������
      * @return   bool �����table�� ����true���򷵻�false
      */
     bool getSeriesTableIterator(_Out_ TextRowIterator &start, _Out_ TextRowIterator &end,
                                 _In_ int textRowIndex = 0);

     /*!
     * @brief    �����滻
     * @param    [i]find ���ҵ��ַ���
     * @param    [i]bCase �Ƿ��д
     * @param    [i]bWord ��������
     * @param    [i]bNext ����
     * @param    [i]replace �滻�ı�
     * @param    [i]findAndImReplace �ҵ��Ƿ������滻
     * @return   �滻�ɹ�����true
     */
     bool replaceSelectText(const QString &find, bool bCase, bool bWord,
                            bool bNext, const QString &replace, bool findAndImReplace);

     /*!
     * @brief    �����滻
     * @param    [i]find ���ҵ��ַ���
     * @param    [i]isCase �Ƿ��д
     * @param    [i]isWord ��������
     * @param    [i]isNext ����
     * @param    [i]replace �滻ͼƬ
     * @param    [i]findAndImReplace �ҵ��Ƿ������滻
     * @return   �滻�ɹ�����true
     */
     bool replaceSelectTextToImage(const QString &find, bool isCase, bool isWord,
                                   bool isNext, const QString &imageFileName,
                                   bool findAndImReplace, int w, int h, bool bMonochrome);

     /*!
      * @brief    �������
      * @param    [i]title �����ı�
      * @param    [i]Num   �����
      * @param    [i]align ���뷽ʽ
      * @return   void
      */
     void insertTitleHead(const QString &titleText, int num, GLDRichTextEdit::Align align);

     QString getTextDocTitle() const;
     void addRightBtnAction();
     void initRBtnMenu();

     void saveAsDoc();    
     void saveToDoc(QString desDocName, QString srcFileName);

     double zoomFactor() const;

     void emitRButtonUp();
     void emitUpdateScroll();
     void setCursorFocus();
     void enterKeyPress();
     void enterKeyPressed();

     virtual void closeEvent(QCloseEvent *e);

signals:
     void activeWindow(GLDRichTextEdit *edit);
     void updateUIState(GLDRichTextEdit *edit, bool bMousePress);
     void onRButtonUp();
     void zoomFactorChanged();
     void updateScroll();
     void updateFocus(GLDRichTextEdit *edit);

public slots:
    void open();
    void onSave();
    void saveAs();

    void catchWheelup();
    void cut();
    void copy();
    void paste();
    void del();
    void undo();
    void redo();
    void setUndoCount(int undoCount);

    void selectAll();

    void insertText(const QString &text);
    void insertTable(int rows, int columns);
    void insertImage(const QString &imageFileName, int w, int h, bool bMonochrome);

    void bold();
    void setBold(bool enable);

    void italic();
    void setItalic(bool enable);

    void underline();
    void setUnderline(bool enable);

    void strikeout();
    void setStrikeout(bool enable);

    void subscript();
    void setSubscript(bool enable);

    void superscript();
    void setSuperscript(bool enable);

    void setFont(const QFont &fnt);
    void setFontSize(int size);
    void setFontColor(const QColor &clr);
    void setFontBackgroundColor(const QColor &color);

    void setReadOnly(bool value);

    // ֧�����ű����� 10% ~ 600% ֮��
    void setZoomFactor(bool bIsZoomIn);

    void setZoomFactorValue(const double &zoomFactor);

    void setAlign(Align align);

    bool findText(const QString &find, bool bCase, bool bWord, bool bNext);
    void replaceSelectText(const QString &find, bool bCase, bool bWord, bool bNext, const QString &replace);
    void replaceAllText(const QString &find, bool bCase, bool bWord, const QString &replace);
    void replaceSelectTextToImage(const QString &find, bool bCase, bool bWord, bool bNext,
                                  const QString &imageFileName, int w, int h, bool bMonochrome);

    void setTextCursorToHead();
    void setTextCursorToTail();
    
    /*!
     * @brief    ѡ����beginText��endText���ı�����beginText, endText
     * @param    [i]beginText
     * @param    [i]endText
     , @param    [i]bCase �Ƿ����ִ�Сд
     * @return   bool
     */
    bool selectRegionText(const QString &beginText, const QString &endText, bool bCase);
    void test();
    
protected:
    virtual QString getOpenFileName() const;
    virtual QString getSaveFileName(const QString &defaultName) const;
    virtual void resizeEvent(QResizeEvent *e);


private:
    friend class GLDRichTextEditNativeEventFilter;
    void updateUI(QEvent::Type eType, Qt::MouseButtons btn, Qt::Key eKey);

public:
    GLDRichTextEditPrivate *m_d;
    int m_nScrollWidth;

private:
    double m_dZoomFactor;//����������
    static double s_PreZoom;
    bool m_bCanUndo;
    bool m_bCanRedo;
    QScrollBar *m_pVerticalScrollBar;
    int m_nCurrentScrollBarPos;
    QMenu *m_pMenu;
    QAction *m_pMenuCut;
    QAction *m_pMenuCopy;
    QAction *m_pMenuPaste;

};

class GLDRichEditScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit GLDRichEditScrollArea(QWidget* parent = 0,
                                   GLDRichTextEdit *richEdit = NULL);
    void setContentHeight();

public slots:
    void updateUISlot();

protected:
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *e);

private:
    bool m_bIsResize;
    int m_nContentHeight;
    int m_nScrollAreaHeight;
    int m_nScrollAreaWidth;
    GLDRichTextEdit *m_pRichEditObject;
    GLDRichTextEditPrivate *m_pRichEditPrivate;
};
#endif // GLDRICHTEXTEDIT_H
