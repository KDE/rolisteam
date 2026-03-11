#ifndef ADDCHILDTOPACKAGECOMMAND_H
#define ADDCHILDTOPACKAGECOMMAND_H

#include <QCoreApplication>
#include <QUndoCommand>

#include "mindmap/data/positioneditem.h"

namespace mindmap
{
class MindItemModel;
class MindMapControllerBase;
class AddChildToPackageCommand : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddItemCommand)

public:
    AddChildToPackageCommand(MindItemModel* nodeModel, const QString& idEnd);

    void undo() override;
    void redo() override;

private:
    QPointer<mindmap::PositionedItem> m_node;
    QPointer<MindItemModel> m_nodeModel;
    QString m_idParent;
};
} // namespace mindmap

#endif // ADDCHILDTOPACKAGECOMMAND_H
