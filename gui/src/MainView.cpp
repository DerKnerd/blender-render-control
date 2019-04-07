//
// Created by imanuel on 04.04.19.
//

#include "MainView.h"

using namespace Qt;

MainView::MainView(QWidget *parent) : QWidget(parent) {
    m_ui.setupUi(this);

    m_ui.availableFiles->resizeColumnsToContents();
}

MainView::~MainView() {

}

void MainView::onLogReceived(const QString &qString) {
    m_ui.logData->appendPlainText(qString);
    m_ui.logData->verticalScrollBar()->setValue(m_ui.logData->verticalScrollBar()->maximum());
}

void MainView::onFilesReceived(QList<File> &data) {
    m_ui.availableFiles->setRowCount(data.size());

    std::sort(data.rbegin(), data.rend());

    for (int i = 0; i < data.size(); ++i) {
        const auto &file = data.at(i);

        auto nameElement = new QTableWidgetItem();
        nameElement->setText(file.getName());
        nameElement->setCheckState(CheckState::Unchecked);
        m_ui.availableFiles->setItem(i, 0, nameElement);

        auto modifiedDateElement = new QTableWidgetItem();
        auto date = file.getModifiedDate().toLocalTime().toString(DateFormat::SystemLocaleShortDate);
        modifiedDateElement->setText(date);
        m_ui.availableFiles->setItem(i, 1, modifiedDateElement);

        auto sizeElement = new QTableWidgetItem();
        sizeElement->setText(QStringLiteral("%1 MiB").arg(file.getSize() / 1024.0 / 1024.0));
        m_ui.availableFiles->setItem(i, 2, sizeElement);

        auto pathElement = new QTableWidgetItem();
        pathElement->setText(file.getPath());
        m_ui.availableFiles->setItem(i, 3, pathElement);
    }

    m_ui.availableFiles->resizeColumnsToContents();
}

QStringList MainView::getFiles() const {
    auto items = m_ui.availableFiles->findItems(QStringLiteral(""), MatchContains);
    auto selectedFiles = QStringList();

    for (int i = 0; i < items.count(); ++i) {
        auto item = items[i];
        if (item->checkState() == CheckState::Checked) {
            auto row = item->row();
            auto path = m_ui.availableFiles->item(row, 3);
            selectedFiles.append(path->text());
        }
    }

    return selectedFiles;
}

void MainView::uncheckAllFiles() {
    auto items = m_ui.availableFiles->findItems(QStringLiteral(""), MatchContains);

    for (int i = 0; i < items.count(); ++i) {
        auto item = items[i];
        if (item->checkState() == CheckState::Checked) {
            item->setCheckState(CheckState::Unchecked);
        }
    }
}
