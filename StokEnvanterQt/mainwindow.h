#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>  // Timer için include ekleyin
#include "depot_page.h"
#include "product_page.h"
#include "login_page.h"
#include "reports_page.h"
#include "transfer_page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUserTypeAndInitialize(const QString &type) {  // Yeni fonksiyon
        currentUserType = type;
        initializePages();
    }

private slots:
    void showProductManagement();
    void showDepotManagement();
    void showStockManagement();
    void showReports();
    void showSettings();
    void setupLoginPage();
    void setupButtonConnections();

private:
    void initializePages();  // Yeni fonksiyon

    Ui::MainWindow *ui;
    ProductPage *productPage = nullptr;
    LoginPage *loginPage = nullptr;
    ReportsPage *reportsPage = nullptr;
    TransferPage *transferPage = nullptr;
    DepotPage* depotPage;
    QString currentUserType;
};
#endif
