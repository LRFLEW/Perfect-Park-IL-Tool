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

#ifndef RECORDS_H
#define RECORDS_H

#include "maps.h"

#include <array>

#include <QtGlobal>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringView>

class Records : public QObject {
    Q_OBJECT
public:
    Records(QObject *parent = nullptr);

    quint32 getPB(Maps map) const { return pbs[static_cast<int>(map)]; }
    quint32 getWR(Maps map) const { return wrs[static_cast<int>(map)]; }
    QString getPBStr(Maps map, bool dashes = true) const { return toString(getPB(map), dashes); }
    QString getWRStr(Maps map, bool dashes = true) const { return toString(getWR(map), dashes); }

public slots:
    void loadPBs();
    void loadWRs();

    void setPB(Maps map, quint32 time);
    void newTime(Maps map, quint32 time);

signals:
    void updatedWR(Maps map);
    void updatedPB(Maps map);
    void newPB(bool isWR);

private:
    void netFinished(Maps map, QNetworkReply *reply);

public:
    static Maps toMap(QStringView map);
    static Maps toMap(QLatin1String map);

    static quint32 toTime(double time);
    static quint32 toTime(QStringView time);
    static quint32 toTime(QLatin1String time);

    static const QString &toString(Maps map);
    static QString toString(quint32 time, bool dashes = true);

private:
    QNetworkAccessManager net;
    QSettings settings;

    std::array<quint32, MapsCount> pbs{};
    std::array<quint32, MapsCount> wrs{};
};

#endif // RECORDS_H
