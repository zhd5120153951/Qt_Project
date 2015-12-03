/****
 * @file   : GLDShellComboBoxEx.h
 * @brief  : 可以美化的GLDShellComboBox
 *           需要注意cpp文件中的strShowComboBox、strComboBoxProp、strIsLineEdit
 *           可以设置是否单行显示，默认为单行显示，使用setEditToLineEdit方法可以更改
 *           可以设置是否显示编辑框图标，默认为不显示
 *           
 *           可以自定义图标，在QSS的注释中，以下面字符串开头，紧接上对应图标的路径，随后换行
 *           "QFileIconProviderComputer" 
 *           "QFileIconProviderDesktop" 
 *           "QFileIconProviderTrashcan" 
 *           "QFileIconProviderNetwork" 
 *           "QFileIconProviderDrive" 
 *           "QFileIconProviderFolder" 
 *           "QFileIconProviderFile";
 *           
 *           同样的方法可以自定义文件列表中的字体颜色，“RootColor”对应第一级节点（磁盘），
 *           "OthersColor"对应剩余的节点
 *
 * @date   : 2014-08-16
 * @author : chenyp-a
 * @remarks:
 ****/
#ifndef GLDSHELLCOMBOBOXNOLINEICON_H
#define GLDSHELLCOMBOBOXNOLINEICON_H

#include <QMap>
#include <QFileIconProvider>

#include "GLDShellWidgets.h"
#include "GLDTextEdit.h"
#include "GLDFileSystemModel.h"
#include "GLDWidget_Global.h"

class GLDShellTreeViewEx;
class GLDFileSystemModelEX;
class GLDShellComboBoxItemDelegate;
class GLDFileIconProvider;
class GLDShellComboBoxEx;

class GLDWIDGETSHARED_EXPORT GLDShellComboBoxItemDelegate : public QStyledItemDelegate
{ 
    Q_OBJECT
public:
    explicit GLDShellComboBoxItemDelegate(const QString &rootPath = QString(), QWidget *parent = 0);
   
public:
    QPixmap pixmap(const QModelIndex &index)const;
    void setTreeView(GLDShellTreeViewEx *treeView);
    void setFileSystemModel(GLDFileSystemModelEX *model);
    void setExpandedIndex(const QModelIndex &index);
    bool isParentExpandedRecently(const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    
private:
    QModelIndex findLonelyExpanded(const QModelIndex &root);    
    QPixmap icon(const QModelIndex &index);

private slots:
    void doAfterExpanded(const QModelIndex &index);
    void doAfterCollapsed(const QModelIndex &index);

private:
    GLDShellTreeViewEx *m_treeView;
    GLDFileSystemModelEX *m_model;
    QModelIndex m_indexExpanded;
};

class GLDWIDGETSHARED_EXPORT GLDFileIconProvider : public QFileIconProvider
{ 
public:
    GLDFileIconProvider();
    virtual ~GLDFileIconProvider();

public:
    virtual QIcon icon(IconType type) const;
    virtual QIcon icon(const QFileInfo & info) const;    
    void loadIcon(QMap<QFileIconProvider::IconType, QString> mapURL);
    void loadSetting(const QString &setting);
    
private:
    QMap<QFileIconProvider::IconType, QIcon> m_map_icon;
};

class GLDWIDGETSHARED_EXPORT GLDFileSystemModelEX : public GLDFileSystemModel
{ 
    Q_OBJECT
public:
    explicit GLDFileSystemModelEX(bool includingFile = false, QObject *parent = 0);
    ~GLDFileSystemModelEX();
    
public:
    QVariant data(const QModelIndex &index, int role) const;    
    QIcon fileIcon(const QModelIndex &index) const;    
    QColor fileNameColor(const QModelIndex &index) const;    
    void setFontColor(const QString &setting);    
    inline void setDelegate(GLDShellComboBoxItemDelegate *delegate){ m_delegate = delegate; }

protected:
    GLDFileSystemModelEX(const QStringList &nameFilters, QDir::Filters filters,
              QDir::SortFlags sort, QObject *parent = 0);
    GLDFileSystemModelEX(GLDFileSystemModelPrivate &model, QObject *parent = 0);
    
private:
    QColor m_colorRoot;
    QColor m_colorOthers;
    GLDShellComboBoxItemDelegate *m_delegate;    
    GLDFileIconProvider *m_pIconProvider;

    Q_DECLARE_PRIVATE(GLDFileSystemModel)
};

/**
 * @brief 
 *
 * @class GLDShellComboBoxEx
 */
class GLDWIDGETSHARED_EXPORT GLDShellComboBoxEx : public GLDShellComboBox
{ 
    Q_OBJECT
public:
    explicit GLDShellComboBoxEx(const QString &rootPath = QString(), QWidget *parent = 0);
    
public:
    void init(const QString &rootPath);
    void setHasBorder(bool bHasBorder);
    void setEditProperty(bool bShow);
    void setFileModel(GLDCustomFileSystemModel *model);

    /*!
     * \brief setShellListView
     * \param 设置联动的GLDShellListView
     *@warning 只能关联一个list
     */
    void setShellListView(GLDShellListView *list);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    void autoSetContainerHeight();
    void keyPressEvent(QKeyEvent *event);
    void updateEditFieldGeometry();

signals:
    void onHide();    
    void shouldShowPopup(bool &value);    
    void afterPopupShowed();//信号，下拉框被显示    
    void afterPopupHid();//信号，下拉框被隐藏    
    void cursorPositionChanged();    
    void expanded(const QModelIndex &index);    
    void collapsed(const QModelIndex &index);

private slots:
    void onSignalOnHide();    
    void onShouldShowPopup(bool &value);    
    void doAfterEditCursorChanged();
    void doAfterExpanded(const QModelIndex &index);    
    void doAfterCollapsed(const QModelIndex &index);

private:
    void initConnection();
    void installEventFilters();
    void initShadow();
    void initFileModel(const QString &rootPath);
    void initTreeView(QModelIndex pathIndex);
    void initTreeViewIndentation();
    void initDelegate(GLDShellTreeViewEx *pTreeView);
    void initPopupFlags();
    
private:
    bool m_bIsShow; /** 下来框是否显示*/
    bool m_bIsLineEdit; /** 编辑框是否为行编辑框*/
    bool m_bIsShowIcon;/** 是否显示编辑框图标*/
    bool m_bIsDefaultIcon;/** 是否使用默认图标*/
    bool m_bIsDrag;
    bool m_bIsPopMove;
    int m_nCanSetCursor;
    bool m_bHasShadow;
    QString m_text;
    GLDShellComboBoxItemDelegate *m_delegate;
};

#endif // GLDSHELLCOMBOBOXNOLINEICON_H
