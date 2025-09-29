#include "iohelper.h"
#include "qbuffer.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QTextStream>

namespace utils
{
namespace IOHelper
{
QByteArray loadFile(const QString& filepath)
{
    QByteArray data;
    if(filepath.isEmpty())
        return data;
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        data= file.readAll();
    }
    return data;
}

bool makeSurePathExist(const QDir& dir)
{
    if(!dir.exists())
        dir.mkpath(dir.path());

    return dir.exists();
}

bool makeDir(const QString& dir)
{
    return makeSurePathExist(QDir(dir));
}

QString copyFile(const QString& source, const QString& destination, bool overwrite)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);
    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    if(makeSurePathExist(dest.dir()))
    {
        if(overwrite)
            QFile::remove(dest.absoluteFilePath());

        if(src.copy(dest.absoluteFilePath()))
            return dest.absoluteFilePath();
    }
    return {};
}

bool removeFile(const QString& source)
{
    return QFile::remove(source);
}

bool writeFile(const QString& path, const QByteArray& arry, bool override)
{
    if(arry.isEmpty())
        return false;

    QFileInfo pathInfo(path);
    makeDir(pathInfo.absolutePath());

    QSaveFile file(path);
    if(file.open(override ? QIODevice::WriteOnly : QIODevice::Append))
    {
        file.write(arry);
        file.commit();
        return true;
    }
    return false;
}

bool moveFile(const QString& source, const QString& destination)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);

    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    bool res= false;
    if(makeSurePathExist(dest.dir()))
    {
        res= src.rename(dest.absoluteFilePath());
    }
    return res;
}

QString readTextFile(const QString& path)
{
    if(path.isEmpty())
        return {};
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return {};

    QTextStream out(&file);
    return out.readAll();
}

QPixmap readPixmapFromFile(const QString& uri)
{
    return QPixmap(uri);
}

QString shortNameFromPath(const QString& path)
{
    QFileInfo info(path);
    return info.baseName();
}

QString fileNameFromPath(const QString& path)
{
    QFileInfo info(path);
    return info.fileName();
}

QString absoluteToRelative(const QString& absolute, const QString& root)
{
    QDir dir(root);
    QFileInfo path(absolute);
    if(!path.isAbsolute())
        return absolute;

    return dir.relativeFilePath(absolute);
}

QImage dataToImage(const QByteArray& data)
{
    QImage img;
    img.loadFromData(data);
    return img;
}

QByteArray imageToData(const QImage& pix)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    return bytes;
}

bool savePixmapInto(const QPixmap& map, const QUrl& destination)
{
    Q_ASSERT(destination.isLocalFile());
    return map.save(destination.toLocalFile(), "PNG");
}

} // namespace IOHelper
} // namespace utils
