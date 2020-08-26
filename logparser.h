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

#ifndef LOGPARSER_H
#define LOGPARSER_H

#include "maps.h"

#include <QtGlobal>
#include <QFile>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>

class LogParser : public QObject {
    Q_OBJECT
public:
    explicit LogParser(QObject *parent = nullptr);

signals:
    void mapChange(Maps map);
    void mapFinish(Maps map, quint32 time);

private:
    void readLog();

private:
    QFile file;
    QFileSystemWatcher fsw;

    qint64 lastSize;
};

#endif // LOGPARSER_H
