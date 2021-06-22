#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QPair>
#include <QWidget>

#include "convertoroperator.h"
#include "customrulemodel.h"
#include "rwidgets/gmtoolbox/gamemastertool.h"
#include "unit.h"
#include "unitmodel.h"
#include <QSettings>
#include <memory>

namespace Ui
{
class Convertor;
}
namespace GMTOOL
{
/**
 * @page Convertor Unit Convertor
 *
 * @section Intro Introduction
 * Convertor provides convertion for any kind of unit. This tool is dedicated to GM to convert value from
 * books.<br/>
 *
 * @section unit Supported Units:
 * @subsection distance Distance Units:
 * @subsection temp Temperature Units:
 * @subsection currency Currency Units:
 *
 *
 */

class Convertor : public QWidget, public GameMasterTool
{
    Q_OBJECT

public:
    explicit Convertor(QWidget* parent= 0);
    virtual ~Convertor();

    void readSettings();
    void writeSettings();
public slots:
    void categoryHasChanged(int i);
    void categoryHasChangedOnSecondPanel(int i);
    void convert();

private:
    Ui::Convertor* ui= nullptr;
    QMap<Unit::Category, QString> m_map;
    std::unique_ptr<UnitModel> m_model= nullptr;
    std::unique_ptr<CategoryModel> m_catModel= nullptr;
    std::unique_ptr<CategoryModel> m_toModel= nullptr;
    std::unique_ptr<CustomRuleModel> m_customRulesModel= nullptr;

    QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*> m_convertorTable;
};
} // namespace GMTOOL
#endif // CONVERTOR_H
