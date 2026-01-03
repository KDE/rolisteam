#include "walkerattachedtype.h"

WalkerAttachedType::WalkerAttachedType(QObject* parent) : QObject{parent} {}

QString WalkerAttachedType::description() const
{
    return m_description;
}

void WalkerAttachedType::setDescription(const QString& newDescription)
{
    if(m_description == newDescription)
        return;
    m_description= newDescription;
    emit descriptionChanged();
}

int WalkerAttachedType::weight() const
{
    return m_weight;
}

void WalkerAttachedType::setWeight(int newWeight)
{
    if(m_weight == newWeight)
        return;
    m_weight= newWeight;
    emit weightChanged();
}
