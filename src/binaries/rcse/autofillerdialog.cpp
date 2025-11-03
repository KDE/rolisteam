#include "autofillerdialog.h"
#include "ui_autofillerdialog.h"

AutoFillerDialog::AutoFillerDialog(FieldModel* model, int col, const QModelIndexList& selection, QWidget* parent)
    : QDialog(parent), ui(new Ui::AutoFillerDialog), m_model(model), m_selection(selection), m_col(col)
{

    ui->setupUi(this);
    ui->m_addBtn->setDefaultAction(ui->actionAdd);
    // ui->actionAdd->setIcon();
    ui->m_removeBtn->setDefaultAction(ui->actionRemove);

    connect(ui->actionAdd, &QAction::triggered, this,
            [this]()
            {
                QListWidgetItem* item= new QListWidgetItem();
                item->setText("text%1");
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                ui->m_rules->addItem(item);

                computeValues();
            });

    ui->m_countLabel->setText(tr("Count: %1").arg(selection.count()));

    connect(ui->actionRemove, &QAction::triggered, this, &AutoFillerDialog::computeValues);
    connect(ui->m_rules, &QListWidget::itemChanged, this, &AutoFillerDialog::computeValues);
    connect(ui->m_startCounter, &QSpinBox::valueChanged, this, &AutoFillerDialog::computeValues);
    connect(ui->m_counterStep, &QSpinBox::valueChanged, this, &AutoFillerDialog::computeValues);
    connect(ui->m_allvalues, &QCheckBox::checkStateChanged, this,
            [this]()
            {
                ui->m_preview->setReadOnly(ui->m_allvalues->checkState() == Qt::Unchecked);
                if(ui->m_preview->isReadOnly())
                {
                    ui->horizontalLayout_8->setStretch(0, 1);
                    ui->horizontalLayout_8->setStretch(1, 0);
                }
                else
                {
                    ui->horizontalLayout_8->setStretch(0, 0);
                    ui->horizontalLayout_8->setStretch(1, 1);
                }
            });

    connect(this, &AutoFillerDialog::accepted, this,
            [this]()
            {
                auto result= m_result;
                // TODO replace with Cpp 23
                // for(auto idx : std::views::zip(m_selection, m_result)) {}
                if(ui->m_allvalues->checkState() != Qt::Unchecked)
                    result= ui->m_preview->toPlainText().split("/n");

                int i= 0;
                for(auto const& idx : std::as_const(m_selection))
                {
                    if(i >= result.size())
                        continue;

                    m_model->setData(idx, result[i], Qt::EditRole);

                    ++i;
                }
            });
}

AutoFillerDialog::~AutoFillerDialog()
{
    delete ui;
}

void AutoFillerDialog::computeValues()
{
    ui->m_preview->clear();
    auto start= ui->m_startCounter->value();
    auto step= ui->m_counterStep->value();

    int inc= start;
    QStringList res;
    for(int i= 0; i < m_selection.count() / std::max(1, ui->m_rules->count()); ++i)
    {
        for(int j= 0; j < ui->m_rules->count(); ++j)
        {
            auto item= ui->m_rules->item(j);
            res << item->text().arg(inc);
        }
        inc+= step;
    }
    m_result= res;
    ui->m_preview->setPlainText(res.join("\n"));
}
