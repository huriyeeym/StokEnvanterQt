#ifndef TRANSFER_PAGE_H
#define TRANSFER_PAGE_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSqlQuery>
#include "database_manager.h"
#include "ui_transfer_page.h"

namespace Ui {
class TransferPage;
}

class TransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransferPage(QWidget *parent = nullptr, const QString &userType = "Admin");
    ~TransferPage();

private slots:
    void setupModels();
    void setupConnections();
    void performTransfer();
    void onSourceDepotChanged(const QString &depotName);
    void onTargetDepotChanged(const QString &depotName);
    void searchSourceProducts(const QString &text);
    void searchTargetProducts(const QString &text);

private:
    void loadDepotComboBoxes();
    void clearForm();
    void setupValidators();

    Ui::TransferPage *ui;
    DatabaseManager dbManager;
    QSqlTableModel *sourceModel;
    QSqlTableModel *targetModel;

    QString userType;
};

#endif // TRANSFER_PAGE_H
