//
// Created by imanuel on 21.04.19.
//

#ifndef BLENDER_RENDER_CONTROL_CENTER_QUEUEENTRY_H
#define BLENDER_RENDER_CONTROL_CENTER_QUEUEENTRY_H

class QueueEntry {
public:
    QueueEntry() {}

    const QString &getPath() const {
        return path;
    }

    void setPath(const QString &path) {
        QueueEntry::path = path;
    }

    int getWidth() const {
        return width;
    }

    void setWidth(int width) {
        QueueEntry::width = width;
    }

    int getHeight() const {
        return height;
    }

    void setHeight(int height) {
        QueueEntry::height = height;
    }

private:
    QString path;
    int width;
    int height;
};

#endif //BLENDER_RENDER_CONTROL_CENTER_QUEUEENTRY_H
