#!/usr/bin/python3

import argparse
import tempfile
import git
import sys
import stat
import subprocess
import glob
import requests
import shutil
import os
import datetime
import tarfile

QT_ROOT="/home/renaud/applications/other/.qt/6.10.0/gcc_64"
QT_LIB="{}/lib".format(QT_ROOT)
QT_PLUGINS="{}/plugins".format(QT_ROOT)
QT_QML="{}/qml".format(QT_ROOT)
QT_LIBEXEC="{}/libexec".format(QT_ROOT)
QT_RESOURCES="{}/resources".format(QT_ROOT)
QT_BIN="{}/bin".format(QT_ROOT)
QT_TRANSLATION="{}/translations".format(QT_ROOT)
QT_QMAKE="{}/bin/qmake".format(QT_ROOT)


def make_tarfile(source, destination):
    tar = tarfile.open(destination, "w:gz")
    tar.add(source)
    tar.close()


def merge_path(path,directory):
    return os.path.join(path, directory)


def copyAll(source, destination):
    files= os.listdir(source)
    baseName = os.path.basename(source)
    destinationFull = os.path.join(destination ,baseName)
    if os.path.isdir(source) and not os.path.exists(destinationFull):
        print("CopyTree: {}".format(source))
        shutil.copytree(source, destinationFull, symlinks=True)
    else:
        for fname in files:
            file = os.path.join(source, fname)
            if os.path.isdir(file):
                print("CopyTree else: {}".format(file))
                shutil.copytree(file, os.path.join(destinationFull,fname), symlinks=True)
            else:
                print("Copy2 : {}".format(file))
                shutil.copy2(file, destinationFull, follow_symlinks=False)

def run_process(args, path):
    text=""
    for i in args:
        text = "{} {}".format(text, i)
    
    print("")
    print(text)
    print("")
    #result = 
    #print(result.stdout)
    #print(result.stderr)
    return  subprocess.run(args, capture_output=True, universal_newlines=True)#,cwd=path, shell=True, capture_output=True, universal_newlines=True
    
    

def clone_repo(path,url_repo,name):
    print("Clone project")
    git.Git(path).clone(url_repo,"--recurse-submodules",name)

def move_content(source_file, to):
    content = os.listdir(source_file)

    for file_name in content:
        shutil.move(os.join.path(source_file, file_name), to)


def get_file(url, destination):
    print("Downloading {}".format(destination))
    r = requests.get(url)
    with open(destination, 'wb') as f:
        f.write(r.content)

def release_translation(path):
    print("release translation")
    print(path)
    proArray = glob.glob(merge_path(path,"*.pro"))
    run_process(["lrelease",proArray[0]],path)

def allow_execution(path):
    st = os.stat(path)
    os.chmod(path, st.st_mode | stat.S_IEXEC)


def icon_path(app_name):
    print("icon_path")
    path_dict={"rolisteam":"resources/logo/rolisteam.svg", "rcse":"resources/logo/rcse.svg","dice":"","rcm":""}
    return path_dict[app_name]


def build_app(path, install_directory):
    print("build app")
    build_directory=merge_path(path, "build")
    os.environ["PATH"] = "{}:{}".format(QT_BIN,os.environ["PATH"])
    print("ENV:")
    print( '\n'.join([f'{k}: {v}' for k, v in sorted(os.environ.items())]) )
    print("END ENV:")
    os.mkdir(build_directory)
    print("cmake build: ",build_directory)
    os.chdir(build_directory)
    print(os.getcwd())

    print("\n\n\nConfigu:\n")
    result = run_process(["{}/bin/qt-cmake".format(QT_ROOT),
                 "-DCMAKE_BUILD_TYPE:STRING=Release",
                 "-DCMAKE_GENERATOR:STRING=Ninja",
                 "-DCMAKE_PREFIX_PATH:PATH={}".format(QT_ROOT),
                 "-DQT_QMAKE_EXECUTABLE:FILEPATH={}".format(QT_QMAKE),
                 "-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc",
                 "-DUPDATE_TRANSLATIONS:BOOL=ON",
                 "-DGENERATE_TS_FILES:BOOL=ON",
                 "-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++",
                 "-DCMAKE_INSTALL_PREFIX:PATH={}".format(install_directory),
                 "-DCMAKE_PREFIX_PATH:PATH={}".format(QT_ROOT),"-S",path,"-B",build_directory],path)
    
    print(result.stdout)
    print(result.stderr)
    print("before build: ",build_directory)
    result = run_process(["cmake","--build", build_directory, "--target", "BuildTranslations", "all"], build_directory)
    print("\n\n\nBuild:\n")
    print(result.stdout)
    print(result.stderr)

    result = run_process(["cmake","--build", build_directory, "--target", "help"], build_directory)
    print("\n\n\nBuild:\n")
    print(result.stdout)
    print(result.stderr)

    result = run_process(['cmake','-S',path,'-B',build_directory], build_directory)
    print("\n\n\nBuild 2:\n")
    print(result.stdout)
    print(result.stderr)
    result = run_process(["cmake","--build", build_directory, "--target", "rolisteam_translations"], build_directory)
    print("\n\n\nBuild 3:\n")
    print(result.stdout)
    print(result.stderr)
    return build_directory


def clean_dir(path):
    list=["lib/Qt6WebEngineQuick.debug",
        "lib/Qt6Pdf.debug",
        "lib/Qt6PdfWidgets.debug",
        "lib/Qt6PdfQuick.debug",
        "lib/Qt6WebEngineWidgets.debug",
        "lib/Qt6WebEngineQuickDelegatesQml.debug",
        "lib/Qt6WebEngineCore.debug",
        "plugins/designer/qwebengineview.debug",
        "plugins/imageformats/qpdf.debug",
        "qml/QtWebEngine/ControlsDelegates/qtwebenginequickdelegatesplugin.debug",
        "qml/QtWebEngine/qtwebenginequickplugin.debug",
        "qml/QtQuick/Pdf/pdfquickplugin.debug",
        "libexec/qwebengine_convert_dict.debug",
        "libexec/QtWebEngineProcess.debug",
        "lib/libQt6QmlAssetDownloader.a", 
        "lib/libQt6ExampleIcons.a", 
        "lib/libQt6FbSupport.a", 
        "lib/libQt6KmsSupport.a", 
        "lib/libQt6BundledEmbree.a", 
        "lib/libQt6ExamplesAssetDownloader.a", 
        "lib/libQt6JsonRpc.a", 
        "lib/libQt6BundledPhysX.a", 
        "lib/libQt6QmlTypeRegistrar.a", 
        "lib/libQt6LanguageServer.a", 
        "lib/libQt6QmlDebug.a", 
        "lib/libQt6DeviceDiscoverySupport.a", 
        "lib/libQt6QuickTestUtils.a", 
        "lib/libQt6BundledLibpng.a", 
        "lib/libQt6BundledResonanceAudio.a", 
        "lib/libQt6QmlToolingSettings.a", 
        "lib/libQt6InputSupport.a", 
        "lib/libQt6FFmpegMediaPluginImpl.a", 
        "lib/libQt6PacketProtocol.a", 
        "lib/libQt6QuickControlsTestUtils.a", 
        "lib/libQt6MultimediaTestLib.a", 
        "lib/libQt6QmlLS.a", 
        "lib/libQt6BundledLibjpeg.a", 
        "lib/libQt6BundledOpenXR.a", 
        "lib/libQt6QmlDom.a", 
        "qml/Assets/Downloader/libqmlassetdownloaderplugin.a",
"libexec/cmake_automoc_parser",
"libexec/ensure_pro_file.cmake",
"libexec/gn",
"libexec/lprodump",
"libexec/lrelease-pro",
"libexec/lupdate-pro",
"libexec/moc",
"libexec/qhelpgenerator",
"libexec/qlalr",
"libexec/qmlaotstats",
"libexec/qmlcachegen",
"libexec/qmlimportscanner",
"libexec/qmljsrootgen",
"libexec/qmltyperegistrar",
"libexec/qscxmlc",
"libexec/qt-android-runner.py",
"libexec/qtattributionsscanner",
"libexec/qt-cmake-private",
"libexec/qt-cmake-private-install.cmake",
"libexec/qt-cmake-standalone-test",
"libexec/qtgrpcgen",
"libexec/qt-internal-configure-examples",
"libexec/qt-internal-configure-tests",
"libexec/qtprotobufgen",
"libexec/qt-testrunner.py",
"libexec/qtwaylandscanner",
"libexec/qvkgen",
"libexec/rcc",
"libexec/repc",
"libexec/sanitizer-testrunner.py",
"libexec/sdpscanner",
"libexec/syncqt",
"libexec/tracegen",
"libexec/tracepointgen",
"libexec/uic",
"libexec/webenginedriver",
"bin/androiddeployqt",
"bin/androiddeployqt6",
"bin/androidtestrunner",
"bin/assistant",
"bin/balsam",
"bin/balsamui",
"bin/canbusutil",
"bin/cooker",
"bin/designer",
"bin/instancer",
"bin/lconvert",
"bin/linguist",
"bin/lrelease",
"bin/lupdate",
"bin/materialeditor",
"bin/meshdebug",
"bin/pixeltool",
"bin/qdbus",
"bin/qdbuscpp2xml",
"bin/qdbusviewer",
"bin/qdbusxml2cpp",
"bin/qdistancefieldgenerator",
"bin/qdoc",
"bin/qmake",
"bin/qmake6",
"bin/qml",
"bin/qmldom",
"bin/qmleasing",
"bin/qmlformat",
"bin/qmllint",
"bin/qmlls",
"bin/qmlplugindump",
"bin/qmlpreview",
"bin/qmlprofiler",
"bin/qmlscene",
"bin/qmltc",
"bin/qmltestrunner",
"bin/qmltime",
"bin/qqem",
"bin/qsb",
"bin/qt-cmake",
"bin/qt-cmake-create",
"bin/qt.conf",
"bin/qt-configure-module",
"bin/qtdiag",
"bin/qtdiag6",
"bin/qtpaths",
"bin/qtpaths6",
"bin/qtplugininfo",
"bin/shadergen",
"bin/shapegen",
"bin/svgtoqml",
"lib/libQt6Graphs.so.6.8.3",
"lib/libQt6QmlCompiler.so.6.8.3",
"lib/libQt6Charts.so.6.8.3",
"lib/libQt6Location.so.6.8.3",
"lib/libQt6DataVisualization.so.6.8.3",
"lib/libQt6Bluetooth.so.6.8.3",
"lib/libQt6RemoteObjects.so.6.8.3",
"lib/libQt6ProtobufWellKnownTypes.so.6.8.3",
"lib/libQt6QuickParticles.so.6.8.3",
"lib/libQt63DAnimation.so.6.8.3",
"lib/libQt6Help.so.6.8.3",
"lib/libQt6Quick3DParticles.so.6.8.3",
"lib/libQt6VirtualKeyboard.so.6.8.3",
"lib/libQt6Protobuf.so.6.8.3",
"lib/libQt6DataVisualizationQml.so.6.8.3",
"lib/libQt6SerialBus.so.6.8.3",
"lib/libQt6Core5Compat.so.6.8.3",
"lib/libQt6ProtobufQtCoreTypes.so.6.8.3",
"lib/libQt6SensorsQuick.so.6.8.3",
"lib/libQt6Sql.so.6.8.3",
"lib/libQt6Sensors.so.6.8.3",
"lib/libQt6ProtobufQtGuiTypes.so.6.8.3",
"lib/libQt6Grpc.so.6.8.3",
"lib/libQt6HttpServer.so.6.8.3",
"lib/libQt6Nfc.so.6.8.3",
"lib/libQt6TextToSpeech.so.6.8.3",
"lib/libQt6QuickTimeline.so.6.8.3"]


    for file in list:
        full_path = os.path.join(path, file)
        if os.path.exists(full_path):
            os.remove(full_path) 

    list_folder=["lib/cmake", "include", "lib/pkgconfig", "qml/QtQuick3D/Particles3D",
                 "plugins/assetimporters",
"plugins/canbus",
"plugins/designer",
"plugins/egldeviceintegrations",
"plugins/geoservices",
"plugins/help",
"plugins/position",
"plugins/qmllint",
"plugins/qmlls",
"plugins/qmltooling",
"plugins/sensors",
"plugins/sqldrivers",
"plugins/texttospeech",
"qml/QmlTime",
"qml/Qt5Compat",
"qml/QtCharts",
"qml/QtDataVisualization",
"qml/QtGraphs",
"qml/QtGrpc",
"qml/QtLocation",
"qml/QtPositioning",
"qml/QtProtobuf",
"qml/QtQuickEffectMaker",
"qml/QtRemoteObjects",
"qml/QtSensors",
"qml/QtTest",
"qml/QtTextToSpeech"
]
#"plugins/xcbglintegrations",


    for folder in list_folder:
        full_path = os.path.join(path, folder)
        print("remove dir: {}".format(full_path))
        shutil.rmtree(full_path)


def build_linux_binary(path, version, app_name):
    install_directory=merge_path(path, "build/installation")
    build_directory = build_app(path, install_directory)
    os.mkdir(install_directory)
    run_process(["cmake","--build", build_directory, "--target", "install"], build_directory)
    os.environ["LD_LIBRARY_PATH"] = "{}:{}".format(QT_LIB,os.environ["LD_LIBRARY_PATH"])
    print("# Copy files")
    copyAll(QT_LIB, install_directory)
    copyAll(QT_PLUGINS, install_directory)
    copyAll(QT_QML, install_directory)
    copyAll(QT_LIBEXEC, install_directory) 
    copyAll(QT_RESOURCES, install_directory)
    copyAll(QT_BIN, install_directory)
    copyAll(QT_TRANSLATION, install_directory)

    print("# Remove useless files")
    clean_dir(install_directory)
          
    launcher = os.path.join(install_directory,"launcher.sh")
    f = open(launcher, "w")
    f.write("""#!/bin/sh

current_dir=`pwd`            
export LD_LIBRARY_PATH="$current_dir/lib:$LD_LIBRARY_PATH"

./bin/rolisteam""")
    f.close()
    os.chmod(launcher, 0o744)
    r = datetime.datetime.now()
    
    print("# Creation archive")
    archive_path = os.path.join(path,"{}_{}_{}.tar.gz".format(app_name, version, r.strftime("%Y%m%d_%H%M")),)
    make_tarfile(install_directory, archive_path)
    

    
def build_tarball(path, version, app_name):
    pass


def build_deb(path, version, app_name):
    dir_path = os.path.dirname(os.path.realpath(__file__))
    print("Build_deb:",dir_path, " path:",path)
    
    #currentDir="{}-{}/".format(path, app_name)
    if app_name == "rolisteam":
        shutil.copy("{}/../rolisteam.desktop".format(dir_path), path)
    shutil.copytree("{}/ubuntu/debian".format(dir_path), "{}/debian".format(path))
    shutil.copy("{}/changelog".format(dir_path), path)
    
    # moving to the temp dir
    
    os.chdir(path)

    os.chdir("..")
    dest_dir="{}-{}/".format(app_name, version)
    print("Build_deb: rename file:",dest_dir)
    os.rename(app_name, dest_dir)
    os.chdir(dest_dir)
    print("#####")
    print(os.getcwd())
    print("#####")

    print("Build_deb: build:",dest_dir)
    run_process(["debuild","-S","-sa"], ".")#"echo","y\n","|",
    #lrelease client/client.pro
    #rm -rf packaging
    #rm -rf .git

def publish_deb(path, version, app_name):
    pass

def build_appimage(path, version, app_name):
    print("build appimage")
    build_directory = build_app(path)

    linuxdeploy_path=merge_path(path, "linuxdeploy-x86_64.AppImage")
    linuxdeployqtplugin_path=merge_path(path, "linuxdeploy-plugin-qt-x86_64.AppImage")
    get_file("https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage",linuxdeploy_path)
    allow_execution(linuxdeploy_path)
    get_file("https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage",linuxdeployqtplugin_path)
    allow_execution(linuxdeployqtplugin_path)
    move_content(merge_path(build_directory,"AppDir/usr/bin/usr/local/bin"), merge_path(build_directory,"AppDir/usr/bin"))
    run_process([linuxdeploy_path,"--appdir","../","AppDir","-e","AppDir/usr/bin/{}".format(app_name),"-i","../$ICON_PATH","-d","$DESKTOP_FILE_PATH","--plugin","qt","--output","appimage"],build_directory)
    appImageArray = glob.glob(build_directory+"*.AppImage")
    shutil.move(appImageArray[0],merge_path(path,"../"))
    run_process(["make","install","INSTALL_ROOT=../AppDir/usr/bin/"],buildDirectory)

def main():
    parser = argparse.ArgumentParser(description="Helper script to generate new rolisteam version.")
    parser.add_argument("-v","--version", help="Give the version of the software")
    parser.add_argument("-p","--package_version",type=int,default=0, help="The package version, default=0")
    parser.add_argument("-b","--branch",default="master", help="Git branch to checkout to build")
    parser.add_argument("-V","--Version",default="1.10.0", help="Version for packages")
    parser.add_argument("-t","--tarball",action="store_true", help="Generate Tarball")
    parser.add_argument("-z","--zip",action="store_true", help="Generate Zip archive")
    parser.add_argument("-d","--deb",action="store_true", help="Create deb on launchpad: -d to upload to the official launchpad, -dd to upload to the dev launchpad.")
    parser.add_argument("-a","--appimage",action="store_true", help="Generate Appimage")
    parser.add_argument("-s","--sourceforge",action="store_true", help="Upload result to sourceforge")
    parser.add_argument("-l","--linux",action="store_true", help="create linux archive")
    #parser.add_argument("-s","--software",default="all", choices=["all","rolisteam","rcse","dice","rcm"], help="List software: all builds everything but rcm.")
    parser.add_argument('softwares', metavar='N', nargs='+',choices=["all","rolisteam","rcse","dice"],
                    help='an integer for the accumulator')
    args = parser.parse_args()

    #repos=["https://invent.kde.org/rolisteam/rolisteam.git","https://invent.kde.org/rolisteam/rcse.git","https://invent.kde.org/rolisteam/rolisteam-diceparser.git","https://github.com/obiwankennedy/rcm"]
    repos=["https://invent.kde.org/rolisteam/rolisteam.git","https://invent.kde.org/rolisteam/rcse.git","https://invent.kde.org/rolisteam/rolisteam-diceparser.git"]
    names=["rolisteam","rolisteam-diceparser"]



    tmpdirname = tempfile.mkdtemp() # dir="./"
    print(tmpdirname)
    i = 0
    for software in args.softwares:
        name=software
        repo=""
        if software == "rolisteam":
            repo="https://invent.kde.org/rolisteam/rolisteam.git"
        elif software == "dice":
            repo="https://invent.kde.org/rolisteam/rolisteam-diceparser.git"


        clone_repo("{}".format(tmpdirname),repo, name)
        path = os.path.join(tmpdirname,name)

        if args.appimage:
            build_appimage(path, args.Version,name)

        if args.tarball:
            build_tarball(path, args.Version, name)

        if args.linux:
            build_linux_binary(path, args.Version, name)

        if args.deb:
            build_deb(path, args.Version, name)
            publish_deb(path, args.Version, name)

        print(args)

        i+=1
    #shutil.rmtree(tmpdirname)


if __name__ == '__main__':
    main()
