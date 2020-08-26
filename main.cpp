/*
 * Copyright 2020 LRFLEW
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "levelwindow.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[]) {
    QApplication::setOrganizationName(QStringLiteral("LRFLEW"));
    QApplication::setOrganizationDomain(QStringLiteral("lrflew.com"));
    QApplication::setApplicationName(QStringLiteral("Perfect Park IL Tool"));
    QApplication a(argc, argv);
    LevelWindow w;
    w.show();
    return a.exec();
}
