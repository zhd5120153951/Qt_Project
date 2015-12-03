#include <QtCore>
#include <QLocale>
#include <QStylePainter>

#include "GLDShadow.h"
#include "GLDStrUtils.h"
#include "GLDFileUtils.h"
#include "GLDScrollStyle.h"
#include "GLDFileSystemModel.h"
#include "GLDShellComboBoxEx.h"
#include "GLDShellTreeViewEx.h"

const QString c_strRootColor = "RootColor";
const QString c_strOthersColor = "OthersColor";
const int c_nColorLength = 7;
const QColor c_colorRootName = QColor("#626262");
const QColor c_colorOthersName = QColor("#010101");

const int c_nTreeViewIndentation = 15;
const int c_nTreeViewItemHeight = 25;
const int c_nTreeViewButtomOffset = -14;
const int c_nTreeViewMaxHeight = 320;
const int c_nTreeViewMinHeight = 20;
//const int c_nMaxImageWidth = 7;
const int c_nArrowOffset = 4;

const QVector<QString> c_vec_iconType = QVector<QString>()
                                        << "QFileIconProviderComputer"
                                        << "QFileIconProviderDesktop"
                                        << "QFileIconProviderTrashcan"
                                        << "QFileIconProviderNetwork"
                                        << "QFileIconProviderDrive"
                                        << "QFileIconProviderFolder"
                                        << "QFileIconProviderFile";

const int c_nArrowToIcon = 8;
//const int c_nComboBoxArrowToRight = 10;

const char c_strIconTreeRoot[] = "://icons/right.png";
const char c_strIconTreeChild[] = "://icons/rigjt02.png";
const char c_strIconTreeHover[] = "://icons/r-hover.png";
const char c_strIconTreeOpen[] = "://icons/hs.png";
const char c_strIconTreeOpenHover[] = "://icons/z-hover.png";

const int c_nShadowWidth = 8;
const int c_nShadowRGB = 240;
const int c_nShadowRGBGrowth = 2;
const int c_nShadowAlpha[8] = {255, 100, 90, 60, 30, 30, 15, 15};

const GString c_sShellComboBoxQssFile = ":/qsses/GLDShellComboBoxEx.qss";

GLDShellComboBoxEx::GLDShellComboBoxEx(const QString &rootPath, QWidget *parent):
    GLDShellComboBox(rootPath, parent),
    m_bIsShow(false),
    m_bIsLineEdit(true),
    m_bIsShowIcon(false),
    m_bIsDrag(false),
    m_bIsPopMove(false),
    m_nCanSetCursor(0),
    m_bHasShadow(true),
    m_delegate(NULL)
{
    init(rootPath);
    // Qss进行美化
    setStyle(new GLDScrollStyle(this));
    setHasBorder(true);
    setEditProperty(false);
    this->setStyleSheet(loadQssFile(c_sShellComboBoxQssFile));

    // 移除阴影
    GLDShadow *pShadow = new GLDShadow(this);
    pShadow->removeShadow();

    initConnection();
    installEventFilters();
}

void GLDShellComboBoxEx::init(const QString &rootPath)
{
    initPopupFlags();
    initFileModel(rootPath);
    initShadow();
    setPopupWidget(m_popupShellTree);
}

void GLDShellComboBoxEx::initPopupFlags()
{
    Qt::WindowFlags flags;
    flags = Qt::MSWindowsFixedSizeDialogHint | Qt::FramelessWindowHint | Qt::Popup;
    comboBoxPopup()->setWindowFlags(flags);
    comboBoxPopup()->repaint();
}

void GLDShellComboBoxEx::initFileModel(const QString &rootPath)
{
    freeAndNil(m_pFileModel);

    m_pFileModel = new GLDFileSystemModelEX(true, this);
    QModelIndex pathIndex = m_pFileModel->index("");

    if (rootPath.length() > 0)
    {
        QModelIndex pathIndexTmp = m_pFileModel->index(rootPath);

        if (pathIndexTmp.isValid() && m_pFileModel->isDir(pathIndexTmp))
        {
            pathIndex = pathIndexTmp;
        }
    }
    initTreeView(pathIndex);
}

void GLDShellComboBoxEx::initTreeView(QModelIndex pathIndex)
{
    freeAndNil(m_popupShellTree);

    GLDShellTreeViewEx *pTreeView = new GLDShellTreeViewEx(true, this);
    m_popupShellTree = pTreeView;
    //set tree as combobox`s popup
    m_popupShellTree->setSingleRoot(false);
    m_popupShellTree->setModel(m_pFileModel);
    Q_ASSERT(m_pFileModel == m_popupShellTree->model());
    m_popupShellTree->setRootIndex(pathIndex);

    //inherited init action
    m_popupShellTree->setAnimated(false);
    m_popupShellTree->setIndentation(c_nTreeViewIndentation);
    m_popupShellTree->setSortingEnabled(true);
    initDelegate(pTreeView);
}

void GLDShellComboBoxEx::initDelegate(GLDShellTreeViewEx *pTreeView)
{
    m_delegate = new GLDShellComboBoxItemDelegate("", this);
    m_popupShellTree->setItemDelegate(m_delegate);
    m_delegate->setTreeView(pTreeView);
    GLDFileSystemModelEX *pModel =
        dynamic_cast<GLDFileSystemModelEX *>(m_pFileModel);

    if (pModel)
    {
        m_delegate->setFileSystemModel(pModel);
        pModel->setDelegate(m_delegate);
    }
}

void GLDShellComboBoxEx::initShadow()
{
    comboBoxPopup()->setAttribute(Qt::WA_TranslucentBackground);
}

void GLDShellComboBoxEx::setHasBorder(bool bHasBorder)
{
    getLineEdit()->setProperty("borderStyle", bHasBorder);
    this->setStyleSheet(loadQssFile(c_sShellComboBoxQssFile));
}

void GLDShellComboBoxEx::setEditProperty(bool bShow)
{
    QString sProperty = QString("");
    if (bShow)
    {
        sProperty = sProperty + QString("show");
    }
    else
    {
        sProperty = sProperty + QString("hide");
    }

    getLineEdit()->setProperty(
                "GLDPlainTextEditState", sProperty);
}

void GLDShellComboBoxEx::setFileModel(GLDCustomFileSystemModel *model)
{
    freeAndNil(m_pFileModel);
    m_pFileModel = model;
    QModelIndex pathIndex = m_pFileModel->index("");
    initTreeView(pathIndex);
}

void GLDShellComboBoxEx::setShellListView(GLDShellListView *list)
{
    GLDFileSystemModelEX *pFileModel = dynamic_cast<GLDFileSystemModelEX*>(m_pFileModel);
    if(pFileModel)
    {
        m_pFileModel = pFileModel;
    }
    GLDShellComboBox::setShellListView(list);
}

void GLDShellComboBoxEx::initConnection()
{
    connect(this, &GLDShellComboBox::onHide,
            this, &GLDShellComboBoxEx::onSignalOnHide);
    connect(this, &GLDShellComboBox::shouldShowPopup,
            this, &GLDShellComboBoxEx::onShouldShowPopup);
    connect(getLineEdit(), &GLDPlainTextEdit::cursorPositionChanged,
            this, &GLDShellComboBoxEx::doAfterEditCursorChanged);
    connect(m_popupShellTree, &GLDShellTreeView::expanded,
            this, &GLDShellComboBoxEx::doAfterExpanded);
    connect(m_popupShellTree, &GLDShellTreeView::collapsed,
            this, &GLDShellComboBoxEx::doAfterCollapsed);
}

void GLDShellComboBoxEx::installEventFilters()
{
    m_popupShellTree->installEventFilter(this);
    m_popupShellTree->viewport()->installEventFilter(this);
    getLineEdit()->installEventFilter(this);
    getLineEdit()->viewport()->installEventFilter(this);
    comboBoxPopup()->installEventFilter(this);
}

void GLDShellComboBoxEx::paintEvent(QPaintEvent *event)
{
    if (m_bIsShowIcon)
    {
        GLDShellComboBox::paintEvent(event);
    }

    if (!framePopup())
    {
        GLDWindowComboBox::paintEvent(event);
    }
}

void GLDShellComboBoxEx::resizeEvent(QResizeEvent *event)
{
    GLDShellComboBox::resizeEvent(event);
    getLineEdit()->setMaximumSize(size());
    getLineEdit()->setGeometry(rect());
    comboBoxPopup()->setFixedWidth(width() + c_nShadowWidth * 2);
}

bool GLDShellComboBoxEx::eventFilter(QObject *object, QEvent *event)
{
    // 如果是ShellTreeView弹出
    if (object == m_popupShellTree)
    {
        if (event->type() != QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            {
                GLDShellTreeViewEx *pTreeView = static_cast <GLDShellTreeViewEx *>(m_popupShellTree);
                if (pTreeView->selectedIndexes().size())
                {
                    closePopup();
                    setEditTextByIndex(pTreeView->selectedIndexes()[0]);
                }
            }
        }
    }
    // 如果是ComboBox弹出
    if (object == comboBoxPopup())
    {
        if (event->type() == QEvent::Paint)
        {
            if (!m_bIsPopMove)
            {
                QPoint point = comboBoxPopup()->pos();
                point.setX(point.x() - c_nShadowWidth);
                comboBoxPopup()->move(point);
                m_bIsPopMove = true;
            }

            if (m_bHasShadow)
            {
                QPainter painter(comboBoxPopup());

                for (int i = 0; i < c_nShadowWidth; i++)
                {
                    int nRGB = c_nShadowRGB + i * c_nShadowRGBGrowth;
                    QPen pen;
                    // 需要先画2像素的阴影
                    const QString c_sShadows[2] = {"#c0cce5", "#e1e5ed"};
                    if (i < 2)
                    {
                        pen.setColor(QColor(c_sShadows[i]));
                        painter.setPen(pen);
                    }
                    else
                    {
                        pen.setColor(QColor(nRGB, nRGB, nRGB, c_nShadowAlpha[i]));
                    }
                    pen.setWidth(1);
                    painter.setPen(pen);

                    QPoint pointStart;
                    QPoint pointEnd;
                    QRect rect = comboBoxPopup()->rect();

                    // 画左侧阴影
                    pointStart.setX(c_nShadowWidth - i - 1);
                    pointStart.setY(rect.bottomLeft().y() - c_nShadowWidth + i + 1);
                    pointEnd.setX(pointStart.x());
                    pointEnd.setY(0);
                    painter.drawLine(pointStart, pointEnd);

                    // 画下边阴影
                    pointStart.setX(c_nShadowWidth - i - 1);
                    pointStart.setY(rect.bottomLeft().y() - c_nShadowWidth + i + 1);
                    pointEnd.setX(rect.bottomRight().x() - c_nShadowWidth + i + 1);
                    pointEnd.setY(pointStart.y());
                    painter.drawLine(pointStart, pointEnd);

                    // 画右侧阴影
                    pointStart.setX(rect.bottomRight().x() - c_nShadowWidth + i + 1);
                    pointStart.setY(rect.bottomRight().y() - c_nShadowWidth + i + 1);
                    pointEnd.setX(pointStart.x());
                    pointEnd.setY(0);
                    painter.drawLine(pointStart, pointEnd);
                }
            }
        }
    }
    return GLDShellComboBox::eventFilter(object, event);
}

void GLDShellComboBoxEx::onSignalOnHide()
{
    if (comboBoxPopup()->isActiveWindow())
    {
        return;
    }

    m_bIsShow = false;
    setEditProperty(m_bIsShow);
    this->setStyleSheet(loadQssFile(c_sShellComboBoxQssFile));
    updateEditFieldGeometry();

    //发送afterPopupHid信号
    emit GLDShellComboBoxEx::afterPopupHid();
    //发送onHide信号
    emit GLDShellComboBoxEx::onHide();
    m_nCanSetCursor = 2;
}

void GLDShellComboBoxEx::autoSetContainerHeight()
{
    GLDShellTreeViewEx *pTreeView = dynamic_cast<GLDShellTreeViewEx *>(m_popupShellTree);

    if (pTreeView)
    {
        int nRows = pTreeView->countAllShownRows();
        int nHeight = nRows * c_nTreeViewItemHeight + c_nTreeViewMinHeight;
        if (nHeight > c_nTreeViewMaxHeight)
        {
            nHeight = c_nTreeViewMaxHeight;
            m_popupShellTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
        else
        {
            m_popupShellTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        if (nHeight != comboBoxPopup()->popuHeight())
        {
            comboBoxPopup()->setPopupHeight(nHeight);
        }
    }
}

void GLDShellComboBoxEx::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up)
    {
        QModelIndex index = m_popupShellTree->currentIndex();

        if (index.isValid())
        {
            int nRow = index.row();

            if (event->key() == Qt::Key_Down)
            {
                if (index.sibling(nRow + 1, 0).isValid())
                {
                    ++nRow;
                }
                else
                {
                    nRow = 0;
                }
            }
            else
            {
                if (index.row() != 0)
                {
                    --nRow;
                }
                else
                {
                    while (index.sibling(++nRow, 0).isValid());
                    --nRow;
                }
            }

            index = index.sibling(nRow, 0);

            GLDShellTreeViewEx *pTreeView = static_cast <GLDShellTreeViewEx *>(m_popupShellTree);
            pTreeView->setCurrentIndex(index);
            m_nCanSetCursor = 2;
            setEditTextByIndex(index);
        }
    }

    GLDShellComboBox::keyPressEvent(event);
}

void GLDShellComboBoxEx::updateEditFieldGeometry()
{
    QStyleOptionComboBox optCombo;
    optCombo.init(this);
    optCombo.editable = true;
    optCombo.subControls = QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField;

    if (m_buttonType != None)
    {
        optCombo.subControls |= QStyle::SC_ComboBoxArrow;
        // Qss需要
        QRect editRect = rect().adjusted(0, 0, 0, 0);
        // 由于在setGeometry的时候，会再次计算并设置MinimumSize和MaximumSize
        // 会出现区域改变的问题，因此在这里先设置好MinimumSize和MaximumSize
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
        lineEdit()->setGeometry(editRect);
    }
    else
    {
        QRect editRect = rect().adjusted(0, 0, 0, -1);
        lineEdit()->setMinimumSize(editRect.width(), editRect.height());
        lineEdit()->setMaximumSize(editRect.width(), editRect.height());
    }
}

void GLDShellComboBoxEx::onShouldShowPopup(bool &value)
{
    setEditProperty(value);
    this->setStyleSheet(loadQssFile(c_sShellComboBoxQssFile));
    updateEditFieldGeometry();
    //发送信号
    emit GLDShellComboBoxEx::shouldShowPopup(value);

    if (value)
    {
        emit GLDShellComboBoxEx::afterPopupShowed();
        m_bIsShow = true;
        m_bIsPopMove = false;
        autoSetContainerHeight();
    }
}

void GLDShellComboBoxEx::doAfterEditCursorChanged()
{
    if (m_nCanSetCursor)
    {
        getLineEdit()->blockSignals(true);
        getLineEdit()->moveCursor(QTextCursor::End);
        getLineEdit()->blockSignals(false);
        --m_nCanSetCursor;
        getLineEdit()->selectAll();
    }

    emit cursorPositionChanged();
}

void GLDShellComboBoxEx::doAfterExpanded(const QModelIndex &index)
{
    autoSetContainerHeight();
    emit expanded(index);
}

void GLDShellComboBoxEx::doAfterCollapsed(const QModelIndex &index)
{
    autoSetContainerHeight();
    emit collapsed(index);
}

GLDFileSystemModelEX::GLDFileSystemModelEX(bool includingFile, QObject *parent):
    GLDFileSystemModel(includingFile, parent),
    m_colorRoot(c_colorRootName),
    m_colorOthers(c_colorOthersName), m_delegate(NULL)
{
    m_pIconProvider = new GLDFileIconProvider;
    setIconProvider(m_pIconProvider);
}

GLDFileSystemModelEX::~GLDFileSystemModelEX()
{
    freeAndNil(m_pIconProvider);
}

QVariant GLDFileSystemModelEX::data(const QModelIndex &index, int role) const
{
    Q_D(const GLDFileSystemModel);

    if (!d->indexValid(index))
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case 0:
            {
                QString strName = d->name(index);

                if (0 == strName.compare("desktop", Qt::CaseInsensitive))
                {
                    QLocale locale;

                    switch (locale.country()) //判断操作系统语言
                    {
                        case QLocale::China:
                        case QLocale::Chinese:
                            strName = TRANS_STRING("桌面");
                            break;

                        default:
                            strName = "DeskTop";
                            break;
                    }
                }

                return strName;
            }

            case 1:
                return d->size(index);

            case 2:
                return d->type(index);

            case 3:
                return d->updateDateTime(index);

            default:
                qWarning("data: invalid display value column %d", index.column());
                return QVariant();
        }
    }

    if (index.column() == 0)
    {
        switch (role)
        {
            case FileIconRole:
                return fileIcon(index);

            case FilePathRole:
                return filePath(index);

            case FileNameRole:
            case Qt::ToolTipRole:
                return fileName(index);

            case Qt::ForegroundRole:
                return fileNameColor(index);

            default:
                return QVariant();
        }
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

QIcon GLDFileSystemModelEX::fileIcon(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);

    if (!d->indexValid(index))
    {
        return d->iconProvider->icon(QFileIconProvider::Computer);
    }

    GLDFileSystemModelPrivate::QDirNode *node = d->node(index);

    if (node->icon.isNull())
    {
        node->icon = d->iconProvider->icon(node->info);
    }

    return node->icon;
}

QColor GLDFileSystemModelEX::fileNameColor(const QModelIndex &index) const
{
    Q_D(const GLDFileSystemModel);

    if (!d->indexValid(index))
    {
        return QColor();
    }

    if (NULL == m_delegate)
    {
        return m_colorRoot;
    }

   else  if (m_delegate->isParentExpandedRecently(index))
    {
        return m_colorOthers;
    }

    return m_colorRoot;
}

void GLDFileSystemModelEX::setFontColor(const QString &setting)
{
    int nIndex = -1;

    nIndex = setting.indexOf(c_strRootColor);

    if (nIndex > -1)
    {
        QString strRootColor = setting.mid(nIndex + c_strRootColor.size(),
                                           c_nColorLength);
        m_colorRoot = QColor(strRootColor);
    }

    nIndex = setting.indexOf(c_strOthersColor);

    if (nIndex > -1)
    {
        QString strOthersColor = setting.mid(nIndex + c_strOthersColor.size(),
                                             c_nColorLength);
        m_colorOthers = QColor(strOthersColor);
    }
}

GLDFileSystemModelEX::GLDFileSystemModelEX(const QStringList &nameFilters,
        QDir::Filters filters,
        QDir::SortFlags sort,
        QObject *parent):
    GLDFileSystemModel(nameFilters, filters, sort, parent)
{
}

GLDFileSystemModelEX::GLDFileSystemModelEX(GLDFileSystemModelPrivate &model, QObject *parent):
    GLDFileSystemModel(model, parent)
{
}

GLDFileIconProvider::GLDFileIconProvider()
{
}

GLDFileIconProvider::~GLDFileIconProvider()
{
}

QIcon GLDFileIconProvider::icon(QFileIconProvider::IconType type) const
{
    if (m_map_icon.contains(type))
    {
        return m_map_icon[type];
    }
    else
    {
        return QFileIconProvider::icon(type);
    }
}

QIcon GLDFileIconProvider::icon(const QFileInfo &info) const
{
    if (info.isRoot())
    {
        if (info.isNativePath() && info.isReadable())//磁盘
        {
            if (m_map_icon.contains(QFileIconProvider::Drive))
            {
                return icon(QFileIconProvider::Drive);
            }
        }

        return QFileIconProvider::icon(info);
    }

    if (info.isDir())//文件夹
    {
        if (info.fileName().compare("Desktop", Qt::CaseInsensitive) == 0)//桌面文件夹
        {
            if (m_map_icon.contains(QFileIconProvider::Desktop))
            {
                return icon(QFileIconProvider::Desktop);
            }
        }
        else
        {
            if (m_map_icon.contains(QFileIconProvider::Folder))
            {
                return icon(QFileIconProvider::Folder);
            }
        }
    }

    return QFileIconProvider::icon(info);
}

void GLDFileIconProvider::loadIcon(QMap<QFileIconProvider::IconType, QString> mapURL)
{
    QMap<QFileIconProvider::IconType, QString>::iterator it;

    for (it = mapURL.begin(); it != mapURL.end(); ++it)
    {
        m_map_icon[it.key()] = QIcon(QPixmap(*it));
    }
}

void GLDFileIconProvider::loadSetting(const QString &setting)
{
    int nIndex = -1;

    for (int i = 0; i < c_vec_iconType.size(); i++)
    {
        if (nIndex > -1)
        {
            int nStringSize = c_vec_iconType[i].size();
            int nLengthIndex = setting.indexOf("\n", nIndex);

            if (nLengthIndex < 0)
            {
                continue;
            }

            int nLength = nLengthIndex - (nIndex + nStringSize);
            QString strIconURL = setting.mid(nIndex + nStringSize, nLength);
            m_map_icon[QFileIconProvider::IconType(i)] = QIcon(QPixmap(strIconURL));
        }
    }
}

GLDShellComboBoxItemDelegate::GLDShellComboBoxItemDelegate(
        const QString &rootPath, QWidget *parent):
    QStyledItemDelegate(parent),
    m_treeView(NULL),
    m_model(NULL)
{
    G_UNUSED(rootPath);
}

QPixmap GLDShellComboBoxItemDelegate::pixmap(const QModelIndex &index) const
{
    if (!m_model->hasChildren(index))
    {
        return QPixmap();
    }

    if (index == m_treeView->hoverIndex())
    {
        if (m_treeView->isExpanded(index))
        {
            return QPixmap(c_strIconTreeOpenHover);
        }
        else
        {
            return QPixmap(c_strIconTreeHover);
        }
    }

    if (m_model->fileInfo(index).isRoot() 
            || m_model->fileInfo(index).fileName().compare("Desktop", Qt::CaseInsensitive) == 0)
    {
        if (m_treeView->isExpanded(index))
        {
            return QPixmap(c_strIconTreeOpen);
        }
        else
        {
            return QPixmap(c_strIconTreeRoot);
        }
    }

    if (m_treeView->isExpanded(index))
    {
        return QPixmap(c_strIconTreeOpen);
    }

    if (isParentExpandedRecently(index))
    {
        return QPixmap(c_strIconTreeChild);
    }

    return QPixmap(c_strIconTreeRoot);
}

bool GLDShellComboBoxItemDelegate::isParentExpandedRecently(const QModelIndex &index)const
{
    return (index.parent() == m_indexExpanded);
}

void GLDShellComboBoxItemDelegate::setExpandedIndex(const QModelIndex &index)
{
    m_indexExpanded = index;
}

QSize GLDShellComboBoxItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(c_nTreeViewItemHeight);
    return size;
}

void GLDShellComboBoxItemDelegate::paint(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    QRect rect = option.rect;

    if (rect.height() != c_nTreeViewItemHeight)
    {
        return;
    }

    if (rect.y() + rect.height() > m_treeView->height() + c_nTreeViewButtomOffset)
    {
        return;
    }

    rect.adjust(-c_nTreeViewIndentation, 0, -rect.width(), 0);
    m_treeView->setModelIndexRect(index, rect);

    rect.adjust(c_nArrowOffset, 0, -c_nArrowToIcon + c_nArrowOffset, 0);
    QPixmap image = pixmap(index);
    int nPosx = rect.x();
    int nPosy = rect.y() + (rect.height() - image.height()) / 2;
    painter->drawPixmap(nPosx, nPosy, image.width(), image.height(), image);

    QStyledItemDelegate::paint(painter, option, index);
}

void GLDShellComboBoxItemDelegate::doAfterExpanded(const QModelIndex &index)
{
    setExpandedIndex(index);
}

void GLDShellComboBoxItemDelegate::doAfterCollapsed(const QModelIndex &index)
{
    QModelIndex indexSiling = index.sibling(0, 0);
    for (int i = 0; indexSiling.isValid(); indexSiling = index.sibling(++i, 0))
    {
        if (m_treeView->isExpanded(indexSiling))
        {
            setExpandedIndex(QModelIndex());
            return;
        }
    }
    setExpandedIndex(index.parent());
}

QModelIndex GLDShellComboBoxItemDelegate::findLonelyExpanded(const QModelIndex &root)
{
    Q_UNUSED(root);
    return QModelIndex();
}

void GLDShellComboBoxItemDelegate::setTreeView(GLDShellTreeViewEx *treeView)
{
    m_treeView = treeView;
    connect(m_treeView, &GLDShellTreeViewEx::expanded, this,
            &GLDShellComboBoxItemDelegate::doAfterExpanded);
    connect(m_treeView, &GLDShellTreeViewEx::collapsed,
            this, &GLDShellComboBoxItemDelegate::doAfterCollapsed);
}

void GLDShellComboBoxItemDelegate::setFileSystemModel(GLDFileSystemModelEX *model)
{
    m_model = model;
    m_indexExpanded = model->index("");
}
