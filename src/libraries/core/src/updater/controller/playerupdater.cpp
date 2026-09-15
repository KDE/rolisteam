#include "updater/controller/playerupdater.h"
#include "common/logcategory.h"
#include "controller/gamecontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "network/connectionprofile.h"
#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

void addPlayerToModel(PlayerModel* model, NetworkMessageReader* msg)
{
    Player* player= new Player();
    PlayerMessageHelper::readPlayer(*msg, player);
    model->addPlayer(player);
}

PlayerUpdater::PlayerUpdater(PlayerController* ctrl, QObject* parent) : NetWorkReceiver{parent}, m_ctrl(ctrl)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::UserCategory, this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::PlayerCharacterCategory, this);

    if(!m_ctrl)
    {
        qCWarning(MessagingCat) << tr("Error: Player controller is null. PlayerUpdate can't work.");
        return;
    }
    auto model= m_ctrl->model();

    connect(model, &PlayerModel::playerJoin, this, &PlayerUpdater::updateNewPlayer);
    connect(model, &PlayerModel::playerLeft, this, &PlayerUpdater::playerLeft);
    connect(model, &PlayerModel::characterAdded, this, &PlayerUpdater::addCharacter);
    connect(model, &PlayerModel::characterRemoved, this, &PlayerUpdater::removeCharacter);
}

NetWorkReceiver::SendType PlayerUpdater::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;
    auto model= m_ctrl->model();
    if(msg->category() == NetMsg::UserCategory)
    {
        switch(msg->action())
        {
        case NetMsg::PlayerConnectionAction:
            addPlayerToModel(model, msg);
            break;
        case NetMsg::DelPlayerAction:
            model->removePlayer(model->playerById(MessageHelper::readPlayerId(*msg)));
            break;
        default:
            break;
        }
    }
    else if(msg->category() == NetMsg::PlayerCharacterCategory)
    {
        switch(msg->action())
        {
        case NetMsg::AddCharacterToPlayerAct:
        {
            m_updating= true;
            msg->string8();
            MessageHelper::addCharacterIntoModel(*msg, m_ctrl->model());
            m_updating= false;
        }
        break;
        case NetMsg::RemoveCharacterToPlayerAct:
            m_updating= true;
            MessageHelper::removeCharacterIntoModel(*msg, m_ctrl->model());
            m_updating= false;
            break;
        case NetMsg::ChangePlayerPropertyAct:
        case NetMsg::ChangeCharacterPropertyAct:
            m_updating= true;
            MessageHelper::updatePerson(*msg, model);
            m_updating= false;
            break;
        default:
            break;
        }
    }

    return type;
}

void PlayerUpdater::setGameController(GameController* gameCtrl)
{
    m_networkCtrl= gameCtrl->networkController();
    connect(gameCtrl, &GameController::connectedChanged, this,
            [this](bool b)
            {
                if(b)
                {
                    auto local= m_ctrl->localPlayer();
                    PlayerMessageHelper::sendOffPlayerInformations(local);

                    connect(local, &Player::avatarChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerAvatar(local->avatar());
                            });
                    connect(local, &Player::nameChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerName(local->name());
                            });
                    connect(local, &Player::colorChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerColor(local->getColor());
                            });

                    auto updateCharacters= [this, local]()
                    {
                        auto pro= m_networkCtrl->currentProfile();
                        if(!pro)
                            return;
                        pro->setPlayerColor(local->getColor());

                        auto const& characters= local->children();
                        std::vector<connection::CharacterData> data;
                        data.reserve(characters.size());

                        std::transform(
                            std::begin(characters), std::end(characters), std::back_inserter(data),
                            [](const std::unique_ptr<Character>& character)
                            {
                                QHash<QString, QVariant> params;
                                params.insert(Core::updater::key_char_property_hp, character->getHealthPointsCurrent());
                                params.insert(Core::updater::key_char_property_maxhp, character->getHealthPointsMax());
                                params.insert(Core::updater::key_char_property_minhp, character->getHealthPointsMin());
                                params.insert(Core::updater::key_char_property_dist, character->getDistancePerTurn());
                                params.insert(Core::updater::key_char_property_state_id, character->stateId());
                                params.insert(Core::updater::key_char_property_life_color, character->getLifeColor());
                                params.insert(Core::updater::key_char_property_init_cmd, character->initCommand());
                                params.insert(Core::updater::key_char_property_has_init, character->hasInitScore());
                                params.insert(Core::updater::key_char_property_init_score,
                                              character->getInitiativeScore());
                                return connection::CharacterData({character->uuid(), character->name(),
                                                                  character->getColor(), character->avatar(), params});
                            });

                        pro->setCharacters(data);
                    };

                    connect(local, &Player::characterChanged, m_networkCtrl, updateCharacters);
                    connect(local, &Player::characterCountChanged, m_networkCtrl, updateCharacters);
                }
                else
                    m_ctrl->clear();
            });
}

void PlayerUpdater::updateNewPlayer(Player* player)
{
    if(!m_ctrl || !player || (!m_ctrl->localIsGm() && player != m_ctrl->localPlayer()))
        return;

    connect(player, &Player::avatarChanged, this,
            [this, player]() { sendOffChanges<QByteArray>(player, false, Core::person::avatar); });
    connect(player, &Player::nameChanged, this,
            [this, player]() { sendOffChanges<QString>(player, false, Core::person::name); });
    connect(player, &Player::colorChanged, this,
            [this, player]() { sendOffChanges<QColor>(player, false, Core::person::color); });

    auto const& characters= player->children();

    for(auto const& character : characters)
    {
        auto p= character.get();
        connect(p, &Character::avatarChanged, this,
                [this, p]() { sendOffChanges<QByteArray>(p, true, Core::person::avatar); });
        connect(p, &Character::nameChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::name); });
        connect(p, &Character::colorChanged, this,
                [this, p]() { sendOffChanges<QColor>(p, true, Core::person::color); });
        connect(p, &Character::currentHealthPointsChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::healthPoints); });
        connect(p, &Character::npcChanged, this, [this, p]() { sendOffChanges<bool>(p, true, Core::person::isNpc); });
        connect(p, &Character::maxHPChanged, this, [this, p]() { sendOffChanges<int>(p, true, Core::person::maxHP); });
        connect(p, &Character::minHPChanged, this, [this, p]() { sendOffChanges<int>(p, true, Core::person::minHP); });
        connect(p, &Character::distancePerTurnChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::distancePerTurn); });

        connect(p, &Character::initCommandChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::initCommand); });
        connect(p, &Character::hasInitScoreChanged, this,
                [this, p]() { sendOffChanges<bool>(p, true, Core::person::hasInitiative); });
        connect(p, &Character::initiativeChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::initiative); });
        connect(p, &Character::stateIdChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::stateId); });
        connect(p, &Character::lifeColorChanged, this,
                [this, p]() { sendOffChanges<QColor>(p, true, Core::person::lifeColor); });
    }
}

void PlayerUpdater::playerLeft(Player* player)
{
    disconnect(player, 0, this, 0);
}

void PlayerUpdater::addCharacter(Player* player, Character* character)
{
    connect(character, &Character::avatarChanged, this,
            [this, character]() { sendOffChanges<QByteArray>(character, true, Core::person::avatar); });
    connect(character, &Character::nameChanged, this,
            [this, character]() { sendOffChanges<QString>(character, true, Core::person::name); });
    connect(character, &Character::colorChanged, this,
            [this, character]() { sendOffChanges<QColor>(character, true, Core::person::color); });
    connect(character, &Character::currentHealthPointsChanged, this,
            [this, character]() { sendOffChanges<int>(character, true, Core::person::healthPoints); });
    connect(character, &Character::npcChanged, this,
            [this, character]() { sendOffChanges<bool>(character, true, Core::person::isNpc); });
    connect(character, &Character::maxHPChanged, this,
            [this, character]() { sendOffChanges<int>(character, true, Core::person::maxHP); });
    connect(character, &Character::minHPChanged, this,
            [this, character]() { sendOffChanges<int>(character, true, Core::person::minHP); });
    connect(character, &Character::distancePerTurnChanged, this,
            [this, character]() { sendOffChanges<int>(character, true, Core::person::distancePerTurn); });

    connect(character, &Character::initCommandChanged, this,
            [this, character]() { sendOffChanges<QString>(character, true, Core::person::initCommand); });
    connect(character, &Character::hasInitScoreChanged, this,
            [this, character]() { sendOffChanges<bool>(character, true, Core::person::hasInitiative); });
    connect(character, &Character::initiativeChanged, this,
            [this, character]() { sendOffChanges<int>(character, true, Core::person::initiative); });
    connect(character, &Character::stateIdChanged, this,
            [this, character]() { sendOffChanges<QString>(character, true, Core::person::stateId); });
    connect(character, &Character::lifeColorChanged, this,
            [this, character]() { sendOffChanges<QColor>(character, true, Core::person::lifeColor); });

    NetworkMessageWriter msg(NetMsg::PlayerCharacterCategory, NetMsg::AddCharacterToPlayerAct);
    msg.string8(player->uuid());
    PlayerMessageHelper::writeCharacterIntoMessage(msg, character);
    msg.sendToServer();
}
void PlayerUpdater::removeCharacter(Player* player, const QString& uuid)
{
    NetworkMessageWriter msg(NetMsg::PlayerCharacterCategory, NetMsg::RemoveCharacterToPlayerAct);
    msg.string8(player->uuid());
    msg.string8(uuid);
    msg.sendToServer();
}
