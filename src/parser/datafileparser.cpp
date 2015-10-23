#include <QtWidgets>

#include "util.h"
#include "datafileparser.h"

DataFileParser::DataFileParser(ParserModel *pParserModel)
{
    _pParserModel = pParserModel;
    _fileContentsEnd = 0;
    _fileEndPos = 0;
}

DataFileParser::~DataFileParser()
{
}

bool DataFileParser::forceProcessDataFile()
{
    _dataLabels.clear();
    _dataRows.clear();

    _fileEndPos = 0;
    _fileContentsEnd = 0;
    _fileContents.clear();

    return processDataFile();
}

bool DataFileParser::processDataFile()
{
    bool bRet = true;

    // Load data file
    bRet = loadDataFile();

    // read data structure, labels when on first load
    if (_dataRows.isEmpty())
    {
        if (bRet)
        {
            // Get number of rows (from dataRow)
            _expectedFields = _fileContents[_pParserModel->dataRow()].split(_pParserModel->fieldSeparator()).size() - _pParserModel->column();

            // Check number of expected fields
            if (_expectedFields < 2)
            {
                Util::showError(tr("Can't parse data. Are you sure field separator and column are correct?"));
                bRet = false;
            }
        }

        if (bRet)
        {
            bRet = readLabels();
        }

        if (bRet)
        {
            // Init data row QLists to empty list
            QList<double> t;
            for (qint32 i = 0; i < _expectedFields; i++)
            {
                _dataRows.append(t);
            }
        }

        if (bRet)
        {
            _fileContentsEnd = _pParserModel->dataRow();
        }
    }

    if (bRet)
    {
        bRet = readData();
    }

    return bRet;
}

QList<QList<double> > & DataFileParser::getDataRows()
{
    return _dataRows;
}

QStringList & DataFileParser::getDataLabels()
{
    return _dataLabels;
}

bool DataFileParser::readData()
{
    bool bRet = true;

    for (qint32 index = _fileContentsEnd; index < _fileContents.size(); index++)
    {
        // ignore empty lines and comment lines
        if(
            (!_fileContents[index].simplified().isEmpty())
            && (!IsCommentLine(_fileContents[index]))
          )
        {
            QStringList paramList = _fileContents[index].split(_pParserModel->fieldSeparator());
            if ((paramList.size() - (qint32)_pParserModel->column()) != _expectedFields)
            {
                QString txt = QString(tr("The number of label columns doesn't match number of data columns (while checking data: line %1).")).arg(index + 1);
                Util::showError(txt);
                bRet = false;
                break;
            }

            for (qint32 i = _pParserModel->column(); i < paramList.size(); i++)
            {
                bool bError = false;

                // Remove group separator
                QString tmpData = paramList[i].simplified().replace(_pParserModel->groupSeparator(), "");

                // Replace decimal point if needed
                if (QLocale::system().decimalPoint() != _pParserModel->decimalSeparator())
                {
                    tmpData = tmpData.replace(_pParserModel->decimalSeparator(), QLocale::system().decimalPoint());
                }

                double number = QLocale::system().toDouble(tmpData, &bError);
                if (tmpData.simplified().isEmpty())
                {
                    number = 0;
                    bError = true;
                }
                else
                {
                    number = QLocale::system().toDouble(tmpData, &bError);
                }

                if (bError == false)
                {
                    QString error = QString(tr("Invalid data (while processing data)\n\n Line %1:\n\"%2\"").arg(index + 1).arg(_fileContents[index]));
                    Util::showError(error);
                    bRet = false;
                    break;
                }
                else
                {
                    _dataRows[i - _pParserModel->column()].append(number);
                }
            }
        }

        // Make sure we break the loop on an error
        if (bRet == false)
        {
            break;
        }

    }

    _fileContentsEnd = _dataRows[0].size();

    return bRet;
}

bool DataFileParser::loadDataFile()
{
    bool bRet = true;
    QString line;
    QFile file(_pParserModel->path());

    /* If we can't open it, let's show an error message. */
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Go to last read position in datafile
        file.seek(_fileEndPos);

        do
        {
            bool bResult = readLineFromFile(&file, &line);
            if(bResult)
            {
                _fileContents.append(line);
            }
            else
            {
                break;
            }
        } while(!file.atEnd());

        if (file.atEnd())
        {
            _fileEndPos = file.pos();
        }
        else
        {
            Util::showError(tr("Error while reading data file: %1").arg(_pParserModel->path()));
            bRet = false;
        }
    }
    else
    {
        Util::showError(tr("Couldn't open data file: %1").arg(_pParserModel->path()));
        bRet = false;
    }

    return bRet;
}

bool DataFileParser::readLabels()
{
   bool bRet = true;

    //Read labels
    QStringList tmpLabels;

    if (_pParserModel->labelRow() != -1)
    {
        tmpLabels = _fileContents[_pParserModel->labelRow()].split(_pParserModel->fieldSeparator());
        if ((tmpLabels.size() - (qint32)_pParserModel->column()) != _expectedFields)
        {
            Util::showError(tr("The number of label columns doesn't match number of data columns (while checking labels)."));
            bRet = false;
        }
    }
    else
    {
        for (qint32 i = 0; i < ((qint32)_pParserModel->column() + _expectedFields); i++)
        {
            tmpLabels.append(QString(""));
        }
        tmpLabels[_pParserModel->column()] = tr("Time");
    }


    if (bRet)
    {
        // Trim white spaces of labels
        for (qint32 i = 0; i < tmpLabels.size(); i++)
        {
            tmpLabels[i] = tmpLabels[i].trimmed();

            if (tmpLabels[i].isEmpty())
            {
                tmpLabels[i] = QString(tr("Unknown column %1")).arg(i);
            }
        }
    }

    // Process labels
    if (bRet)
    {
        for (qint32 i = _pParserModel->column(); i < tmpLabels.size(); i++)
        {
            _dataLabels.append(tmpLabels[i]);
        }
    }

    return bRet;
}


// Return false on error
bool DataFileParser::readLineFromFile(QFile * file, QString *pLine)
{
    bool bRet = false;
    char buf[2048];
    qint32 lineLength;

    // Read first line of data (labels)
    lineLength = file->readLine(buf, sizeof(buf));
    if (lineLength > 0)
    {
        bRet = true;
        *pLine = QString(buf);
    }

    return bRet;
}

bool DataFileParser::IsCommentLine(QString line)
{
    bool bRet = false;

    const QString commentSequence = _pParserModel->commentSequence();
    if (!commentSequence.isEmpty())
    {
        if (line.trimmed().left(commentSequence.length()) == commentSequence)
        {
            bRet = true;
        }
    }

    return bRet;
}


