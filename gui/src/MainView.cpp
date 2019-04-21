//
// Created by imanuel on 04.04.19.
//

#include "MainView.h"

MainView::MainView(QWidget *parent) : QWidget(parent) {
    m_ui.setupUi(this);

    m_ui.availableFiles->resizeColumnsToContents();
}

void MainView::onLogReceived(const QString &qString) {
    m_ui.logData->appendPlainText(qString);
    m_ui.logData->verticalScrollBar()->setValue(m_ui.logData->verticalScrollBar()->maximum());
}

void MainView::onFilesReceived(QList<File> &data) {
    m_ui.availableFiles->setRowCount(data.size());

    sort(data.rbegin(), data.rend());

    for (auto file = data.constBegin(); file < data.constEnd(); ++file) {
        auto i = data.indexOf(*file);
        auto nameElement = new QTableWidgetItem();
        nameElement->setText(file->getName());
        nameElement->setCheckState(Qt::CheckState::Unchecked);
        m_ui.availableFiles->setItem(i, 0, nameElement);

        auto modifiedDateElement = new QTableWidgetItem();
        auto date = file->getModifiedDate().toLocalTime().toString(Qt::DateFormat::SystemLocaleShortDate);
        modifiedDateElement->setText(date);
        m_ui.availableFiles->setItem(i, 1, modifiedDateElement);

        auto sizeElement = new QTableWidgetItem();
        sizeElement->setText(QStringLiteral("%1 MiB").arg(file->getSize() / 1024.0 / 1024.0));
        m_ui.availableFiles->setItem(i, 2, sizeElement);

        auto pathElement = new QTableWidgetItem();
        pathElement->setText(file->getPath());
        m_ui.availableFiles->setItem(i, 3, pathElement);
    }

    m_ui.availableFiles->resizeColumnsToContents();
}

QList<QueueEntry> MainView::getFiles() const {
    auto items = m_ui.availableFiles->findItems(QStringLiteral(""), Qt::MatchContains);
    auto selectedFiles = QList<QueueEntry>();

    for (int i = 0; i < items.count(); ++i) {
        auto item = items[i];
        if (item->checkState() == Qt::CheckState::Checked) {
            auto row = item->row();
            auto path = m_ui.availableFiles->item(row, 3);
            auto entry = QueueEntry();
            entry.setPath(path->text());
            entry.setWidth(7860);
            entry.setHeight(4320);

            selectedFiles.append(entry);
        }
    }

    return selectedFiles;
}

void MainView::uncheckAllFiles() {
    auto items = m_ui.availableFiles->findItems(QStringLiteral(""), Qt::MatchContains);

    for (int i = 0; i < items.count(); ++i) {
        auto item = items[i];
        if (item->checkState() == Qt::CheckState::Checked) {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
    }
}

void MainView::updateQueue(const QStringList &files) {
    m_ui.renderQueue->clear();

    for (const auto &file : files) {
        auto fileInfo = QFileInfo(file);
        auto filenameWithoutExtension = fileInfo.fileName().section(".", 0, 0);
        auto row = new QListWidgetItem();

        row->setData(Qt::UserRole, file);
        row->setText(filenameWithoutExtension);
        m_ui.renderQueue->addItem(row);
    }
}

QStringList MainView::getSelectedFilesFromQueue() const {
    auto stringList = QStringList();
    auto selectedItems = m_ui.renderQueue->selectedItems();

    for (const auto &selectedItem : selectedItems) {
        auto data = selectedItem->data(Qt::UserRole).toString();

        stringList.append(data);
    }

    return stringList;
}
