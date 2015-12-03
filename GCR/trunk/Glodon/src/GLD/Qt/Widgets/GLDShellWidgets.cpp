#include <QDir>
#include <QTimer>
#include <QVariant>
#include <QDirModel>
#include <QMouseEvent>
#include <QHeaderView>
#include <QApplication>
#include "GLDStrUtils.h"
#include <QStylePainter>
#include <QFileSystemModel>
#include <QDesktopServices>
#include "GLDShellWidgets.h"
#include "GLDFileSystemModel.h"
#include "GLDShellComboBoxEx.h"

const int c_NormalSmallIconSize = 16;
GLDShellTreeView::GLDShellTreeView(QWidget *parent) :
    QTreeView(parent), m_getPathType(AbsoluteName), m_comboBox(NULL), m_iFirstShow(0), m_bTreeRootWithCombo(true),
    m_bSelfClicked(false), m_bComboClickeSignal(false), m_pDirModel(NULL), m_pHiddenBuddies(NULL),
    m_pHiddenBuddys(NULL), m_preRoot(QModelIndex()), m_bSingleRoot(true), m_bUsingVariant(true), m_neverChangeRoot(false)
{
    GLDShellHeaderView *pHeader = new GLDShellHeaderView(this);
    this->setHeader(pHeader);

    setMouseTracking(true);
    setUniformRowHeights(true);
    resizeColumnToContents(0);

    QObject::connect(this, SIGNAL(expanded(const QModelIndex &)),
                     this,SLOT(adjustColumnWidth(const QModelIndex &)));
    QObject::connect(this, SIGNAL(pressed(QModelIndex)), this, SIGNAL(itemJustBeSelected(QModelIndex)));
    QObject::connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(itemJustBeDoubleClicked(QModelIndex)));
}

GLDShellTreeView::~GLDShellTreeView()
{
    freeAndNil(m_pHiddenBuddies)
    freeAndNil(m_pHiddenBuddys)
}

void GLDShellTreeView::setModel(QAbstractItemModel *model)
{
    if (GLDCustomFileSystemModel *pModel = dynamic_cast<GLDCustomFileSystemModel *>(model))
    {
        m_pDirModel = pModel;
    }
    inherit::setModel(model);
    GLDShellHeaderView *pHeader = dynamic_cast<GLDShellHeaderView *>(this->header());
    if (pHeader)
    {
        pHeader->setSection0AutoResize();
        pHeader->setSectionsHiddenExcept0();
    }
    else
    {
        const int c_HeaderSectionCount = header()->count();
        for (int i = 1; i < c_HeaderSectionCount; ++i)
        {
//            hideColumn(i);
//            setColumnWidth(i, 0);
//            header()->resizeSection(i, 0);
            header()->setSectionHidden(i, false);
        }
    }
    setHeaderHidden(true);
}

void GLDShellTreeView::setNameFilters(const QStringList &filters)
{
    return doSetNameFilters(model(), filters);
}

QStringList GLDShellTreeView::nameFilters() const
{
    return doNameFilters(model());
}

QString GLDShellTreeView::currentPath(DirName dirType, SLASHTYPE xieGang) const
{
    QString tmp("");
    QModelIndex currentIndex = this->currentIndex();
    if (!currentIndex.isValid())
    {
        currentIndex = this->rootIndex();
    }
    if (currentIndex.isValid())
    {
        if (this->selectionModel())
        {
            tmp = filePathByType(currentIndex, dirType);
        }
    }
    if (xieGang == BACKSLASH && tmp.length() > 0)
    {
        tmp = stringReplace(tmp,"/","\\");
    }
    return tmp;
}

GLDDirFilter::ISDIR GLDShellTreeView::isCurrentPathDir() const
{
    GLDFileSystemModel *pModel = dynamic_cast<GLDFileSystemModel *>(model());
    if (pModel)
    {
        QModelIndex current = currentIndex();
        if (current.isValid())
        {
            if (pModel->isDir(current))
                return GLDShellListView::ISFOLDER;
            else
                return GLDShellListView::ISFILE;
        }
        else
            return GLDShellListView::NOITEMSELECTED;
    }
    return GLDShellListView::NOITEMSELECTED;
}

void GLDShellTreeView::setAssociatedItemView(QAbstractItemView *view)
{
    QListView *list = dynamic_cast<QListView *>(view);
    if (list)
    {
        const QModelIndex c_tmp = currentIndex();
        if (c_tmp.isValid())
            list->setRootIndex(c_tmp);
        QObject::connect(this, SIGNAL(clicked(QModelIndex)), list,
                         SLOT(setRootIndex(QModelIndex)));
    }
    else
    {
        GLDShellTreeView *tree = dynamic_cast<GLDShellTreeView *>(view);
        if (tree)
        {
            const QModelIndex c_tmp = rootIndex();
            if (tree->singleRoot())
            {
                if (c_tmp.isValid())
                {
                    QString rootPath = dynamic_cast<GLDFileSystemModel *>(tree->model())->filePath(c_tmp);
                    tree->setRootPath(rootPath);
                }
            }
            else
            {
                if (c_tmp.isValid())
                    tree->setRootIndex(c_tmp);
            }
            connect(this, SIGNAL(expanded(QModelIndex)), tree, SLOT(expand(QModelIndex)));
            connect(this, SIGNAL(collapsed(QModelIndex)), tree, SLOT(collapse(QModelIndex)));
            connect(tree, SIGNAL(expanded(QModelIndex)), this, SLOT(slotExpand(QModelIndex)));
            connect(tree, SIGNAL(collapsed(QModelIndex)), this, SLOT(slotCollapse(QModelIndex)));

            connect(this, SIGNAL(clicked(QModelIndex)), tree, SLOT(setRootIndex(QModelIndex)));
            connect(this, SIGNAL(expanded(QModelIndex)), tree, SLOT(setCurrentIndex(QModelIndex)));
            connect(this, SIGNAL(collapsed(QModelIndex)), tree, SLOT(setCurrentIndex(QModelIndex)));
        }
    }
}

void GLDShellTreeView::adjustColumnWidth(const QModelIndex &)
{
    resizeColumnToContents(0);
}

void GLDShellTreeView::upToParentDir(const QModelIndex &parentDirIndex)
{
    if (m_bTreeRootWithCombo)
    {
        if (!isAssociatedAComboBox())
            this->setRootIndex(parentDirIndex);
    }
    if (isAssociatedAComboBox())
    {
        inherit::setCurrentIndex(parentDirIndex);
        m_comboBox->setEditText(m_pDirModel->filePath(parentDirIndex));
        m_comboBox->getLineEdit()->moveCursor(QTextCursor::Start);
    }
}

void GLDShellTreeView::slotExpand(QModelIndex index)
{
    if (!isExpanded(index))
        expand(index);
}

void GLDShellTreeView::slotCollapse(QModelIndex index)
{
    if (isExpanded(index))
        collapse(index);
}

void GLDShellTreeView::makePreBuddiesVisible()
{
    if (!m_bUsingVariant)
    {
        if (m_pHiddenBuddies && m_pHiddenBuddies->count() > 0)
        {
            QModelIndex par = m_pHiddenBuddies->front();
            m_pHiddenBuddies->pop_front();
            Q_ASSERT(m_pDirModel == model());
            while (m_pHiddenBuddies->count() > 0)
            {
                if (m_pHiddenBuddies->front().isValid())
                {
                    setRowHidden(m_pHiddenBuddies->front().row(), par, false);
                }
                m_pHiddenBuddies->pop_front();
            }
        }
    }
    else
    {
        if (m_pHiddenBuddys && m_pHiddenBuddys->count() > 0)
        {
            QVariant var = m_pHiddenBuddys->front();
            QModelIndex par = m_pDirModel->index(var.toString());
            m_pHiddenBuddys->pop_front();
            Q_ASSERT(m_pDirModel == model());
            while (m_pHiddenBuddys->count() > 0)
            {
                QVariant buddyRow = m_pHiddenBuddys->front();
                setRowHidden(buddyRow.toInt(), par, false);
                m_pHiddenBuddys->pop_front();
            }
        }
    }
}

void GLDShellTreeView::makesureBuddyContainerExist()
{
    if (!m_bUsingVariant)
    {
        if (NULL == m_pHiddenBuddies)
        {
            m_pHiddenBuddies = new QList<QModelIndex>;
            m_pHiddenBuddies->reserve(200);
        }
    }
    else
    {
        if (NULL == m_pHiddenBuddys)
        {
            m_pHiddenBuddys = new QList<QVariant>;
        }
    }
}

void GLDShellTreeView::makeRootParentAsRootAndHideBuddies()
{
    QModelIndex root = this->rootIndex();
    if (root.isValid())
    {
        QDirModel *pModel = dynamic_cast<QDirModel *>(model());
        if (!pModel)
            return;
        m_preRoot = root;
        root = root.parent();
//        if (root.isValid())   //磁盘根目录时root就是!isValid的
        {
            inherit::setRootIndex(root);
            inherit::setCurrentIndex(m_preRoot);
            makesureBuddyContainerExist();
            hideBuddiesAndInsertList(m_preRoot, root, pModel->rowCount(root));
        }
    }
}

void GLDShellTreeView::hideBuddiesAndInsertList(const QModelIndex &visibleChild, const QModelIndex &par, int buddyCount)
{
    if (!m_bUsingVariant)
    {
        m_pHiddenBuddies->append(par);
        while (buddyCount-- > 0)
        {
            if (buddyCount == visibleChild.row())
                continue;
            QModelIndex buddyIndex = par.child(buddyCount, visibleChild.column());
            if (!par.isValid())
            {
                buddyIndex = visibleChild.sibling(buddyCount, visibleChild.column());
            }
            /*if (!isRowHidden(buddyCount, par))*/  //当有过滤时，被隐藏的不必增加，否则回设状态时将与过滤隐藏的行为矛盾
            {
                m_pHiddenBuddies->append(buddyIndex);
                setRowHidden(buddyCount, par, true);
            }
        }
    }
    else
    {
        m_pHiddenBuddys->append(m_pDirModel->filePath(par));
        while (buddyCount-- > 0)
        {
            if (buddyCount == visibleChild.row())
                continue;
            QModelIndex buddyIndex = par.child(buddyCount, visibleChild.column());
            if (!par.isValid())
            {
                buddyIndex = visibleChild.sibling(buddyCount, visibleChild.column());
            }
            /*if (!isRowHidden(buddyCount, par))*/  //当有过滤时，被隐藏的不必增加，否则回设状态时将与过滤隐藏的行为矛盾
            {
                m_pHiddenBuddys->append(buddyIndex.row());
                Q_ASSERT(buddyIndex.row() == buddyCount);
                setRowHidden(buddyCount, par, true);
            }
        }
    }
}

void GLDShellTreeView::setCurrentIndex(const QModelIndex &index)
{
    if (!m_bSingleRoot)
    {
        if (index.isValid() && !isAssociatedAComboBox() && m_bTreeRootWithCombo)
        {
            bool bPrevState = this->blockSignals(true);
            if (index.parent().isValid())
            {
                inherit::setRootIndex(index.parent());
            }
            else
            {
                inherit::setRootIndex(index);
            }
            this->blockSignals(bPrevState);
        }
        else
        {
            inherit::setCurrentIndex(index);
        }
    }
    else
    {
        if (!m_bSelfClicked)
        {
            if (m_bTreeRootWithCombo && m_bComboClickeSignal)
            {
                setRootIndex(index);
                m_bComboClickeSignal = false;
            }
            inherit::setCurrentIndex(index);
        }
        else
        {
            inherit::setCurrentIndex(index);
        }
    }
}

void GLDShellTreeView::shellTreeOwnScrollTo()
{
    scrollTo(currentIndex());
}

QString GLDShellTreeView::getIndexFileName(QModelIndex pressedIndex)
{
    if (pressedIndex.column() > 0)
    {
        QModelIndex sibling = pressedIndex.sibling(pressedIndex.row(), 0);
        return filePathByType(sibling, m_getPathType);
    }
    return filePathByType(pressedIndex, m_getPathType);
}

void GLDShellTreeView::setSingleRoot(bool singleRoot)
{
    m_bSingleRoot = singleRoot;
}

bool GLDShellTreeView::singleRoot() const
{
    return m_bSingleRoot;
}

void GLDShellTreeView::setRootIndex(const QModelIndex &index)
{
    if (m_neverChangeRoot)
    {
        return;
    }
    if (m_bSingleRoot)
    {
        makePreBuddiesVisible();
        inherit::setRootIndex(index);
        makeRootParentAsRootAndHideBuddies();
    }
    else
    {
        inherit::setRootIndex(index);
    }
}

void GLDShellTreeView::setRootPath(const QString &path)
{
    QModelIndex root = m_pDirModel->index(path);
    this->setRootIndex(root);
}

void GLDShellTreeView::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);
    QModelIndex pressedIndex = indexAt(event->pos());
    if (!visualRect(pressedIndex).contains(event->pos()))
        return;
    if (m_comboBox && pressedIndex.isValid())
    {
        if (isAssociatedAComboBox())
        {
            if (visualRect(pressedIndex).contains(event->pos()))
            {
                m_comboBox->closePopup();
                //                    m_comboBox->hidePopup();
                m_comboBox->setEditTextByIndex(pressedIndex);
            }
            if (isAssociatedAComboBox() && m_comboBox->m_shellTreeView)
            {
                bool bPreState = m_comboBox->m_shellTreeView->m_bComboClickeSignal;
                m_comboBox->m_shellTreeView->m_bComboClickeSignal = true;
                emit clicked(pressedIndex);
                m_comboBox->m_shellTreeView->expand(pressedIndex);
                m_comboBox->m_shellTreeView->m_bComboClickeSignal = bPreState;
            }
            else
                emit clicked(pressedIndex);
        }
    }
    bool bPrev = m_bTreeRootWithCombo;
    m_bTreeRootWithCombo = false;

    m_bSelfClicked = true;
    setCurrentIndex(pressedIndex);
    m_bSelfClicked = false;

    m_bTreeRootWithCombo = bPrev;
}

void GLDShellTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QTreeView::currentChanged(current, previous);
    emit currentItemJustChanged(current, previous);
}

void GLDShellTreeView::showEvent(QShowEvent *e)
{
    ++m_iFirstShow;
    if (m_iFirstShow < 4)
    {
        QModelIndex index = currentIndex();
        if (index.isValid())
            scrollTo(index);
    }
    inherit::showEvent(e);
}

void GLDShellTreeView::doItemsLayout()
{
    QTreeView::doItemsLayout();
    if (!m_bSingleRoot)
    {
        if (currentIndex().isValid())
        {
            QTimer::singleShot(1, this, SLOT(shellTreeOwnScrollTo()));
        }
    }
}

void GLDShellTreeView::setCurrentIndexPro(const QModelIndex &index)
{
    emit itemJustBeSelected(index);
    setCurrentIndex(index);
}

GLDShellTreeView::GLDShellTreeView(bool popup, GLDShellComboBox *combo) :
  QTreeView(combo), m_getPathType(AbsoluteName), m_comboBox(combo), m_iFirstShow(0), m_bTreeRootWithCombo(false),
  m_pDirModel(NULL), m_pHiddenBuddies(NULL), m_pHiddenBuddys(NULL), m_preRoot(QModelIndex())
{
    Q_UNUSED(popup);
    GLDShellHeaderView *pHeader = new GLDShellHeaderView(this);
    this->setHeader(pHeader);
    setMouseTracking(true);
    setUniformRowHeights(true);
    resizeColumnToContents(0);
    QObject::connect(this, SIGNAL(expanded(const QModelIndex &)),
                     this,SLOT(adjustColumnWidth(const QModelIndex &)));
}

GLDShellComboBox::GLDShellComboBox(const QString &rootPath, QWidget *parent) :
    GLDWindowComboBox(parent), m_popupShellTree(NULL), m_shellListView(NULL),
    m_shellTreeView(NULL), m_pFileModel(NULL), m_currentPathSetted(false)
{
    getLineEdit()->setLineWrapMode(QPlainTextEdit::NoWrap);
    setFrame(true);
    setAnimationPopup(false);
    init(rootPath);
}

GLDShellComboBox::~GLDShellComboBox()
{

}

void GLDShellComboBox::setFileModel(GLDCustomFileSystemModel *model)
{
    freeAndNil(m_pFileModel);
    m_pFileModel = model;
    m_popupShellTree->setModel(model);
    m_shellListView->setModel(model);
}

void GLDShellComboBox::disconnectClick(QAbstractItemView *view)
{
    QObject::disconnect(m_popupShellTree, SIGNAL(clicked(QModelIndex)), view,
                     SLOT(setRootIndex(QModelIndex)));
}

void GLDShellComboBox::setShellListView(GLDShellListView *list)
{
    if (m_shellListView)
    {
        disconnect(m_shellListView, SIGNAL(upToDir(QModelIndex)), m_popupShellTree, SLOT(upToParentDir(QModelIndex)));
        if (m_shellTreeView)
        {
            disconnect(m_shellListView, SIGNAL(upToDir(QModelIndex)),
                       m_shellTreeView, SLOT(upToParentDir(QModelIndex)));
        }
        //防止内存泄漏
        if (!m_shellListView->parentWidget())
            m_shellListView->setParent(this);
    }
    m_shellListView = list;
    if (m_popupShellTree && m_pFileModel && m_shellListView)
    {
        m_shellListView->setModel(m_pFileModel);
        Q_ASSERT(m_popupShellTree->model() == m_pFileModel);
        m_shellListView->setRootIndex(this->m_popupShellTree->currentIndex());
        m_popupShellTree->setAssociatedItemView(m_shellListView);

        connect(m_shellListView, SIGNAL(upToDir(QModelIndex)), m_popupShellTree, SLOT(upToParentDir(QModelIndex)));
        connect(m_shellListView, SIGNAL(upToDir(QModelIndex)), m_shellTreeView, SLOT(upToParentDir(QModelIndex)));
        connect(m_shellListView, SIGNAL(itemJustBeSelected(QModelIndex)), this, SLOT(synToComboAndTree(QModelIndex)));
    }
    if (m_currentPathSetted && m_shellListView)
    {
        QModelIndex path = m_popupShellTree->currentIndex();
        if (path.isValid())
        {
            locateListTo(path);
        }
    }
}

void GLDShellComboBox::locateListTo(QModelIndex path)
{
    m_shellListView->setRootIndex(path);
    m_shellListView->setCurrentIndex(path);
    m_shellListView->scrollTo(path);
}

void GLDShellComboBox::expandTreeTo(QModelIndex path)
{
    m_shellTreeView->setCurrentIndex(path);
    m_shellTreeView->expand(path);
    m_shellTreeView->scrollTo(path);
}

void GLDShellComboBox::setShellTreeView(GLDShellTreeView *tree)
{
    if (m_shellTreeView)
    {
        if (m_shellListView)
        {
            disconnect(m_shellListView, SIGNAL(upToDir(QModelIndex)),
                       m_shellTreeView, SLOT(upToParentDir(QModelIndex)));
            disconnect(m_shellListView, SIGNAL(upToDir(QModelIndex)),
                       m_popupShellTree, SLOT(upToParentDir(QModelIndex)));
        }
        //防止内存泄漏
        if (!m_shellTreeView->parentWidget())
            m_shellTreeView->setParent(this);
    }
    m_shellTreeView = tree;
    if (m_popupShellTree && m_pFileModel && m_shellTreeView)
    {
        m_shellTreeView->setModel(m_pFileModel);
        Q_ASSERT(m_pFileModel == m_popupShellTree->model());
        m_shellTreeView->setRootIndex(m_popupShellTree->rootIndex());
        m_popupShellTree->setAssociatedItemView(m_shellTreeView);
        if (m_shellListView)
            connect(m_shellListView, SIGNAL(upToDir(QModelIndex)), m_shellTreeView, SLOT(upToParentDir(QModelIndex)));

        connect(m_shellTreeView, SIGNAL(itemJustBeSelected(QModelIndex)), this, SLOT(synToComboAndList(QModelIndex)));
    }
    if (m_currentPathSetted && m_shellTreeView)
    {
        QModelIndex path = m_popupShellTree->currentIndex();
        if (path.isValid())
        {
            ISDIR isDir = getTheIndexDirType(path, m_pFileModel);
            QModelIndex dirIndex = path.parent();
            QString sCurrentPath = m_pFileModel->filePath(path);
            if (isDir == ISFOLDER)
            {
                m_shellTreeView->setRootPath(sCurrentPath);
                m_shellTreeView->expand(m_popupShellTree->m_pDirModel->index(sCurrentPath));
            }
            else if (isDir == ISFILE)
            {
                if (!m_shellTreeView->singleRoot())
                {
                    expandTreeTo(dirIndex);
                    m_shellTreeView->setCurrentIndex(path);
                }
                else
                {
                    m_shellTreeView->setRootIndex(dirIndex);
                    m_shellTreeView->setCurrentIndex(path);
                }
            }
        }
    }
}

void GLDShellComboBox::setChildrenRootIndex(QModelIndex path)
{
    m_popupShellTree->setRootIndex(path);
    if (m_shellListView)
        m_shellListView->setRootIndex(path);
    if (m_shellTreeView)
        m_shellTreeView->setRootIndex(path);
}

void GLDShellComboBox::setCustomPath(const QString &rootPath)
{
    return setCurrentPath(rootPath);
//    if (m_pFileModel && m_popupShellTree)
//    {
//        QModelIndex path = m_pFileModel->index(rootPath);
//        if (path.isValid()&& m_pFileModel->isDir(path) )
//        {
//            setChildrenRootIndex(path);
//        }
//    }
}

void GLDShellComboBox::setEditTextByIndex(QModelIndex curIndex)
{
    QString strFilePathName = m_popupShellTree->getIndexFileName(curIndex);
    if (curIndex.column() > 0 && curIndex.isValid())
    {
        curIndex = curIndex.sibling(curIndex.row(), 0);
        if (curIndex.isValid())
        {
            strFilePathName = m_popupShellTree->getIndexFileName(curIndex);
        }
    }
    setEditText(strFilePathName);
    lineEdit()->moveCursor(QTextCursor::Start);
}

void GLDShellComboBox::setCurrentPath(const QString &currentPath)
{
    if (m_pFileModel && m_popupShellTree)
    {
        QString sCurrentPath = m_pFileModel->makeCaseCorrespondingToSystem(currentPath);
        QModelIndex curIndex = m_pFileModel->index(sCurrentPath);
        if (curIndex.isValid() && m_pFileModel->isDir(curIndex))
        {
            m_popupShellTree->setCurrentIndex(curIndex);
            m_popupShellTree->scrollTo(curIndex);
            setEditTextByIndex(curIndex);
            m_currentPathSetted = true;

            if (m_shellListView)
            {
                locateListTo(curIndex);
            }
            if (m_shellTreeView)
            {
                if (!m_shellTreeView->singleRoot())
                    expandTreeTo(curIndex);
                else
                    m_shellTreeView->setRootPath(sCurrentPath);
            }
        }
    }
}

void GLDShellComboBox::setCurrentFile(const QString &currentFilePath)
{
    if (m_pFileModel && m_popupShellTree)
    {
        QString sCurrentPath = m_pFileModel->makeCaseCorrespondingToSystem(currentFilePath);
        QModelIndex curIndex = m_pFileModel->index(sCurrentPath);
        ISDIR isDir = getTheIndexDirType(curIndex, m_pFileModel);
        if (curIndex.isValid())
        {
            if (isDir == ISFOLDER)
            {
                m_popupShellTree->setCurrentIndex(curIndex);
                m_popupShellTree->scrollTo(curIndex);
                setEditTextByIndex(curIndex);
                m_currentPathSetted = true;

                if (m_shellListView)
                {
                    locateListTo(curIndex);
                }
                if (m_shellTreeView)
                {
                    if (!m_shellTreeView->singleRoot())
                        expandTreeTo(curIndex);
                    else
                        m_shellTreeView->setRootPath(sCurrentPath);
                }
            }
            else if (isDir == ISFILE)
            {
                QModelIndex dirIndex = curIndex.parent();
                m_popupShellTree->setCurrentIndex(curIndex);
                m_popupShellTree->expand(dirIndex);
                m_popupShellTree->scrollTo(dirIndex);
                setEditTextByIndex(curIndex);
                m_currentPathSetted = true;

                if (m_shellListView)
                {
                    locateListTo(dirIndex);
                }
                if (m_shellTreeView)
                {
                    if (!m_shellTreeView->singleRoot())
                    {
                        expandTreeTo(dirIndex);
                        m_shellTreeView->setCurrentIndex(curIndex);
                    }
                    else
                    {
                        m_shellTreeView->setRootIndex(dirIndex);
                        m_shellTreeView->setCurrentIndex(curIndex);
                    }
                }
            }
        }
    }
}

void GLDShellComboBox::setNameFilters(const QStringList &filters)
{
    if (m_popupShellTree)
        doSetNameFilters(m_popupShellTree->model(), filters);
}

QStringList GLDShellComboBox::nameFilters() const
{
    if (m_popupShellTree)
        return doNameFilters(m_popupShellTree->model());
    return QStringList();
}

void GLDShellComboBox::setDirNameType(GLDDirFilter::DirName type)
{
    m_popupShellTree->m_getPathType = type;
}

GLDDirFilter::DirName GLDShellComboBox::dirNameType() const
{
    return m_popupShellTree->m_getPathType;
}

QString GLDShellComboBox::getCurrentPath(DirName type, SLASHTYPE xieGang) const
{
    return m_popupShellTree->currentPath(type, xieGang);
}

QString GLDShellComboBox::getIndexFilePath(const QModelIndex &index, GLDDirFilter::SLASHTYPE xieGang) const
{
    QString tmp(m_popupShellTree->getIndexFileName(index));
    if (xieGang == BACKSLASH && tmp.length() > 0)
    {
        tmp = stringReplace(tmp,"/","\\");
    }
    return tmp;
}

void GLDShellComboBox::synToComboAndTree(QModelIndex clickedIndex)
{
    bool bs = m_popupShellTree->blockSignals(true);
    m_popupShellTree->setCurrentIndex(clickedIndex);
    setEditTextByIndex(clickedIndex);
    if (!bs)
        m_popupShellTree->blockSignals(false);
    if (m_shellTreeView && !m_shellTreeView->neverChangeRoot())
    {
        bool bs = m_shellTreeView->blockSignals(true);
        m_shellTreeView->setCurrentIndex(clickedIndex);
        if (!bs)
            m_shellTreeView->blockSignals(false);
    }
}

void GLDShellComboBox::synToComboAndList(QModelIndex clickedIndex)
{
    bool bs = m_popupShellTree->blockSignals(true);
    m_popupShellTree->setCurrentIndex(clickedIndex);
    setEditTextByIndex(clickedIndex);
    if (!bs)
        m_popupShellTree->blockSignals(false);
    if (m_shellListView)
    {
        bool bs = m_shellListView->blockSignals(true);
        m_shellListView->setRootIndex(clickedIndex);
        m_shellListView->setCurrentIndex(clickedIndex);
        if (!bs)
            m_shellListView->blockSignals(false);
    }
}

void GLDShellComboBox::init(const QString &rootPath)
{
    m_pFileModel = new GLDFileSystemModel(true, this);
//    m_pFileModel->setRootPath(rootPath);

    QModelIndex pathIndex = m_pFileModel->index("");
    if (rootPath.length() > 0)
    {
        QModelIndex pathIndexTmp = m_pFileModel->index(rootPath);
        if (pathIndexTmp.isValid() && m_pFileModel->isDir(pathIndexTmp))
            pathIndex = pathIndexTmp;
    }
    //set tree as combobox`s popup
    m_popupShellTree = new GLDShellTreeView(true, this);
    m_popupShellTree->setSingleRoot(false);
    m_popupShellTree->setModel(m_pFileModel);
    Q_ASSERT(m_pFileModel == m_popupShellTree->model());
    m_popupShellTree->setRootIndex(pathIndex);

    //inherited init action
    m_popupShellTree->setAnimated(false);
    m_popupShellTree->setIndentation(10);
    m_popupShellTree->setSortingEnabled(true);

    setPopupWidget(m_popupShellTree);
}

void GLDShellComboBox::doAfterResize()
{
    if (m_popupShellTree)
    {
        double dHeight = width() / 0.618;
        QPoint pos(this->width(), this->height());
        QRect screen = QApplication::desktop()->availableGeometry(pos);
        pos = mapToGlobal(pos);
        double distance = screen.bottom() - pos.y();
        if (distance > 0)
        {
            while (dHeight > 15)
            {
                if (dHeight > distance)
                    dHeight = dHeight / 1.6666667;
                else
                    break;
            }
        }
        else
        {
            dHeight = qMax(10, (int)(-distance));
        }
        QSize geo(width(), dHeight);
        setPopupSize(geo);
        setPopupFixedWidth(geo.width());
        getLineEdit()->setFixedWidth(width() - c_NormalSmallIconSize - 18);
    }
}

QIcon GLDShellComboBox::itemIcon(const QModelIndex &index, const QAbstractItemModel *model)
{
    QVariant decoration = model->data(index, Qt::DecorationRole);
    if (decoration.type() == QVariant::Pixmap)
        return QIcon(qvariant_cast<QPixmap>(decoration));
    else
        return qvariant_cast<QIcon>(decoration);

}

void GLDShellComboBox::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    GLDWindowComboBox::resizeEvent(event);
    doAfterResize();
}

void GLDShellComboBox::paintEvent(QPaintEvent *event)
{
    QRect rect = getLineEdit()->geometry();
    if (rect.left() < 10)
    {
        getLineEdit()->setGeometry(rect.adjusted(c_NormalSmallIconSize, 0, 0, 0));
    }
    if (!framePopup())
    {
        GLDWindowComboBox::paintEvent(event);
        return;
    }

    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    QStyleOptionComboBox optCombo;

    int niconWidth = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
    QSize tmpSize(niconWidth, niconWidth);

    optCombo.init(this);
    optCombo.editable = true;
    optCombo.frame = opt.frame;
    optCombo.subControls = opt.subControls;
    optCombo.activeSubControls = opt.activeSubControls;
    optCombo.state = opt.state;
    if (lineEdit()->isReadOnly())
    {
        optCombo.state &= ~QStyle::State_Enabled;
    }
    optCombo.iconSize = tmpSize;

    int nitemRole = (isEditable() ? Qt::EditRole : Qt::DisplayRole);
    if (m_popupShellTree && m_popupShellTree->currentIndex().isValid())
    {
        optCombo.currentText = m_popupShellTree->currentIndex().isValid() ?
                    m_pFileModel->data(m_popupShellTree->currentIndex(), nitemRole).toString() : QString();
        optCombo.currentIcon = itemIcon(m_popupShellTree->currentIndex(), m_popupShellTree->model());
    }

    //鼠标位于箭头所在区域高亮，其他地方不高亮
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QRect subRect = style()->proxy()->subControlRect(QStyle::CC_ComboBox, &optCombo, QStyle::SC_ComboBoxArrow, this);
    if (subRect.contains(curPos))
        optCombo.activeSubControls |= newHoverControl(curPos);
    else
        optCombo.activeSubControls &= newHoverControl(curPos);

    QStylePainter painter(this);

    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    painter.drawComplexControl(QStyle::CC_ComboBox, optCombo);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, optCombo);
}

GLDShellHeaderView::GLDShellHeaderView(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
    setDefaultSectionSize(0);
    setSectionsMovable(true);
    setStretchLastSection(true);
    setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setMouseTracking(true);
}

GLDShellHeaderView::~GLDShellHeaderView()
{

}

void GLDShellHeaderView::setSection0AutoResize()
{
//    setSectionsMovable(true);
//    setSectionResizeMode(0, QHeaderView::ResizeToContents);
    setSectionResizeMode(0, QHeaderView::Stretch);
//    setSectionResizeMode(1, QHeaderView::Custom);
//    setSectionResizeMode(2, QHeaderView::Custom);
//    setSectionResizeMode(3, QHeaderView::Custom);
}

void GLDShellHeaderView::setSectionsHiddenExcept0()
{
    const int c_SectionCount = count();
    if (c_SectionCount > 0)
    {
//        for (int i = 1; i < iSectionCount; ++i)
//        {
////            setSectionHidden(i, true);
//            resizeSection(i, 0);
//        }
        setStretchLastSection(false);
    }
}

GLDShellListView::GLDShellListView(QWidget *parent) : GLDListView(parent),
    commentFrame(0),m_hintExistSeconds(2), m_bShowHint(false), m_bFirstShow(false),
    m_bOpenFileWhileDblClk(false)
{
    connect(this, SIGNAL(itemJustBeSelected(const QModelIndex&)), this, SLOT(showHint(const QModelIndex&)));
    connect(this, SIGNAL(itemJustBeDoubleClicked(const QModelIndex&)),
            this, SLOT(mouseDoubleClickSlot(const QModelIndex&)));
    init();
}

void GLDShellListView::init()
{
    GLDFileSystemModelEX *pModel = new GLDFileSystemModelEX(true, this);
//    pModel->setRootPath("");
    this->setModel(pModel);
}

void GLDShellListView::setCustomPath(QString rootPath)
{
    GLDFileSystemModel *pModel = dynamic_cast<GLDFileSystemModel *>(this->model());
    if (pModel)
    {
        QModelIndex path = pModel->index(rootPath);
        if (path.isValid() && pModel->isDir(path))
        {
            setRootIndex(path);
        }
    }
}

QString GLDShellListView::currentPath(DirName dirType, SLASHTYPE defType) const
{
    QString tmp = "";
    if (!currentReportMode())
    {
        QModelIndex currentIndex = this->currentIndex();
        if (!currentIndex.isValid())
        {
            currentIndex = this->rootIndex();
        }
        if (currentIndex.isValid())
        {
            if (this->selectionModel() && this->selectionModel()->selectedIndexes().size() > 0)
            {
                tmp = filePathByType(currentIndex, dirType);
            }
        }
    }
    else
    {
        QModelIndex currentIndex = this->m_shellTree->currentIndex();
        if (!currentIndex.isValid())
        {
            currentIndex = m_shellTree->rootIndex();
        }
        if (currentIndex.isValid())
        {
            if (m_shellTree->selectionModel() && m_shellTree->selectionModel()->selectedIndexes().size() > 0)
            {
                tmp = filePathByType(currentIndex, dirType);
            }
        }
    }
    if (defType == BACKSLASH && tmp.length() > 0)
    {
        tmp = stringReplace(tmp,"/","\\");
    }
    return tmp;
}

GLDDirFilter::ISDIR GLDShellListView::isCurrentPathDir() const
{
    const GLDCustomFileSystemModel * pModel = dynamic_cast<GLDCustomFileSystemModel *>(model());
    ISDIR dir = GLDShellListView::NOITEMSELECTED;
    if (pModel)
    {
        QModelIndex current = GLDListView::currentIndex();
        if (currentReportMode())
        {
            pModel = dynamic_cast<GLDCustomFileSystemModel *>(m_shellTree->model());
            current = m_shellTree->dataIndex(m_shellTree->currentIndex());
        }
        if (pModel)
        {
            dir = getTheIndexDirType(current, pModel);
        }
    }
    return dir;
}

bool GLDShellListView::upToParentDir()
{
    if (!currentReportMode())
    {
        QModelIndex currentRoot = currentIndex();
        if (!currentRoot.isValid())
            currentRoot = rootIndex();
        if (currentRoot.isValid())
        {
            QModelIndex parentIndex = currentRoot.parent();
            if (parentIndex.isValid())
            {
                setRootIndex(parentIndex);
                setCurrentIndex(parentIndex);
                emit rootIndexChanged(parentIndex);
                emit upToDir(parentIndex);
                return true;
            }
            else
            {
                if (dynamic_cast<GLDFileSystemModel *>(model()))
                {
                    this->GLDListView::setRootIndex(parentIndex);
                    this->GLDListView::setCurrentIndex(parentIndex);
                    emit rootIndexChanged(parentIndex);
                    emit upToDir(parentIndex);
                    return true;
                }
            }
        }
    }
    else
    {
        QModelIndex currentRoot = m_shellTree->currentIndex();
        if (!currentRoot.isValid())
            currentRoot = m_shellTree->rootIndex();
        if (currentRoot.isValid())
        {
            QModelIndex parentIndex = currentRoot.parent();
            if (parentIndex.isValid())
            {
                setRootIndex(m_shellTree->dataIndex(parentIndex));
                m_shellTree->setCurrentIndex(parentIndex);
                emit rootIndexChanged(parentIndex);
                emit upToDir(parentIndex);
                return true;
            }
            else
            {
                if (dynamic_cast<GLDFileSystemModel *>(model()))
                {
                    this->GLDListView::setRootIndex(parentIndex);
                    this->GLDListView::setCurrentIndex(parentIndex);
                    emit rootIndexChanged(parentIndex);
                    emit upToDir(parentIndex);
                    return true;
                }
            }
        }
    }
    return false;
}

void GLDShellListView::setNameFilters(const QStringList &filters)
{
    doSetNameFilters(model(), filters);
}

QStringList GLDShellListView::nameFilters() const
{
    return doNameFilters(model());
}

void GLDShellListView::setCurrentPath(const QString &path)
{
    if (path.isEmpty())
    {
        if (qobject_cast<GLDFileSystemModel *>(model()))
        {
            this->GLDListView::setRootIndex(QModelIndex());
            this->GLDListView::setCurrentIndex(QModelIndex());

            emit rootIndexChanged(QModelIndex());
            emit upToDir(QModelIndex());
        }
    }
}

void GLDShellListView::setRootIndex(const QModelIndex &index)
{
    GLDCustomFileSystemModel *pModel = dynamic_cast<GLDCustomFileSystemModel *>(this->model());

    if (pModel && index.isValid())
    {
        if (!pModel->isDir(index))
        {
            if (m_bOpenFileWhileDblClk)
            {
                QVariant varPath = model()->data(index, GLDCustomFileSystemModel::FilePathRole);
                QDesktopServices::openUrl(QUrl::fromLocalFile(varPath.toString()));
            }
            else
            {
                GLDListView::setRootIndex(index);
                GLDListView::setCurrentIndex(index);
                emit rootIndexChanged(index);
            }
        }
        else
        {
            GLDListView::setRootIndex(index);
            GLDListView::setCurrentIndex(index);
            emit rootIndexChanged(index);
        }
    }
}

void GLDShellListView::showHint(const QModelIndex &index)
{
    if (!m_bFirstShow)
    {
        if (m_bShowHint && index.isValid())
        {
            drawComment(index);
        }
    }
    else
    {
        m_bFirstShow = false;
    }
}

void GLDShellListView::mouseDoubleClickSlot(const QModelIndex &index)
{
    if (index.isValid())
    {
        if (!m_bOpenFileWhileDblClk)
        {
            GLDCustomFileSystemModel *pModel = dynamic_cast<GLDCustomFileSystemModel*>(model());
            if (pModel && ISFILE == getTheIndexDirType(index, pModel))
            {
                setVsViewMode(m_viewMode);
                if (m_shellTree && !currentReportMode())
                {
                    m_shellTree->setCurrentIndex(m_shellTree->proxyIndex(index));
                }
                return;
            }
        }
        setRootIndex(index);
        setVsViewMode(m_viewMode);
        if (m_shellTree && !currentReportMode())
        {
            m_shellTree->setRootIndex(m_shellTree->proxyIndex(index));
            m_shellTree->setCurrentIndex(m_shellTree->proxyIndex(index));
        }
    }
}

void GLDShellListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex clickedIndex = indexAt(event->pos());
    GLDListView::mousePressEvent(event);
    if ((event->button() == Qt::RightButton) && (!clickedIndex.isValid()))
    {
        return;
    }
    setCurrentIndex(clickedIndex);
}

void GLDShellListView::drawComment(const QPersistentModelIndex &index)
{
    if (!(commentFrame))
    {
        commentFrame = new GCommentFrame(this);
        commentFrame->m_arrow->hide();
    }
    if (!index.isValid())
    {
        commentFrame->hide();
    }
    else
    {
        QVariant name = index.data(GLDFileSystemModel::FileNameRole);
        QString str = name.toString();
        if (str == "")
            str = index.data().toString(); //model()->data(index, gidrCommentRole).toString();
        if (str != "")
        {
            commentFrame->setCommentText(str);
            //move的位置参数是相对于父窗体中的坐标
            commentFrame->move(visualRect(index).topRight() - commentFrame->rect().topRight()/*+ viewport()->pos()*/);
            adjustChildWidgetPos(commentFrame);
            if (!currentReportMode())
            {
                commentFrame->setParent(this);
            }
            else if (m_shellTree)
            {
                commentFrame->setParent(m_shellTree);
            }
            commentFrame->show();
            commentFrame->m_arrow->hide();
            //定时消失
            QTimer::singleShot(m_hintExistSeconds * 1000, commentFrame, SLOT(hide()));
        }
        else
        {
            commentFrame->hide();
        }
    }
}

bool GLDShellListView::adjustChildWidgetPos(QWidget *child)
{
    if (child->parentWidget() != this)
        return false;
    else
    {
        QRect posRect = child->geometry();
        QRect hostRect = this->viewport()->rect();
        if (hostRect.contains(posRect))
        {
            doNothingMacro();
        }
        else
        {
            QPoint tl = posRect.topLeft();
            QPoint tr = posRect.topRight();
            QPoint bl = posRect.bottomLeft();
            QPoint br = posRect.bottomRight();
            if (!hostRect.contains(tl))
            {
                child->move(0,0);
            }
            else if (!hostRect.contains(tr))
            {
                child->move(qMax(0,(hostRect.width() - posRect.width())), 0);
            }
            else if (!hostRect.contains(bl))
            {
                child->move(0, qMax((hostRect.height() - posRect.height()), 0));
            }
            else if (!hostRect.contains(br))
            {
                child->move(qMax((hostRect.width() - posRect.width()), 0) ,
                            qMax((hostRect.height() - posRect.height()), 0));
            }
        }
        return true;
    }
}

QString GLDDirFilter::filePathByType(const QModelIndex &currentIndex, GLDDirFilter::DirName dirType) const
{
    QString tmp;
    if (AbsoluteName == dirType)
    {
        QVariant path = currentIndex.data(GLDCustomFileSystemModel::FilePathRole);
        tmp = path.toString();
    }
    else
    {
        QVariant name = currentIndex.data(GLDCustomFileSystemModel::FileNameRole);
        tmp = name.toString();
    }
    return tmp;
}

GLDDirFilter::ISDIR GLDDirFilter::getTheIndexDirType(const QModelIndex &index, const GLDCustomFileSystemModel *pModel) const
{
    if (index.isValid())
    {
        if (pModel->isDir(index))
            return GLDShellListView::ISFOLDER;
        else
            return GLDShellListView::ISFILE;
    }
    else
        return GLDShellListView::NOITEMSELECTED;
}

void GLDDirFilter::doSetNameFilters(QAbstractItemModel *model, const QStringList &filters)
{
    if (model)
    {
        GLDFileSystemModel *pModel = dynamic_cast<GLDFileSystemModel *>(model);
        if (pModel == NULL)
        {
            GLDCustomFileSystemModel *pMutilModel = dynamic_cast<GLDCustomFileSystemModel *>(model);
            pModel = pMutilModel->fileSystemModel();
        }
        if (pModel != NULL)
        {
            QDir::Filters filter = pModel->filter();
            if (filters.count() > 0)
            {
                filter |= QDir::AllDirs;
                pModel->setFilter(filter);
            }
            pModel->setNameFilters(filters);
        }
    }
}

QStringList GLDDirFilter::doNameFilters(QAbstractItemModel *model) const
{
    if (model)
    {
        if (GLDFileSystemModel *pModel = dynamic_cast<GLDFileSystemModel *>(model))
        {
            return pModel->nameFilters();
        }
    }
    return QStringList();
}
