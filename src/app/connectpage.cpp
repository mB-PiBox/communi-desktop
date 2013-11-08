/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "connectpage.h"
#include "simplecrypt.h"
#include <QRegExpValidator>
#include <QStringList>
#include <QPushButton>
#include <QCompleter>
#include <QSettings>
#include <QShortcut>
#include <QTime>
#include <Irc>

static const int SSL_PORTS[] = { 6697, 7000, 7070 };
static const int NORMAL_PORTS[] = { 6667, 6666, 6665 };

ConnectPage::ConnectPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    ui.displayNameCompleter = 0;
    ui.hostCompleter = 0;
    ui.nickNameCompleter = 0;
    ui.realNameCompleter = 0;
    ui.userNameCompleter = 0;

    QRegExpValidator* validator = new QRegExpValidator(this);
    validator->setRegExp(QRegExp("\\S*"));
    ui.hostField->setValidator(validator);
    ui.nickNameField->setValidator(validator);
    ui.userNameField->setValidator(validator);

    qsrand(QTime::currentTime().msec());
    ui.nickNameField->setPlaceholderText(ui.nickNameField->placeholderText().arg(qrand() % 9999));
    ui.realNameField->setPlaceholderText(ui.realNameField->placeholderText().arg(IRC_VERSION_STR));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(onDisplayNameFieldChanged()));
    connect(ui.hostField, SIGNAL(textChanged(QString)), this, SLOT(onHostFieldChanged()));
    connect(ui.portField, SIGNAL(valueChanged(int)), this, SLOT(onPortFieldChanged(int)));
    connect(ui.secureField, SIGNAL(toggled(bool)), this, SLOT(onSecureFieldToggled(bool)));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.hostField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.nickNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.realNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.userNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.passwordField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.portField, SIGNAL(valueChanged(int)), this, SLOT(updateUi()));
    connect(ui.secureField, SIGNAL(toggled(bool)), this, SLOT(updateUi()));

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(saveSettings()));
    restoreSettings();
    updateUi();
}

ConnectPage::~ConnectPage()
{
}

static QString fieldValue(const QString& value, const QString& fallback)
{
    return value.isEmpty() ? fallback : value;
}

QString ConnectPage::displayName() const
{
    return fieldValue(ui.displayNameField->text(), ui.displayNameField->placeholderText());
}

void ConnectPage::setDisplayName(const QString& name)
{
    ui.displayNameField->setText(name);
}

QString ConnectPage::host() const
{
    return fieldValue(ui.hostField->text(), ui.hostField->placeholderText());
}

void ConnectPage::setHost(const QString& host)
{
    ui.hostField->setText(host);
}

int ConnectPage::port() const
{
    return ui.portField->value();
}

void ConnectPage::setPort(int port)
{
    ui.portField->setValue(port);
}

bool ConnectPage::isSecure() const
{
    return ui.secureField->isChecked();
}

void ConnectPage::setSecure(bool secure)
{
    ui.secureField->setChecked(secure);
}

QString ConnectPage::nickName() const
{
    return fieldValue(ui.nickNameField->text(), ui.nickNameField->placeholderText());
}

void ConnectPage::setNickName(const QString& name)
{
    ui.nickNameField->setText(name);
}

QString ConnectPage::realName() const
{
    return fieldValue(ui.realNameField->text(), ui.realNameField->placeholderText());
}

void ConnectPage::setRealName(const QString& name)
{
    ui.realNameField->setText(name);
}

QString ConnectPage::userName() const
{
    return fieldValue(ui.userNameField->text(), ui.userNameField->placeholderText());
}

void ConnectPage::setUserName(const QString& name)
{
    ui.userNameField->setText(name);
}

QString ConnectPage::password() const
{
    return ui.passwordField->text();
}

void ConnectPage::setPassword(const QString& password)
{
    ui.passwordField->setText(password);
}

QDialogButtonBox* ConnectPage::buttonBox() const
{
    return ui.buttonBox;
}

static QString capitalize(const QString& str)
{
    return str.left(1).toUpper() + str.mid(1);
}

void ConnectPage::onDisplayNameFieldChanged()
{
    QString name = ui.displayNameField->text();
    if (!name.isEmpty() && ui.hostField->text().isEmpty())
        setHost(defaultValue("hosts", name).toString());
}

void ConnectPage::onHostFieldChanged()
{
    // pick the longest part of the url, skipping the most common domain prefixes and suffixes
    QString name;
    QStringList parts = host().split(QLatin1Char('.'), QString::SkipEmptyParts);
    while (!parts.isEmpty()) {
        QString part = parts.takeFirst();
        const QString lower = part.toLower();
        if (!QString("irc").startsWith(lower) && !QString("org").startsWith(lower)
                && !QString("net").startsWith(lower) && !QString("com").startsWith(lower)) {
            if (part.length() > name.length())
                name = part;
        }
    }
    if (!name.isEmpty())
        ui.displayNameField->setPlaceholderText(capitalize(name));

    QString displayName = ui.displayNameField->text();
    QString host = ui.hostField->text();
    if (!displayName.isEmpty() || !host.isEmpty()) {
        if (port() == NORMAL_PORTS[0])
            setPort(defaultValue("ports", displayName, defaultValue("ports", host, port()).toInt()).toInt());
        if (!isSecure())
            setSecure(defaultValue("secures", displayName, defaultValue("secures", host, isSecure()).toBool()).toBool());
        if (ui.nickNameField->text().isEmpty())
            setNickName(defaultValue("nickNames", displayName, defaultValue("nickNames", host).toString()).toString());
        if (ui.realNameField->text().isEmpty())
            setRealName(defaultValue("realNames", displayName, defaultValue("realNames", host).toString()).toString());
        if (ui.userNameField->text().isEmpty())
            setUserName(defaultValue("userNames", displayName, defaultValue("userNames", host).toString()).toString());
    }
}

void ConnectPage::onPortFieldChanged(int port)
{
    if (port == SSL_PORTS[0] || port == SSL_PORTS[1] || port == SSL_PORTS[2])
        ui.secureField->setChecked(true);
}

void ConnectPage::onSecureFieldToggled(bool secure)
{
    if (secure) {
        const int port = ui.portField->value();
        if (port == NORMAL_PORTS[0] || port == NORMAL_PORTS[1] || port == NORMAL_PORTS[2])
            ui.portField->setValue(SSL_PORTS[0]);
    }
}

static QCompleter* createCompleter(const QStringList& list, QLineEdit* lineEdit)
{
    QCompleter* completer = new QCompleter(list, lineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    return completer;
}

void ConnectPage::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("Credentials");

    ui.displayNameField->setText(settings.value("displayName").toString());
    ui.hostField->setText(settings.value("host").toString());
    ui.portField->setValue(settings.value("port", NORMAL_PORTS[0]).toInt());
    ui.secureField->setChecked(settings.value("secure", false).toBool());
    ui.nickNameField->setText(settings.value("nickName").toString());
    ui.realNameField->setText(settings.value("realName").toString());
    ui.userNameField->setText(settings.value("userName").toString());

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    ui.passwordField->setText(crypto.decryptToString(settings.value("password").toString()));

    ui.displayNameCompleter = createCompleter(settings.value("displayNames").toStringList(), ui.displayNameField);
    ui.hostCompleter = createCompleter(settings.value("allHosts").toStringList(), ui.hostField);
    ui.nickNameCompleter = createCompleter(settings.value("allNickNames").toStringList(), ui.nickNameField);
    ui.realNameCompleter = createCompleter(settings.value("allRealNames").toStringList(), ui.realNameField);
    ui.userNameCompleter = createCompleter(settings.value("allUserNames").toStringList(), ui.userNameField);
}

void ConnectPage::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Credentials");

    const QString displayName = ui.displayNameField->text();
    const QString host = ui.hostField->text();
    const int port = ui.portField->value();
    const bool secure = ui.secureField->isChecked();
    const QString nickName = ui.nickNameField->text();
    const QString realName = ui.realNameField->text();
    const QString userName = ui.userNameField->text();

    settings.setValue("displayName", displayName);
    settings.setValue("host", host);
    settings.setValue("port", port);
    settings.setValue("secure", secure);
    settings.setValue("nickName", nickName);
    settings.setValue("realName", realName);
    settings.setValue("userName", userName);

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    settings.setValue("password", crypto.encryptToString(ui.passwordField->text()));

    if (!displayName.isEmpty()) {
        QStringList displayNames = settings.value("displayNames").toStringList();
        if (!displayNames.contains(displayName, Qt::CaseInsensitive))
            settings.setValue("displayNames", displayNames << displayName);
    }

    if (!host.isEmpty()) {
        QStringList allHosts = settings.value("allHosts").toStringList();
        if (!allHosts.contains(host, Qt::CaseInsensitive))
            settings.setValue("allHosts", allHosts << host);

        QMap<QString, QVariant> hosts = settings.value("hosts").toMap();
        hosts.insert(ConnectPage::displayName(), host);
        settings.setValue("hosts", hosts);
    }

    if (port != NORMAL_PORTS[0]) {
        QMap<QString, QVariant> ports = settings.value("ports").toMap();
        ports.insert(ConnectPage::displayName(), port);
        ports.insert(ConnectPage::host(), port);
        settings.setValue("ports", ports);
    }

    if (secure) {
        QMap<QString, QVariant> secures = settings.value("secures").toMap();
        secures.insert(ConnectPage::displayName(), secure);
        secures.insert(ConnectPage::host(), secure);
        settings.setValue("secures", secures);
    }

    if (!nickName.isEmpty()) {
        QStringList allNickNames = settings.value("allNickNames").toStringList();
        if (!allNickNames.contains(nickName, Qt::CaseInsensitive))
            settings.setValue("allNickNames", allNickNames << nickName);

        QMap<QString, QVariant> nickNames = settings.value("nickNames").toMap();
        nickNames.insert(ConnectPage::displayName(), nickName);
        nickNames.insert(ConnectPage::host(), nickName);
        settings.setValue("nickNames", nickNames);
    }

    if (!realName.isEmpty()) {
        QStringList allRealNames = settings.value("allRealNames").toStringList();
        if (!allRealNames.contains(realName, Qt::CaseInsensitive))
            settings.setValue("allRealNames", allRealNames << realName);

        QMap<QString, QVariant> realNames = settings.value("realNames").toMap();
        realNames.insert(ConnectPage::displayName(), realName);
        realNames.insert(ConnectPage::host(), realName);
        settings.setValue("realNames", realNames);
    }

    if (!userName.isEmpty()) {
        QStringList allUserNames = settings.value("allUserNames").toStringList();
        if (!allUserNames.contains(userName, Qt::CaseInsensitive))
            settings.setValue("allUserNames", allUserNames << userName);

        QMap<QString, QVariant> userNames = settings.value("userNames").toMap();
        userNames.insert(ConnectPage::displayName(), userName);
        userNames.insert(ConnectPage::host(), userName);
        settings.setValue("userNames", userNames);
    }
}

void ConnectPage::updateUi()
{
    QPushButton* button = ui.buttonBox->button(QDialogButtonBox::Reset);
    button->setEnabled(!ui.displayNameField->text().isEmpty() ||
                       !ui.hostField->text().isEmpty() ||
                        ui.portField->value() != NORMAL_PORTS[0] ||
                        ui.secureField->isChecked() ||
                       !ui.nickNameField->text().isEmpty() ||
                       !ui.realNameField->text().isEmpty() ||
                       !ui.userNameField->text().isEmpty() ||
                       !ui.passwordField->text().isEmpty());
}

void ConnectPage::reset()
{
    ui.displayNameField->clear();
    ui.hostField->clear();
    ui.portField->setValue(NORMAL_PORTS[0]);
    ui.secureField->setChecked(false);
    ui.nickNameField->clear();
    ui.realNameField->clear();
    ui.userNameField->clear();
    ui.passwordField->clear();
    saveSettings();
}

QVariant ConnectPage::defaultValue(const QString& key, const QString& field, const QVariant& defaultValue) const
{
    QSettings settings;
    settings.beginGroup("Credentials");
    QMap<QString, QVariant> values = settings.value(key).toMap();
    return values.value(field, defaultValue);
}