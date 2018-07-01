#include "unitmodel.h"
#include <QDebug>
namespace GMTOOL
{
QHash<Unit::Category,QString> UnitModel::m_cat2Text({{Unit::CURRENCY,tr("Currency")},
                                                     {Unit::DISTANCE,tr("Distance")},
                                                     {Unit::TEMPERATURE,tr("Temperature")},
                                                    {Unit::MASS,tr("MASS")},
                                                    {Unit::MASS,tr("OTHER")}});

                                                     //{Unit::VOLUME,tr("Volume")},

CategoryModel::CategoryModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterRole(Qt::UserRole);
    setFilterKeyColumn(0);
    setDynamicSortFilter(true);
}

void CategoryModel::addUnit(Unit* unit)
{
    beginInsertRows(QModelIndex(),rowCount(),rowCount());
    auto unitModel = dynamic_cast<UnitModel*>(sourceModel());
    unitModel->insertData(unit);
    endInsertRows();

    /*if(insertRow(rowCount()))
    {
    }*/

}

QString CategoryModel::currentCategory() const
{
    return m_currentCategory;
}

void CategoryModel::setCurrentCategory(const QString &currentCategory)
{
    m_currentCategory = currentCategory;
    invalidateFilter();
    setFilterFixedString(currentCategory);
}
bool CategoryModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

    return (index0.data(Qt::UserRole).toString()==m_currentCategory);
}

UnitModel::UnitModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

Unit* UnitModel::indexToUnit(const QModelIndex& index) const
{
    auto r = index.row();
    for(auto list : m_data)
    {
        if(r>=list.size())
        {
            r-=list.size();
            continue;
        }
        else
        {
            return list.at(r);
        }
    }
    return nullptr;
}

QVariant UnitModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto unit = indexToUnit(index);
    if(nullptr == unit)
        return {};
    if(role == Qt::DisplayRole)
    {
        return unit->name();
    }
    else if(role == Category)
    {
        return getCatNameFromId(unit->currentCat());
    }
    else if(role == UnitRole)
    {
        return QVariant::fromValue(unit);
    }

    return QVariant();
}

int UnitModel::rowCount(const QModelIndex&) const
{
    qDebug() << "data count total " << m_data.values().size();
    int sum = 0;
    for(auto list : m_data)
    {
        sum += list.size();
    }
    return sum;
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

Unit* UnitModel::insertData(Unit *unit)
{
    auto& list = m_data[unit->currentCat()];
    beginInsertRows(index(unit->currentCat(),0),list.size(),list.size());
    list.append(unit);
    endInsertRows();
    return unit;
}

Unit *UnitModel::getUnitFromIndex(const QModelIndex &i, int currentCat)
{
    if(!i.isValid())
        return nullptr;
    auto list = m_data.value(static_cast<Unit::Category>(currentCat));
    return list.at(i.row());
}

QHash<Unit::Category, QString> UnitModel::cat2Text() const
{
    return m_cat2Text;
}

void UnitModel::setCat2Text(const QHash<Unit::Category, QString> &cat2Text)
{
    m_cat2Text = cat2Text;
}

QString UnitModel::getCatNameFromId(Unit::Category id) const
{
    return m_cat2Text[id];
}

bool UnitModel::insertUnit(Unit::Category cat)
{
    int sum=0;
    for(auto key:m_data.keys())
    {
        const auto& list = m_data[key];
        if(key <= cat)
        {
            sum += list.size();
        }
    }
    auto& list = m_data[cat];

    beginInsertRows(QModelIndex(), sum, sum);
    Unit* unit = new Unit(tr("New Unit"),"",cat);
    list.append(unit);
    endInsertRows();

    return true;
}

}

