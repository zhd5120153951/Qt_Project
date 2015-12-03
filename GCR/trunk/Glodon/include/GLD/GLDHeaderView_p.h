#ifndef GLDHEADERVIEW_P_H
#define GLDHEADERVIEW_P_H

#include "GLDAbstractItemView_p.h"
#include "GLDHeaderView.h"

class GlodonHeaderViewPrivate: public GlodonAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(GlodonHeaderView)

public:
    enum StateVersion { VersionMarker = 0xff };

    GlodonHeaderViewPrivate()
        : m_state(NoState),
          offset(0),
          sortIndicatorOrder(Qt::DescendingOrder),
          sortIndicatorSection(0),
          sortIndicatorShown(false),
          fixedCount(0),
          isTree(false),
          autoSectionData(false),
          lastPos(-1),
          firstPos(-1),
          originalSize(-1),
          section(-1),
          target(-1),
          pressed(-1),
          m_hover(-1),
          drawWidth(0),
          gridLineWidth(1),
          length(0),
          m_srcLength(-1),
          preventCursorChangeInSetOffset(false),
          movableSections(true),
          clickableSections(true),
          highlightSelected(true),
          cascadingResizing(false),
          m_inZoomForceResize(false),
          defaultSectionSize(0),
          minimumSectionSize(-1),
          lastSectionSize(0),
          sectionIndicatorOffset(0),
          sectionIndicator(0),
          globalResizeMode(GlodonHeaderView::Interactive),
          sectionStartposRecalc(true),
          resizeDelay(true),
          m_mousePosition(),
          fixedCellEvent(false),
          gridLineColor(255, 255, 255),
          m_isMouseDoubleClick(false),
          m_srcFontPointSize(-1),
          m_createSectionSpanSeparate(false),
          m_drawBorder(false),
          m_allowDoubleClicked(true),
          m_boldWithSelected(true),
          m_bFontShadow(false),
          m_clrFontShadow(244, 245, 249),
          m_bLinearGradient(false),
          m_logicalMovable(false)

    {}

    int modelSectionCount() const;
    int childIndexRowCount(QModelIndex index) const;
    int lastVisibleVisualIndex() const;
    int sectionHandleAt(int position);
    void setupSectionIndicator(int section, int position);
    void updateSectionIndicator(int section, int position);
    void updateHiddenSections(int logicalFirst, int logicalLast);
    void _q_sectionsRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast);
    void _q_layoutAboutToBeChanged();
    void _q_layoutChanged();

    bool isSectionSelected(int section) const;
    bool isFirstVisibleSection(int section) const;
    bool isLastVisibleSection(int section) const;

    inline bool rowIntersectsSelection(int row) const
    {
        return (m_selectionModel ? m_selectionModel->rowIntersectsSelection(row, m_root) : false);
    }

    inline bool columnIntersectsSelection(int column) const
    {
        return (m_selectionModel ? m_selectionModel->columnIntersectsSelection(column, m_root) : false);
    }

    inline bool sectionIntersectsSelection(int logical) const
    {
        return (orientation == Qt::Horizontal ? columnIntersectsSelection(logical) : rowIntersectsSelection(logical));
    }

    inline bool isRowSelected(int row) const
    {
        return (m_selectionModel ? m_selectionModel->isRowSelected(row, m_root) : false);
    }

    inline bool isColumnSelected(int column) const
    {
        return (m_selectionModel ? m_selectionModel->isColumnSelected(column, m_root) : false);
    }

    inline void prepareSectionSelected()
    {
        if (!m_selectionModel || !m_selectionModel->hasSelection())
        {
            sectionSelected.clear();
        }
        else if (sectionSelected.count() != sectionCount() * 2)
        {
            sectionSelected.fill(false, sectionCount() * 2);
        }
        else
        {
            sectionSelected.fill(false);
        }
    }

    inline int sectionCount() const
    {
        return sectionItems.count();
    }

    inline bool reverse() const
    {
        return orientation == Qt::Horizontal && q_func()->isRightToLeft();
    }

    inline int logicalIndex(int visualIndex) const
    {
        return logicalIndices.isEmpty() ? visualIndex : logicalIndices.at(visualIndex);
    }

    inline int visualIndex(int logicalIndex) const
    {
        return visualIndices.isEmpty() ? logicalIndex : visualIndices.at(logicalIndex);
    }

    inline void setDefaultValues(Qt::Orientation o)
    {
        orientation = o;
        defaultSectionSize = (o == Qt::Horizontal ? 100
                              : qMax(q_func()->minimumSectionSize(), 30));
        defaultAlignment = Qt::AlignCenter;
    }

    inline bool isVisualIndexHidden(int visual) const
    {
        return !sectionHidden.isEmpty() && sectionHidden.at(visual);
    }

    inline void setVisualIndexHidden(int visual, bool hidden)
    {
        if (!sectionHidden.isEmpty())
        {
            sectionHidden.setBit(visual, hidden);
        }
    }

    QStyleOptionHeader getStyleOption() const;

    inline void invalidateCachedSizeHint() const
    {
        cachedSizeHint = QSize();
    }

    inline void initializeIndexMapping() const
    {
        if (visualIndices.count() != sectionCount()
                || logicalIndices.count() != sectionCount())
        {
            visualIndices.resize(sectionCount());
            logicalIndices.resize(sectionCount());

            for (int s = 0; s < sectionCount(); ++s)
            {
                visualIndices[s] = s;
                logicalIndices[s] = s;
            }
        }
    }

    inline void clearCascadingSections()
    {
        firstCascadingSection = sectionCount();
        lastCascadingSection = 0;
        cascadingSectionSize.clear();
    }

    inline void saveCascadingSectionSize(int visual, int size)
    {
        if (!cascadingSectionSize.contains(visual))
        {
            cascadingSectionSize.insert(visual, size);
            firstCascadingSection = qMin(firstCascadingSection, visual);
            lastCascadingSection = qMax(lastCascadingSection, visual);
        }
    }

    inline bool sectionIsCascadable(int visual) const
    {
        return headerSectionResizeMode(visual) == GlodonHeaderView::Interactive;
    }

    inline bool modelIsEmpty() const
    {
        return (m_model->rowCount(m_root) == 0 || m_model->columnCount(m_root) == 0);
    }

    int fixedSectionsSize() const;

    void clear();
    void flipSortIndicator(int section);
    void cascadingResize(int visual, int newSize, bool isManual);

    enum State { NoState, ResizeSection, MoveSection, SelectSections, NoClear, CXMoveSection } m_state;

    int offset;
    Qt::Orientation orientation;
    Qt::SortOrder sortIndicatorOrder;
    int sortIndicatorSection;
    bool sortIndicatorShown;
    int fixedCount;
    bool isTree;
    bool autoSectionData;

    mutable QVector<int> visualIndices; // visualIndex = visualIndices.at(logicalIndex)
    mutable QVector<int> logicalIndices; // logicalIndex = row or column in the model
    mutable QBitArray sectionSelected; // from logical index to bit
    mutable QBitArray sectionHidden; // from visual index to bit, 有行/列折叠时，所有行加入进来，全部展开时，为空。
    mutable QHash<int, int> hiddenSectionSize; // from logical index to section size
    mutable QHash<int, int> cascadingSectionSize; // from visual index to section size
    mutable QSize cachedSizeHint;
    mutable QBasicTimer delayedResize;

    int firstCascadingSection;
    int lastCascadingSection;

    int lastPos;
    int firstPos;
    int originalSize;
    int section; // used for resizing and moving sections
    int target;
    int pressed;
    int m_hover;

    int drawWidth;
    int gridLineWidth;
    int length;
    int m_srcLength;
    bool preventCursorChangeInSetOffset;
    bool movableSections;
    bool clickableSections;
    bool highlightSelected;
    bool cascadingResizing;
    bool m_inZoomForceResize;
    bool m_logicalMovable;

    int defaultSectionSize;
    int minimumSectionSize;
    int lastSectionSize;
    int sectionIndicatorOffset;
    Qt::Alignment defaultAlignment;
    QLabel *sectionIndicator;
    GlodonHeaderView::ResizeMode globalResizeMode;
    QList<QPersistentModelIndex> persistentHiddenSections;

    mutable bool sectionStartposRecalc;

    bool resizeDelay; //调整行或者列大小时，是否实施刷新，默认为延迟刷新
    QPoint m_mousePosition;//存储调整行（列）大小时，鼠标的位置
    bool fixedCellEvent; //自定义固定格背景色
    QColor gridLineColor;
    bool m_isMouseDoubleClick;//当鼠标双击时，置为True，防止在开启延迟调整行高（列宽）时，是否在MouseRelease中再次调整
    int m_srcFontPointSize;
    bool m_createSectionSpanSeparate;//每列，行，单独作为一个sectionSpan，在使用放大缩小功能时，建议开启

    mutable QHash<int, GlodonCellAttr> fixedCellsColor;

    QMultiHash<int, QString> m_logicalIndexHash; //表头加图标

    int m_spanTop; //所加图片的位置

    // header section
    struct SectionItem
    {
        uint size : 20;
        uint reservedForIsHidden : 1;
        uint resizeMode : 5;  // (holding GlodonHeaderView::ResizeMode)
        uint currentlyUnusedPadding : 6;

        int srcSize;

        union   // This union is made in order to save space and ensure good vector performance (on remove)
        {
            mutable int calculated_startpos; // <- this is the primary used member.
            mutable int tmpLogIdx;         // When one of these 'tmp'-members has been used we call
            int tmpDataStreamSectionCount; // recalcSectionStartPos() or set sectionStartposRecalc to true
        };

        inline SectionItem()
            : size(0), srcSize(-1), resizeMode(GlodonHeaderView::Interactive) {}
        inline SectionItem(int length, GlodonHeaderView::ResizeMode mode)
            : size(length), srcSize(-1), resizeMode(mode), calculated_startpos(-1) {}

        inline int sectionSize() const
        {
            return size;
        }

        inline int calculatedEndPos() const
        {
            return calculated_startpos + size;
        }

#ifndef QT_NO_DATASTREAM
        inline void write(QDataStream &out) const
        {
            out << static_cast<int>(size);
            out << 1;
            out << (int)resizeMode;
        }

        inline void read(QDataStream &in)
        {
            int m;
            in >> m;
            size = m;
            in >> tmpDataStreamSectionCount;
            in >> m;
            resizeMode = m;
        }
#endif

    };

    QVector<SectionItem> sectionItems;

    void createSectionItems(int start, int end, int size, GlodonHeaderView::ResizeMode mode);
    void removeSectionsFromSectionItems(int start, int end);
    void resetGroupSectionSpans(int start, int end);
    void resizeSectionItem(int visualIndex, int oldSize, int newSize, bool isManual);
    void setDefaultSectionSize(int size);
    void recalcSectionStartPos() const; // not really const
    void resetAfterTreeBuild(const QVector<int> &showRows);
    void resetTreeSectionItems(const QVector<int> &showRows);
    int headerLength() const;    // for debugging
    int headerSectionSize(int visual) const;
    int headerSectionPosition(int visual) const;

    /**
     * @brief 根据绘制位置，返回行（列）号，不考虑滚动条的位置
     * @param position
     * @return
     */
    int headerVisualIndexAt(int position) const;

    // resize mode
    void setHeaderSectionResizeMode(int visual, GlodonHeaderView::ResizeMode mode);
    GlodonHeaderView::ResizeMode headerSectionResizeMode(int visual) const;
    void setGlobalHeaderResizeMode(GlodonHeaderView::ResizeMode mode);

    // other
    int viewSectionSizeHint(int logical) const;
    int adjustedVisualIndex(int visualIndex) const;
    void setScrollOffset(const QScrollBar *scrollBar, GlodonAbstractItemView::ScrollMode scrollMode);

    QRect viewportScrollArea();

    void resizeSectionItem(int newSize, SectionItem &oCurrentSectionItem);
    void excludeNotNeedHideSections(const QSet<int> &logicalIndexs, bool hide, QSet<QPair<int, int>> &oNeedHideLogicalAndVisualIndexs);

    void doBatchResizeSection(const QMap<int, int> &logicalIndexSizes, bool isInHiddenSection);

#ifndef QT_NO_DATASTREAM
    void write(QDataStream &out) const;
    bool read(QDataStream &in);
#endif
    bool m_drawBorder;          // 如果为treu则自绘左边线和上边线
    bool m_allowDoubleClicked;
    bool m_boldWithSelected;    // 当前列，行被选择时，字体是否加粗

    bool m_bLinearGradient;     // 表头背景是否渐变
    bool m_bFontShadow;         // 表头字体是否带有泛白阴影
    QColor m_clrFontShadow;     // 表头字体是否带有泛白阴影颜色
};

#endif // GLDHEADERVIEW_P_H
