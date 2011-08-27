/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef SESSION_H
#define SESSION_H

#include <ircsession.h>
#include "connection.h"

class Session : public IrcSession
{
    Q_OBJECT

public:
    Session(QObject* parent = 0);
    ~Session();

    Connection connection() const;

    QStringList autoJoinChannels() const;
    void setAutoJoinChannels(const QStringList& channels);

    int autoReconnectDelay() const;
    void setAutoReconnectDelay(int seconds);

public slots:
    void connectTo(const Connection& connection);

private slots:
    void onPassword(QString* password);
    void onConnected();

private:
    mutable Connection conn;
    QStringList channels;
    int delay;
};

#endif // SESSION_H
