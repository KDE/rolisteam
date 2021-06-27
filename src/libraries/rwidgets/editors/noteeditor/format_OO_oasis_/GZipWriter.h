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
#ifndef GZIPWRITER_H
#define GZIPWRITER_H

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

class GZipWriterPrivate;

namespace CZIP
{
    class QZipWriter
    {
    public:
        QZipWriter(const QString& fileName, QIODevice::OpenMode mode= (QIODevice::WriteOnly | QIODevice::Truncate));

        explicit QZipWriter(QIODevice* device);

        bool isWritable() const;
        bool exists() const;

        enum Status
        {
            NoError,
            FileWriteError,
            FileOpenError,
            FilePermissionsError,
            FileError
        };

        Status status() const;

        enum CompressionPolicy
        {
            AlwaysCompress,
            NeverCompress,
            AutoCompress
        };

        void setCompressionPolicy(CompressionPolicy policy);
        CompressionPolicy compressionPolicy() const;

        void setCreationPermissions(QFile::Permissions permissions);
        QFile::Permissions creationPermissions() const;

        void addFile(const QString& fileName, const QByteArray& data);

        void addFile(const QString& fileName, QIODevice* device);

        void addDirectory(const QString& dirName);

        void addSymLink(const QString& fileName, const QString& destination);

        void close();

    private:
        GZipWriterPrivate* d;
        Q_DISABLE_COPY(QZipWriter)
    };

} // namespace CZIP

#endif // QZIPWRITER_H
