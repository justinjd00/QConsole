#include "QtWidgetsApplication2.h"
#include <QtWidgets/QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>
#include <QPalette>
#include <QStyle>
#include <QFileDialog>
#include <QProcess>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QStringList>
#include <windows.h>

// Funktion zum Hinzufügen eines Logs mit Zeitstempel
void logAction(QTextEdit* logWindow, const QString& action) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logWindow->append("[" + timestamp + "] " + action);
}

// Funktion zum Importieren und Anzeigen von Dateien sowie Laden der Accounts in das Dropdown-Menü
void importFile(const QString& filter, QTextEdit* outputWindow, QComboBox* accountDropdown) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", QDir::homePath(), filter);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            outputWindow->clear();
            accountDropdown->clear();
            QString formattedText = "USERNAME\tPASSWORD\n"; // Strukturierte Anzeige hinzufügen

            // Lade die Accounts in das Dropdown-Menü
            QStringList lines = content.split('\n', Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                QString trimmedLine = line.trimmed();
                if (!trimmedLine.isEmpty() && trimmedLine.contains(':')) {
                    QStringList parts = trimmedLine.split(':');
                    if (parts.size() == 2) {
                        QString username = parts[0].trimmed();
                        QString password = parts[1].trimmed();
                        accountDropdown->addItem(trimmedLine);  // Account zum Dropdown-Menü hinzufügen
                        formattedText += username + "\t" + password + "\n";  // Strukturierter Text
                    }
                }
            }
            outputWindow->setText(formattedText);  // Zeige strukturierten Text im ACCS-Fenster an
            accountDropdown->setEnabled(true);  // Dropdown-Menü aktivieren, da eine Datei importiert wurde
            file.close();
        }
        else {
            outputWindow->setText("Fehler: Datei konnte nicht geöffnet werden.");
        }
    }
    else {
        qDebug() << "Keine Datei ausgewählt.";
    }
}

// Funktion zum Senden von Tastaturereignissen
void sendKeystrokes(const QString& input) {
    if (input.isEmpty()) {
        qDebug() << "Keystrokes: Eingabe ist leer!";
        return;
    }
    for (QChar c : input) {
        INPUT ip = { 0 };
        ip.type = INPUT_KEYBOARD;
        ip.ki.time = 0;
        ip.ki.dwExtraInfo = 0;
        ip.ki.wScan = c.unicode();
        ip.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &ip, sizeof(INPUT));

        ip.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
}

// Funktion zum Simulieren der Tab-Taste (zum Wechseln des Eingabefeldes)
void sendTabKey() {
    INPUT ip = { 0 };
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = VK_TAB;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

// Funktion zum Senden von Enter (zum Bestätigen)
void sendEnterKey() {
    INPUT ip = { 0 };
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = VK_RETURN;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

// Funktion zum Starten eines Programms (z.B. Riot Client)
void startProgram(const QString& filePath) {
    if (filePath.isEmpty()) {
        qDebug() << "Der Pfad ist leer.";
        return;
    }

    std::wstring wFilePath = filePath.toStdWString();
    LPCWSTR path = wFilePath.c_str();

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    BOOL result = CreateProcessW(
        path, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi
    );

    if (result) {
        qDebug() << "Programm erfolgreich gestartet.";
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        qDebug() << "Fehler beim Starten des Programms. Error Code:" << GetLastError();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Hauptfenster
    QWidget* window = new QWidget;
    window->setWindowTitle("League of Legends Login");

    // Tab-Widget erstellen
    QTabWidget* tabWidget = new QTabWidget;
    QWidget* loginTab = new QWidget;
    QWidget* logTab = new QWidget;
    QWidget* accsTab = new QWidget;

    // TextEdit für Logs und für accs
    QTextEdit* logWindow = new QTextEdit;
    logWindow->setReadOnly(true);

    QTextEdit* accsWindow = new QTextEdit;
    accsWindow->setReadOnly(true);

    // Layout für den Log-Tab
    QVBoxLayout* logLayout = new QVBoxLayout;
    logLayout->addWidget(logWindow);
    logTab->setLayout(logLayout);

    // Layout für den accs-Tab
    QVBoxLayout* accsLayout = new QVBoxLayout;
    accsLayout->addWidget(accsWindow);
    accsTab->setLayout(accsLayout);

    // Buttons zum Importieren von Dateien
    QPushButton* importTxtButton = new QPushButton("Import .txt File");
    QPushButton* importCsvButton = new QPushButton("Import .csv File");

    // Dropdown-Menü für die Auswahl von Accounts
    QComboBox* accountDropdown = new QComboBox;
    accountDropdown->setPlaceholderText("Wähle einen Account");
    accountDropdown->setEnabled(false);  // Deaktiviert, bis eine Datei importiert wird

    // Eingabefelder für Benutzername und Passwort
    QLineEdit* usernameInput = new QLineEdit;
    usernameInput->setPlaceholderText("Username");

    QLineEdit* passwordInput = new QLineEdit;
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);

    // Verbindung des Dropdown-Menüs mit dem Befüllen der Eingabefelder
    QObject::connect(accountDropdown, &QComboBox::currentTextChanged, [&]() {
        QString selectedAccount = accountDropdown->currentText();
        QStringList parts = selectedAccount.split(":");
        if (parts.size() == 2) {
            usernameInput->setText(parts[0].trimmed());
            passwordInput->setText(parts[1].trimmed());
        }
        else {
            qDebug() << "Ungültiges Format für Account.";
        }
        });

    QPushButton* startButton = new QPushButton("Start League");
    startButton->setEnabled(false);

    // Validierung: Aktivere den Button nur, wenn beide Felder ausgefüllt sind
    QObject::connect(usernameInput, &QLineEdit::textChanged, [&]() {
        startButton->setEnabled(!usernameInput->text().isEmpty() && !passwordInput->text().isEmpty());
        });

    QObject::connect(passwordInput, &QLineEdit::textChanged, [&]() {
        startButton->setEnabled(!usernameInput->text().isEmpty() && !passwordInput->text().isEmpty());
        });

    // Verbindung des Buttons mit dem Starten von League of Legends
    QObject::connect(startButton, &QPushButton::clicked, [&]() {
        logAction(logWindow, "Starte League of Legends Client...");
        QString username = usernameInput->text();
        QString password = passwordInput->text();
        QString riotClientPath = "C:/Riot Games/Riot Client/RiotClientServices.exe";

        // Starte League of Legends Client
        startProgram(riotClientPath);

        QTimer::singleShot(5000, [&]() {
            // Gebe Benutzernamen ein und wechsle dann zum Passwortfeld
            sendKeystrokes(username);
            sendTabKey();
            sendKeystrokes(password);
            sendEnterKey();  // Drücke Enter zur Anmeldung

            logAction(logWindow, "League of Legends gestartet und Anmeldedaten gesendet.");
            });
        });

    // Verbindung der Buttons zum Öffnen und Importieren von Dateien
    QObject::connect(importTxtButton, &QPushButton::clicked, [&]() {
        importFile("Text Files (*.txt)", accsWindow, accountDropdown);
        logAction(logWindow, "TXT-Datei importiert.");
        });

    QObject::connect(importCsvButton, &QPushButton::clicked, [&]() {
        importFile("CSV Files (*.csv)", accsWindow, accountDropdown);
        logAction(logWindow, "CSV-Datei importiert.");
        });

    // Layout für das Login-Tab
    QVBoxLayout* loginLayout = new QVBoxLayout;
    loginLayout->addWidget(new QLabel("Sign in"));
    loginLayout->addWidget(usernameInput);
    loginLayout->addWidget(passwordInput);
    loginLayout->addWidget(accountDropdown);
    loginLayout->addWidget(startButton);
    loginLayout->addWidget(importTxtButton);
    loginLayout->addWidget(importCsvButton);

    loginTab->setLayout(loginLayout);
     
    // Tabs hinzufügen
    tabWidget->addTab(loginTab, "Login");
    tabWidget->addTab(accsTab, "Accs");
    tabWidget->addTab(logTab, "Logs");

    // Hauptlayout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);

    window->setLayout(mainLayout);
    window->resize(400, 300);
    window->show();

    return app.exec();
}
