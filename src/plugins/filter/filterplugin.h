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

#ifndef FILTERPLUGIN_H
#define FILTERPLUGIN_H

#include <QPair>
#include <QHash>
#include <QString>
#include <QtPlugin>
#include <QDateTime>
#include <IrcCommandFilter>
#include <IrcMessageFilter>
#include "connectionplugin.h"

class FilterPlugin : public QObject, public ConnectionPlugin, public IrcMessageFilter, public IrcCommandFilter
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin IrcCommandFilter IrcMessageFilter)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")

public:
    FilterPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

    bool commandFilter(IrcCommand* command);
    bool messageFilter(IrcMessage* message);

private:
    struct Private {
        QHash<int, QPair<QDateTime, QString> > sentCommands;
        QHash<QString, QPair<QDateTime, QString> > awayReplies;
    } d;
};

#endif // FILTERPLUGIN_H
