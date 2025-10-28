#include "transfer_page.h"
#include "ui_transfer_page.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDateTime>

TransferPage::TransferPage(QWidget *parent, const QString &userType)
    : QWidget(parent)
    , ui(new Ui::TransferPage)
    , sourceModel(new QSqlTableModel(this))
    , targetModel(new QSqlTableModel(this))
    , userType(userType)
{
    ui->setupUi(this);
    setupModels();
    setupConnections();
    loadDepotComboBoxes();
    setupValidators();
}

TransferPage::~TransferPage()
{
    delete ui;
}

void TransferPage::setupModels()
{
    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    // Source model setup
    sourceModel->setTable("Products");
    sourceModel->setHeaderData(0, Qt::Horizontal, tr("Ürün ID"));
    sourceModel->setHeaderData(1, Qt::Horizontal, tr("Ürün Kodu"));
    sourceModel->setHeaderData(2, Qt::Horizontal, tr("Ürün Adı"));
    sourceModel->setHeaderData(3, Qt::Horizontal, tr("Kategori"));
    sourceModel->setHeaderData(4, Qt::Horizontal, tr("Fiyat"));
    sourceModel->setHeaderData(5, Qt::Horizontal, tr("Stok"));

    // Tabloların görünümünü ayarla
    ui->sourceTableView->setModel(sourceModel);
    ui->sourceTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->sourceTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sourceTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->sourceTableView->setAlternatingRowColors(true);

    // Modern görünüm için stil
    QString tableStyle = "QTableView {"
                         "background-color: #1e1e2f;"
                         "alternate-background-color: #252540;"
                         "color: #ecf0f1;"
                         "border: none;"
                         "}"
                         "QHeaderView::section {"
                         "background-color: #1a237e;"
                         "color: white;"
                         "padding: 6px;"
                         "border: none;"
                         "}"
                         "QTableView::item:selected {"
                         "background-color: #3F51B5;"
                         "color: white;"
                         "}";

    ui->sourceTableView->setStyleSheet(tableStyle);
    ui->targetTableView->setStyleSheet(tableStyle);

    // Target model için aynı ayarları yap
    targetModel->setTable("Products");
    targetModel->setHeaderData(0, Qt::Horizontal, tr("Ürün ID"));
    targetModel->setHeaderData(1, Qt::Horizontal, tr("Ürün Kodu"));
    targetModel->setHeaderData(2, Qt::Horizontal, tr("Ürün Adı"));
    targetModel->setHeaderData(3, Qt::Horizontal, tr("Kategori"));
    targetModel->setHeaderData(4, Qt::Horizontal, tr("Fiyat"));
    targetModel->setHeaderData(5, Qt::Horizontal, tr("Stok"));

    ui->targetTableView->setModel(targetModel);
    ui->targetTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->targetTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->targetTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->targetTableView->setAlternatingRowColors(true);

    // İlk veri yüklemesini yap
    sourceModel->select();
    targetModel->select();
}

void TransferPage::setupConnections()
{
    connect(ui->sourceDepotCombo, &QComboBox::currentTextChanged,
            this, &TransferPage::onSourceDepotChanged);
    connect(ui->targetDepotCombo, &QComboBox::currentTextChanged,
            this, &TransferPage::onTargetDepotChanged);
    connect(ui->transferButton, &QPushButton::clicked,
            this, &TransferPage::performTransfer);
    connect(ui->sourceSearchEdit, &QLineEdit::textChanged,
            this, &TransferPage::searchSourceProducts);
    connect(ui->targetSearchEdit, &QLineEdit::textChanged,
            this, &TransferPage::searchTargetProducts);
}

void TransferPage::loadDepotComboBoxes()
{
    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    QSqlQuery query(dbManager.getDatabase());
    // DISTINCT ekleyerek tekrarları önlüyoruz
    query.exec("SELECT DISTINCT DepotName FROM Depots ORDER BY DepotName");

    ui->sourceDepotCombo->clear();
    ui->targetDepotCombo->clear();

    QSet<QString> uniqueDepots; // Tekrarları kontrol etmek için set kullanıyoruz

    while (query.next()) {
        QString depotName = query.value(0).toString();
        if (!uniqueDepots.contains(depotName)) {
            ui->sourceDepotCombo->addItem(depotName);
            ui->targetDepotCombo->addItem(depotName);
            uniqueDepots.insert(depotName);
        }
    }
}

void TransferPage::onSourceDepotChanged(const QString &depotName)
{
    QSqlQuery query(dbManager.getDatabase());
    query.prepare("SELECT DepotID FROM Depots WHERE DepotName = :name");
    query.bindValue(":name", depotName);
    query.exec();

    if (query.next()) {
        int depotId = query.value(0).toInt();
        sourceModel->setFilter(QString("DepotID = %1").arg(depotId));
        sourceModel->select();
    }
}

void TransferPage::onTargetDepotChanged(const QString &depotName)
{
    QSqlQuery query(dbManager.getDatabase());
    query.prepare("SELECT DepotID FROM Depots WHERE DepotName = :name");
    query.bindValue(":name", depotName);
    query.exec();

    if (query.next()) {
        int depotId = query.value(0).toInt();
        targetModel->setFilter(QString("DepotID = %1").arg(depotId));
        targetModel->select();
    }
}

void TransferPage::performTransfer()
{
    qDebug() << "Transfer işlemi başlatıldı - userType:" << userType;

    if (userType == "Çalışan") {
        qDebug() << "Transfer operation blocked - Employee user";
        QMessageBox::warning(this, tr("Yetki Hatası"),
                             tr("Çalışan kullanıcılar transfer işlemi yapma yetkisine sahip değildir!"));
        return;
    }
    qDebug() << "Kaynak Depo:" << ui->sourceDepotCombo->currentText();
    qDebug() << "Hedef Depo:" << ui->targetDepotCombo->currentText();

    // Seçili ürünü kontrol et
    QModelIndex currentIndex = ui->sourceTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen transfer edilecek ürünü seçin."));
        return;
    }

    // Transfer miktarını al
    int quantity = ui->quantitySpinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen geçerli bir miktar girin."));
        return;
    }

    // Kaynak depodaki stok miktarını kontrol et
    int currentStock = sourceModel->data(sourceModel->index(currentIndex.row(), 5)).toInt();
    if (quantity > currentStock) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Yetersiz stok!"));
        return;
    }

    // Kaynak ve hedef depo adlarını al
    QString sourceDepotName = ui->sourceDepotCombo->currentText();
    QString targetDepotName = ui->targetDepotCombo->currentText();

    if (sourceDepotName == targetDepotName) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Kaynak ve hedef depo aynı olamaz!"));
        return;
    }

    QSqlQuery query(dbManager.getDatabase());
    dbManager.getDatabase().transaction();

    try {
        // Kaynak depodan ürünü azalt
        int productId = sourceModel->data(sourceModel->index(currentIndex.row(), 0)).toInt();
        qDebug() << "Ürün ID:" << productId;

        query.prepare("UPDATE Products SET Stock = Stock - :quantity WHERE ProductID = :productId");
        query.bindValue(":quantity", quantity);
        query.bindValue(":productId", productId);

        if (!query.exec()) {
            qDebug() << "Kaynak depo hatası:" << query.lastError().text();
            throw std::runtime_error("Kaynak depodaki ürün güncellenemedi.");
        }

        // Hedef depo ID'sini al
        query.prepare("SELECT DepotID FROM Depots WHERE DepotName = :depotName");
        query.bindValue(":depotName", targetDepotName);

        if (!query.exec()) {
            qDebug() << "Hedef depo ID sorgu hatası:" << query.lastError().text();
            throw std::runtime_error("Hedef depo ID'si alınamadı.");
        }

        if (!query.next()) {
            qDebug() << "Hedef depo bulunamadı:" << targetDepotName;
            throw std::runtime_error("Hedef depo bulunamadı.");
        }

        int targetDepotId = query.value(0).toInt();
        qDebug() << "Hedef Depo ID:" << targetDepotId;

        // Ürün bilgilerini al
        QString productCode = sourceModel->data(sourceModel->index(currentIndex.row(), 1)).toString();
        QString productName = sourceModel->data(sourceModel->index(currentIndex.row(), 2)).toString();
        QString category = sourceModel->data(sourceModel->index(currentIndex.row(), 3)).toString();
        double price = sourceModel->data(sourceModel->index(currentIndex.row(), 4)).toDouble();

        // Hedef depoda ürün var mı kontrol et
        query.prepare("SELECT ProductID, Stock FROM Products WHERE ProductCode = :productCode AND DepotID = :depotId");
        query.bindValue(":productCode", productCode);
        query.bindValue(":depotId", targetDepotId);

        if (!query.exec()) {
            qDebug() << "Ürün kontrol hatası:" << query.lastError().text();
            throw std::runtime_error("Hedef depo sorgulanamadı.");
        }

        if (query.next()) {
            // Ürün varsa stok güncelle
            qDebug() << "Ürün mevcut, stok güncelleniyor...";
            query.prepare("UPDATE Products SET Stock = Stock + :quantity WHERE ProductCode = :productCode AND DepotID = :depotId");
            query.bindValue(":quantity", quantity);
            query.bindValue(":productCode", productCode);
            query.bindValue(":depotId", targetDepotId);

            if (!query.exec()) {
                qDebug() << "Güncelleme hatası:" << query.lastError().text();
                throw std::runtime_error("Hedef depodaki ürün güncellenemedi.");
            }
        } else {
            // Ürün yoksa yeni kayıt oluştur
            qDebug() << "Yeni ürün ekleniyor:";
            qDebug() << "Kod:" << productCode;
            qDebug() << "Ad:" << productName;
            qDebug() << "Kategori:" << category;
            qDebug() << "Fiyat:" << price;
            qDebug() << "Stok:" << quantity;
            qDebug() << "Depo ID:" << targetDepotId;

            query.prepare("INSERT INTO Products (ProductCode, ProductName, Category, Price, Stock, Status, DepotID) "
                          "VALUES (:code, :name, :category, :price, :quantity, 'Aktif', :depotId)");

            query.bindValue(":code", productCode);
            query.bindValue(":name", productName);
            query.bindValue(":category", category);
            query.bindValue(":price", price);
            query.bindValue(":quantity", quantity);
            query.bindValue(":depotId", targetDepotId);

            if (!query.exec()) {
                qDebug() << "Ekleme hatası:" << query.lastError().text();
                throw std::runtime_error("Hedef depoya yeni ürün eklenemedi.");
            }
        }

        dbManager.getDatabase().commit();
        qDebug() << "Transfer başarılı!";
        QMessageBox::information(this, tr("Başarılı"), tr("Transfer işlemi tamamlandı."));

        // Tabloları güncelle
        onSourceDepotChanged(sourceDepotName);
        onTargetDepotChanged(targetDepotName);
        ui->quantitySpinBox->setValue(0);

    } catch (const std::runtime_error& e) {
        dbManager.getDatabase().rollback();
        qDebug() << "Hata:" << e.what();
        QMessageBox::critical(this, tr("Hata"), tr("Transfer işlemi başarısız: ") + e.what());
    }
}

void TransferPage::searchSourceProducts(const QString &text)
{
    QString filter = QString("DepotID = (SELECT DepotID FROM Depots WHERE DepotName = '%1')")
    .arg(ui->sourceDepotCombo->currentText());

    if (!text.isEmpty()) {
        filter += QString(" AND (ProductName LIKE '%%1%' OR ProductCode LIKE '%%1%' OR Category LIKE '%%1%')")
        .arg(text);
    }

    sourceModel->setFilter(filter);
    sourceModel->select();
}

void TransferPage::searchTargetProducts(const QString &text)
{
    QString filter = QString("DepotID = (SELECT DepotID FROM Depots WHERE DepotName = '%1')")
    .arg(ui->targetDepotCombo->currentText());

    if (!text.isEmpty()) {
        filter += QString(" AND (ProductName LIKE '%%1%' OR ProductCode LIKE '%%1%' OR Category LIKE '%%1%')")
        .arg(text);
    }

    targetModel->setFilter(filter);
    targetModel->select();
}

void TransferPage::setupValidators()
{
    ui->quantitySpinBox->setMinimum(1);
    ui->quantitySpinBox->setMaximum(999999);
}
