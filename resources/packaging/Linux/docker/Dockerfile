#
# Dockerfile for rolisteam-server
#


# Use Debian as base for the build
ARG DEBIAN_VERSION=trixie
FROM debian:${DEBIAN_VERSION} AS builder


# Base directory for installed build artifacts.
# Due to limitations of the Docker image build process, this value is
# duplicated in an ARG in the second stage of the build.
#

ARG PREFIX_DIR=/usr/local/rolisteam

# Build arguments
ARG BUILD_DIR=/tmp/rolisteam-docker-BUILD
ARG BUILD_DEPENDENCIES="\
	qt6-base-dev			\
	qt6-base-dev-tools 		\
	cmake libqt6network6 	\
	zlib1g-dev build-essential 	\
	git 				\
    ca-certificates			\
	libqt6multimedia6 		\
	libqt63dcore6 			\
	libqt6gui6                      \ 
	libqt6pdf6            		\
	libqt6qml6 			\
	qtdeclarative5-dev		\	
	libqt6scxml6			\
	libqt6svg6			\
	libqt6quickcontrols2-6		\
	libqt6quick6			\
	libqt6webchannel6		\
	libqt6quicktemplates2-6		\
	libqt6webenginequick6		\
	libqt6webchannelquick6		\
	libqt6multimediawidgets6 \
	libqt6quickwidgets6		\
	libqt6websockets6		\
	libqt6widgets6			\
	qt6-declarative-dev		\
	qt6-declarative-dev-tools	\
	qt6-pdf-dev			\
	qt6-httpserver-dev \
	qt6-l10n-tools			\
	qt6-quick3dphysics-dev-tools	\
	qt6-quick3d-dev			\
	qt6-quick3d-dev-tools		\
	qt6-websockets-dev \
	qt6-websockets-private-dev \
	qml6-module-qtwebsockets \
	qt6-webengine-dev  \
	qt6-multimedia-dev \
	qml6-module-qtmultimedia \
	qml6-module-qtquick3d-spatialaudio \
	qt6-svg-dev			\
	qt6-scxml-dev			\
	qt6-tools-dev			\
	qml6-module-qtwebengine \
	qt6-remoteobjects-dev \
	qt6-tools-dev-tools		\
	libqt6webenginecore6    \
	qt6-quick3dphysics-dev"

# build environment up to date and install build dependencies
#
RUN apt-get update                         && \
	apt-get --no-install-recommends install -y $BUILD_DEPENDENCIES && \
	rm -rf /var/lib/apt/lists/*

RUN mkdir $BUILD_DIR && \
    git -C $BUILD_DIR clone --recursive https://invent.kde.org/rolisteam/rolisteam.git && \
    cd $BUILD_DIR/rolisteam && \
    mkdir build && \
    cd build && \
    cmake ../ -DUPDATE_TRANSLATIONS:BOOL=ON && \
    cmake --build . --target release_translations roliserver install && \
    cd ~ && \
    rm -rf $BUILD_DIR && \
    apt-get -y remove *-dev *dev-tools  && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    /usr/local/bin/roliserver -p /opt/default.config 

# Expose the default listener port
EXPOSE 6660

CMD /usr/local/bin/roliserver -c /opt/default.config 


