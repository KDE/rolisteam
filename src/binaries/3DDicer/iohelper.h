#ifndef UTIL_IOHELPER_H
#define UTIL_IOHELPER_H

#include <QImage>
#include <QPixmap>
#include <QString>
#include <QUrl>

namespace utils
{
namespace IOHelper
{
// file API
QByteArray loadFile(const QString& file);
bool writeFile(const QString& path, const QByteArray& arry, bool override= true);
bool moveFile(const QString& source, const QString& destination);
bool removeFile(const QString& soursce);
bool moveFilesToDirectory(const QString& files, const QString& dest);
QString copyFile(const QString& source, const QString& destination, bool overwrite= false);
bool makeDir(const QString& dir);
QString shortNameFromPath(const QString& path);
QString shortNameFromUrl(const QUrl& url);
QString absoluteToRelative(const QString& absolute, const QString& root);
QString readTextFile(const QString& file);
QPixmap readPixmapFromURL(const QUrl& url);
bool savePixmapInto(const QPixmap& map, const QUrl& destination);
QPixmap readPixmapFromFile(const QString& url);
QImage dataToImage(const QByteArray& data);
QByteArray imageToData(const QImage& pix);
QString fileNameFromPath(const QString& path);
} // namespace IOHelper
} // namespace utils
#endif // UTIL_IOHELPER_H
