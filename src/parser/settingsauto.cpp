#include <QFile>
#include <QLocale>

#include <QRegularExpression>

#include "settingsauto.h"
#include "util.h"

const QString SettingsAuto::_cNotANumber = QString("nan");

SettingsAuto::SettingsAuto()
{
}

bool SettingsAuto::updateSettings(QStringList previewData)
{
    bool bRet = true;

    _column = 0;

    // Find first non-comment line
    qint32 lineIdx = 0;
    lineIdx = nextDataLine(0, previewData, &bRet);

    if (bRet)
    {
        _labelRow = lineIdx;

        // find next dataline
        lineIdx++;
        lineIdx = nextDataLine(lineIdx, previewData, &bRet);
    }

    if (bRet)
    {
        // Set data row
        _dataRow = lineIdx;

        /*
         * Different locales
         * ------------------
         * French (fr_FR):      4 294 967 295,000
         * Italian (it_IT):     4.294.967.295,000
         * US-English (en_US):  4,294,967,295.00
         */
        QLocale frLocale(QLocale("fr_FR"));
        QLocale itLocale(QLocale("it_IT"));
        QLocale enLocale(QLocale("en_US"));

        // First try French locale
        bool bParse;
        bParse = parseFields(previewData, frLocale, ';');

        if (!bParse)
        {
            // Else try Italian locale
            bParse = parseFields(previewData, itLocale, ';');

            if (!bParse)
            {
                // Else try US English locale
                bRet = parseFields(previewData, enLocale, ',');
            }
        }
    }
    else
    {
        bRet = false;
    }

    return bRet;
}


QChar SettingsAuto::fieldSeparator()
{
    return _fieldSeparator;
}

QChar SettingsAuto::groupSeparator()
{
    return _groupSeparator;
}

QChar SettingsAuto::decimalSeparator()
{
    return _decimalSeparator;
}

QString SettingsAuto::commentSequence()
{
    return _commentSequence;
}

quint32 SettingsAuto::dataRow()
{
    return _dataRow;
}

quint32 SettingsAuto::column()
{
    return _column;
}

qint32 SettingsAuto::labelRow()
{
    return _labelRow;
}

bool SettingsAuto::timeInMilliSeconds()
{
    return _bTimeInMilliSeconds;
}

bool SettingsAuto::isAbsoluteDate(QString rawData)
{

    QRegularExpression re("\\d{2,4}.*\\d{2}.*\\d{2,4}\\s*\\d{1,2}:\\d{1,2}:\\d{1,2}.*");
    QRegularExpressionMatch match = re.match(rawData);

    return match.hasMatch();
}

bool SettingsAuto::isNaN(QString rawData)
{
    return rawData.trimmed().toLower() == _cNotANumber;
}

bool SettingsAuto::isComment(QString line)
{
    bool bRet = false;
    if (_commentSequence.isEmpty())
    {
        // Check first character for comment char
        if (!line.at(0).isLetterOrNumber())
        {
            if (line.at(0) == QChar('-'))
            {
                // Minus sign can only be a comment sign when there are 2
                if (line.at(1) == QChar('-'))
                {
                    _commentSequence = line.left(2);
                    bRet = true;
                }
            }
            else
            {
                // Check second character
                if (
                    (!line.at(1).isLetterOrNumber())
                    && (!line.at(1).isSpace())
                    )
                {
                    _commentSequence = line.left(2);
                    bRet = true;
                }
                else
                {
                    _commentSequence = line.left(1);
                    bRet = true;
                }
            }
        }
    }
    else
    {
        if (line.left(_commentSequence.size()) == _commentSequence)
        {
            bRet = true;
        }
    }

    return bRet;
}

bool SettingsAuto::parseFields(QStringList previewData, QLocale locale, QChar fieldSeparator)
{
    quint32 aSeparatorCount[2];
    aSeparatorCount[0] = previewData[_labelRow].count(fieldSeparator);
    aSeparatorCount[1] = previewData[_dataRow].count(fieldSeparator);

    if (aSeparatorCount[0] == aSeparatorCount[1])
    {
        qint32 parseIdx;
        for (parseIdx = _dataRow; parseIdx < previewData.size(); parseIdx++)
        {
            if (!isComment(previewData[parseIdx]))
            {
                QStringList fields = previewData[parseIdx].split(fieldSeparator);

                for (qint32 idx = 1; idx < fields.size(); idx++)
                {
                    const QString field = fields[idx];

                    if (isAbsoluteDate(field))
                    {
                        _column = idx;
                    }
                    else if (isNaN(field))
                    {
                        /* Ignore data */
                    }
                    else
                    {
                        bool bOk;
                        locale.toDouble(field, &bOk);
                        if (!bOk)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    else
    {
        return false;
    }


    // If first time field is between 0 and 1, then presume in seconds
    QString firstTimeField = previewData[_dataRow].split(fieldSeparator)[0];
    if (
        (locale.toDouble(firstTimeField) > 0)
        && (locale.toDouble(firstTimeField) < 1)
        )
    {
        _bTimeInMilliSeconds = false;
    }
    else
    {
        _bTimeInMilliSeconds = true;
    }

    _fieldSeparator = fieldSeparator;
    _decimalSeparator = locale.decimalPoint();

    // In US locale field separator is the same as group (thousands) point
    if (fieldSeparator == locale.groupSeparator())
    {
        _groupSeparator = ' ';
    }
    else if (locale.groupSeparator() == 160) // Small space
    {
        _groupSeparator = ' ';
    }
    else
    {
       _groupSeparator = locale.groupSeparator();
    }

    return true;
}

quint32 SettingsAuto::nextDataLine(quint32 startIdx, QStringList previewData, bool *bOk)
{
    qint32 lineIdx;
    for (lineIdx = startIdx; lineIdx < previewData.size(); lineIdx++)
    {
        QString line = previewData[lineIdx].trimmed();
        if (!line.isEmpty())
        {
            if (!isComment(line))
            {
                break;
            }
        }
    }

    if (lineIdx < previewData.size())
    {
        *bOk = true;
    }
    else
    {
        *bOk = false;
    }

    return lineIdx;
}


