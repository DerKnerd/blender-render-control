//
// Created by imanuel on 05.04.19.
//

#include "File.h"

const QString &File::getPath() const {
    return path;
}

void File::setPath(const QString &path) {
    File::path = path;
}

const QString &File::getName() const {
    return name;
}

void File::setName(const QString &name) {
    File::name = name;
}

int File::getSize() const {
    return size;
}

void File::setSize(int size) {
    File::size = size;
}

const QDateTime &File::getModifiedDate() const {
    return modified_date;
}

void File::setModifiedDate(const QDateTime &modifiedDate) {
    modified_date = modifiedDate;
}
