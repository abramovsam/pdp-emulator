#include "tablemodel.h"
#include "formain.h"
#include <stdio.h>

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}
/*
void TableModel::setSharedMem(SharedMem *sharedMem) {
    this->sharedMem = sharedMem;
}
*/
int TableModel::rowCount(const QModelIndex &parent) const {
    return 8; // сделаем фиксированно 8 строк в таблице
}

int TableModel::columnCount(const QModelIndex &parent) const {
    return 3; // количество колонок сделаем также фиксированным
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    /*
    if (role == Qt::DisplayRole) {
        QString unswer;
        if (index.column() == 0) {
            if (sharedMem->asmCommand[index.row()].breakePointIsSet)
                unswer = "     X";
            else
                unswer = "";
        } else if (index.column() == 1) {
            char str[100];
            sprintf(str, "%x", sharedMem->asmCommand[index.row()].address);
            unswer = "0x" + QString(str);
        } else {
            unswer = QString(sharedMem->asmCommand[index.row()].command);
        }
        return QVariant(unswer);
    }
    */
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("BP");
            case 1:
                return QString("Address");
            case 2:
                return QString("Command");
            }
        }
    }
    return QVariant();
}

void TableModel::headerDoubleClick(QModelIndex index) {
    /*
    sharedMem->asmCommand[index.row()].breakePointIsSet =
            !sharedMem->asmCommand[index.row()].breakePointIsSet;
    */
}



