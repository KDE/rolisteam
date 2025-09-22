#include "dicemaincontroller.h"
#include <QScreen>
#include <QtConcurrent>

DiceMainController::DiceMainController(QObject* parent)
    : QObject{parent}, m_model{new RollModel}, m_parser{new DiceParser}, m_dice3DCtrl{new Dice3DController}
{
}

DiceMainController::Page DiceMainController::currentPage() const
{
    return m_currentPage;
}

void DiceMainController::setCurrentPage(DiceMainController::Page newCurrentPage)
{
    if(m_currentPage == newCurrentPage)
        return;
    m_currentPage= newCurrentPage;
    emit currentPageChanged();
}

RollModel* DiceMainController::model() const
{
    return m_model.get();
}

int DiceMainController::profileCount() const
{
    return m_profileCount;
}

void DiceMainController::runCommand(const QString& cmd)
{
    auto it= QtConcurrent::run(
        [this, cmd]()
        {
            if(!m_parser->parseLine(cmd))
            {
                setErrorHumanReadable(m_parser->humanReadableError());
                return;
            }

            m_parser->start();

            auto error= m_parser->humanReadableError();
            if(!error.isEmpty())
            {
                setErrorHumanReadable(m_parser->humanReadableError());
                return;
            }

            QByteArray result
                = m_parser
                      ->resultAsJSon(
                          [](const QString& value, const QString& color, bool highlight)
                          {
                              QString resultTmp= value;
                              bool hasColor= !color.isEmpty();
                              QString style;
                              if(hasColor)
                              {
                                  style+= QStringLiteral("color:%1;").arg(color);
                              }
                              if(highlight)
                              {
                                  if(style.isEmpty())
                                      style
                                          += QStringLiteral("color:%1;")
                                                 .arg(
                                                     "red"); // default color must get the value from the setting object
                                  style+= QStringLiteral("font-weight:bold;");
                              }
                              if(!style.isEmpty())
                                  resultTmp= QString("<span style=\"%2\">%1</span>").arg(value, style);
                              return resultTmp;
                          })
                      .toUtf8();

            QMetaObject::invokeMethod(m_model.get(), &RollModel::addRoll, result);
        });
}

QString DiceMainController::errorHumanReadable() const
{
    return m_errorHumanReadable;
}

void DiceMainController::setErrorHumanReadable(const QString& newErrorHumanReadable)
{
    if(m_errorHumanReadable == newErrorHumanReadable)
        return;
    m_errorHumanReadable= newErrorHumanReadable;
    emit errorHumanReadableChanged();
}

Dice3DController* DiceMainController::dice3dCtrl() const
{
    return m_dice3DCtrl.get();
}

QSize DiceMainController::dice3dSize() const
{
    return m_dice3DCtrl->size();
}

void DiceMainController::setDice3dSize(const QSize& newDice3dSize)
{
    m_dice3DCtrl->setSize(newDice3dSize);
}
