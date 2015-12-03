#ifndef GLDSHELLTREEVIEW_H
#define GLDSHELLTREEVIEW_H

#include <QListView>
#include <QTreeView>
#include <QHeaderView>
#include "GLDWidget_Global.h"
#include "GLDListView.h"
#include "GLDWindowComboBox.h"
#include "GLDAbstractItemView.h"

class QDirModel;
class GLDShellComboBox;
class GLDShellListView;
class QFileSystemModel;
class GLDCustomFileSystemModel;

class GLDWIDGETSHARED_EXPORT GLDDirFilter
{
public:
    enum DirName{
        RelativeName = 0,
        AbsoluteName = 1
    };

    enum ISDIR{
        ISFOLDER = 0,
        ISFILE = 1,
        NOITEMSELECTED = 2
    };

    enum SLASHTYPE{
        FORWORDSLASH = 0,   //正斜杠( slash '/' )
        BACKSLASH = 1       //反斜杠( backslash '\')
    };

    //主要是为Tree型，暂开放给单独的GLDShellTreeView使用
    enum TreeRootPolicyFlag{
        ROOTVISIBLESINGLETON = 0x01, //显示当前被设为root的对应的文件/目录所在的上一层目录，并不再显示该目录的兄弟目录
        HIDE = 0x02,                 //Qt的默认行为。与上一行的行为互斥
        ROOTWITHCURRENT = 0x04,      //root跟随current。与下一行的行为互斥
        ROOTSTEADY = 0x08            //root不跟随current,受setRootIndex影响
    };
    Q_DECLARE_FLAGS(TreeRootPolicy, TreeRootPolicyFlag)

    virtual void setNameFilters(const QStringList &filters) = 0;
    virtual QStringList nameFilters() const = 0;
    virtual QString filePathByType(const QModelIndex &currentIndex, DirName dirType) const;
    virtual ISDIR getTheIndexDirType(const QModelIndex &index, const GLDCustomFileSystemModel *pModel) const;
protected:
    virtual void doSetNameFilters(QAbstractItemModel *model, const QStringList &filters);
    virtual QStringList doNameFilters(QAbstractItemModel *model) const;
};

class GLDWIDGETSHARED_EXPORT GLDShellTreeView : public QTreeView, public GLDDirFilter
{
    Q_OBJECT
public:
    explicit GLDShellTreeView(QWidget *parent = 0);
    ~GLDShellTreeView();

    void setModel(QAbstractItemModel *model);

    /*!
     * \brief setNameFilters 设置要显示的文件后缀列表，不在其中自动过滤掉
     * \param filters 其子项是形如"*.exe"的字符串,必须满足类似的正则表达
     *@warning 当调用setModel将model设置为其它非QFielSystemModel子类类型的实例对象时，此方法将无效，行为将是未定义的。
     *@see GLDShellComboBox::setNameFilters(const QStringList &filters)
     */
    void setNameFilters(const QStringList &filters);

    /*!
     * \brief nameFilters 获得当前的文件过滤后缀列表
     * \return
     *@see GLDShellComboBox::nameFilters()
     */
    QStringList nameFilters() const;

    /*!
     * \brief currentPath
     * \param dirType: RelativeName:相对路径，一般指文件名或者文件夹名，不包含路径。AbsoluteName:绝对路径名,包含路径
     * \return 路径名,如果没有选中，则返回当前的根结点路径
     */
    QString currentPath(DirName dirType = AbsoluteName, SLASHTYPE xieGang = BACKSLASH) const;
    /*!
     * \brief isCurrentPathDir
     * \return ISFOLDER:当前选中的是文件夹。ISFILE:当前选中的是文件。NOITEMSELECTED:当前未选中项
     */
    ISDIR isCurrentPathDir() const;

    /*!
     * \brief setRootWithCombo 当tree与shellcomboBox关联时，此属性决定root是否跟随combo下拉框选中的项而变化
     * \param withCombo
     */
    void setRootWithCombo(bool withCombo) { m_bTreeRootWithCombo = withCombo; }

    bool rootWithCombo() const { return m_bTreeRootWithCombo; }

    /*!
     * \brief setSingleRoot 兼容delphi版本的单一的根结点样式的显示方式
     * \param singleRoot
     */
    void setSingleRoot(bool singleRoot);
    bool singleRoot() const;

    void setRootIndex(const QModelIndex &index);
    void setRootPath(const QString &path);

    inline bool neverChangeRoot() { return m_neverChangeRoot; }
    inline void setNeverChangeRoot(bool value) { m_neverChangeRoot = value; }

    void doItemsLayout();

    /*!
     * \brief 设置当前tree的焦点，可触发联动
     * \param index
     */
    void setCurrentIndexPro(const QModelIndex &index);

signals:
    void itemJustBeSelected(const QModelIndex &index);
    void itemJustBeDoubleClicked(const QModelIndex &index);
    void currentItemJustChanged(const QModelIndex &current, const QModelIndex &previous);

public slots:
    void adjustColumnWidth(const QModelIndex &);

protected slots:
    void upToParentDir(const QModelIndex &parentDirIndex);
    void slotExpand(QModelIndex index);
    void slotCollapse(QModelIndex index);
    void setCurrentIndex(const QModelIndex &index);
    void shellTreeOwnScrollTo();
protected:
    void mousePressEvent(QMouseEvent *event);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void showEvent(QShowEvent *e);

    explicit GLDShellTreeView(bool popup, GLDShellComboBox *combo);
    virtual void setAssociatedItemView(QAbstractItemView *view);
    bool isAssociatedAComboBox() const { return NULL != m_comboBox; }

    QString getIndexFileName(QModelIndex pressedIndex);
private:
    void makePreBuddiesVisible();
    void hideBuddiesAndInsertList(const QModelIndex &visibleChild, const QModelIndex &par, int buddyCount);
    void makesureBuddyContainerExist();
    void makeRootParentAsRootAndHideBuddies();

    void setTreeRootMode(TreeRootPolicy rootPolicy = TreeRootPolicy(5));
    inline TreeRootPolicy treeRootMode() { return m_treeRootPolicy; }
private:
    DirName m_getPathType;
    GLDShellComboBox *m_comboBox;
    int m_iFirstShow;
    TreeRootPolicy m_treeRootPolicy;
    bool m_bTreeRootWithCombo;       //暂时替代上面的属性工作
    bool m_bSelfClicked;
    bool m_bComboClickeSignal;
//    QDirModel *m_pDirModel;
    GLDCustomFileSystemModel *m_pDirModel;//m_pFileModel;
    QList<QModelIndex> *m_pHiddenBuddies;   //for VISIBLESINGLETON 状态
    QList<QVariant> *m_pHiddenBuddys;       //for VISIBLESINGLETON 状态
    QModelIndex m_preRoot;                  //for VISIBLESINGLETON 状态
    bool m_bSingleRoot;
    bool m_bUsingVariant;
    bool m_neverChangeRoot; //始终不改变根节点
private:
    Q_DISABLE_COPY(GLDShellTreeView)
    friend class GLDShellComboBox;
    typedef QTreeView inherit;
};

class GLDWIDGETSHARED_EXPORT GLDShellComboBox : public GLDWindowComboBox, public GLDDirFilter
{
Q_OBJECT
public:
    explicit GLDShellComboBox(const QString &rootPath = QString(), QWidget *parent = 0);
    virtual ~GLDShellComboBox();

public:
    /*!
     * \brief setShellListView
     * \param 设置联动的GLDShellListView
     *@warning 只能关联一个list
     */
    virtual void setShellListView(GLDShellListView *list);

    /*!
     * \brief setShellTreeView
     * \param 设置联动的GLDShellTreeView
     *@warning 只能关联一个tree
     */
    void setShellTreeView(GLDShellTreeView *tree);

    /*!
     * \brief setCustomPath 设置用户目录，即指定根文件夹路径。也可不设置，默认是整个磁盘
     * \param rootPath
     *@note !! 2014.1.23 直接转调用setCurrentPath
     */
    void setCustomPath(const QString &rootPath);

    /*!
     * \brief setCurrentPath 设置当前用户目录，即指定当前要定位到的文件夹路径
     * \param currentPath
     *@warning currentPath需要在setCustomPath设置的rootPath下。否则请不要调用setCustomPath
     *@see 参见GLDShellComboBox::setCustomPath(QString rootPath)
     */
    void setCurrentPath(const QString &currentPath);

    /*!
     * \brief setCurrentFile 设置当前用户文件，即指定当前要选中的文件，并且关联的tree定位到该文件所在文件夹。文件处于选中状态
     * \param currentFilePath 需要包含路径
     *@note 直接调用setCurrentFile()，请不要在setCurrentPath()调用后再调用setCurrentFile()。
     */
    void setCurrentFile(const QString &currentFilePath);

    /*!
     * \brief setNameFilters    设置文件后缀名过滤
     * \param filters
     *@warning 关联的List与Tree均将使用此过滤,只须此处设置一次。
     *@see GLDShellListView::setNameFilters(const QStringList &filters)
     */
    void setNameFilters(const QStringList &filters);    
    QStringList nameFilters() const;

    /*!
     * \brief setDirNameType 设置combo的编辑框中的文件名是否按照绝对路径显示
     * \param type
     */
    void setDirNameType(DirName type);
    DirName dirNameType() const;

    /*!
     * \brief getPathInEdit 获取Edit中显示的路径，若该路径被改动过，为无效的，则返回下拉tree中对应的有效路径
     * \param type 指定要获得的路径的类型。 xieGang 指定要获得的路径中的斜杠类型
     * \return
     */
    QString getCurrentPath(DirName type = AbsoluteName, SLASHTYPE xieGang = BACKSLASH) const;

    /*!
     * \brief getIndexFilePath
     * \param index 指定的QModelIndex，与当前combo关联的tree、list取得的QModelIndex均可。即只要是同一个GLDFileSystemModel下的
     * modelindex均可
     * \param xieGang
     * \return
     */
    QString getIndexFilePath(const QModelIndex &index, SLASHTYPE xieGang = BACKSLASH) const;

    QString text() const { return getCurrentPath(); }

    void setFileModel(GLDCustomFileSystemModel *model);

    /*!
     * \brief 取消点击时更改root的联动
     * \param view外部绑定的控件
     * \return
     */
    void disconnectClick(QAbstractItemView *view);
protected slots:
    void synToComboAndTree(QModelIndex clickedIndex);
    void synToComboAndList(QModelIndex clickedIndex);

protected:
    virtual void doAfterResize();
    QIcon itemIcon(const QModelIndex &index, const QAbstractItemModel *model);
    void setEditTextByIndex(QModelIndex curIndex);

    void expandTreeTo(QModelIndex path);
    void locateListTo(QModelIndex path);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

public:
    void setChildrenRootIndex(QModelIndex path);
    void init(const QString &rootPath);

    //popup中的tree
    GLDShellTreeView *m_popupShellTree;
    //关联的其它单独的控件
    GLDShellListView *m_shellListView;
    GLDShellTreeView *m_shellTreeView;
    GLDCustomFileSystemModel *m_pFileModel;
    bool m_showFullPath;
    bool m_currentPathSetted;

private:
    Q_DISABLE_COPY(GLDShellComboBox)
    friend class GLDShellTreeView;
};

class GLDWIDGETSHARED_EXPORT GLDShellHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit GLDShellHeaderView(QWidget *parent = 0);
    ~GLDShellHeaderView();
    void setSection0AutoResize();
    void setSectionsHiddenExcept0();

private:
    Q_DISABLE_COPY(GLDShellHeaderView)
};

/*!
 *@brief The GLDShellListView class
 *@note :使用基类的slot作为选中与双击的信号，并连接自己的槽
 */
class GLDWIDGETSHARED_EXPORT GLDShellListView : public GLDListView, public GLDDirFilter
{
    Q_OBJECT
public:
    explicit GLDShellListView(QWidget *parent = 0);
    ~GLDShellListView(){freeAndNil(commentFrame);}

    /*!
     * \brief setCustomPath
     * \param rootPath 指定顶层路径
     */
    virtual void setCustomPath(QString rootPath);

    /*!
     * \brief currentPath
     * \param dirType: RelativeName:相对路径，一般指文件名或者文件夹名，不包含路径。AbsoluteName:绝对路径名,包含路径
     * \return 路径名,如果没有选中，则返回当前的根结点路径
     */
    QString currentPath(DirName dirType = AbsoluteName, SLASHTYPE defType = BACKSLASH) const;

    /*!
     * \brief isCurrentPathDir
     * \return ISFOLDER:当前选中的是文件夹。ISFILE:当前选中的是文件。NOITEMSELECTED:当前未选中项
     */
    ISDIR isCurrentPathDir() const;

    /*!
     * \brief upToParentIndex
     * \return 向上层移动,并返回是否成功
     */
    bool upToParentDir();

    /*!
     * \brief setHintBeShown 设置是否显示提示小框。默认提示
     * \param show
     */
    void setHintBeShown(bool show){m_bShowHint = show;}

    bool hintBeShown() const{return m_bShowHint;}

    /*!
     * \brief setHintExistTime 设置提示小框的自动消失时间，单位:秒。默认2秒
     * \param seconds
     */
    void setHintExistTime(int seconds){m_hintExistSeconds = qMax(1, seconds);}

    int hintExistTime() const{return m_hintExistSeconds;}

    /*!
     * \brief setOpenFileWhileDoubleClicked 设置是否在双击文件时打开。默认关闭
     * \param openFile
     */
    void setOpenFileWhileDoubleClicked(bool openFile){m_bOpenFileWhileDblClk = openFile;}

    bool openFileWhileDoubleClicked() const{return m_bOpenFileWhileDblClk;}

    /*!
     * \brief setNameFilters 设置要显示的文件后缀列表
     * \param filters 其子项是形如"*.exe"的字符串
     *@warning 当调用setModel将model设置为其它非QFielSystemModel子类类型的实例对象时，此方法将无效，行为将是未定义的。
     */
    void setNameFilters(const QStringList &filters);

    /*!
     * \brief nameFilters 获得当前的文件过滤后缀列表
     * \return
     */
    QStringList nameFilters() const;

    /*!
     * \brief 设置当前路径,为空，则跳转到根节点 add by hanll-a
     * \return
     */
    void setCurrentPath(const QString &path);

signals:
    void upToDir(const QModelIndex &parentDirIndex);
    void rootIndexChanged(const QModelIndex &rootIndex);

public slots:
    void setRootIndex(const QModelIndex &index);
    void showHint(const QModelIndex &index);
    void mouseDoubleClickSlot(const QModelIndex &index);

protected:
    void mousePressEvent(QMouseEvent *event);
    void drawComment(const QPersistentModelIndex &index);
    bool adjustChildWidgetPos(QWidget *child);

protected:
    GCommentFrame *commentFrame;
    int m_hintExistSeconds;
    bool m_bShowHint;
    bool m_bFirstShow;
    bool m_bOpenFileWhileDblClk;

private:
    Q_DISABLE_COPY(GLDShellListView)
    void init();
    friend class GLDShellComboBox;
};

#endif // GLDSHELLTREEVIEW_H
