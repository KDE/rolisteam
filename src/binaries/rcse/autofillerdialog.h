#ifndef AUTOFILLERDIALOG_H
#define AUTOFILLERDIALOG_H

#include "fieldmodel.h"
#include <QDialog>

namespace Ui
{
class AutoFillerDialog;
}

class AutoFillerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoFillerDialog(FieldModel* model, int col, const QModelIndexList& selection, QWidget* parent= nullptr);
    ~AutoFillerDialog();

private slots:
    void computeValues();

private:
    Ui::AutoFillerDialog* ui;
    QPointer<FieldModel> m_model;
    QModelIndexList m_selection;
    QStringList m_result;
    int m_col{0};
    int m_count{0};
};

#endif // AUTOFILLERDIALOG_H
