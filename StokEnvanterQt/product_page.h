#ifndef PRODUCT_PAGE_H
#define PRODUCT_PAGE_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include "database_manager.h"

namespace Ui {
class ProductPage;
}

class ProductPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProductPage(QWidget *parent = nullptr, const QString &userType = "Admin");
    ~ProductPage();

private slots:
    void loadProductData();   // Ürün verilerini yükle
    void showAddProductForm();       // Ürün ekleme formunu göster
    void showUpdateProductForm();    // Ürün güncelleme formunu göster
    void saveProduct();              // Ürün kaydet
    void deleteProduct();            // Ürün sil
    void clearForm();               // Form alanlarını temizle
    void cancelForm();              // Formu iptal et
    void switchToMainView();        // Ana görünüme dön
    void switchToFormView();        // Form görünümüne geç

private:
    Ui::ProductPage *ui;
    QSqlTableModel *model;
    DatabaseManager dbManager;
    bool isNewProduct;  // Yeni ürün mü güncelleme mi kontrolü

    void setupModel();        // Model ayarları
    void setupConnections();  // Sinyal-slot bağlantıları
    void setupValidators();   // Form doğrulama kuralları
    void setupComboBoxes();   // Kombo kutularını doldur

    QString userType;
};

#endif // PRODUCT_PAGE_H
