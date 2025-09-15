#ifndef ANCHORVMAPITEMCOMMAND_H
#define ANCHORVMAPITEMCOMMAND_H

#include <QPointer>
#include <QString>
#include <QUndoCommand>

namespace vmap
{
class VisualItemController;
} // namespace vmap

class AnchorVMapItemCommand : public QUndoCommand
{
public:
    AnchorVMapItemCommand(vmap::VisualItemController* child, vmap::VisualItemController* newParent);

    void redo() override;
    void undo() override;

private:
    QPointer<vmap::VisualItemController> m_child;
    QString m_parentId;
    QString m_previousParent;
};

#endif // ANCHORVMAPITEMCOMMAND_H
