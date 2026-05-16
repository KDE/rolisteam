#include "model/filteredcharactermodel.h"

namespace campaign
{

template <typename T>
bool isNull(const T& value);

template <>
bool isNull(const QPixmap& value)
{
    return value.isNull();
}
template <>
bool isNull(const QString& value)
{
    return value.isEmpty();
}

template <>
bool isNull(const int& value)
{
    return value == 0;
}

template <>
bool isNull(const bool& value)
{
    return !value;
}

template <typename T>
bool isAccepted(FilteredCharacterModel::Definition def, T value)
{
    if(def == FilteredCharacterModel::All)
    {
        return true;
    }
    else if(def == FilteredCharacterModel::With)
    {
        return !isNull(value);
    }
    else
    {
        return isNull(value);
    }
}

FilteredCharacterModel::FilteredCharacterModel()
{
    setDynamicSortFilter(true);

    // TODO change it
    connect(this, &FilteredCharacterModel::searchChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::initiativeCmdDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::propertiesDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::shapeDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::actionDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::initiativeScoreDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::avatarDefinitionChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::advancedChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::excludeChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::tagsChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::excludeTagsChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::hlStateChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::characterStateIdChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::gmdetailsDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::gmdetailsChanged, this, &FilteredCharacterModel::invalidateFilter);
}

FilteredCharacterModel::Definition FilteredCharacterModel::initiativeCmdDef() const
{
    return m_initiativeCmdDef;
}

void FilteredCharacterModel::setInitiativeCmdDef(FilteredCharacterModel::Definition newInitiativeCmdDef)
{
    if(m_initiativeCmdDef == newInitiativeCmdDef)
        return;
    m_initiativeCmdDef= newInitiativeCmdDef;
    emit initiativeCmdDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::propertiesDef() const
{
    return m_propertiesDef;
}

void FilteredCharacterModel::setPropertiesDef(Definition newPropertiesDef)
{
    if(m_propertiesDef == newPropertiesDef)
        return;
    m_propertiesDef= newPropertiesDef;
    emit propertiesDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::shapeDef() const
{
    return m_shapeDef;
}

void FilteredCharacterModel::setShapeDef(FilteredCharacterModel::Definition newShapeDef)
{
    if(m_shapeDef == newShapeDef)
        return;
    m_shapeDef= newShapeDef;
    emit shapeDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::actionDef() const
{
    return m_actionDef;
}

void FilteredCharacterModel::setActionDef(FilteredCharacterModel::Definition newActionDef)
{
    if(m_actionDef == newActionDef)
        return;
    m_actionDef= newActionDef;
    emit actionDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::initiativeScoreDef() const
{
    return m_initiativeScoreDef;
}

void FilteredCharacterModel::setInitiativeScoreDef(FilteredCharacterModel::Definition newInitiativeScoreDef)
{
    if(m_initiativeScoreDef == newInitiativeScoreDef)
        return;
    m_initiativeScoreDef= newInitiativeScoreDef;
    emit initiativeScoreDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::avatarDefinition() const
{
    return m_avatarDefinition;
}

void FilteredCharacterModel::setAvatarDefinition(FilteredCharacterModel::Definition newAvatarDefinition)
{
    if(m_avatarDefinition == newAvatarDefinition)
        return;
    m_avatarDefinition= newAvatarDefinition;
    emit avatarDefinitionChanged();
}

bool FilteredCharacterModel::advanced() const
{
    return m_advanced;
}

void FilteredCharacterModel::setAdvanced(bool newAdvanced)
{
    if(m_advanced == newAdvanced)
        return;
    m_advanced= newAdvanced;
    emit advancedChanged();
}

const QString& FilteredCharacterModel::exclude() const
{
    return m_exclude;
}

void FilteredCharacterModel::setExclude(const QString& newExclude)
{
    if(m_exclude == newExclude)
        return;
    m_exclude= newExclude;
    emit excludeChanged();
}

const QString& FilteredCharacterModel::tags() const
{
    return m_tags;
}

void FilteredCharacterModel::setTags(const QString& newTags)
{
    if(m_tags == newTags)
        return;
    m_tags= newTags;
    emit tagsChanged();
}

const QString& FilteredCharacterModel::excludeTags() const
{
    return m_excludeTags;
}

void FilteredCharacterModel::setExcludeTags(const QString& newExcludeTags)
{
    if(m_excludeTags == newExcludeTags)
        return;
    m_excludeTags= newExcludeTags;
    emit excludeTagsChanged();
}

QString FilteredCharacterModel::search() const
{
    return m_search;
}

void FilteredCharacterModel::setSearch(const QString& search)
{
    if(search == m_search)
        return;
    m_search= search;
    emit searchChanged();
}

bool FilteredCharacterModel::isDefaultFilter() const
{
    bool res= m_search.isEmpty();
    if(advanced() && res)
    {
        res&= m_initiativeCmdDef == All;
        res&= m_propertiesDef == All;
        res&= m_shapeDef == All;
        res&= m_actionDef == All;
        res&= m_initiativeScoreDef == All;
        res&= m_avatarDefinition == All;
        res&= m_gmdetailsDef == All;
        res&= m_exclude.isEmpty();
        res&= m_tags.isEmpty();
        res&= m_excludeTags.isEmpty();
        res&= m_gmdetails.isEmpty();
        res&= m_hlState == HS_All;
        res&= m_characterStateId.isEmpty();
    }
    return res;
}

bool FilteredCharacterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    using nm= NonPlayableCharacterModel;
    auto nameIndex= sourceModel()->index(source_row, 0, source_parent);
    auto name= sourceModel()->data(nameIndex, nm::RoleName).toString();
    auto tagList= sourceModel()->data(nameIndex, nm::RoleTags).toStringList();
    auto description= sourceModel()->data(nameIndex, nm::RoleDescription).toString();

    if(isDefaultFilter())
        return true;

    if(advanced())
    {
        auto gmdetails= sourceModel()->data(nameIndex, nm::RoleGmDetails).toString();

        if(!(m_search.isEmpty() ?
                 true :
                 (name.contains(m_search, Qt::CaseInsensitive) || description.contains(m_search, Qt::CaseInsensitive))))
            return false;
        // if(!(m_search.isEmpty() ? true : )))
        //            return false;
        if(!(m_tags.isEmpty() ? true : tagList.join(QString()).contains(m_tags, Qt::CaseInsensitive)))
            return false;
        if((m_excludeTags.isEmpty() ? false : tagList.join(QString()).contains(m_excludeTags, Qt::CaseInsensitive)))
            return false;
        if(!isAccepted<bool>(m_initiativeScoreDef, sourceModel()->data(nameIndex, nm::RoleHasInitiative).toBool()))
            return false;
        if(!isAccepted<QString>(m_initiativeCmdDef, sourceModel()->data(nameIndex, nm::RoleInitCommand).toString()))
            return false;
        if(!isAccepted<int>(m_actionDef, sourceModel()->data(nameIndex, nm::RoleActionCount).toInt()))
            return false;
        if(!isAccepted<int>(m_propertiesDef, sourceModel()->data(nameIndex, nm::RolePropertiesCount).toInt()))
            return false;
        if(!isAccepted<int>(m_shapeDef, sourceModel()->data(nameIndex, nm::RoleShapeCount).toInt()))
            return false;
        if(!isAccepted<QString>(m_gmdetailsDef, gmdetails))
            return false;
        if(!(m_characterStateId.isEmpty() ?
                 true :
                 (m_characterStateId == sourceModel()->data(nameIndex, nm::RoleState).toString())))
            return false;

        auto current= sourceModel()->data(nameIndex, nm::RoleCurrentHp).toInt();
        auto min= sourceModel()->data(nameIndex, nm::RoleMinHp).toInt();
        auto max= sourceModel()->data(nameIndex, nm::RoleMaxHp).toInt();
        auto health= m_hlState == FilteredCharacterModel::HS_All  ? true :
                     m_hlState == FilteredCharacterModel::HS_Full ? current == max :
                     m_hlState == FilteredCharacterModel::HS_Dead ? min == current :
                                                                    (current > min && current < max);
        if(!health)
            return false;
        if(!(m_gmdetails.isEmpty() ? true : gmdetails.contains(m_gmdetails, Qt::CaseInsensitive)))
            return false;
        if((m_exclude.isEmpty() ? false : name.contains(m_exclude, Qt::CaseInsensitive)))
            return false;
        if(!isAccepted<QPixmap>(m_avatarDefinition, sourceModel()->data(nameIndex, nm::RoleAvatar).value<QPixmap>()))
            return false;
        return true;
    }
    else
    {
        auto nameVal= m_search.isEmpty() ? true : name.contains(m_search, Qt::CaseInsensitive);
        auto tags= m_search.isEmpty() ? true : tagList.join(QString()).contains(m_search, Qt::CaseInsensitive);
        auto descVal= m_search.isEmpty() ? true : description.contains(m_search, Qt::CaseInsensitive);
        return (nameVal || tags || descVal);
    }
}

bool FilteredCharacterModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    auto data1= source_left.data().toString();
    auto data2= source_right.data().toString();

    return data1 < data2;
}

QString FilteredCharacterModel::characterStateId() const
{
    return m_characterStateId;
}

void FilteredCharacterModel::setCharacterStateId(const QString& newCharaceterStateId)
{
    if(m_characterStateId == newCharaceterStateId)
        return;
    m_characterStateId= newCharaceterStateId;
    emit characterStateIdChanged();
}

} // namespace campaign
