/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "controller/item_controllers/sightcontroller.h"

#include <QDebug>
#include <QPolygonF>

// #include "controller/item_controllers/characteritemcontrollermanager.h"
#include "controller/view_controller/vectorialmapcontroller.h"

namespace vmap
{
/////////////////////////////
SightController::SightController(VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::SIGHT, std::map<QString, QVariant>(), ctrl, parent)
{
    // constructor
    setUuid("sightController");

    connect(m_ctrl, &VectorialMapController::visibilityChanged, this,
            [this]() { setVisible(m_ctrl->visibility() == Core::VisibilityMode::FOGOFWAR); });
    connect(m_ctrl, &VectorialMapController::characterVisionChanged, this, &SightController::setCharacterSight);

    connect(m_ctrl, &VectorialMapController::visualRectChanged, this, &vmap::SightController::rectChanged);
    setVisible(m_ctrl->visibility() == Core::VisibilityMode::FOGOFWAR);

    connect(this, &SightController::characterSightChanged, this, [this] { setModified(); });
    connect(this, &SightController::fowPathChanged, this, [this] { setModified(); });
    connect(this, &SightController::rectChanged, this, [this] { setModified(); });
    connect(this, &SightController::characterCountChanged, this, [this] { setModified(); });
    setEditable(false);
    setInitialized(true);

    connect(this, &SightController::blockUChanged, this,
            [this]()
            {
                if(!m_blockUpdate)
                    emit fowPathChanged();
            });
}

void SightController::aboutToBeRemoved()
{
    emit removeItem();
}

const QList<QPointer<CharacterVision>>& SightController::visionData() const
{
    return m_visions;
}

void SightController::setCorner(const QPointF&, int, Core::TransformType) {}

void SightController::endGeometryChange() {}

int SightController::fowItemCount() const
{
    return static_cast<int>(m_fogSingularityList.size());
}

int SightController::characterCount() const
{
    return m_visions.count();
}

bool SightController::characterSight() const
{
    return m_characterSight;
}

QRectF SightController::rect() const
{
    auto rect= m_ctrl->visualRect();
    rect= rect.united(m_rect);
    return rect;
}

void SightController::setRect(const QRectF& rect)
{
    if(m_rect == rect)
        return;

    m_rect= rect;
    emit rectChanged(m_rect);
}

void SightController::setCharacterSight(bool b)
{
    if(b == m_characterSight)
        return;
    m_characterSight= b;
    emit characterSightChanged();
}

void SightController::setFowPath(const QPainterPath& path)
{
    if(m_remoteFowPath == path)
        return;
    m_remoteFowPath= path;
    emit fowPathChanged();
}

QPainterPath SightController::fowPath() const
{
    QPainterPath path;
    if(remote())
    {
        path.addRect(rect());
        path= path.intersected(m_remoteFowPath);
        auto bRect= m_remoteFowPath.boundingRect();
        auto cRect= rect();
        if(cRect.height() > bRect.height())
        {
            QPainterPath subPoly;
            subPoly.addRect(QRectF(0, bRect.height(), cRect.width(), cRect.height() - bRect.height()));
            path= path.united(subPoly);
        }

        if(cRect.width() > bRect.width())
        {
            QPainterPath subPoly;
            subPoly.addRect(QRectF(bRect.width(), 0, cRect.width() - bRect.width(), cRect.height()));
            path= path.united(subPoly);
        }
    }
    else
    {
        path.addRect(rect());
        for(auto const& fogs : m_fogSingularityList)
        {
            QPainterPath subPoly;
            subPoly.addPolygon(fogs.first);
            path= fogs.second ? path.united(subPoly) : path.subtracted(subPoly);
        }

        for(auto it= m_tempPolygons.cbegin(); it != m_tempPolygons.cend(); ++it)
        {
            QPainterPath subPoly;
            subPoly.addPolygon(it.value().first);
            path= it.value().second ? path.united(subPoly) : path.subtracted(subPoly);
        }
    }
    return path;
}

void SightController::addPolygon(const QPolygonF& poly, bool mask, bool temp)
{
    // Legacy path used by character vision and old callers.
    // Temp polygons are stored under a fixed key so they don't collide
    // with per-light entries inserted via setLightPolygon().
    if(temp)
    {
        m_tempPolygons.insert(QStringLiteral("__legacy__"), std::make_pair(poly, mask));
    }
    else
    {
        m_fogSingularityList.push_back(std::make_pair(poly, mask));
        // A permanent fog change invalidates any legacy temp entry,
        // but must NOT clear per-light entries — lights are still present.
        m_tempPolygons.remove(QStringLiteral("__legacy__"));
    }

    if(!m_blockUpdate)
        emit fowPathChanged();
}

void SightController::setLightPolygon(const QString& lightId, const QPolygonF& poly, bool mask)
{
    m_tempPolygons.insert(lightId, std::make_pair(poly, mask));
    if(!m_blockUpdate)
        emit fowPathChanged();
}

QMap<QString, std::pair<QPolygonF, bool>> SightController::tempPolygons() const
{
    return m_tempPolygons;
}

void SightController::addCharacterVision(CharacterVision* vision)
{
    connect(vision, &CharacterVision::angleChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::radiusChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::positionChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::shapeChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::rotationChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::sideChanged, this, &SightController::requiredUpdate);
    connect(vision, &CharacterVision::removedChanged, this, &SightController::requiredUpdate);
    m_visions.push_back(vision);
    emit characterCountChanged();
}

void SightController::removeCharacterVision(CharacterVision* vision)
{
    connect(vision, 0, this, 0);
    if(m_visions.removeOne(vision))
        emit characterCountChanged();
}

void SightController::removeLightPolygon(const QString& lightId)
{
    Q_ASSERT(!m_blockUpdate);
    if(m_tempPolygons.remove(lightId) > 0)
        emit fowPathChanged();
}

void SightController::clearTempPolygons()
{
    // Only clears the legacy single-vision entry, not per-light entries.
    // Call removeLightPolygon(uuid) to remove a specific light.
    if(!m_tempPolygons.contains(QStringLiteral("__legacy__")))
        return;
    m_tempPolygons.remove(QStringLiteral("__legacy__"));
    emit fowPathChanged();
}

const std::vector<std::pair<QPolygonF, bool>>& SightController::singularityList() const
{
    return m_fogSingularityList;
}

bool SightController::blockU() const
{
    return m_blockUpdate;
}

void SightController::setBlockU(bool newBlockUpdate)
{
    if(m_blockUpdate == newBlockUpdate)
        return;
    m_blockUpdate= newBlockUpdate;
    emit blockUChanged();
}

QPointF SightController::transformOrigin() const
{
    return m_rect.center();
}

} // namespace vmap
