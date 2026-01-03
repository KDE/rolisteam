#ifndef WALKERITEM_H
#define WALKERITEM_H

#include <QColor>
#include <QQuickItem>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGOpacityNode>
#include <QSGSimpleRectNode>
#include <QTimer>

#include "walkerattachedtype.h"
// #include <QTimer>

class WalkerNode : public QSGNode
{
public:
    WalkerNode();
    virtual ~WalkerNode();
    void update(const QRectF& outRect, const QRectF& inRect);
    void updateColor(const QColor& dim);
    void updateOpacity(qreal opacity);

private:
    QSGOpacityNode m_opacity;
    QSGFlatColorMaterial m_dimMat;
    QSGGeometryNode m_dim;
};

struct WalkInfo
{
    QPointer<WalkerAttachedType> attached;
    QPointer<QQuickItem> item;
    int weight;
};

class WalkerItem : public QQuickItem
{
    Q_OBJECT
    QML_ATTACHED(WalkerAttachedType)
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int current READ current NOTIFY currentChanged FINAL)
    Q_PROPERTY(QString currentDesc READ currentDesc NOTIFY currentChanged FINAL)
    Q_PROPERTY(QColor dimColor READ dimColor WRITE setDimColor NOTIFY dimColorChanged FINAL)
    Q_PROPERTY(qreal dimOpacity READ dimOpacity WRITE setDimOpacity NOTIFY dimOpacityChanged FINAL)
    Q_PROPERTY(QRectF availableRect READ availableRect NOTIFY availableRectChanged FINAL)
    Q_PROPERTY(QRectF borderRect READ borderRect NOTIFY borderRectChanged FINAL)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)
public:
    enum class ChangeType
    {
        NoChanges= 0x0,
        GeometryChanged= 0x1,
        OpacityChanged= 0x2,
        ColorChanged= 0x4,
    };
    Q_DECLARE_FLAGS(Changes, ChangeType)
    Q_FLAG(Changes)
    WalkerItem();

    int current() const;
    int count() const;

    QString currentDesc() const;

    QColor dimColor() const;
    void setDimColor(const QColor& newDimColor);

    qreal dimOpacity() const;
    void setDimOpacity(qreal newDimOpacity);

    QRectF availableRect() const;
    QRectF borderRect() const;

    static WalkerAttachedType* qmlAttachedProperties(QObject* object)
    {
        QQuickItem* item= qobject_cast<QQuickItem*>(object);
        if(!item)
            qDebug() << "Walker must be attached to an Item";
        s_items.append(item);
        return new WalkerAttachedType(object);
    }
    static QList<QPointer<QQuickItem>> s_items;
    int interval() const;
    void setInterval(int newInterval);

    bool active() const;

public slots:
    void next();
    void previous();
    void start();
    void skip();

signals:
    void currentChanged();
    void countChanged();
    void dimColorChanged();
    void dimOpacityChanged();
    void availableRectChanged();
    void borderRectChanged();
    void intervalChanged();
    void activeChanged();

protected:
    void setActive(bool newActive);
    void updateHighLightItems();
    void setAvailableRect(const QRectF& newAvailableRect);
    void setBorderRect(const QRectF& borderRect);
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    void setCurrent(int current);
    const WalkInfo info(int index) const;
    void updateComputation();
    void enter();
    void exit();
    bool currentIsVisible();

private:
    QList<WalkInfo> m_highLightedItems;
    int m_current{0};
    int m_count{0};
    QString m_currentDesc;
    Changes m_change{ChangeType::NoChanges};
    QColor m_dimColor;
    qreal m_dimOpacity{0.6};
    QRectF m_availableRect;
    QRectF m_targetRect;
    QRectF m_borderRect;
    int m_interval{-1};
    std::unique_ptr<QTimer> m_timer;
    bool m_active{false};
};

#endif // WALKERITEM_H
