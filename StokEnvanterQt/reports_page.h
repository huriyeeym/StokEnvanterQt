#ifndef REPORTS_PAGE_H
#define REPORTS_PAGE_H

#include <QWidget>
#include <QTableView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTabWidget>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include "database_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReportsPage; }
QT_END_NAMESPACE

class ReportsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsPage(QWidget *parent = nullptr);
    ~ReportsPage();

private slots:
    // Stok Raporu için
    void generateStockReport();
    void updateStockTable();
    void exportStockToExcel();
    void exportStockToPDF();

    // Depo Raporu için
    void generateDepotReport();
    void updateDepotTable();
    void exportDepotToExcel();
    void exportDepotToPDF();

    // Finansal Rapor için
    void generateFinancialReport();
    void updateFinancialTable();
    void exportFinancialToExcel();
    void exportFinancialToPDF();
    void onFinanceTypeChanged(const QString& type);

private:
    void setupUI();
    void setupConnections();
    void setupModels();

    // Yardımcı fonksiyonlar
    void exportToExcel(QTableView* table, const QString& filename);
    void exportToPDF(QTableView* table, const QString& filename);
    QString getSaveFilePath(const QString& filter);

    Ui::ReportsPage *ui;
    DatabaseManager dbManager;

    // TableModel'ler
    QSqlTableModel *stockModel;
    QSqlTableModel *depotModel;
    QSqlTableModel *financialModel;
};

#endif // REPORTS_PAGE_H
