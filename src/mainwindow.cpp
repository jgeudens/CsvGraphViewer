
#include "datatypes.h"
#include "datafileparser.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::_cWindowTitle = QString("CsvGraphViewer");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _graphViewer(NULL)
{
    _ui->setupUi(this);

    this->setWindowTitle(_cWindowTitle);
    
    _graphViewer = new GraphViewer(_ui->customPlot, this);

    connect(_ui->actionLoadDataFile, SIGNAL(triggered()), this, SLOT(getDataFileSettings()));
    connect(_ui->actionReloadDataFile, SIGNAL(triggered()), this, SLOT(reloadDataFile()));
    connect(_ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
    connect(_ui->actionExportImage, SIGNAL(triggered()), this, SLOT(prepareImageExport()));
    connect(_ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::getDataFileSettings()
{
    Dialog dialog;

    if (dialog.exec())
    {
        dialog.getDataSettings(&_dataFileSettings);

        if (dialog.result() == QDialog::Accepted)
        {
            _parser.setDataFileSettings(_dataFileSettings);

            updateGraph();
        }
    }
}

void MainWindow::updateGraph()
{
    if (_parser.loadDataFile())
    {
        // TODO
        QList<QList<double> > data;
        QStringList labels;

        if (_parser.parseData(data, labels))
        {
            _graphViewer->setupGraph(&data, &labels);

            setWindowTitle(QString(tr("%1 - %2")).arg(_cWindowTitle, QFileInfo(_dataFileSettings.path).fileName()));

            _ui->actionReloadDataFile->setEnabled(true);
            _ui->actionExportImage->setEnabled(true);
        }
    }
}

void MainWindow::exitApplication()
{
    QApplication::quit();
}

void MainWindow::reloadDataFile()
{
    updateGraph();
}

void MainWindow::prepareImageExport()
{
    QString filePath;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setOption(QFileDialog::HideNameFilterDetails, false);
    dialog.setDefaultSuffix("png");
    dialog.setWindowTitle(tr("Select png file"));
    dialog.setNameFilter(tr("PNG files (*.png)"));

    QStringList docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (docPath.size() > 0)
    {
        dialog.setDirectory(docPath[0]);
    }

    if (dialog.exec())
    {
        filePath = dialog.selectedFiles().first();
        _graphViewer->exportGraphImage(filePath);
    }
}

void MainWindow::showAbout()
{
    QString lnkAuthor("<a href='mailto:jensgeudens@hotmail.com'>jgeudens</a>");
    QString lnkGpl("<a href='http://www.gnu.org/licenses/gpl.html#content'>GPL</a>");
    QString lnkGitHub("<a href='https://github.com/jgeudens/CsvGraphViewer'>GitHub</a>");

    QString lnkQt("<a href='http://qt-project.org/'>Qt</a>");
    QString lnkQCustomPlot("<a href='http://www.qcustomplot.com/'>QCustomPlot</a>");

    QString version = QString(tr("<b>CsvGraphViewer v%1</b><br><br>")).arg(APP_VERSION);

    QString aboutTxt = tr(
                        "%1"
                        "CsvGraphViewer is created and maintained by %2. This software is released under the %3 license. "
                        "The source is freely available at %4.<br><br>"
                        "CsvGraphViewer uses following libraries:<br>"
                        "%5<br>"
                        "%6<br>").arg(version, lnkAuthor, lnkGpl, lnkGitHub, lnkQt, lnkQCustomPlot);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About");
    msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msgBox.setText(aboutTxt);
    msgBox.exec();
}
