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

#include "mainwindow.h"
#include "pluginloader.h"
#include <QApplication>
#include <QNetworkProxy>
#include <QSettings>
#include <QIcon>
#include <QUrl>
#include <Irc>

static void setApplicationProxy(QUrl url)
{
    if (!url.isEmpty()) {
        if (url.port() == -1)
            url.setPort(8080);
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, url.host(), url.port(), url.userName(), url.password());
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

int main(int argc, char* argv[])
{
#ifdef Q_OS_MAC
    // QTBUG-32789 - GUI widgets use the wrong font on OS X Mavericks
    QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
#endif

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if QT_VERSION >= 0x050600
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Communi");
    app.setOrganizationName("Communi");
    app.setApplicationVersion(Irc::version());
    app.setOrganizationDomain("communi.github.com");
    app.setProperty("description", MainWindow::tr("%1 %2 - http://%3").arg(app.applicationName())
                                                                     .arg(app.applicationVersion())
                                                                     .arg(app.organizationDomain()));

    foreach (const QString& path, PluginLoader::paths())
        app.addLibraryPath(path);

    QStringList args = app.arguments();
    if (args.contains("-reset"))
        QSettings().clear();

    QUrl proxy;
    int index = args.indexOf("-proxy");
    if (index != -1)
        proxy = QUrl(args.value(index + 1));
    else
        proxy = QUrl(qgetenv("http_proxy"));
    setApplicationProxy(proxy);

    MainWindow window;
    window.show();
    return app.exec();
}
