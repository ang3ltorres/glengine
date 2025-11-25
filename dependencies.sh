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
	echo "Dependencies already built"
	exit 0
else
	mkdir -p $downloadPath $extractPath $buildPath $includePath $libPath

	# Download dependencies
	echo "Downloading glfw3";     wget -P "$downloadPath" https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz > /dev/null 2>&1
	echo "Downloading glew";      wget -P "$downloadPath" -O "$downloadPath/glew-2.2.0.tar.gz" https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz > /dev/null 2>&1
	echo "Downloading glm";       wget -P "$downloadPath" https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz > /dev/null 2>&1
	echo "Downloading freetype";  wget -P "$downloadPath" -O "$downloadPath/freetype-2.13.3.tar.gz" https://netactuate.dl.sourceforge.net/project/freetype/freetype2/2.13.3/freetype-2.13.3.tar.gz?viasf=1 > /dev/null 2>&1
	echo "Downloading tinygltf";  wget -P "$downloadPath" https://github.com/syoyo/tinygltf/archive/refs/tags/v2.9.7.tar.gz > /dev/null 2>&1
	echo "Downloading stbvorbis"; wget -P "$downloadPath" https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_vorbis.c > /dev/null 2>&1
	echo "Downloading miniaudio"; wget -P "$downloadPath" https://github.com/mackron/miniaudio/archive/refs/tags/0.11.23.tar.gz > /dev/null 2>&1

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

## FREETYPE
cd $buildPath; mkdir freetype; cd freetype

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D BUILD_SHARED_LIBS=ON \
	-D FT_DISABLE_BROTLI=ON \
	-D FT_DISABLE_BZIP2=ON \
	-D FT_DISABLE_HARFBUZZ=ON \
	-D FT_DISABLE_PNG=ON \
	-D FT_DISABLE_ZLIB=ON \
	-D FT_REQUIRE_PNG=OFF \
	-D FT_REQUIRE_ZLIB=OFF \
	../../extract/freetype-2.13.3

make -j $cores
mv libfreetype.so libfreetype.so.6 libfreetype.so.6.20.2 $libPath
cd $extractPath/freetype-2.13.3/include; cp -r --preserve=all ./ $includePath/.
cd $buildPath/freetype/include/freetype/config; cp *.h $includePath/freetype/config/.

## TINYGLTF
cd $buildPath; mkdir tinygltf; cd tinygltf

cmake \
	-G "Unix Makefiles" \
	-D CMAKE_BUILD_TYPE=Release \
	-D BUILD_SHARED_LIBS=ON \
	-D TINYGLTF_BUILD_BUILDER_EXAMPLE=OFF \
	-D TINYGLTF_BUILD_GL_EXAMPLES=OFF \
	-D TINYGLTF_BUILD_LOADER_EXAMPLE=OFF \
	-D TINYGLTF_BUILD_VALIDATOR_EXAMPLE=OFF \
	-D TINYGLTF_HEADER_ONLY=OFF \
	../../extract/tinygltf-2.9.7

make -j $cores
mv libtinygltf.so $libPath
cd $extractPath/tinygltf-2.9.7; cp *.h *.hpp $includePath/.

## STB_VORBIS
cd $buildPath; mkdir stb_vorbis; cd stb_vorbis
cp $downloadPath/stb_vorbis.c .
g++ -c stb_vorbis.c -o stb_vorbis.o -O3 -s
ar rcs libstb_vorbis.a stb_vorbis.o
mv libstb_vorbis.a $libPath
cp stb_vorbis.c $includePath/.

## MINIAUDIO
cp -p $extractPath/miniaudio-0.11.23/*.h $includePath/.
