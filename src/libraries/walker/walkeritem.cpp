#include "walkeritem.h"

#include <QQuickWindow>

// SGNODE
QList<QPointer<QQuickItem>> WalkerItem::s_items;
namespace
{
constexpr auto triangleCount{8};
}
WalkerNode::~WalkerNode() {}
WalkerNode::WalkerNode()
{
    auto dimGeo= new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
    dimGeo->setDrawingMode(QSGGeometry::DrawTriangles);
    dimGeo->allocate(triangleCount * 3);

    m_dim.setGeometry(dimGeo);
    m_dim.setMaterial(&m_dimMat);
    m_dimMat.setColor(Qt::black);

    m_opacity.setOpacity(0.6);

    m_opacity.appendChildNode(&m_dim);
    appendChildNode(&m_opacity);

    markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyGeometry | QSGNode::DirtyOpacity);
}

void WalkerNode::updateOpacity(qreal opacity)
{
    m_opacity.setOpacity(opacity);
    m_opacity.markDirty(QSGNode::DirtyOpacity);
}

void WalkerNode::update(const QRectF& out, const QRectF& in)
{
    const auto a= out.topLeft();
    const auto b= in.topLeft();
    const auto c= in.topRight();
    const auto d= out.topRight();
    const auto e= in.bottomRight();
    const auto f= out.bottomRight();
    const auto g= in.bottomLeft();
    const auto h= out.bottomLeft();

    {
        auto gem= m_dim.geometry();
        auto vertices= gem->vertexDataAsPoint2D();
        QList<std::array<QPointF, 3>> triangles{{a, b, d}, {b, d, c}, {d, c, f}, {c, f, e},
                                                {f, e, h}, {e, g, h}, {h, g, a}, {g, a, b}};
        int i= 0;
        for(auto t : triangles)
        {
            vertices[i + 0].set(t[0].x(), t[0].y());
            vertices[i + 1].set(t[1].x(), t[1].y());
            vertices[i + 2].set(t[2].x(), t[2].y());
            i+= 3;
        }

        m_dim.markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    }

    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}
void WalkerNode::updateColor(const QColor& dim)
{
    m_dimMat.setColor(dim);
    m_dim.markDirty(QSGNode::DirtyMaterial);
}

// ITEM
WalkerItem::WalkerItem() : m_timer(new QTimer())
{
    setFlag(QQuickItem::ItemHasContents);
    setAntialiasing(true);

    connect(this, &WalkerItem::windowChanged, this,
            [this]()
            {
                auto w= window();
                if(w)
                {
                    connect(w, &QQuickWindow::widthChanged, this, &WalkerItem::updateComputation);
                    connect(w, &QQuickWindow::heightChanged, this, &WalkerItem::updateComputation);
                }
            });
    connect(this, &WalkerItem::dimColorChanged, this, &WalkerItem::updateComputation);
    connect(this, &WalkerItem::dimOpacityChanged, this, &WalkerItem::updateComputation);
    connect(this, &WalkerItem::currentChanged, this, &WalkerItem::updateComputation);
    connect(this, &WalkerItem::widthChanged, this, &WalkerItem::updateComputation);
    connect(this, &WalkerItem::heightChanged, this, &WalkerItem::updateComputation);
    connect(this, &WalkerItem::currentChanged, this, &WalkerItem::updateComputation);

    connect(m_timer.get(), &QTimer::timeout, this, &WalkerItem::next);
}

void WalkerItem::setCurrent(int current)
{
    updateHighLightItems();
    if(m_current == current || current >= m_highLightedItems.size() || current < 0)
        return;

    exit();
    m_current= current;
    enter();
    updateHighLightItems();
    emit currentChanged();
}

void WalkerItem::enter()
{
    auto wInfo= info(m_current);
    if(wInfo.attached && wInfo.item)
        emit wInfo.attached->enter();
}
void WalkerItem::exit()
{
    auto wInfo= info(m_current);
    if(wInfo.attached && wInfo.item)
        emit wInfo.attached->exit();
}

const WalkInfo WalkerItem::info(int index) const
{
    if(index < 0 || index >= m_highLightedItems.size())
        return {};
    return m_highLightedItems[index];
}

int WalkerItem::current() const
{
    return m_current;
}

void WalkerItem::updateHighLightItems()
{
    QList<QPointer<QQuickItem>> newItems;
    if(m_highLightedItems.empty())
        newItems= s_items;
    else
        std::copy_if(std::begin(s_items), std::end(s_items), std::back_inserter(newItems),
                     [this](const QPointer<QQuickItem>& d)
                     {
                         return std::end(m_highLightedItems)
                                == std::find_if(std::begin(m_highLightedItems), std::end(m_highLightedItems),
                                                [d](const WalkInfo& info) { return d == info.item; });
                     });

    for(const auto& child : std::as_const(newItems))
    {
        if(!child)
            continue;

        if(!child->isVisible())
            continue;

        WalkerAttachedType* attached
            = qobject_cast<WalkerAttachedType*>(qmlAttachedPropertiesObject<WalkerItem>(child, false));
        if(!attached)
        {
            continue;
        }

        m_highLightedItems.append({attached, child, attached->weight()});

        connect(child, &QQuickItem::widthChanged, this, &WalkerItem::updateComputation);
        connect(child, &QQuickItem::heightChanged, this, &WalkerItem::updateComputation);
        connect(child, &QQuickItem::xChanged, this, &WalkerItem::updateComputation);
        connect(child, &QQuickItem::yChanged, this, &WalkerItem::updateComputation);

        std::sort(std::begin(m_highLightedItems), std::end(m_highLightedItems),
                  [](const WalkInfo& a, const WalkInfo& b) { return a.weight < b.weight; });
    }
}

void WalkerItem::start()
{
    setCurrent(0);
    m_highLightedItems.clear();
    updateHighLightItems();
    setVisible(!m_highLightedItems.isEmpty());
    setActive(true);
    emit countChanged();
    emit currentChanged();
}

void WalkerItem::skip()
{
    setCurrent(0);
    m_highLightedItems.clear();
    setVisible(false);
    setActive(false);
    m_timer->stop();
    emit countChanged();
    emit currentChanged();
}

void WalkerItem::updateComputation()
{
    static auto computeArea= [](const QRectF& r) { return r.width() * r.height(); };

    auto item= info(m_current).item;
    if(!item)
    {
        setAvailableRect(QRectF());
        return;
    }

    auto rect= item->boundingRect();

    // Find real bounding rect
    {
        auto bg= item->property("background").value<QQuickItem*>();
        if(bg)
        {
            auto areaBg= computeArea(bg->boundingRect());
            auto areaItem= computeArea(rect);
            if(areaBg > areaItem)
                rect= bg->boundingRect();
        }
    }
    auto sPos= item->mapToScene(QPointF{0, 0});
    auto fPos= mapFromScene(sPos);

    m_targetRect.setRect(fPos.x(), fPos.y(), rect.width(), rect.height());

    // update available Rect
    auto big= boundingRect();
    const auto wMargin= m_targetRect.width() * 0.1;
    const auto hMargin= m_targetRect.height() * 0.1;
    auto targetMarged= m_targetRect - QMarginsF(wMargin, hMargin, wMargin, hMargin);

    std::array<QRectF, 3> availables{QRectF{big.topLeft(), QPointF{big.width(), fPos.y()}},
                                     QRectF{QPointF{big.x(), fPos.y() + m_targetRect.height()}, big.bottomRight()},
                                     targetMarged};

    auto max= std::max_element(std::begin(availables), std::end(availables),
                               [](const QRectF& a, const QRectF& b) { return computeArea(a) < computeArea(b); });

    setAvailableRect((*max));
    setBorderRect(m_targetRect);
    m_change|= WalkerItem::ChangeType::GeometryChanged;
    update();
}

QSGNode* WalkerItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
    auto wNode= static_cast<WalkerNode*>(node);
    if(!wNode)
    {
        wNode= new WalkerNode();
    }

    if(m_change & WalkerItem::ChangeType::ColorChanged)
        wNode->updateColor(m_dimColor);
    if(m_change & WalkerItem::ChangeType::GeometryChanged)
        wNode->update(boundingRect(), m_targetRect);
    if(m_change & WalkerItem::ChangeType::OpacityChanged)
        wNode->updateOpacity(m_dimOpacity);

    m_change= WalkerItem::ChangeType::NoChanges;
    return wNode;
}

void WalkerItem::next()
{
    auto c= m_current;
    do
    {
        setCurrent(m_current + 1);
    } while(!currentIsVisible());
    if(c == m_current)
        m_timer->stop();
}

bool WalkerItem::currentIsVisible()
{
    auto wInfo= info(m_current);
    return wInfo.item ? wInfo.item->isVisible() : false;
}

void WalkerItem::previous()
{
    do
    {
        setCurrent(m_current - 1);
    } while(!currentIsVisible());
}

int WalkerItem::count() const
{
    return m_highLightedItems.size();
}

QString WalkerItem::currentDesc() const
{
    auto att= info(m_current).attached;
    return att ? att->description() : QString();
}

QColor WalkerItem::dimColor() const
{
    return m_dimColor;
}

void WalkerItem::setDimColor(const QColor& newDimColor)
{
    if(m_dimColor == newDimColor)
        return;
    m_dimColor= newDimColor;
    emit dimColorChanged();
}

qreal WalkerItem::dimOpacity() const
{
    return m_dimOpacity;
}

void WalkerItem::setDimOpacity(qreal newDimOpacity)
{
    if(qFuzzyCompare(m_dimOpacity, newDimOpacity))
        return;
    m_dimOpacity= newDimOpacity;
    emit dimOpacityChanged();
}

QRectF WalkerItem::availableRect() const
{
    return m_availableRect;
}

void WalkerItem::setAvailableRect(const QRectF& newAvailableRect)
{
    if(m_availableRect == newAvailableRect)
        return;
    m_availableRect= newAvailableRect;
    emit availableRectChanged();
}

void WalkerItem::setBorderRect(const QRectF& borderRect)
{
    if(m_borderRect == borderRect)
        return;
    m_borderRect= borderRect;
    emit borderRectChanged();
}

QRectF WalkerItem::borderRect() const
{
    return m_borderRect;
}

int WalkerItem::interval() const
{
    return m_interval;
}

void WalkerItem::setInterval(int newInterval)
{
    if(m_interval == newInterval)
        return;
    m_interval= newInterval;
    emit intervalChanged();
}

bool WalkerItem::active() const
{
    return m_active;
}

void WalkerItem::setActive(bool newActive)
{
    if(m_active == newActive)
        return;
    m_active= newActive;
    emit activeChanged();
}
