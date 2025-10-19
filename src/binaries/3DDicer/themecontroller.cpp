#include "themecontroller.h"

int index(bool darkMode)
{
    return darkMode ? 1 : 0;
}
ThemeController::ThemeController(QObject* parent) : QObject{parent} {}

int ThemeController::iconSize() const
{
    return m_iconSize;
}

int ThemeController::tabIconSize() const
{
    return m_tabIconSize;
}

QColor ThemeController::deleteBtnColor() const
{
    return m_deleteBtnColor[index(m_darkMode)];
}

int ThemeController::titleFontSize() const
{
    return m_titleFontSize;
}

int ThemeController::name() const
{
    return m_commandFontSize;
}

int ThemeController::finalDiceResultFontSize() const
{
    return m_finalDiceResultFontSize;
}

int ThemeController::textFieldFontSize() const
{
    return m_textFieldFontSize;
}

int ThemeController::timeFontSize() const
{
    return m_timeFontSize;
}

int ThemeController::margin() const
{
    return m_margin;
}

int ThemeController::spacing() const
{
    return m_spacing;
}

int ThemeController::radius() const
{
    return m_radius;
}

int ThemeController::penWidth() const
{
    return m_penWidth;
}

int ThemeController::colorButtonSize() const
{
    return m_colorButtonSize;
}

int ThemeController::minimalTextField() const
{
    return m_minimalTextField;
}

QColor ThemeController::textColor() const
{
    return m_textColor[index(m_darkMode)];
}

QColor ThemeController::borderColor() const
{
    return m_borderColor[index(m_darkMode)];
}

QColor ThemeController::disabledColor() const
{
    return m_disabledColor[index(m_darkMode)];
}

QColor ThemeController::selectedColor() const
{
    return m_selectedColor[index(m_darkMode)];
}

QColor ThemeController::transparent() const
{
    return m_transparent[index(m_darkMode)];
}

QColor ThemeController::deleteTextColor() const
{
    return m_deleteTextColor[index(m_darkMode)];
}

int ThemeController::padding() const
{
    return m_padding;
}

bool ThemeController::darkMode() const
{
    return m_darkMode;
}

void ThemeController::setDarkMode(bool newDarkMode)
{
    if(m_darkMode == newDarkMode)
        return;
    m_darkMode= newDarkMode;
    emit darkModeChanged();
}

QColor ThemeController::darkColor() const
{
    return m_darkColor[index(m_darkMode)];
}

QColor ThemeController::fullOpacity(const QColor& color)
{
    return QColor(color.red(), color.green(), color.blue());
}

QColor ThemeController::highLightColor() const
{
    return m_highLightColor[index(m_darkMode)];
}

QColor ThemeController::windowColor() const
{
    return m_windowColor[index(m_darkMode)];
}

QColor ThemeController::placeHolderTextColor() const
{
    return m_placeHolderTextColor[index(m_darkMode)];
}

QColor ThemeController::midColor() const
{
    return m_midColor[index(m_darkMode)];
}

QColor ThemeController::contentBackGroundColor() const
{
    return m_contentBackGroundColor[index(m_darkMode)];
}

QColor ThemeController::buttonColor() const
{
    return m_buttonColor[index(m_darkMode)];
}

QColor ThemeController::checkedSwitchColor() const
{
    return m_checkedSwitchColor[index(m_darkMode)];
}

QColor ThemeController::uncheckedSwitchColor() const
{
    return m_uncheckedSwitchColor[index(m_darkMode)];
}

int ThemeController::animationTime() const
{
    return m_animationTime;
}
