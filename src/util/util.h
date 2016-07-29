#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QMessageBox>
#include <QLocale>

class Util : public QObject
{
    Q_OBJECT

public:

    static const quint32 cCurrentDataLevel = 1;

    static void showError(QString text)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("GraphViewer"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(text);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    static quint32 currentDataLevel()
    {
        return cCurrentDataLevel;
    }

    static const QString & currentVersion()
    {
        static const QString version(APP_VERSION);
        return version;
    }

    static QString formatDoubleForExport(double number)
    {
        QString tmp;

        // Round number to 3 decimals and remove group separator
        tmp = QLocale::system().toString(number, 'f', 3).replace(QLocale::system().groupSeparator(), "");

        // convert back to double
        double doubleTmp = QLocale::system().toDouble(tmp);

        // Make sure trailing zeros are removed and remove group separator
        tmp = QLocale::system().toString(doubleTmp, 'g', 9).replace(QLocale::system().groupSeparator(), "");

        return tmp;
    }

    static QString formatTime(qint64 tickKey, bool bSmallScale)
    {
        QString tickLabel;

        if (bSmallScale)
        {
            tickLabel = QString("%1").arg(tickKey);
        }
        else
        {
            bool bNegative;
            quint64 tmp;

            if (tickKey < 0)
            {
                bNegative = true;
                tmp = -1 * tickKey;
            }
            else
            {
                bNegative = false;
                tmp = tickKey;
            }

            tmp %= 24 * 60 * 60 * 1000; // Number of seconds in a day

            quint32 hours = tmp / (60 * 60 * 1000);
            tmp = tmp % (60 * 60 * 1000);

            quint32 minutes = tmp / (60 * 1000);
            tmp = tmp % (60 * 1000);

            quint32 seconds = tmp / 1000;
            quint32 milliseconds = tmp % 1000;

            tickLabel = QString("%1:%2:%3%4%5").arg(hours)
                                                        .arg(minutes, 2, 10, QChar('0'))
                                                        .arg(seconds, 2, 10, QChar('0'))
                                                        .arg(QLocale::system().decimalPoint())
                                                       .arg(milliseconds, 3, 10, QChar('0'));

            // Make sure minus sign is shown when tick number is negative
            if (bNegative)
            {
                tickLabel = "-" + tickLabel;
            }
        }

        return tickLabel;
    }

    static QString formatTimeDiff(qint64 tickKeyDiff)
    {
        QString tickLabel;
        bool bNegative;
        quint64 tmp;

        if (tickKeyDiff < 0)
        {
            bNegative = true;
            tmp = -1 * tickKeyDiff;
        }
        else
        {
            bNegative = false;
            tmp = tickKeyDiff;
        }

        tmp %= 24 * 60 * 60 * 1000; // Number of seconds in a day

        quint32 hours = tmp / (60 * 60 * 1000);
        tmp = tmp % (60 * 60 * 1000);

        quint32 minutes = tmp / (60 * 1000);
        tmp = tmp % (60 * 1000);

        quint32 seconds = tmp / 1000;
        quint32 milliseconds = tmp % 1000;

        if (
            (hours == 0)
            && (minutes == 0)
            )
        {
            tickLabel = QString("%1%2%3").arg(seconds, 1, 10, QChar('0'))
                                                       .arg(QLocale::system().decimalPoint())
                                                       .arg(milliseconds, 3, 10, QChar('0'));
        }
        else
        {
            tickLabel = QString("%1:%2:%3%4%5").arg(hours)
                                                        .arg(minutes, 2, 10, QChar('0'))
                                                        .arg(seconds, 2, 10, QChar('0'))
                                                        .arg(QLocale::system().decimalPoint())
                                                        .arg(milliseconds, 3, 10, QChar('0'));
        }


        // Make sure minus sign is shown when tick number is negative
        if (bNegative)
        {
            tickLabel = "-" + tickLabel;
        }

        return tickLabel;
    }
    
    static const QList<QColor> cColorlist;

private:

};


#endif // UTIL_H
