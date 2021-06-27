#ifndef CONNECTIONPROFILE_H
#define CONNECTIONPROFILE_H

#include "data/player.h"

struct CharacterData
{
    QString m_name;
    QColor m_color;
    QByteArray m_avatarData;
    QHash<QString, QVariant> m_params;
};

/**
 * @brief The ConnectionProfile class stores any data about the connection: Mode (client or server) or role (GM or
 * Player)
 */
class ConnectionProfile
{
public:
    ConnectionProfile();
    virtual ~ConnectionProfile();
    void setProfileTitle(const QString&);
    QString profileTitle() const;

    void setPlayerName(const QString&);
    QString playerName() const;
    void setPlayerColor(const QColor&);
    QColor playerColor() const;
    void setPlayerAvatar(const QByteArray&);
    QByteArray playerAvatar() const;
    QString playerId() const;
    void setPlayerId(const QString& playerId);

    void setAddress(const QString&);
    QString address() const;

    void setPort(quint16);
    quint16 port() const;

    void setServerMode(bool);
    bool isServer() const;

    void setGm(bool);
    bool isGM() const;

    QString campaignPath() const;
    void setCampaignPath(const QString& id);

    void setPassword(const QString& password);
    QByteArray password() const;

    const std::vector<CharacterData>& characters();
    CharacterData& character(int i);
    void addCharacter(const CharacterData& data);
    void removeCharacter(int index);
    int characterCount();
    void clearCharacter();

    void setHash(const QByteArray& password);
    void cloneProfile(const ConnectionProfile* src);

private:
    // Profile data
    QString m_title; ///< @brief defines the name of the profile. It can be what ever users want.

    // Player data
    QString m_playerName;
    QColor m_playerColor;
    QByteArray m_playerAvatar;
    QString m_playerId;
    bool m_isGM= false;

    // Connection data
    bool m_server= false;
    quint16 m_port= 6660;
    QString m_address;
    QByteArray m_password;

    // campaign dir
    QString m_campaignDir;

    // Character info
    std::vector<CharacterData> m_characters;
};

#endif // CONNECTIONPROFILE_H
