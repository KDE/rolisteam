#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QPair>
#include <QWidget>

#include "convertoroperator.h"
#include "customrulemodel.h"
#include "rwidgets/gmtoolbox/gamemastertool.h"
#include "rwidgets_global.h"
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
class RWIDGET_EXPORT Convertor : public QWidget, public GameMasterTool
{
    Q_OBJECT
    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged FINAL)

public:
    explicit Convertor(QWidget* parent= 0);
    virtual ~Convertor();

    void readSettings();
    void writeSettings();
    bool modified() const;
    void setModified(bool newModified= true);

public slots:
    void categoryHasChanged(int i);
    void categoryHasChangedOnSecondPanel(int i);
    void convert();

signals:
    void modifiedChanged();

private:
    Ui::Convertor* ui= nullptr;
    QMap<Unit::Category, QString> m_map;
    std::unique_ptr<UnitModel> m_model= nullptr;
    std::unique_ptr<CategoryModel> m_catModel= nullptr;
    std::unique_ptr<CategoryModel> m_toModel= nullptr;
    std::unique_ptr<CustomRuleModel> m_customRulesModel= nullptr;

    QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*> m_convertorTable;
    bool m_modified{false};
};
} // namespace GMTOOL
#endif // CONVERTOR_H
