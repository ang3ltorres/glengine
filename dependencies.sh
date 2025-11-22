#!/bin/bash

set -eu

# libgl1-mesa-dev
# libglu1-mesa-dev
# libwayland-dev
# pkg-config
# libxkbcommon-dev
# libxrandr-dev
# libxinerama-dev
# libxcursor-dev
# libxi-dev

rootDir=$(pwd)
cores="$(nproc)"
echo "Root dir: ${rootDir}"

downloadPath="$rootDir/build/dep/download"
extractPath="$rootDir/build/dep/extract"
buildPath="$rootDir/build/dep/build"
includePath="$rootDir/build/dep/include"
libPath="$rootDir/build/dep/lib"

if [[ -f "$rootDir/build/dep/done" ]]; then
	echo "Files already downloaded and extracted"
	echo "Rebuilding dependencies..."

	rm -rf $buildPath $includePath $libPath
	mkdir -p $buildPath $includePath $libPath

else
	mkdir -p $downloadPath $extractPath $buildPath $includePath $libPath

	# Download dependencies
	echo "Downloading glfw3";    wget -P "$downloadPath" https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz  > /dev/null 2>&1
	echo "Downloading glew";     wget -P "$downloadPath" -O "$downloadPath/glew-2.2.0.tar.gz" https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz  > /dev/null 2>&1
	echo "Downloading glm";      wget -P "$downloadPath" https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz  > /dev/null 2>&1
	echo "Downloading zlib";     wget -P "$downloadPath" https://zlib.net/zlib-1.3.1.tar.gz  > /dev/null 2>&1
	echo "Downloading libpng";   wget -P "$downloadPath" https://download.sourceforge.net/libpng/libpng-1.6.45.tar.gz  > /dev/null 2>&1
	echo "Downloading freetype"; wget -P "$downloadPath" -O "$downloadPath/freetype-2.13.3.tar.gz" https://netactuate.dl.sourceforge.net/project/freetype/freetype2/2.13.3/freetype-2.13.3.tar.gz?viasf=1 > /dev/null 2>&1

	# Extract downloaded dependencies
	for file in "$downloadPath"/*.tar.gz; do
		echo "Extracting $file"
		tar -xvf "$file" -C "$extractPath" > /dev/null 2>&1
	done

	# Create download & extraction flag
	touch $rootDir/build/dep/done
fi

## GLFW3
cd $buildPath; mkdir glfw3; cd glfw3

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D BUILD_SHARED_LIBS=ON \
	-D GLFW_BUILD_DOCS=OFF \
	-D GLFW_BUILD_EXAMPLES=OFF \
	-D GLFW_BUILD_TESTS=OFF \
	-D GLFW_BUILD_WAYLAND=ON \
	-D GLFW_BUILD_X11=ON \
	../../extract/glfw-3.4

make -j $cores
cd src; mv libglfw.so libglfw.so.3 libglfw.so.3.4 $libPath
cp -r "${extractPath}/glfw-3.4/include/GLFW" $includePath/.

## GLEW
cd $buildPath; mkdir glew; cd glew

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D BUILD_UTILS=OFF \
	-D OpenGL_GL_PREFERENCE=GLVND \
	-Wno-dev \
	../../extract/glew-2.2.0/build/cmake

make -j $cores
cd lib; mv libGLEW.so libGLEW.so.2.2 libGLEW.so.2.2.0 $libPath
cp -r "${extractPath}/glew-2.2.0/include/GL" $includePath/.

## GLM
cp -r "${extractPath}/glm-1.0.1/glm" $includePath/.

## ZLIB
cd $buildPath; mkdir zlib; cd zlib

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D ZLIB_BUILD_EXAMPLES=OFF \
	../../extract/zlib-1.3.1

make -j $cores
mv libz.so libz.so.1 libz.so.1.3.1 $libPath
cp zconf.h $includePath/.
cp "${extractPath}/zlib-1.3.1/zlib.h" $includePath/.

## LIBPNG
cd $buildPath; mkdir libpng; cd libpng

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D PNG_BUILD_ZLIB=OFF \
	-D PNG_HARDWARE_OPTIMIZATIONS=ON \
	-D PNG_SHARED=ON \
	-D PNG_STATIC=OFF \
	-D PNG_TESTS=OFF \
	-D PNG_TOOLS=OFF \
	-D ZLIB_INCLUDE_DIR=$includePath \
	-D ZLIB_LIBRARY_RELEASE=$libPath/libz.so \
	../../extract/libpng-1.6.45

make -j $cores
mv libpng.so libpng16.so libpng16.so.16 libpng16.so.16.45.0 $libPath
cp pnglibconf.h $includePath/.
cd $extractPath/libpng-1.6.45; cp png.h pngconf.h $includePath/.

## FREETYPE
cd $buildPath; mkdir freetype; cd freetype

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D BUILD_SHARED_LIBS=ON \
	-D FT_DISABLE_BROTLI=ON \
	-D FT_DISABLE_BZIP2=ON \
	-D FT_DISABLE_HARFBUZZ=ON \
	-D FT_DISABLE_PNG=OFF \
	-D FT_DISABLE_ZLIB=OFF \
	-D FT_REQUIRE_PNG=ON \
	-D FT_REQUIRE_ZLIB=ON \
	-D PNG_LIBRARY_DEBUG=PNG_LIBRARY_DEBUG-NOTFOUND \
	-D PNG_LIBRARY_RELEASE=$libPath/libpng16.so \
	-D PNG_PNG_INCLUDE_DIR=$includePath \
	-D ZLIB_LIBRARY_DEBUG=ZLIB_LIBRARY_DEBUG-NOTFOUND \
	-D ZLIB_LIBRARY_RELEASE=$libPath/libz.so \
	-D ZLIB_INCLUDE_DIR=$includePath \
	../../extract/freetype-2.13.3

make -j $cores
mv libfreetype.so libfreetype.so.6 libfreetype.so.6.20.2 $libPath
cd $extractPath/freetype-2.13.3/include; cp -r --preserve=all ./ $includePath/.
cd $buildPath/freetype/include/freetype/config; cp *.h $includePath/freetype/config/.
