/*
  Copyright (C) 2008-2016 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "treeheader.h"
#include <QStyleOptionHeader>
#include <QStylePainter>
#include <QHash>

TreeHeader::TreeHeader(QWidget* parent) : QFrame(parent)
{
    d.state = QStyle::State_None;
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
}

TreeHeader* TreeHeader::instance(QWidget* parent)
{
    static QHash<QWidget*, TreeHeader*> headers;
    QWidget* window = parent ? parent->window() : 0;
    TreeHeader* header = headers.value(window);
    if (!header) {
        header = new TreeHeader(window);
        headers.insert(window, header);
    }
    return header;
}

void TreeHeader::paintEvent(QPaintEvent*)
{
    QStyleOptionHeader option;
    option.init(this);
#if defined(Q_OS_WIN)
    option.rect.adjust(0, 0, 0, 1);
#elif defined(Q_OS_MAC)
    option.rect.adjust(0, 0, 1, 0);
#endif
    option.state = (d.state | QStyle::State_Raised | QStyle::State_Horizontal);
    QPixmap px(16, 16);
    px.fill(Qt::transparent);
    option.icon = px;
    option.text = d.text;
    option.textAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option.position = QStyleOptionHeader::OnlyOneSection;
    QStylePainter painter(this);
    painter.drawControl(QStyle::CE_Header, option);
}
