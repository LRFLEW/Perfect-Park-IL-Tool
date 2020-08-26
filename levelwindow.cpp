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
#include "./ui_levelwindow.h"

#include "pbeditor.h"

#include <QApplication>
#include <QMenu>

LevelWindow::LevelWindow(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      ui(new Ui::LevelWindow) {
    connect(&lp, &LogParser::mapChange, this, &LevelWindow::setMap);
    connect(&lp, &LogParser::mapFinish, this, &LevelWindow::setTime);
    connect(&lp, &LogParser::mapFinish, &recs, &Records::newTime);

    connect(&recs, &Records::updatedPB, [this](Maps map) {
        if (this->curMap == map) ui->pbtime->setText(recs.getPBStr(map));
    });
    connect(&recs, &Records::updatedWR, [this](Maps map) {
        if (this->curMap == map) ui->wrtime->setText(recs.getWRStr(map));
    });
    connect(&recs, &Records::newPB, [this](bool isWR) {
        if (!ui->timelabel->text().isEmpty()) {
            ui->reslabel->setText(isWR ? QStringLiteral("WR") : QStringLiteral("PB"));
        }
    });

    ui->setupUi(this);
    ui->reclabel->hide();
    if (settings.contains(QStringLiteral("pos"))) {
        this->move(settings.value(QStringLiteral("pos"), this->pos()).toPoint());
    }
}

LevelWindow::~LevelWindow() {
    delete ui;
}

void LevelWindow::setMap(Maps map) {
    ui->timelabel->setText({});
    ui->reslabel->setText({});

    if (map != this->curMap) {
        ui->maplabel->setText(Records::toString(map));
        ui->pbtime->setText(recs.getPBStr(map));
        ui->wrtime->setText(recs.getWRStr(map));

        this->curMap = map;
    }
}

void LevelWindow::setTime(Maps map, quint32 time) {
    setMap(map);
    ui->timelabel->setText(Records::toString(time));
}

void LevelWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPos = event->globalPos();
    }
}

void LevelWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        const QPoint delta = event->globalPos() - dragPos;
        this->move(this->pos() + delta);
        dragPos = event->globalPos();
    }
}

void LevelWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        settings.setValue(QStringLiteral("pos"), this->pos());
    }
}

void LevelWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.addAction("Update PBs", [this](){
        PBEditor pbe(this->recs);
        pbe.exec();
    });
    menu.addAction("Quit", QApplication::quit);
    menu.exec(event->globalPos());
}
