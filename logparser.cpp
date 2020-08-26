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

#include "logparser.h"

#include "records.h"

#include <cstdlib>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLatin1String>
#include <QMessageBox>

#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fcntl.h>
#include <io.h>
#include <string>
#endif

#if defined(Q_OS_WIN)
#define LOGDIR "AppData/LocalLow/tjern/perfect-park"
#else
#error "Unsupported Platform"
#endif

#define LOGPATH LOGDIR "/SPDRUN-LOG.txt"

static bool openSharedFile(QFile &file, const QString &path) {
    file.setFileName(path);
#if defined(Q_OS_WIN)
    std::wstring pathw = path.toStdWString();
    HANDLE handle = CreateFileW(pathw.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) return false;
    int fd = _open_osfhandle(reinterpret_cast<intptr_t>(handle), _O_RDONLY);
    if (fd == -1) return false;
    return file.open(fd, QIODevice::ReadOnly | QIODevice::Text, QFileDevice::AutoCloseHandle);
#else
    return file.open(QIODevice::ReadOnly | QIODevice::Text);
#endif
}

LogParser::LogParser(QObject *parent) : QObject(parent), lastSize(0) {
    connect(&fsw, &QFileSystemWatcher::fileChanged, [this](const QString &) { this->readLog(); });
    QDir home = QDir::home();
    home.mkpath(QStringLiteral(LOGDIR));
    QString path = home.filePath(QStringLiteral(LOGPATH));
    bool good = openSharedFile(file, path);
    if (!good) {
        QMessageBox msg(QMessageBox::Critical, QStringLiteral("Error Reading Perfect Park Log"),
                        QStringLiteral("Unable to open the log file for Perfect Park. This is unexpected."));
        msg.exec();
        std::exit(1);
    } else {
        file.seek(file.size());
        fsw.addPath(path);
    }
}

void LogParser::readLog() {
    // Check for file reset
    qint64 size = file.size();
    if (size < lastSize || size <= 20) file.seek(0);
    lastSize = size;

    // Read Data
    for(QString s = file.readLine(); !s.isEmpty(); s = file.readLine()) {
        try {
            if (s.startsWith(QLatin1String("[START] -"))) {
                Maps map = Records::toMap(s.midRef(9));
                qDebug() << "Start Map" << Records::toString(map);
                emit mapChange(map);
            } else if (s.startsWith(QLatin1String("[FINISH] -"))) {
                int ind = s.indexOf('-', 10);
                if (ind >= 0) {
                    Maps map = Records::toMap(s.midRef(10, ind - 10));
                    quint32 time = Records::toTime(s.midRef(ind + 1));
                    qDebug() << "Finish Map" << Records::toString(map) << Records::toString(time);
                    emit mapFinish(map, time);
                }
            }
        } catch (const std::exception &e) {
            qDebug() << s << e.what();
        }
    }
}
