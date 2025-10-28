#include "mainwindow.h"
#include "depot_page.h"
#include "ui_mainwindow.h"
#include "login_page.h"
#include <QSqlTableModel>
#include <QDebug>
#include "product_page.h"
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , productPage(nullptr)
    , currentUserType("")
{
    ui->setupUi(this);
    setupButtonConnections();
    // Butonları checkable yap
    ui->productButton->setCheckable(true);
    ui->depotButton->setCheckable(true);
    ui->stockButton->setCheckable(true);
    ui->reportButton->setCheckable(true);

    // Başlangıçta ürün yönetimi seçili olsun
    ui->settingsButton->hide();

}
void MainWindow::initializePages() {
    // Mevcut sayfaları temizle
    if (productPage) {
        delete productPage;
        productPage = nullptr;
    }

    // Yeni sayfaları oluştur
    productPage = new ProductPage(this, currentUserType);
    ui->stackedWidget->addWidget(productPage);

    // Diğer sayfalarda da userType'ı kullan
    // ... diğer sayfalar için benzer kodlar
}
// mainwindow.cpp
void MainWindow::setupLoginPage() {
    loginPage = new LoginPage(this);  // parent olarak this'i geçiyoruz
    loginPage->setWindowTitle("Stok Envanter Sistemi - Giriş");
    loginPage->setFixedSize(400, 300);
    loginPage->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    loginPage->show();
    hide();

    connect(loginPage, &LoginPage::loginSuccessful, this, [this](const QString &userType){
        qDebug() << "MainWindow received login signal with userType:" << userType;
        currentUserType = userType;

        // Mevcut sayfaları temizle
        if (productPage) {
            delete productPage;
            productPage = nullptr;
        }

        qDebug() << "MainWindow currentUserType set to:" << currentUserType;

        loginPage->close();
        loginPage->deleteLater();
        loginPage = nullptr;
        show();

    });
}


void MainWindow::setupButtonConnections() {
    connect(ui->productButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "Product button clicked. Current user type:" << currentUserType;  // Debug mesajı
        if (productPage) {
            delete productPage;  // Eğer varsa eskisini sil
            productPage = nullptr;
        }
        showProductManagement();  // Yeni sayfa oluştur
        ui->productButton->setChecked(true);
        ui->depotButton->setChecked(false);
        ui->stockButton->setChecked(false);
        ui->reportButton->setChecked(false);
    });

    // Diğer bağlantılar aynı kalabilir
    connect(ui->depotButton, &QPushButton::clicked, this, [this]() {
        showDepotManagement();
        ui->productButton->setChecked(false);
        ui->depotButton->setChecked(true);
        ui->stockButton->setChecked(false);
        ui->reportButton->setChecked(false);
    });

    connect(ui->stockButton, &QPushButton::clicked, this, [this]() {
        showStockManagement();
        ui->productButton->setChecked(false);
        ui->depotButton->setChecked(false);
        ui->stockButton->setChecked(true);
        ui->reportButton->setChecked(false);

    });
    connect(ui->reportButton, &QPushButton::clicked, this, [this]() {
        showReports();
        ui->productButton->setChecked(false);
        ui->depotButton->setChecked(false);
        ui->stockButton->setChecked(false);
        ui->reportButton->setChecked(true);
    });

    connect(ui->reportButton, &QPushButton::clicked, this, &MainWindow::showReports);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
}
void MainWindow::showProductManagement() {
    if (!productPage) {
        QString userType = this->currentUserType;
        productPage = new ProductPage(this, userType);
        ui->stackedWidget->addWidget(productPage);
    }
    ui->stackedWidget->setCurrentWidget(productPage); // Direkt widget'ı set ediyor
}

void MainWindow::showDepotManagement() {
    if (!ui->stackedWidget->findChild<DepotPage*>()) {
        DepotPage *depotPage = new DepotPage(this, currentUserType);  // userType eklendi
        ui->stackedWidget->addWidget(depotPage);
    }

    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        if (DepotPage *dp = qobject_cast<DepotPage*>(ui->stackedWidget->widget(i))) {
            ui->stackedWidget->setCurrentIndex(i);
            return;
        }
    }
}



void MainWindow::showReports() {
    if (!reportsPage) {
        reportsPage = new ReportsPage(this);
        ui->stackedWidget->addWidget(reportsPage);
    }

    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        if (ReportsPage *rp = qobject_cast<ReportsPage*>(ui->stackedWidget->widget(i))) {
            ui->stackedWidget->setCurrentIndex(i);
            return;
        }
    }
}

void MainWindow::showStockManagement() {
    if (!transferPage) {
        transferPage = new TransferPage(this, currentUserType);
        ui->stackedWidget->addWidget(transferPage);
    }

    // Index üzerinden geçiş yapıyor
    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        if (TransferPage *tp = qobject_cast<TransferPage*>(ui->stackedWidget->widget(i))) {
            ui->stackedWidget->setCurrentIndex(i);
            return;
        }
    }
}
void MainWindow::showSettings() {
    ui->stackedWidget->setCurrentIndex(4);
}

MainWindow::~MainWindow() {
    delete ui;
}
