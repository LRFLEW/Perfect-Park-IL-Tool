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

#ifndef LEVELWINDOW_H
#define LEVELWINDOW_H

#include "logparser.h"
#include "maps.h"
#include "records.h"

#include <optional>

#include <QtGlobal>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class LevelWindow; }
QT_END_NAMESPACE

class LevelWindow : public QWidget {
    Q_OBJECT

public:
    LevelWindow(QWidget *parent = nullptr);
    ~LevelWindow();

public slots:
    void setMap(Maps map);
    void setTime(Maps map, quint32 time);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Ui::LevelWindow *ui;

    LogParser lp;
    Records recs;
    QSettings settings;

    QPoint dragPos;
    std::optional<Maps> curMap;
};
#endif // LEVELWINDOW_H
