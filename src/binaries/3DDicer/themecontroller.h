#ifndef THEMECONTROLLER_H
#define THEMECONTROLLER_H

#include <QColor>
#include <QObject>
#include <QQmlEngine>

class ThemeController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Theme)
    QML_SINGLETON
    // clang-format off
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged FINAL)

    Q_PROPERTY(int iconSize READ iconSize CONSTANT FINAL)
    Q_PROPERTY(int tabIconSize READ tabIconSize CONSTANT FINAL)
    Q_PROPERTY(int titleFontSize READ titleFontSize CONSTANT FINAL)
    Q_PROPERTY(int commandFontSize READ name CONSTANT FINAL)
    Q_PROPERTY(int finalDiceResultFontSize READ finalDiceResultFontSize CONSTANT FINAL)
    Q_PROPERTY(int textFieldFontSize READ textFieldFontSize  CONSTANT FINAL)
    Q_PROPERTY(int timeFontSize READ timeFontSize  CONSTANT FINAL)
    Q_PROPERTY(int margin READ margin  CONSTANT FINAL)
    Q_PROPERTY(int spacing READ spacing  CONSTANT FINAL)
    Q_PROPERTY(int padding READ padding  CONSTANT FINAL)
    Q_PROPERTY(int radius READ radius  CONSTANT FINAL)
    Q_PROPERTY(int penWidth READ penWidth  CONSTANT FINAL)
    Q_PROPERTY(int colorButtonSize READ colorButtonSize CONSTANT FINAL)
    Q_PROPERTY(int minimalTextField READ minimalTextField CONSTANT FINAL)
    Q_PROPERTY(int animationTime READ animationTime CONSTANT FINAL)

    Q_PROPERTY(QColor textColor READ textColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor borderColor READ borderColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor disabledColor READ disabledColor  NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor selectedColor READ selectedColor  NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor transparent READ transparent  NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor deleteTextColor READ deleteTextColor  NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor deleteBtnColor READ deleteBtnColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor highLightColor READ highLightColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor buttonColor READ buttonColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor darkColor READ darkColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor midColor READ midColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor checkedSwitchColor READ checkedSwitchColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor uncheckedSwitchColor READ uncheckedSwitchColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor windowColor READ windowColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor contentBackGroundColor READ contentBackGroundColor NOTIFY darkModeChanged FINAL)
    Q_PROPERTY(QColor placeHolderTextColor READ placeHolderTextColor NOTIFY darkModeChanged FINAL)
    // clang-format on
public:
    explicit ThemeController(QObject* parent= nullptr);

    int iconSize() const;
    int tabIconSize() const;
    int titleFontSize() const;
    int name() const;
    int finalDiceResultFontSize() const;
    int textFieldFontSize() const;
    int timeFontSize() const;
    int margin() const;
    int spacing() const;
    int radius() const;
    int penWidth() const;
    int colorButtonSize() const;
    int minimalTextField() const;
    int padding() const;
    bool darkMode() const;
    QColor textColor() const;
    QColor borderColor() const;
    QColor disabledColor() const;
    QColor selectedColor() const;
    QColor transparent() const;
    QColor deleteBtnColor() const;
    QColor deleteTextColor() const;
    QColor highLightColor() const;
    QColor windowColor() const;
    QColor placeHolderTextColor() const;
    QColor midColor() const;
    QColor contentBackGroundColor() const;
    QColor checkedSwitchColor() const;
    QColor buttonColor() const;
    QColor darkColor() const;

    Q_INVOKABLE QColor fullOpacity(const QColor& color);

    void setDarkMode(bool newDarkMode);

    QColor uncheckedSwitchColor() const;

    int animationTime() const;

signals:
    void darkModeChanged();

private:
    int m_iconSize{30};
    int m_tabIconSize{60};
    int m_titleFontSize{40};
    int m_commandFontSize{20};
    int m_finalDiceResultFontSize{30};
    int m_textFieldFontSize{30};
    int m_timeFontSize{8};
    int m_margin{8};
    int m_spacing{8};
    int m_padding{8};
    int m_radius{10};
    int m_penWidth{1};
    int m_colorButtonSize{50};
    int m_minimalTextField{30};

    std::array<QColor, 2> m_textColor{Qt::black, Qt::white};
    std::array<QColor, 2> m_borderColor{Qt::black, Qt::white};
    std::array<QColor, 2> m_disabledColor{QColor{220, 20, 60, 40}, QColor{92, 8, 25}};   //"#66DC143C"
    std::array<QColor, 2> m_selectedColor{QColor{46, 139, 87, 102}, QColor{18, 54, 34}}; //"#662e8b57"
    std::array<QColor, 2> m_transparent{Qt::transparent, Qt::transparent};
    std::array<QColor, 2> m_deleteTextColor{Qt::white, Qt::black};
    std::array<QColor, 2> m_deleteBtnColor{"tomato", QColor{71, 227, 255}};
    std::array<QColor, 2> m_highLightColor{QColor{53, 72, 222}, QColor{222, 53, 53}};
    std::array<QColor, 2> m_buttonColor{QColor{255, 255, 240}, QColor{0, 0, 10}};
    std::array<QColor, 2> m_darkColor{QColor{200, 200, 200}, QColor{50, 50, 60}};
    std::array<QColor, 2> m_midColor{QColor{200, 200, 200}, QColor{50, 50, 60}};
    std::array<QColor, 2> m_windowColor{QColor{255 - 23, 255 - 23, 255 - 23}, QColor{23, 23, 23}};
    std::array<QColor, 2> m_contentBackGroundColor{Qt::white, Qt::black};
    std::array<QColor, 2> m_placeHolderTextColor{QColor{160, 160, 160}, QColor{170, 170, 170}};
    std::array<QColor, 2> m_checkedSwitchColor{QColor{53, 72, 222, 128}, QColor{222, 53, 53, 128}};
    std::array<QColor, 2> m_uncheckedSwitchColor{QColor{128, 128, 128, 82}, QColor{128, 128, 128, 82}};
    bool m_darkMode{false};
    int m_animationTime{400};
};

#endif // THEMECONTROLLER_H
