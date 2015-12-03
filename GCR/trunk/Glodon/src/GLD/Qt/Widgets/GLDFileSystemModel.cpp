#include "GLDFileSystemModel.h"

#ifndef QT_NO_DIRMODEL

#include "GLDStrUtils.h"
#include "GLDShellComboBoxEx.h"
#include "GLDFileUtils.h"

#include <QStack>
#include <QFile>
#include <QFileSystemModel>
#include <QUrl>
#include <QMimeData>
#include <QStandardPaths>
#include <QPair>
#include <QVector>
#include <QObject>
#include <QDateTime>
#include <QLocale>
#include <QStyle>
#include <QApplication>

/**
 * @class GLDFileSystemModel
 * @brief 文件系统模型
 * @brief Copy from QDirModel source file
 */

/*!
    Constructs a new directory model with the given \a parent.
    Only those files matching the \a nameFilters and the
    \a filters are included in the model. The sort order is given by the
    \a sort flags.
*/
GLDFileSystemModel::GLDFileSystemModel(const QStringList &nameFilters,
                     QDir::Filters filters,
                     QDir::SortFlags sort,
                     QObject *parent)
    : GLDCustomFileSystemModel(*new GLDFileSystemModelPrivate, parent)
{
    Q_D(GLDFileSystemModel);
    // we always start with QDir::drives()
    d->nameFilters = nameFilters.isEmpty() ? QStringList(QLatin1String("*")) : nameFilters;
    d->filters = filters;
    d->sort = sort;
    d->root.parent = 0;
    d->root.info = QFileInfo();
    d->clear(&d->root);
}

/*!
  Constructs a directory model with the given \a parent.
*/
GLDFileSystemModel::GLDFileSystemModel(bool includingFile, QObject *parent)
    : GLDCustomFileSystemModel(*new GLDFileSystemModelPrivate, parent)
{
    Q_D(GLDFileSystemModel);
    QStringList nameFilters;
    d->nameFilters = nameFilters.isEmpty() ? QStringList(QLatin1String("*")) : nameFilters;
    QDir::Filters filts = QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot;
    if (!includingFile)
    {
        filts &= ~QDir::Files;
    }
    d->filters = filts;

    QDir::SortFlags sorts = QDir::DirsFirst | QDir::Name | QDir::IgnoreCase;
    d->sort = sorts;
    d->root.parent = 0;
    d->root.info = QFileInfo();
    d->clear(&d->root);

    d->init();
}

/*!
    \internal
*/
GLDFileSystemModel::GLDFileSystemModel(GLDFileSystemModelPrivate &dd, QObject *parent)
    : GLDCustomFileSystemModel(dd, parent)
{
    Q_D(GLDFileSystemModel);
    d->init();
}

/*!
  Destroys this directory model.
*/

GLDFileSystemModel::~GLDFileSystemModel()
{

}

/*!
  Returns the model item index for the item in the \a parent with the
  given \a row and \a column.

*/

QModelIndex GLDFileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const GLDFileSystemModel);
    // note that rowCount does lazy population
    if (column < 0 || column >= columnCount(parent) || row < 0 || parent.column() > 0)
        return QModelIndex();
    // make sure the list of children is up to date
    GLDFileSystemModelPrivate::QDirNode *nod = (d->indexValid(parent) ? d->node(parent) : &d->root);
    Q_ASSERT(nod);
    if (!nod->populated)
        d->populate(nod); // populate without stat'ing
    if (row >= nod->children.count())
        return QModelIndex();
    // now get the internal pointer for the index
    GLDFileSystemModelPrivate::QDirNode *pNode = d->node(row, d->indexValid(parent) ? nod : 0);
    Q_ASSERT(pNode);

    return createIndex(row, column, pNode);
}

/*!
  Return the parent of the given \a child model item.
*/

QModelIndex GLDFileSystemModel::parent(const QModelIndex &child) const
{
    Q_D(const GLDFileSystemModel);

    if (!d->indexValid(child))
        return QModelIndex();
    GLDFileSystemModelPrivate::QDirNode *node = d->node(child);
    GLDFileSystemModelPrivate::QDirNode *par = (node ? node->parent : 0);
    if (par == 0) // parent is the root node
        return QModelIndex();

    // get the parent's row
    const QVector<GLDFileSystemModelPrivate::QDirNode> c_children =
        par->parent ? par->parent->children : d->root.children;
    Q_ASSERT(c_children.count() > 0);
    int row = (par - &(c_children.at(0)));
    Q_ASSERT(row >= 0);

    return createIndex(row, 0, par);
}

/*!
  Returns the number of rows in the \a parent model item.

*/

int GLDFileSystemModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const GLDFileSystemModel);
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
    {
        if (!d->root.populated) // lazy population
            d->populate(&d->root);
        return d->root.children.count();
    }
    if (parent.model() != this)
        return 0;
    GLDFileSystemModelPrivate::QDirNode *pNode = d->node(parent);
    if (pNode->info.isDir() && !pNode->populated) // lazy population
        d->populate(pNode);
    return pNode->children.count();
}

/*!
  Returns the number of columns in the \a parent model item.

*/

int GLDFileSystemModel::columnCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    return 5;
}

/*!
  Returns the data for the model item \a index with the given \a role.
*/
QVariant GLDFileSystemModel::data(const QModelIndex &index, int role) const
{
    Q_D(const GLDFileSystemModel);
    if (!d->indexValid(index))
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0:
        {
            QString strName = d->name(index);
            if (0 == strName.compare("desktop", Qt::CaseInsensitive))
            {
                strName = tr("desktop");//TRANS_STRING("桌面"); //TODO 翻译的权宜之计
            }
            return strName;
        }
        case 1: return d->type(index);
        case 2: return d->updateDateTime(index);
        case 3: return d->createDateTime(index);
        case 4: return d->size(index);
        default:
            qWarning("data: invalid display value column %d", index.column());
            return QVariant();
        }
    }

    if (index.column() == 0)
    {
        if (role == FileIconRole)
            return fileIcon(index);
        if (role == FilePathRole)
            return filePath(index);
        if (role == FileNameRole)
            return fileName(index);
    }

    if (role == Qt::TextAlignmentRole)
    {
        if (m_enViewMode == GLDListView::VSReportMode)
        {
            switch (index.column())
            {
            case 0:
            case 1:
            case 2:
            case 3:
                return (QVariant)(Qt::AlignLeft | Qt::AlignVCenter);
            case 4:
                return (QVariant)(Qt::AlignRight | Qt::AlignVCenter);
            }
            return Qt::AlignCenter;
        }
        else if(m_enViewMode == GLDListView::VSIconMode)
        {
            return Qt::AlignCenter;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

/*!
  Sets the data for the model item \a index with the given \a role to
  the data referenced by the \a value. Returns true if successful;
  otherwise returns false.

  \sa Qt::ItemDataRole
*/

bool GLDFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(GLDFileSystemModel);
    if (!d->indexValid(index) || index.column() != 0
        || (flags(index) & Qt::ItemIsEditable) == 0 || role != Qt::EditRole)
        return false;

    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);
    QDir dir = node->info.dir();
    QString strname = value.toString();
    if (dir.rename(node->info.fileName(), strname))
    {
        node->info = QFileInfo(dir, strname);
        QModelIndex sibling = index.sibling(index.row(), 3);
        emit dataChanged(index, sibling);

        d->toBeRefreshed = index.parent();
        QMetaObject::invokeMethod(this, "_q_refresh", Qt::QueuedConnection);

        return true;
    }

    return false;
}

/*!
  Returns the data stored under the given \a role for the specified \a section
  of the header with the given \a orientation.
*/

QVariant GLDFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role != Qt::DisplayRole)
            return QVariant();
        switch (section) {
        case 0: return tr("Name");
        case 1: return tr("Size");
        case 2: return
        #ifdef __APPLE__
                    tr("Kind", "Match OS X Finder");
#else
                    tr("Type", "All other platforms");
#endif
            // Windows   - Type
            // OS X      - Kind
            // Konqueror - File Type
            // Nautilus  - Type
        case 3: return tr("Date Modified");
        default: return QVariant();
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

/*!
  Returns true if the \a parent model item has children; otherwise
  returns false.
*/

bool GLDFileSystemModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const GLDFileSystemModel);
    if (parent.column() > 0)
        return false;

    if (!parent.isValid()) // the invalid index is the "My Computer" item
        return true; // the drives
    GLDFileSystemModelPrivate::QDirNode *pNode = d->node(parent);
    Q_ASSERT(pNode);

    if (d->lazyChildCount) // optimization that only checks for children if the node has been populated
        return pNode->info.isDir();
    return pNode->info.isDir() && rowCount(parent) > 0;
}

/*!
  Returns the item flags for the given \a index in the model.

  \sa Qt::ItemFlags
*/
Qt::ItemFlags GLDFileSystemModel::flags(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!d->indexValid(index))
        return flags;
    flags |= Qt::ItemIsDragEnabled;
    if (d->readOnly)
        return flags;
    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);
    if ((index.column() == 0) && node->info.isWritable())
    {
        flags |= Qt::ItemIsEditable;
        if (fileInfo(index).isDir()) // is directory and is editable
            flags |= Qt::ItemIsDropEnabled;
    }
    return flags;
}

/*!
  Sort the model items in the \a column using the \a order given.
  The order is a value defined in \l Qt::SortOrder.
*/

void GLDFileSystemModel::sort(int column, Qt::SortOrder order)
{
    QDir::SortFlags sort = QDir::DirsFirst | QDir::IgnoreCase;
    if (order == Qt::DescendingOrder)
        sort |= QDir::Reversed;

    switch (column) {
    case 0:
        sort |= QDir::Name;
        break;
    case 1:
        sort |= QDir::Size;
        break;
    case 2:
        sort |= QDir::Type;
        break;
    case 3:
        sort |= QDir::Time;
        break;
    default:
        break;
    }

    setSorting(sort);
}

/*!
    Returns a list of MIME types that can be used to describe a list of items
    in the model.
*/

QStringList GLDFileSystemModel::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

/*!
    Returns an object that contains a serialized description of the specified
    \a indexes. The format used to describe the items corresponding to the
    indexes is obtained from the mimeTypes() function.

    If the list of indexes is empty, 0 is returned rather than a serialized
    empty list.
*/

QMimeData *GLDFileSystemModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;
    QList<QModelIndex>::const_iterator it = indexes.begin();
    for (; it != indexes.end(); ++it)
    {
        if ((*it).column() == 0)
        {
            urls << QUrl::fromLocalFile(filePath(*it));
        }
    }
    QMimeData *data = new QMimeData();
    data->setUrls(urls);
    return data;
}

/*!
    Handles the \a data supplied by a drag and drop operation that ended with
    the given \a action over the row in the model specified by the \a row and
    \a column and by the \a parent index.

    Returns true if the drop was successful, and false otherwise.

    \sa supportedDropActions()
*/

bool GLDFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                             int /* row */, int /* column */, const QModelIndex &parent)
{
    Q_D(GLDFileSystemModel);
    if (!d->indexValid(parent) || isReadOnly())
        return false;

    bool bsuccess = true;
    QString to = filePath(parent) + QDir::separator();
    QModelIndex _parent = parent;

    QList<QUrl> urls = data->urls();
    QList<QUrl>::const_iterator it = urls.constBegin();

    switch (action) {
    case Qt::CopyAction:
        for (; it != urls.constEnd(); ++it)
        {
            QString strpath = (*it).toLocalFile();
            bsuccess = QFile::copy(strpath, to + QFileInfo(strpath).fileName()) && bsuccess;
        }
        break;
    case Qt::LinkAction:
        for (; it != urls.constEnd(); ++it)
        {
            QString strpath = (*it).toLocalFile();
            bsuccess = QFile::link(strpath, to + QFileInfo(strpath).fileName()) && bsuccess;
        }
        break;
    case Qt::MoveAction:
        for (; it != urls.constEnd(); ++it)
        {
            QString strpath = (*it).toLocalFile();
            if (QFile::copy(strpath, to + QFileInfo(strpath).fileName())
               && QFile::remove(strpath))
            {
                QModelIndex idx=index(QFileInfo(strpath).path());
                if (idx.isValid())
                {
                    refresh(idx);
                    //the previous call to refresh may invalidate the _parent. so recreate a new QModelIndex
                    _parent = index(to);
                }
            }
            else
            {
                bsuccess = false;
            }
        }
        break;
    default:
        return false;
    }

    if (bsuccess)
        refresh(_parent);

    return bsuccess;
}

/*!
  Returns the drop actions supported by this model.

  \sa Qt::DropActions
*/

Qt::DropActions GLDFileSystemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction; // FIXME: LinkAction is not supported yet
}

/*!
  Sets the \a provider of file icons for the directory model.

*/

void GLDFileSystemModel::setIconProvider(QFileIconProvider *provider)
{
    Q_D(GLDFileSystemModel);
    d->iconProvider = provider;
}

/*!
  Returns the file icon provider for this directory model.
*/

QFileIconProvider *GLDFileSystemModel::iconProvider() const
{
    Q_D(const GLDFileSystemModel);
    return d->iconProvider;
}

/*!
  Sets the name \a filters for the directory model.
*/

void GLDFileSystemModel::setNameFilters(const QStringList &filters)
{
    Q_D(GLDFileSystemModel);
    d->nameFilters = filters;
    d->populate(&(d->root));
    emit layoutAboutToBeChanged();
    if (d->shouldStat)
        refresh(QModelIndex());
    else
        d->invalidate();
    emit layoutChanged();
}

/*!
  Returns a list of filters applied to the names in the model.
*/

QStringList GLDFileSystemModel::nameFilters() const
{
    Q_D(const GLDFileSystemModel);
    return d->nameFilters;
}

/*!
  Sets the directory model's filter to that specified by \a filters.

  Note that the filter you set should always include the QDir::AllDirs enum value,
  otherwise GLDFileSystemModel won't be able to read the directory structure.

  \sa QDir::Filters
*/

void GLDFileSystemModel::setFilter(QDir::Filters filters)
{
    Q_D(GLDFileSystemModel);
    d->filters = filters;
    emit layoutAboutToBeChanged();
    if (d->shouldStat)
        refresh(QModelIndex());
    else
        d->invalidate();
    emit layoutChanged();
}

/*!
  Returns the filter specification for the directory model.

  \sa QDir::Filters
*/

QDir::Filters GLDFileSystemModel::filter() const
{
    Q_D(const GLDFileSystemModel);
    return d->filters;
}

/*!
  Sets the directory model's sorting order to that specified by \a sort.

  \sa QDir::SortFlags
*/

void GLDFileSystemModel::setSorting(QDir::SortFlags sort)
{
    Q_D(GLDFileSystemModel);
    d->sort = sort;
    emit layoutAboutToBeChanged();
    if (d->shouldStat)
        refresh(QModelIndex());
    else
        d->invalidate();
    emit layoutChanged();
}

/*!
  Returns the sorting method used for the directory model.

  \sa QDir::SortFlags
*/

QDir::SortFlags GLDFileSystemModel::sorting() const
{
    Q_D(const GLDFileSystemModel);
    return d->sort;
}

/*!
    \property GLDFileSystemModel::resolveSymlinks
    \brief Whether the directory model should resolve symbolic links

    This is only relevant on operating systems that support symbolic
    links.
*/
void GLDFileSystemModel::setResolveSymlinks(bool enable)
{
    Q_D(GLDFileSystemModel);
    d->resolveSymlinks = enable;
}

bool GLDFileSystemModel::resolveSymlinks() const
{
    Q_D(const GLDFileSystemModel);
    return d->resolveSymlinks;
}

/*!
  \property GLDFileSystemModel::readOnly
  \brief Whether the directory model allows writing to the file system

  If this property is set to false, the directory model will allow renaming, copying
  and deleting of files and directories.

  This property is true by default
*/

void GLDFileSystemModel::setReadOnly(bool enable)
{
    Q_D(GLDFileSystemModel);
    d->readOnly = enable;
}

bool GLDFileSystemModel::isReadOnly() const
{
    Q_D(const GLDFileSystemModel);
    return d->readOnly;
}

/*!
  \property GLDFileSystemModel::lazyChildCount
  \brief Whether the directory model optimizes the hasChildren function
  to only check if the item is a directory.

  If this property is set to false, the directory model will make sure that a directory
  actually containes any files before reporting that it has children.
  Otherwise the directory model will report that an item has children if the item
  is a directory.

  This property is false by default
*/

void GLDFileSystemModel::setLazyChildCount(bool enable)
{
    Q_D(GLDFileSystemModel);
    d->lazyChildCount = enable;
}

bool GLDFileSystemModel::lazyChildCount() const
{
    Q_D(const GLDFileSystemModel);
    return d->lazyChildCount;
}

/*!
  GLDFileSystemModel caches file information. This function updates the
  cache. The \a parent parameter is the directory from which the
  model is updated; the default value will update the model from
  root directory of the file system (the entire model).
*/

void GLDFileSystemModel::refresh(const QModelIndex &parent)
{
    Q_D(GLDFileSystemModel);

    GLDFileSystemModelPrivate::QDirNode *nnod = d->indexValid(parent) ? d->node(parent) : &(d->root);

    int nrowscounter = nnod->children.count();
    if (nrowscounter == 0)
    {
        emit layoutAboutToBeChanged();
        nnod->stat = true; // make sure that next time we read all the info
        nnod->populated = false;
        emit layoutChanged();
        return;
    }

    emit layoutAboutToBeChanged();
    d->savePersistentIndexes();
    d->rowsAboutToBeRemoved(parent, 0, nrowscounter - 1);
    nnod->stat = true; // make sure that next time we read all the info
//    d->clear(nnod);
    d->rowsRemoved(parent, 0, nrowscounter - 1);
    d->restorePersistentIndexes();
    emit layoutChanged();
}

/*!
    \overload

    Returns the model item index for the given \a path.
*/

QModelIndex GLDFileSystemModel::index(const QString &path, int column) const
{
    Q_D(const GLDFileSystemModel);

    if (path.isEmpty() || path == QCoreApplication::translate("QFileDialog", "My Computer"))
        return QModelIndex();

    QString strabsolutePath = QDir(path).absolutePath();
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    strabsolutePath = strabsolutePath.toLower();
    // On Windows, "filename......." and "filename" are equivalent
    if (strabsolutePath.endsWith(QLatin1Char('.')))
    {
        int nIter;
        for (nIter = strabsolutePath.count() - 1; nIter >= 0; --nIter)
        {
            if (strabsolutePath.at(nIter) != QLatin1Char('.'))
                break;
        }
        strabsolutePath = strabsolutePath.left(nIter + 1);
    }
#endif

    QStringList pathElements = strabsolutePath.split(QLatin1Char('/'), QString::SkipEmptyParts);
    if ((pathElements.isEmpty() || !QFileInfo(path).exists())
#if !defined(Q_OS_WIN) || defined(Q_OS_WINCE)
        && path != QLatin1String("/")
#endif
        )
        return QModelIndex();

    QModelIndex idx; // start with "My Computer"
    if (!d->root.populated) // make sure the root is populated
        d->populate(&d->root);

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (strabsolutePath.startsWith(QLatin1String("//"))) // UNC path
    {
        QString strhost = pathElements.first();
        int row = 0;
        for (row = 0; row < d->root.children.count(); ++row)
        {
            if (d->root.children.at(row).info.fileName() == strhost)
            {
                break;
            }
        }
        bool bchildAppended = false;
        if (row >= d->root.children.count() && d->allowAppendChild)
        {
            d->appendChild(&d->root, QLatin1String("//") + strhost);
            bchildAppended = true;
        }
        idx = index(row, 0, QModelIndex());
        pathElements.pop_front();
        if (bchildAppended)
            emit const_cast<GLDFileSystemModel*>(this)->layoutChanged();
    } else
#endif
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (pathElements.at(0).endsWith(QLatin1Char(':')))
    {
        pathElements[0] += QLatin1Char('/');
    }
#else
    // add the "/" item, since it is a valid path element on unix
    pathElements.prepend(QLatin1String("/"));
#endif

    for (int i = 0; i < pathElements.count(); ++i)
    {
        Q_ASSERT(!pathElements.at(i).isEmpty());
        QString strelement = pathElements.at(i);
        GLDFileSystemModelPrivate::QDirNode *parent = (idx.isValid() ? d->node(idx) : &d->root);

        Q_ASSERT(parent);
        if (!parent->populated)
            d->populate(parent);

        // search for the element in the child nodes first
        int row = -1;
        for (int j = parent->children.count() - 1; j >= 0; --j)
        {
            const QFileInfo& fi = parent->children.at(j).info;
            QString strchildFileName;
            strchildFileName = idx.isValid() ? fi.fileName() : fi.absoluteFilePath();
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
            strchildFileName = strchildFileName.toLower();
#endif
            if (strchildFileName == strelement)
            {
                if (i == pathElements.count() - 1)
                    parent->children[j].stat = true;
                row = j;
                break;
            }
        }

        // we couldn't find the path element, we create a new node since we _know_ that the path is valid
        if (row == -1)
        {
#if defined(Q_OS_WINCE)
            QString strNewPath;
            if (parent->info.isRoot())
                strNewPath = parent->info.absoluteFilePath() + element;
            else
                strNewPath = parent->info.absoluteFilePath() + QLatin1Char('/') + element;
#else
            QString strNewPath = parent->info.absoluteFilePath() + QLatin1Char('/') + strelement;
#endif
            if (!d->allowAppendChild || !QFileInfo(strNewPath).isDir())
                return QModelIndex();
            d->appendChild(parent, strNewPath);
            row = parent->children.count() - 1;
            if (i == pathElements.count() - 1) // always stat children of  the last element
                parent->children[row].stat = true;
            emit const_cast<GLDFileSystemModel*>(this)->layoutChanged();
        }

        Q_ASSERT(row >= 0);
        idx = createIndex(row, 0, static_cast<void*>(&parent->children[row]));
        Q_ASSERT(idx.isValid());
    }

    if (column != 0)
        return idx.sibling(idx.row(), column);
    return idx;
}

/*!
  Returns true if the model item \a index represents a directory;
  otherwise returns false.
*/

bool GLDFileSystemModel::isDir(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    if (!d->indexValid(index))
        return false;
    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);
    if (sameText(node->info.path(), "A:/"))
    {
        return false;
    }
    return node->info.isDir();
}

/*!
  Create a directory with the \a name in the \a parent model item.
*/
QModelIndex GLDFileSystemModel::mkdir(const QModelIndex &parent, const QString &name)
{
    Q_D(GLDFileSystemModel);
    if (!d->indexValid(parent) || isReadOnly())
        return QModelIndex();

    GLDFileSystemModelPrivate::QDirNode *ppNode = d->node(parent);
    QString strpath = ppNode->info.absoluteFilePath();
    // For the indexOf() method to work, the new directory has to be a direct child of
    // the parent directory.

    QDir newDir(name);
    QDir dir(strpath);
    if (newDir.isRelative())
        newDir = QDir(strpath + QLatin1Char('/') + name);
    QString strchildName = newDir.dirName(); // Get the singular name of the directory
    newDir.cdUp();

    if (newDir.absolutePath() != dir.absolutePath() || !dir.mkdir(name))
        return QModelIndex(); // nothing happened
    GString strChildPath;
    if (strpath.endsWith('/'))
    {
        strChildPath = strpath + strchildName;
    }
    else
    {
        strChildPath = strpath + "/" + strchildName;
    }

    beginInsertRows(parent, ppNode->children.count(), ppNode->children.count());
    d->appendChild(ppNode, strChildPath);
    endInsertRows();

    QModelIndex inde = index(ppNode->children.count() - 1, 0, parent); // return an invalid index

    return inde;
}

/*!
  Removes the directory corresponding to the model item \a index in the
  directory model and \b{deletes the corresponding directory from the
  file system}, returning true if successful. If the directory cannot be
  removed, false is returned.

  \warning This function deletes directories from the file system; it does
  \b{not} move them to a location where they can be recovered.

  \sa remove()
*/

bool GLDFileSystemModel::rmdir(const QModelIndex &index)
{
    Q_D(GLDFileSystemModel);
    if (!d->indexValid(index) || isReadOnly())
        return false;

    GLDFileSystemModelPrivate::QDirNode *node = d_func()->node(index);
    if (!node->info.isDir())
    {
        qWarning("rmdir: the node is not a directory");
        return false;
    }

    QModelIndex par = parent(index);
    GLDFileSystemModelPrivate::QDirNode *pNode = d_func()->node(par);
    QString strPath = node->info.absoluteFilePath();
    if (!deleteTree(strPath))
    {
        return false;
    }

    beginRemoveRows(index.parent(), index.row(), index.row());

    d->removeChild(pNode, index);
    refresh(par);

    endRemoveRows();
    return true;
}

/*!
  Removes the model item \a index from the directory model and \b{deletes the
  corresponding file from the file system}, returning true if successful. If the
  item cannot be removed, false is returned.

  \warning This function deletes files from the file system; it does \b{not}
  move them to a location where they can be recovered.

  \sa rmdir()
*/

bool GLDFileSystemModel::remove(const QModelIndex &index)
{
    Q_D(GLDFileSystemModel);
    if (!d->indexValid(index) || isReadOnly())
        return false;

    GLDFileSystemModelPrivate::QDirNode *pNod = d_func()->node(index);
    if (pNod->info.isDir())
        return false;

    QModelIndex par = parent(index);
    GLDFileSystemModelPrivate::QDirNode *pNode = d_func()->node(par);
    QDir dir = pNode->info.dir(); // parent dir
    QString strPath = pNod->info.absoluteFilePath();
    if (!dir.remove(strPath))
        return false; 

    beginRemoveRows(index.parent(), index.row(), index.row());

    d->removeChild(pNode, index);
    refresh(par);

    endRemoveRows();
    return true;
}

/*!
  Returns the path of the item stored in the model under the
  \a index given.

*/

QString GLDFileSystemModel::filePath(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    if (d->indexValid(index)) {
        QFileInfo fi = fileInfo(index);
        if (d->resolveSymlinks && fi.isSymLink())
            fi = d->resolvedInfo(fi);
        return QDir::cleanPath(fi.absoluteFilePath());
    }
    return QString(); // root path
}

/*!
  Returns the name of the item stored in the model under the
  \a index given.

*/

QString GLDFileSystemModel::fileName(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    if (!d->indexValid(index))
        return QString();
    QFileInfo info = fileInfo(index);
    if (info.isRoot())
        return info.absoluteFilePath();
    if (d->resolveSymlinks && info.isSymLink())
        info = d->resolvedInfo(info);
    return info.fileName();
}

/*!
  Returns the icons for the item stored in the model under the given
  \a index.
*/

QIcon GLDFileSystemModel::fileIcon(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    if (!d->indexValid(index))
        return d->iconProvider->icon(QFileIconProvider::Computer);
    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);
    if (node->icon.isNull())
        node->icon = d->iconProvider->icon(node->info);
    return node->icon;
}

/*!
  Returns the file information for the specified model \a index.

  \b{Note:} If the model index represents a symbolic link in the
  underlying filing system, the file information returned will contain
  information about the symbolic link itself, regardless of whether
  resolveSymlinks is enabled or not.

  \sa QFileInfo::symLinkTarget()
*/

QFileInfo GLDFileSystemModel::fileInfo(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);
    Q_ASSERT(d->indexValid(index));

    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);
    return node->info;
}

QString GLDFileSystemModel::makeCaseCorrespondingToSystem(const QString &path) const
{
    QModelIndex dirIndex = index(path);
    QString rightCase = filePath(dirIndex);
    return rightCase;
}

/*!
  \fn QObject *GLDFileSystemModel::parent() const
  \internal
*/

/**
 * @class GLDFileSystemModelPrivate
 * @brief 文件系统模型私有类
 * @brief The root node is never seen outside the model.
 */

void qt_setDirModelShouldNotStat(GLDFileSystemModelPrivate *modelPrivate)
{
    modelPrivate->shouldStat = false;
}

void GLDFileSystemModelPrivate::init()
{
    filters = QDir::AllEntries | QDir::NoDotAndDotDot;
    sort = QDir::Name;
    if (nameFilters.isEmpty())
    {
        nameFilters << QLatin1String("*");
    }
    root.parent = 0;
    root.info = QFileInfo();
    clear(&root);
    roleNames.insertMulti(GLDCustomFileSystemModel::FileIconRole, QByteArrayLiteral("fileIcon")); // == Qt::decoration
    roleNames.insert(GLDCustomFileSystemModel::FilePathRole, QByteArrayLiteral("filePath"));
    roleNames.insert(GLDCustomFileSystemModel::FileNameRole, QByteArrayLiteral("fileName"));
}

GLDFileSystemModelPrivate::QDirNode *GLDFileSystemModelPrivate::node(int row, QDirNode *parent) const
{
    if (row < 0)
        return 0;

    bool bIsDir = !parent || parent->info.isDir();
    QDirNode *pNode = (parent ? parent : &root);
    if (bIsDir && !pNode->populated)
        populate(pNode); // will also resolve symlinks

    if (row >= pNode->children.count())
    {
        qWarning("node: the row does not exist");
        return 0;
    }

    return const_cast<QDirNode*>(&pNode->children.at(row));
}

QVector<GLDFileSystemModelPrivate::QDirNode> GLDFileSystemModelPrivate::children(QDirNode *parent, bool stat) const
{
    Q_ASSERT(parent);
    //此处先这么绕过去，有的有A盘的机子，会出现驱动器未就绪
    if (sameText(parent->info.path(), "A:/"))
    {
        QVector<QDirNode> nodes(0);
        return nodes;
    }
    QFileInfoList infoList;
    if (parent == &root)
    {
        parent = 0;
        QString strDesktopDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QFileInfo ifleInfo(strDesktopDir);
        infoList = QDir::drives();
        infoList.prepend(ifleInfo);
    }
    else if (parent->info.isDir())
    {
        //resolve directory links only if requested.
        if (parent->info.isSymLink() && resolveSymlinks)
        {
            QString strlink = parent->info.symLinkTarget();
            if (strlink.size() > 1 && strlink.at(strlink.size() - 1) == QDir::separator())
                strlink.chop(1);
            if (stat)
                infoList = entryInfoList(strlink);
            else
                infoList = QDir(strlink).entryInfoList(nameFilters, QDir::AllEntries | QDir::System);
        }
        else
        {
            if (stat)
                infoList = entryInfoList(parent->info.absoluteFilePath());
            else
                infoList = QDir(parent->info.absoluteFilePath()).entryInfoList(nameFilters,
                                                                               QDir::AllEntries | QDir::System);
        }
    }

    QVector<QDirNode> nodes(infoList.count());
    int nDriverAIndex = -1;
    for (int i = 0; i < infoList.count(); ++i)
    {
        QDirNode &node = nodes[i];
        node.parent = parent;
        node.info = infoList.at(i);
        if (sameText(node.info.path(), "A:/"))
        {
            nDriverAIndex = i;
        }
        node.populated = false;
        node.stat = shouldStat;
    }
    if (nDriverAIndex != -1)
    {
        nodes.remove(nDriverAIndex);
    }

    return nodes;
}

void GLDFileSystemModelPrivate::_q_refresh()
{
    Q_Q(GLDFileSystemModel);
    q->refresh(toBeRefreshed);
    toBeRefreshed = QModelIndex();
}

void GLDFileSystemModelPrivate::savePersistentIndexes()
{
    Q_Q(GLDFileSystemModel);
    savedPersistent.clear();
    foreach (QPersistentModelIndexData *data, persistent.indexes) {
        SavedPersistent saved;
        QModelIndex index = data->index;
        saved.path = q->filePath(index);
        saved.column = index.column();
        saved.data = data;
        saved.index = index;
        savedPersistent.append(saved);
    }
}

void GLDFileSystemModelPrivate::restorePersistentIndexes()
{
    Q_Q(GLDFileSystemModel);
    bool ballow = allowAppendChild;
    allowAppendChild = false;
    for (int i = 0; i < savedPersistent.count(); ++i)
    {
        QPersistentModelIndexData *data = savedPersistent.at(i).data;
        QString strpath = savedPersistent.at(i).path;
        int ncolumn = savedPersistent.at(i).column;
        QModelIndex idx = q->index(strpath, ncolumn);
        if (idx != data->index || data->model == 0)
        {
            //data->model may be equal to 0 if the model is getting destroyed
            persistent.indexes.remove(data->index);
            data->index = idx;
            data->model = q;
            if (idx.isValid())
                persistent.indexes.insert(idx, data);
        }
    }
    savedPersistent.clear();
    allowAppendChild = ballow;
}

QFileInfoList GLDFileSystemModelPrivate::entryInfoList(const QString &path) const
{
    const QDir c_dir(path);
    return c_dir.entryInfoList(nameFilters, filters, sort);
}

QStringList GLDFileSystemModelPrivate::entryList(const QString &path) const
{
    const QDir c_dir(path);
    return c_dir.entryList(nameFilters, filters, sort);
}

QString GLDFileSystemModelPrivate::name(const QModelIndex &index) const
{
    const QDirNode *pnode = node(index);
    const QFileInfo c_info = pnode->info;
    if (c_info.isRoot())
    {
        QString strName = c_info.absoluteFilePath();
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
        if (strName.startsWith(QLatin1Char('/'))) // UNC host
            return c_info.fileName();
        if (strName.endsWith(QLatin1Char('/')))
            strName.chop(1);
#endif
        return strName;
    }
    return c_info.fileName();
}

QString GLDFileSystemModelPrivate::size(const QModelIndex &index) const
{
    const QDirNode *pnode = node(index);
    if (pnode->info.isDir())
    {
#ifdef __APPLE__
        return QLatin1String("--");
#else
        return QLatin1String("");
#endif
    // Windows   - ""
    // OS X      - "--"
    // Konqueror - "4 KB"
    // Nautilus  - "9 items" (the number of children)
    }

    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calulated by dividing by 1024 so we do what they do.
    const quint64 c_kb = 1024;
    const quint64 c_mb = 1024 * c_kb;
    const quint64 c_gb = 1024 * c_mb;
    const quint64 c_tb = 1024 * c_gb;
    quint64 bytes = pnode->info.size();
    if (bytes >= c_tb)
        return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / c_tb, 'f', 3));
    if (bytes >= c_gb)
        return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / c_gb, 'f', 2));
    if (bytes >= c_mb)
        return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / c_mb, 'f', 1));
    if (bytes >= c_kb)
        return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / c_kb));
    return QFileSystemModel::tr("%1 byte(s)").arg(QLocale().toString(bytes));
}

QString GLDFileSystemModelPrivate::type(const QModelIndex &index) const
{
    return iconProvider->type(node(index)->info);
}

QString GLDFileSystemModelPrivate::updateDateTime(const QModelIndex &index) const
{
#ifndef QT_NO_DATESTRING
    return node(index)->info.lastModified().toString(Qt::LocalDate);
#else
    Q_UNUSED(index);
    return QString();
#endif
}

QString GLDFileSystemModelPrivate::createDateTime(const QModelIndex &index) const
{
#ifndef QT_NO_DATESTRING
    return node(index)->info.created().toString(Qt::LocalDate);
#else
    Q_UNUSED(index);
    return QString();
#endif
}

void GLDFileSystemModelPrivate::appendChild(GLDFileSystemModelPrivate::QDirNode *parent, const QString &path) const
{
    GLDFileSystemModelPrivate::QDirNode node;
    node.populated = false;
    node.stat = shouldStat;
    node.parent = (parent == &root ? 0 : parent);
    node.info = QFileInfo(path);
    node.info.setCaching(true);

    // The following append(node) may reallocate the vector, thus
    // we need to update the pointers to the childnodes parent.
    GLDFileSystemModelPrivate *that = const_cast<GLDFileSystemModelPrivate *>(this);
    that->savePersistentIndexes();
    parent->children.append(node);
    for (int i = 0; i < parent->children.count(); ++i)
    {
        QDirNode *childNode = &parent->children[i];
        for (int j = 0; j < childNode->children.count(); ++j)
        {
            childNode->children[j].parent = childNode;
        }
    }
    that->restorePersistentIndexes();
}

void GLDFileSystemModelPrivate::removeChild(GLDFileSystemModelPrivate::QDirNode *parent,
                                            const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return;
    } 
    if ((index.row()) >= (parent->children.count()))
    {
        return;
    }

    GLDFileSystemModelPrivate *that = const_cast<GLDFileSystemModelPrivate *>(this);
    that->savePersistentIndexes();
    GLDFileSystemModelPrivate::QDirNode *pToRemoveChild = node(index);
    if (pToRemoveChild->info.isDir())
    {
        that->removeAll(pToRemoveChild);
    }
    parent->children.remove(index.row());
    that->restorePersistentIndexes();
}

void GLDFileSystemModelPrivate::removeAll(GLDFileSystemModelPrivate::QDirNode *pNode)
{
    foreach (QDirNode childNode, pNode->children)
    {
        if (childNode.info.isDir())
        {
            removeAll(&childNode);
        }
    }
    pNode->children.clear();
}

QFileInfo GLDFileSystemModelPrivate::resolvedInfo(QFileInfo info)
{
#ifdef Q_OS_WIN
    // On windows, we cannot create a shortcut to a shortcut.
    return QFileInfo(info.symLinkTarget());
#else
    QStringList paths;
    do {
        QFileInfo link(info.symLinkTarget());
        if (link.isRelative())
            info.setFile(info.absolutePath(), link.filePath());
        else
            info = link;
        if (paths.contains(info.absoluteFilePath()))
            return QFileInfo();
        paths.append(info.absoluteFilePath());
    } while (info.isSymLink());
    return info;
#endif
}

GLDFileSystemModelPrivate::QDirNode *GLDFileSystemModelPrivate::node(const QModelIndex &index) const
{
    GLDFileSystemModelPrivate::QDirNode *pNode =
        static_cast<GLDFileSystemModelPrivate::QDirNode*>(index.internalPointer());
    Q_ASSERT(pNode);
    return pNode;
}

void GLDFileSystemModelPrivate::populate(QDirNode *parent) const
{
    Q_ASSERT(parent);
    parent->children = children(parent, parent->stat);
    parent->populated = true;
}

void GLDFileSystemModelPrivate::clear(QDirNode *parent) const
{
     Q_ASSERT(parent);
     parent->children.clear();
     parent->populated = false;
}

void GLDFileSystemModelPrivate::invalidate()
{
    QStack<const QDirNode*> nodes;
    nodes.push(&root);
    while (!nodes.empty())
    {
        const QDirNode *current = nodes.pop();
        current->stat = false;
        const QVector<QDirNode> c_children = current->children;
        for (int i = 0; i < c_children.count(); ++i)
        {
            nodes.push(&c_children.at(i));
        }
    }
}

#endif // QT_NO_DIRMODEL

/**
 * @class GLDCustomFileSystemModel
 * @brief 文件系统模型基类
 */
GLDCustomFileSystemModel::GLDCustomFileSystemModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

GLDCustomFileSystemModel::GLDCustomFileSystemModel(QAbstractItemModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{

}

GLDFileSystemModel *GLDCustomFileSystemModel::fileSystemModel() const
{
    return NULL;
}

bool GLDCustomFileSystemModel::rmdir(const QModelIndex &index)
{
    return false;
    G_UNUSED(index)
}

bool GLDCustomFileSystemModel::remove(const QModelIndex &index)
{
    return false;
    G_UNUSED(index)
}

void GLDCustomFileSystemModel::setViewMode(GLDListView::GLDViewMode viewMode)
{
    m_enViewMode = viewMode;
}

GLDListView::GLDViewMode GLDCustomFileSystemModel::viewMode()
{
    return m_enViewMode;
}
