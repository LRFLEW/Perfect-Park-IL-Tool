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

#include "records.h"

#include <limits>
#include <stdexcept>

#include <QDebug>
#include <QMetaObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QStringList>

static const QStringList names {
    QStringLiteral("Gates & Fortress"),
    QStringLiteral("Flowers"),
    QStringLiteral("Mist"),
    QStringLiteral("The Climb"),
    QStringLiteral("Pillars"),
    QStringLiteral("Float"),
    QStringLiteral("Ascent"),
};

static const QStringList shortNames {
    QStringLiteral("gates"),
    QStringLiteral("flowers"),
    QStringLiteral("mist"),
    QStringLiteral("climb"),
    QStringLiteral("pillars"),
    QStringLiteral("float"),
    QStringLiteral("ascent"),
};

static const QStringList speedrunIds {
    QStringLiteral("495mvj3w"),
    QStringLiteral("rdqn602d"),
    QStringLiteral("5d72636w"),
    QStringLiteral("kwjvoe7w"),
    QStringLiteral("owo3g2kw"),
    QStringLiteral("xd14v55d"),
    QStringLiteral("ewp8kqj9"),
};

Records::Records(QObject *parent) : QObject(parent) {
    // guarunteed to happen in the objects thread as it defaults to the constructing thread
    loadPBs();
    loadWRs();
}

void Records::loadPBs() {
    for (int i=0; i < MapsCount; ++i) {
        pbs[i] = settings.value(QStringLiteral("pb/%1").arg(shortNames[i]), 0u).toUInt();
        wrs[i] = settings.value(QStringLiteral("wr/%1").arg(shortNames[i]), 0u).toUInt();
    }
}

void Records::loadWRs() {
    for (int i=0; i < MapsCount; ++i) {
        const Maps map = static_cast<Maps>(i);
        QNetworkRequest req(QStringLiteral("https://www.speedrun.com/api/v1/leaderboards/yd4oenk1/level/%1/9d833y32?top=1")
                            .arg(speedrunIds[i]));
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);
        req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Perfect Park IL Tool/" PPT_VERSION));
        req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
        QNetworkReply *res = net.get(req);
        connect(res, &QNetworkReply::finished, [this, map, res]() { this->netFinished(map, res); });
    }
}

void Records::setPB(Maps map, quint32 time) {
    const int i = static_cast<int>(map);
    pbs[i] = time;
    // ensure the update happens in the object's thread
    QMetaObject::invokeMethod(this, [this, i](){
        settings.setValue(QStringLiteral("pb/%1").arg(shortNames[i]), pbs[i]);
    });
    emit updatedPB(map);
}

void Records::newTime(Maps map, quint32 time) {
    const int i = static_cast<int>(map);
    if (pbs[i] == 0 || time < pbs[i]) {
        setPB(map, time);
        emit newPB(time < wrs[i]);
    }
}

void Records::netFinished(Maps map, QNetworkReply *res) {
    const int i = static_cast<int>(map);
    res->deleteLater();
    if (res->error() != QNetworkReply::NoError) {
        qDebug() << names[i] << res->errorString();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(res->readAll());
    double time = json[QLatin1String("data")][QLatin1String("runs")][0][QLatin1String("run")]
            [QLatin1String("times")][QLatin1String("primary_t")].toDouble();
    if (time <= 0.0) {
        qDebug() << names[i] << QStringLiteral("Unexpected speedrun.com API response");
        return;
    }

    this->wrs[i] = toTime(time);
    settings.setValue(QStringLiteral("wr/%1").arg(shortNames[i]), this->wrs[i]);
    emit updatedWR(map);
}

template<typename T>
static Maps toMapImpl(T level) {
    level = level.trimmed();
    int index = names.indexOf(level);
    if (index < 0) index = shortNames.indexOf(level);
    if (index < 0) throw std::out_of_range("Unknown Level Name");
    return static_cast<Maps>(index);
}

Maps Records::toMap(QStringView level) { return toMapImpl(level); }
Maps Records::toMap(QLatin1String level) { return toMapImpl(level); }

quint32 Records::toTime(double time) {
    return static_cast<quint32>(std::lround(time * 1000));
}

static int digit(char c) { return c >= '0' && c <= '9' ? c - '0' : -1; }
static int digit(QChar c) { return c.digitValue(); }
template<typename T>
static quint32 toTimeImpl(T time) {
    time = time.trimmed();
    int decimal = -1;
    quint32 value = 0;
    for (auto c : time) {
        if (int d = digit(c); d >= 0) {
            if (decimal < 3) value = value * 10 + d;
            else if (decimal == 3 && d >= 5) ++value;
            if (decimal >= 0) ++decimal;
        } else if (c == '.' && decimal < 0) {
            decimal = 0;
        } else {
            throw std::out_of_range("Invalid Time String");
        }
    }
    if (decimal == 2) value *= 10;
    else if (decimal == 1) value *= 100;
    else if (decimal <= 0) value *= 1000;
    return value;
}

quint32 Records::toTime(QStringView time) { return toTimeImpl(time); }
quint32 Records::toTime(QLatin1String time) { return toTimeImpl(time); }

const QString &Records::toString(Maps map) {
    return names.at(static_cast<int>(map));
}

QString Records::toString(quint32 time, bool dashes) {
    return dashes && time == 0 ?
                QStringLiteral("--.---") :
                QStringLiteral("%1.%2").arg(time / 1000).arg(time % 1000, 3, 10, QChar('0'));
}
