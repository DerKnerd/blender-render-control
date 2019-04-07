//
// Created by imanuel on 05.04.19.
//

#ifndef GUI_FILE_H
#define GUI_FILE_H


#include <QtCore/QString>
#include <QtCore/QDateTime>

class File {

private:
    QString path;
public:
    const QString &getPath() const;

    void setPath(const QString &path);

    const QString &getName() const;

    void setName(const QString &name);

    int getSize() const;

    void setSize(int size);

    const QDateTime &getModifiedDate() const;

    void setModifiedDate(const QDateTime &modifiedDate);

    bool operator<(const File &rhs) const;

private:

    QString name;

    int size;

    QDateTime modified_date;
};


#endif //GUI_FILE_H
