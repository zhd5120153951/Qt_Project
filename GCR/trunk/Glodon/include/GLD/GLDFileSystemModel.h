#ifndef GLDFILESYSTEMMODEL_H
#define GLDFILESYSTEMMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qdir.h>
#include <QtWidgets/qfileiconprovider.h>
#include <private/qabstractitemmodel_p.h>
#include "GLDListView.h"

#ifndef QT_NO_DIRMODEL

class GLDShellComboBoxItemDelegate;
class GLDFileSystemModelPrivate;
class GLDFileSystemModel;

class GLDWIDGETSHARED_EXPORT GLDCustomFileSystemModel : public QAbstractItemModel
{
public:
    explicit GLDCustomFileSystemModel(QObject *parent = 0);
    GLDCustomFileSystemModel(QAbstractItemModelPrivate &dd, QObject *parent = 0);
    virtual ~GLDCustomFileSystemModel() {}

public:
    enum Roles {
        FileIconRole = Qt::DecorationRole,
        FilePathRole = Qt::UserRole + 1,
        FileNameRole = Qt::UserRole + 2,
        HideChildRole = Qt::UserRole + 3 // 用户GLDFileDialog中，在左边的目录树中，一个子都不需要显示，需要返回true
    };
    virtual QModelIndex index(const QString &path, int column = 0) const = 0;

    virtual bool isDir(const QModelIndex &index) const = 0;
    virtual QString filePath(const QModelIndex &index) const = 0;
    virtual QString makeCaseCorrespondingToSystem(const QString &path) const = 0;
    virtual GLDFileSystemModel *fileSystemModel() const;
    virtual QModelIndex mkdir(const QModelIndex &parent, const QString &name) = 0;
    virtual void refresh(const QModelIndex &parent = QModelIndex()) = 0;
    virtual bool rmdir(const QModelIndex &index);
    virtual bool remove(const QModelIndex &index);

public:
    /*!
     * \brief 设置view的显示模式模式,model需要根据view的显示模式，来调节对齐方式
     * \param viewMode
     */
    virtual void setViewMode(GLDListView::GLDViewMode viewMode);
    virtual GLDListView::GLDViewMode viewMode();

protected:
    GLDListView::GLDViewMode m_enViewMode;
};

class GLDWIDGETSHARED_EXPORT GLDFileSystemModel : public GLDCustomFileSystemModel
{
    Q_OBJECT
    Q_PROPERTY(bool resolveSymlinks READ resolveSymlinks WRITE setResolveSymlinks)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool lazyChildCount READ lazyChildCount WRITE setLazyChildCount)

public:
    explicit GLDFileSystemModel(bool includingFile = false, QObject *parent = 0);
    ~GLDFileSystemModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool hasChildren(const QModelIndex &index = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;

    // GLDFileSystemModel specific API

    void setIconProvider(QFileIconProvider *provider);
    QFileIconProvider *iconProvider() const;

    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;

    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;

    void setSorting(QDir::SortFlags sort);
    QDir::SortFlags sorting() const;

    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;

    void setReadOnly(bool enable);
    bool isReadOnly() const;

    void setLazyChildCount(bool enable);
    bool lazyChildCount() const;

    QModelIndex index(const QString &path, int column = 0) const;

    bool isDir(const QModelIndex &index) const;
    QModelIndex mkdir(const QModelIndex &parent, const QString &name);
    bool rmdir(const QModelIndex &index);
    bool remove(const QModelIndex &index);

    QString filePath(const QModelIndex &index) const;
    QString fileName(const QModelIndex &index) const;
    QIcon fileIcon(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;

#ifdef Q_NO_USING_KEYWORD
    inline QObject *parent() const { return QObject::parent(); }
#else
    using QObject::parent;
#endif

    QString makeCaseCorrespondingToSystem(const QString &path) const;

    void refresh(const QModelIndex &parent = QModelIndex());

protected:
    GLDFileSystemModel(const QStringList &nameFilters, QDir::Filters filters,
              QDir::SortFlags sort, QObject *parent = 0);
    GLDFileSystemModel(GLDFileSystemModelPrivate &, QObject *parent = 0);
    friend class QFileDialogPrivate;
private:
    Q_DECLARE_PRIVATE(GLDFileSystemModel)
    Q_DISABLE_COPY(GLDFileSystemModel)
    Q_PRIVATE_SLOT(d_func(), void _q_refresh())
};

class GLDWIDGETSHARED_EXPORT GLDFileSystemModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(GLDFileSystemModel)

public:
    struct QDirNode
    {
        QDirNode() : parent(0), populated(false), stat(false) {}
        ~QDirNode() { children.clear(); }

        QDirNode *parent;
        QFileInfo info;
        QIcon icon; // cache the icon
        mutable QVector<QDirNode> children;
        mutable bool populated; // have we read the children
        mutable bool stat;
    };

    GLDFileSystemModelPrivate()
        : resolveSymlinks(true),
          readOnly(true),
          lazyChildCount(false),
          allowAppendChild(true),
          iconProvider(&defaultProvider),
          shouldStat(true) // ### This is set to false by QFileDialog
    { }

    void init();
    QDirNode *node(int row, QDirNode *parent) const;
    QVector<QDirNode> children(QDirNode *parent, bool stat) const;

    void _q_refresh();

    void savePersistentIndexes();
    void restorePersistentIndexes();

    QFileInfoList entryInfoList(const QString &path) const;
    QStringList entryList(const QString &path) const;

    QString name(const QModelIndex &index) const;
    QString size(const QModelIndex &index) const;
    QString type(const QModelIndex &index) const;
    QString updateDateTime(const QModelIndex &index) const;
    QString createDateTime(const QModelIndex &index) const;

    void appendChild(GLDFileSystemModelPrivate::QDirNode *parent, const QString &path) const;
    void removeChild(GLDFileSystemModelPrivate::QDirNode *parent, const QModelIndex &index) const;
    void removeAll(GLDFileSystemModelPrivate::QDirNode *pNode);
    static QFileInfo resolvedInfo(QFileInfo info);

    QDirNode *node(const QModelIndex &index) const;
    void populate(QDirNode *parent) const;
    void clear(QDirNode *parent) const;

    void invalidate();

    mutable QDirNode root;
    bool resolveSymlinks;
    bool readOnly;
    bool lazyChildCount;
    bool allowAppendChild;

    QDir::Filters filters;
    QDir::SortFlags sort;
    QStringList nameFilters;

    QFileIconProvider *iconProvider;
    QFileIconProvider defaultProvider;

    struct SavedPersistent {
        QString path;
        int column;
        QPersistentModelIndexData *data;
        QPersistentModelIndex index;
    };

    QList<SavedPersistent> savedPersistent;
    QPersistentModelIndex toBeRefreshed;

    bool shouldStat; // use the "carefull not to stat directories" mode
};

#endif //ifndef QT_NO_DIRMODEL
#endif // GLDFILESYSTEMMODEL_H
