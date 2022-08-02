#include "qmlgeneratorcontroller.h"

#include <QFontDatabase>
#include <QJsonArray>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QTemporaryFile>

#include "charactercontroller.h"
#include "charactersheet/rolisteamimageprovider.h"
#include "codeeditor.h"
#include "imagecontroller.h"

QmlGeneratorController::QmlGeneratorController(QObject* parent)
    : QObject(parent), m_model(new FieldModel), m_mockCharacter(new MockCharacter)
{
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this, [this](const QString& name) {
        emit reportLog(tr("The character value %1 has been defined to %2")
                           .arg(name)
                           .arg(m_mockCharacter->property(name.toStdString().c_str()).toString()),
                       LogController::Features);
    });
    connect(m_mockCharacter.get(), &MockCharacter::log, this,
            [this](const QString& log) { emit reportLog(log, LogController::Features); });

    connect(m_model.get(), &FieldModel::modelChanged, this, [this]() {
        emit sectionChanged(m_model->getRootSection());
        emit dataChanged();
    });
}

QString QmlGeneratorController::headCode() const
{
    return m_headCode;
}

QString QmlGeneratorController::bottomCode() const
{
    return m_bottomCode;
}

QString QmlGeneratorController::importCode() const
{
    return m_importCode;
}

bool QmlGeneratorController::flickable() const
{
    return m_flickableSheet;
}

qreal QmlGeneratorController::fixedScale() const
{
    return m_fixedScale;
}

bool QmlGeneratorController::textEdited() const
{
    return m_textEdited;
}
void QmlGeneratorController::setHeadCode(QString headCode)
{
    if(m_headCode == headCode)
        return;

    m_headCode= headCode;
    emit headCodeChanged(m_headCode);
    emit dataChanged();
}

void QmlGeneratorController::setBottomCode(QString bottomCode)
{
    if(m_bottomCode == bottomCode)
        return;

    m_bottomCode= bottomCode;
    emit bottomCodeChanged(m_bottomCode);
    emit dataChanged();
}

void QmlGeneratorController::setImportCode(QString importCode)
{
    if(m_importCode == importCode)
        return;

    m_importCode= importCode;
    emit importCodeChanged(m_importCode);
    emit dataChanged();
}

void QmlGeneratorController::setFlickable(bool flickable)
{
    if(m_flickableSheet == flickable)
        return;

    m_flickableSheet= flickable;
    emit flickableChanged(m_flickableSheet);
    emit dataChanged();
}

void QmlGeneratorController::setFixedScale(qreal fixedScale)
{
    qWarning("Floating point comparison needs context sanity check");
    if(qFuzzyCompare(m_fixedScale, fixedScale))
        return;

    m_fixedScale= fixedScale;
    emit fixedScaleChanged(m_fixedScale);
    emit dataChanged();
}

void QmlGeneratorController::setLastPageId(unsigned int pageId)
{
    m_lastPageId= pageId;
}

QStringList QmlGeneratorController::fonts() const
{
    return m_fonts;
}

void QmlGeneratorController::setTextEdited(bool t)
{
    if(t == m_textEdited)
        return;
    m_textEdited= t;
    emit textEditedChanged();
    emit dataChanged();
}
void QmlGeneratorController::setFonts(QStringList fonts)
{
    if(m_fonts == fonts)
        return;

    m_fonts= fonts;
    emit fontsChanged(m_fonts);
    emit dataChanged();
}

FieldModel* QmlGeneratorController::fieldModel() const
{
    return m_model.get();
}

MockCharacter* QmlGeneratorController::mockCharacter() const
{
    return m_mockCharacter.get();
}

void QmlGeneratorController::clearData()
{
    m_headCode= "";
    m_importCode= "";
    m_fixedScaleSheet= 1.0;
    m_bottomCode= "";
    m_flickableSheet= false;
    m_fonts.clear();

    m_model->clearModel();

    m_mockCharacter.reset(new MockCharacter);
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this, [this](const QString& name) {
        emit reportLog(tr("The character value %1 has been defined to %2")
                           .arg(name)
                           .arg(m_mockCharacter->property(name.toStdString().c_str()).toString()),
                       LogController::Features);
    });
    connect(m_mockCharacter.get(), &MockCharacter::log, this,
            [this](const QString& log) { emit reportLog(log, LogController::Features); });
}

QString QmlGeneratorController::uuidCharacter() const
{
    return m_uuidCharacter;
}

void QmlGeneratorController::setUuidCharacter(QString uuidCharacter)
{
    if(m_uuidCharacter == uuidCharacter)
        return;

    m_uuidCharacter= uuidCharacter;
    emit uuidCharacterChanged(m_uuidCharacter);
}

void QmlGeneratorController::generateQML(const ImageController* ctrl)
{
    QString data;
    QTextStream text(&data);
    QSize size= ctrl->backgroundSize();
    qreal ratio= 1;
    qreal ratioBis= 1;
    bool hasImage= false;

    if(size.isValid())
    {
        ratio= static_cast<qreal>(size.width()) / static_cast<qreal>(size.height());
        ratioBis= static_cast<qreal>(size.height()) / static_cast<qreal>(size.width());
        hasImage= true;
    }

    QString key= ctrl->uuid();
    QStringList keyParts= key.split('_');
    if(!keyParts.isEmpty())
    {
        key= keyParts[0];
    }
    text << "import QtQuick\n";
    text << "import QtQuick.Layouts\n";
    text << "import QtQuick.Controls\n";
    text << "import Rolisteam\n";

    if(!m_importCode.isEmpty())
    {
        text << "   " << m_importCode << "\n";
    }
    text << "\n";
    if(m_flickableSheet)
    {
        text << "Flickable {\n";
        text << "    id:root\n";
        if(hasImage)
            text << "    contentWidth: imagebg.width;\n    contentHeight: imagebg.height;\n";
        text << "    boundsBehavior: Flickable.StopAtBounds;\n";
    }
    else
    {
        text << "Item {\n";
        text << "    id:root\n";
    }
    if(hasImage)
    {
        text << "    property alias realscale: imagebg.realscale\n";
    }
    text << "    focus: true\n";
    text << "    property int page: 0\n";
    text << "    property int maxPage:" << m_lastPageId << "\n";
    text << "    onPageChanged: {\n";
    text << "        page=page>maxPage ? maxPage : page<0 ? 0 : page\n";
    text << "    }\n";
    text << "    Keys.onLeftPressed: --page\n";
    text << "    Keys.onRightPressed: ++page\n";
    text << "    signal rollDiceCmd(string cmd, bool alias)\n";
    text << "    signal showText(string text)\n";
    text << "    MouseArea {\n";
    text << "         anchors.fill:parent\n";
    text << "         onClicked: root.focus = true\n";
    text << "     }\n";
    if(!m_headCode.isEmpty())
    {
        text << "   " << m_headCode << "\n";
    }
    if(hasImage)
    {
        text << "    Image {\n";
        text << "        id:imagebg"
             << "\n";
        text << "        objectName:\"imagebg\""
             << "\n";
        text << "        property real iratio :" << ratio << "\n";
        text << "        property real iratiobis :" << ratioBis << "\n";
        if(m_flickableSheet)
        {
            text << "       property real realscale: " << m_fixedScaleSheet << "\n";
            text << "       width: sourceSize.width*realscale"
                 << "\n";
            text << "       height: sourceSize.height*realscale"
                 << "\n";
        }
        else
        {
            text << "       property real realscale: width/" << size.width() << "\n";
            text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width"
                 << "\n";
            text << "       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width"
                 << "\n";
        }
        text << "       source: \"image://rcs/" + key + "_background_%1.jpg\".arg(root.page)"
             << "\n";
        m_model->generateQML(text, 1, false);
        text << "\n";
        text << "  }\n";
    }
    else
    {
        if(m_flickableSheet)
        {
            text << "    property real realscale: " << m_fixedScaleSheet << "\n";
        }
        else
        {
            text << "    property real realscale: 1\n";
        }
        m_model->generateQML(text, 1, false);
    }
    if(!m_bottomCode.isEmpty())
    {
        text << "   " << m_bottomCode << "\n";
    }
    text << "}\n";
    text.flush();

    setQmlCode(data);
    setTextEdited(false);
}

const QString& QmlGeneratorController::qmlCode() const
{
    return m_qmlCode;
}

void QmlGeneratorController::setQmlCode(const QString& newQmlCode)
{
    if(m_qmlCode == newQmlCode)
        return;
    m_qmlCode= newQmlCode;
    emit qmlCodeChanged();
    setTextEdited(true);
}
