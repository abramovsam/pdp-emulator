#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlTableModel>
#include <QVariant>
//#include "process.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);
//    void setSharedMem(SharedMem *sharedMem);
    void headerDoubleClick(QModelIndex index);
    int rowCount();

signals:

public slots:

private:
 //   SharedMem *sharedMem;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

};



#endif // TABLEMODEL_H
