/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef GZIPREADER_H
#define GZIPREADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qfile.h>
#include <QtCore/qstring.h>

class GZipReaderPrivate;

namespace CZIP
{
    class QZipReader
    {
    public:
        QZipReader(const QString& fileName, QIODevice::OpenMode mode= QIODevice::ReadOnly);

        explicit QZipReader(QIODevice* device);
        ~QZipReader();

        bool isReadable() const;
        bool exists() const;

        struct Q_AUTOTEST_EXPORT FileInfo
        {
            FileInfo();
            FileInfo(const FileInfo& other);
            ~FileInfo();
            FileInfo& operator=(const FileInfo& other);
            QString filePath;
            uint isDir : 1;
            uint isFile : 1;
            uint isSymLink : 1;
            QFile::Permissions permissions;
            uint crc32;
            qint64 size;
            void* d;
        };

        QList<FileInfo> fileInfoList() const;
        int count() const;

        FileInfo entryInfoAt(int index) const;
        QByteArray fileData(const QString& fileName) const;
        bool extractAll(const QString& destinationDir) const;

        enum Status
        {
            NoError,
            FileReadError,
            FileOpenError,
            FilePermissionsError,
            FileError
        };

        Status status() const;

        void close();

    private:
        GZipReaderPrivate* d;
        Q_DISABLE_COPY(QZipReader)
    };

} // namespace CZIP

#endif // QZIPREADER_H
