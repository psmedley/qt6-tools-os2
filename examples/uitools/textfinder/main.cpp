// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "textfinder.h"

#include <QApplication>

//! [0]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TextFinder textFinder;
    textFinder.show();

    return app.exec();
}
//! [0]
