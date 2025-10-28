#ifndef DEPOT_PAGE_H
#define DEPOT_PAGE_H

#include <QWidget>
#include <QSqlTableModel>
#include "database_manager.h"

namespace Ui {
class DepotPage;
}

class DepotPage : public QWidget
{
    Q_OBJECT

public:
    explicit DepotPage(QWidget *parent = nullptr, const QString &userType = "Admin");

    ~DepotPage();

private slots:
    void loadDepotData();
    void showAddDepotForm();
    void showUpdateDepotForm();
    void saveDepot();
    void deleteDepot();
    void clearForm();
    void cancelForm();
    void switchToMainView();
    void switchToFormView();

private:
    void setupModel();
    void setupConnections();  // Yeni eklendi
    void setupComboBoxes();   // Yeni eklendi
    void setupValidators();   // İsterseniz bunu da ekleyebilirsiniz
    void updateDepotGraphView();

    QString userType;
    Ui::DepotPage *ui;
    QSqlTableModel *model;
    DatabaseManager dbManager;
    bool isNewDepot;
};

#endif // DEPOT_PAGE_H
