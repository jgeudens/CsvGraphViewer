#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QObject>

class ParserModel : public QObject
{
    Q_OBJECT
public:
    explicit ParserModel(QObject *parent = 0);
    ~ParserModel();

    void triggerUpdate(void);

    QString path();
    bool dynamicSession() const;
    QChar fieldSeparator() const;
    QChar groupSeparator() const;
    QChar decimalSeparator() const;
    QString commentSequence() const;
    quint32 dataRow() const;
    quint32 column() const;
    qint32 labelRow() const;
    
    void setPath(QString path);
    void setFieldSeparator(QChar fieldSeparator);
    void setGroupSeparator(QChar groupSeparator);
    void setDecimalSeparator(QChar decimalSeparator);
    void setCommentSequence(QString commentSequence);
    void setDataRow(quint32 dataRow);
    void setColumn(quint32 column);
    void setLabelRow(qint32 labelRow);

signals:
    void dynamicSessionChanged();
    void pathChanged();
    void fieldSeparatorChanged();
    void groupSeparatorChanged();
    void decimalSeparatorChanged();
    void commentSequenceChanged();
    void dataRowChanged();
    void columnChanged();
    void labelRowChanged();

public slots:
    void setDynamicSession(bool bDynamicSession);

private:

    QString _path;
    bool _bDynamicSession;
    QChar _fieldSeparator;
    QChar _groupSeparator;
    QChar _decimalSeparator;
    QString _commentSequence;
    quint32 _dataRow;
    quint32 _column;
    qint32 _labelRow;

};

#endif // SETTINGSMODEL_H
