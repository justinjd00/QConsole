#include "QtWidgetsApplication2.h"
#include <QtWidgets/QApplication>

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QIcon>
#include <QPalette>
#include <QStyle>
#include <QApplication>
#include <QFileDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFile>
#include <QMessageBox>



int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget* window = new QWidget;
    window->setWindowTitle("Dynamische Fensteranpassung");

    QPushButton* button = new QPushButton("Klick mich");
    button->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font: bold; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #45a049; }");
    QPushButton* settingsButton = new QPushButton("Settings");
    settingsButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font: bold; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #1E88E5; }");
    QPushButton* loadButton = new QPushButton("Load");
    loadButton->setStyleSheet("QPushButton { background-color: #FFC107; color: white; font: bold; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #FFB300; }");

    
    QTextEdit* logArea = new QTextEdit;
    logArea->setPlaceholderText("Hier werden Logs angezeigt...");
    logArea->setReadOnly(true);
    logArea->setStyleSheet("QTextEdit { background-color: #2830941; border: 1px solid #ccc; border-radius: 5px; padding: 5px; }");

    // Layouts für die rechte Seite erstellen
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(button);
    rightLayout->addWidget(settingsButton);
    rightLayout->addWidget(loadButton);
    rightLayout->addStretch();

    // Hauptlayout erstellen und Widgets hinzufügen
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(logArea, 1);
    mainLayout->addLayout(rightLayout, 0);

    // Layouts zum Hauptwidget hinzufügen
    window->setLayout(mainLayout);

    QScreen* screen = QApplication::primaryScreen();
    QRect screenSize = screen->availableGeometry();
    window->resize(screenSize.width() * 0.5, screenSize.height() * 0.5);

    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(window, "Open File", QDir::homePath(), "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QMessageBox::warning(window, "Error", "Couldn't open the file.");
                return;
            }
            QTextStream in(&file);
            logArea->setPlainText(in.readAll());
        }
        });
	window->move(screenSize.center() - window->rect().center());
    window->show();

    return app.exec();
}