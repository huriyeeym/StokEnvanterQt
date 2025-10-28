#include "product_page.h"
#include "ui_product_page.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QDateTime>
// product_page.cpp
ProductPage::ProductPage(QWidget *parent, const QString &userType)
    : QWidget(parent)
    , ui(new Ui::ProductPage)
    , model(new QSqlTableModel(this))
    , isNewProduct(true)
    , userType(userType)
{
    qDebug() << "ProductPage constructor - Received userType:" << userType;
    ui->setupUi(this);

    setupModel();
    setupConnections();
    setupValidators();
    setupComboBoxes();
    loadProductData();
    switchToMainView();
}
ProductPage::~ProductPage()
{
    delete ui;
}

void ProductPage::setupModel()
{
    if (!dbManager.connectToDatabase()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veritabanına bağlanılamadı!"));
        return;
    }

    model->setTable("Products");
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Ürün Kodu"));
    model->setHeaderData(2, Qt::Horizontal, tr("Ürün Adı"));
    model->setHeaderData(3, Qt::Horizontal, tr("Kategori"));
    model->setHeaderData(4, Qt::Horizontal, tr("Fiyat"));
    model->setHeaderData(5, Qt::Horizontal, tr("Stok"));
    model->setHeaderData(6, Qt::Horizontal, tr("Durum"));
    model->setHeaderData(7, Qt::Horizontal, tr("Depo ID"));

    ui->productTableView->setModel(model);
    ui->productTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->productTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ProductPage::setupConnections()
{
    // Ana butonlar
    connect(ui->addButton, &QPushButton::clicked, this, &ProductPage::showAddProductForm);
    connect(ui->updateButton, &QPushButton::clicked, this, &ProductPage::showUpdateProductForm);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ProductPage::deleteProduct);

    // Form butonları
    connect(ui->saveButton, &QPushButton::clicked, this, &ProductPage::saveProduct);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ProductPage::cancelForm);


}

void ProductPage::setupValidators()
{
    // Fiyat için validator
    ui->priceSpinBox->setMinimum(0.0);
    ui->priceSpinBox->setMaximum(999999.99);
    ui->priceSpinBox->setDecimals(2);

    // Stok için validator
    ui->stockSpinBox->setMinimum(0);
    ui->stockSpinBox->setMaximum(999999);
}

void ProductPage::setupComboBoxes()
{
    // Kategori combobox'ını doldur
    ui->categoryCombo->addItems({"Elektronik", "Mobilya", "Eğitim", "Diğer"});

    // Durum combobox'ını doldur
    ui->statusCombo->addItems({"Aktif", "Pasif"});
}

void ProductPage::loadProductData()
{
    if (!model->select()) {
        QMessageBox::critical(this, tr("Hata"), tr("Veriler yüklenemedi: ") + model->lastError().text());
        return;
    }
    ui->productTableView->resizeColumnsToContents();
}

void ProductPage::showAddProductForm()
{
    isNewProduct = true;
    clearForm();
    switchToFormView();
}

void ProductPage::showUpdateProductForm()
{
    QModelIndex currentIndex = ui->productTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen güncellenecek ürünü seçin."));
        return;
    }

    isNewProduct = false;

    // Seçili ürün verilerini forma doldur
    ui->productCodeEdit->setText(model->data(model->index(currentIndex.row(), 1)).toString());
    ui->productNameEdit->setText(model->data(model->index(currentIndex.row(), 2)).toString());
    ui->categoryCombo->setCurrentText(model->data(model->index(currentIndex.row(), 3)).toString());
    ui->priceSpinBox->setValue(model->data(model->index(currentIndex.row(), 4)).toDouble());
    ui->stockSpinBox->setValue(model->data(model->index(currentIndex.row(), 5)).toInt());
    ui->statusCombo->setCurrentText(model->data(model->index(currentIndex.row(), 6)).toString());
    ui->depotIDEdit->setText(model->data(model->index(currentIndex.row(), 7)).toString());  // Line edit için

    switchToFormView();
}
void ProductPage::saveProduct()
{
    if (ui->productCodeEdit->text().isEmpty() || ui->productNameEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen zorunlu alanları doldurun!"));
        return;
    }

    if (isNewProduct) {
        QSqlRecord record = model->record();
        record.remove(record.indexOf("ProductID"));

        record.setValue("ProductCode", ui->productCodeEdit->text());
        record.setValue("ProductName", ui->productNameEdit->text());
        record.setValue("Category", ui->categoryCombo->currentText());
        record.setValue("Price", ui->priceSpinBox->value());
        record.setValue("Stock", ui->stockSpinBox->value());
        record.setValue("Status", ui->statusCombo->currentText());
        record.setValue("DepotID", ui->depotIDEdit->text().toInt());  // Line edit'ten değeri al
        record.setValue("CreatedAt", QDateTime::currentDateTime());

        if (!model->insertRecord(-1, record)) {
            QMessageBox::critical(this, tr("Hata"), tr("Ürün eklenemedi: ") + model->lastError().text());
            return;
        }
    } else {
        QModelIndex currentIndex = ui->productTableView->currentIndex();
        model->setData(model->index(currentIndex.row(), 1), ui->productCodeEdit->text());
        model->setData(model->index(currentIndex.row(), 2), ui->productNameEdit->text());
        model->setData(model->index(currentIndex.row(), 3), ui->categoryCombo->currentText());
        model->setData(model->index(currentIndex.row(), 4), ui->priceSpinBox->value());
        model->setData(model->index(currentIndex.row(), 5), ui->stockSpinBox->value());
        model->setData(model->index(currentIndex.row(), 6), ui->statusCombo->currentText());
        model->setData(model->index(currentIndex.row(), 7), ui->depotIDEdit->text().toInt());  // Line edit'ten değeri al
    }

    if (!model->submitAll()) {
        QMessageBox::critical(this, tr("Hata"), tr("Değişiklikler kaydedilemedi: ") + model->lastError().text());
        model->revertAll();
        return;
    }

    loadProductData();
    switchToMainView();
}
void ProductPage::deleteProduct()
{
    qDebug() << "deleteProduct called - userType:" << userType;

    // Yetki kontrolü
    if (userType == "Çalışan") {
        qDebug() << "Delete operation blocked - Employee user";
        QMessageBox::warning(this, tr("Yetki Hatası"),
                             tr("Çalışan kullanıcılar ürün silme yetkisine sahip değildir!"));
        return;
    }

    QModelIndex currentIndex = ui->productTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen silinecek ürünü seçin."));
        return;
    }

    QString productName = model->data(model->index(currentIndex.row(), 2)).toString();
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Ürün Sil"),
                                                              tr("\"%1\" ürününü silmek istediğinize emin misiniz?").arg(productName),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (!model->removeRow(currentIndex.row())) {
        QMessageBox::critical(this, tr("Hata"), tr("Ürün silinemedi: ") + model->lastError().text());
        return;
    }

    if (!model->submitAll()) {
        QMessageBox::critical(this, tr("Hata"), tr("Değişiklikler kaydedilemedi: ") + model->lastError().text());
        model->revertAll();
        return;
    }

    loadProductData();
}

void ProductPage::clearForm()
{
    ui->productCodeEdit->clear();
    ui->productNameEdit->clear();
    ui->categoryCombo->setCurrentIndex(0);
    ui->priceSpinBox->setValue(0.0);
    ui->stockSpinBox->setValue(0);
    ui->statusCombo->setCurrentIndex(0);
    ui->depotIDEdit->clear();  // Line edit'i temizle
}

void ProductPage::cancelForm()
{
    switchToMainView();
}

void ProductPage::switchToMainView()
{
    ui->stackedWidget->setCurrentIndex(0); // Ana sayfa
    clearForm();
}

void ProductPage::switchToFormView()
{
    ui->stackedWidget->setCurrentIndex(1); // Form sayfası
}
