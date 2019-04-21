//
// Created by imanuel on 05.04.19.
//

#ifndef GUI_FILE_H
#define GUI_FILE_H

#include <QtCore/QString>
#include <QtCore/QDateTime>

class File {
public:
    bool operator==(const File &rhs) const {
        return modified_date == rhs.modified_date;
    }

    bool operator!=(const File &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const File &rhs) const {
        return modified_date < rhs.modified_date;
    }

    bool operator>(const File &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const File &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const File &rhs) const {
        return !(*this < rhs);
    }

    const QString &getPath() const {
        return path;
    }

    void setPath(const QString &path) {
        File::path = path;
    }

    const QString &getName() const {
        return name;
    }

    void setName(const QString &name) {
        File::name = name;
    }

    int getSize() const {
        return size;
    }

    void setSize(int size) {
        File::size = size;
    }

    const QDateTime &getModifiedDate() const {
        return modified_date;
    }

    void setModifiedDate(const QDateTime &modifiedDate) {
        modified_date = modifiedDate;
    }

private:
    QString path;

    QString name;

    int size;

    QDateTime modified_date;
};


#endif //GUI_FILE_H
