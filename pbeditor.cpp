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

#include "pbeditor.h"

#include "maps.h"
#include "records.h"

#include <QDoubleValidator>
#include <QFont>
#include <QFontMetrics>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QString>

PBEditor::PBEditor(Records &recs, QWidget *parent) :
    QDialog(parent, Qt::Window | Qt::WindowStaysOnTopHint),
    recs(recs) {
    QFormLayout *layout = new QFormLayout;
    this->setLayout(layout);

    for (int i=0; i < MapsCount; ++i) {
        Maps map = static_cast<Maps>(i);

        QLabel *label = new QLabel(Records::toString(map), this);
        QFont font = label->font();
        font.setPointSize(12);
        label->setFont(font);

        QLineEdit *time = new QLineEdit(this);
        font = time->font();
        font.setPointSize(10);
        time->setFont(font);
        time->setFixedWidth(QFontMetrics(font).size(0, QStringLiteral("999.999")).width() + 8);
        QDoubleValidator *dval = new QDoubleValidator(0.000, 999.999, 3, time);
        dval->setNotation(QDoubleValidator::StandardNotation);
        time->setValidator(dval);
        time->setText(recs.getPBStr(map, false));

        connect(time, &QLineEdit::textChanged, [this, map](const QString &time) {
            this->recs.setPB(map, Records::toTime(time));
        });

        layout->addRow(label, time);
    }
}
