/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PHRASEMODEL_H
#define PHRASEMODEL_H

#include "phrase.h"

#include <QList>
#include <QAbstractItemModel>

QT_BEGIN_NAMESPACE

class PhraseModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    PhraseModel(QObject *parent = 0)
        : QAbstractTableModel(parent)
    {}

    void removePhrases();
    QList<Phrase *> phraseList() const {return plist;}

    QModelIndex addPhrase(Phrase *p);
    void removePhrase(const QModelIndex &index);

    Phrase *phrase(const QModelIndex &index) const;
    void setPhrase(const QModelIndex &indx, Phrase *ph);
    QModelIndex index(Phrase * const phr) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    { return QAbstractTableModel::index(row, column, parent); }

    // from qabstracttablemodel
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    // HACK: This model will be displayed in a _TreeView_
    // which has a tendency to expand 'children' on double click
    bool hasChildren(const QModelIndex &parent) const override
    { return !parent.isValid(); }

private:
    QList<Phrase *> plist;
};

QT_END_NAMESPACE

#endif // PHRASEMODEL_H
