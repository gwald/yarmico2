#!/bin/bash
set -x #echo on
echo set -e

# work/test build - 
# mkdir build; cd build;  cmake .. -D32BLIT_DIR=$HOME/32blit-sdk -DCMAKE_BUILD_TYPE=Debug 

# create deploy folders
# mkdir build.pico; cd build.pico;  cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/pico.toolchain -D32BLIT_DIR=$HOME/32blit-sdk -DPICO_SDK_PATH=$HOME/pico-sdk -DPICO_EXTRAS_PATH=$HOME/pico-extras -DPICO_BOARD=pimoroni_picosystem -DCMAKE_BUILD_TYPE=Release 
# mkdir build.vga; cd build.vga;  cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/pico.toolchain -D32BLIT_DIR=$HOME/32blit-sdk -DPICO_SDK_PATH=$HOME/pico-sdk -DPICO_EXTRAS_PATH=$HOME/pico-extras -DPICO_BOARD=vgaboard -DCMAKE_BUILD_TYPE=Release 
		
#mkdir build.vga.new.sdk; cd build.vga.new.sdk;  cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/pico.toolchain -D32BLIT_DIR=$HOME/32blit-sdk-new -DPICO_SDK_PATH=$HOME/pico-sdk -DPICO_EXTRAS_PATH=$HOME/pico-extras -DPICO_BOARD=vgaboard -DCMAKE_BUILD_TYPE=Release 					       

# mkdir build.32blit; cd build.32blit; cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/32blit.toolchain -D32BLIT_DIR=$HOME/32blit-sdk -DCMAKE_BUILD_TYPE=Release  
# mkdir build.release; cd build.release;  cmake .. -D32BLIT_DIR=$HOME/32blit-sdk -DCMAKE_BUILD_TYPE=Release 

# mkdir build.windows; cd build.windows;  cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/mingw.toolchain -D32BLIT_DIR=$HOME/32blit-sdk -DCMAKE_BUILD_TYPE=Release 
# mkdir build.html5; cd build.html5; emcmake cmake .. -D32BLIT_DIR=$HOME/32blit-sdk  -DCMAKE_BUILD_TYPE=Release 

PROJHOME=/home/dev/eclipse-workspace/yarmico2
PROJDEPLOY=$PROJHOME/00-deploy

mkdir -p $PROJDEPLOY/HTML5/
mkdir -p $PROJDEPLOY/picosystem
mkdir -p $PROJDEPLOY/vgaboard
mkdir -p $PROJDEPLOY/32blit
mkdir -p $PROJDEPLOY/PC/linux
mkdir -p $PROJDEPLOY/PC/windows


echo for logging errors, run
echo ./backup.sh > out.txt 2>&1

cd $PROJHOME
rm $PROJDEPLOY/picosystem/*
rm $PROJDEPLOY/vgaboard/*
rm $PROJDEPLOY/32blit/*
rm $PROJDEPLOY/PC/linux/*
rm $PROJDEPLOY/PC/windows/*
rm $PROJDEPLOY/HTML5/*


pwd




echo 32blit -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef HARDWARE_TARGET " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#define HARDWARE_TARGET HARDWARE_TARGET_32BLIT_HANDHELD " >> /home/dev/eclipse-workspace/yarmico2/build.h
#echo "#define printf // " >> /home/dev/eclipse-workspace/yarmico2/build.h




cd build.32blit
make clean
pwd
make -j1 

echo make ; ll yarmico2.blit ; 32blit install yarmico2.blit ; 32blit launch yarmico2.blit



echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "yarmico2.blit" ]; then
    echo "yarmico2.blit exists."
    cp yarmico2.blit $PROJDEPLOY/32blit/yarmico2.blit
    ll $PROJDEPLOY/32blit/yarmico2.blit
else 
    echo "yarmico2.blit does not exist."
    pwd
    exit 1
fi


make clean
echo 32blit -----------------------------------------------------
pwd
cd ..
pwd


echo yarmico2 -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef HARDWARE_TARGET " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#define HARDWARE_TARGET HARDWARE_TARGET_PICOSYSTEM " >> /home/dev/eclipse-workspace/yarmico2/build.h
#echo "#define printf // " >> /home/dev/eclipse-workspace/yarmico2/build.h


cd build.pico
make clean
pwd
make -j1 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "yarmico2.uf2" ]; then
    echo "yarmico2.uf2 exists."
    cp yarmico2.uf2 $PROJDEPLOY/picosystem/yarmico2-picosystem.uf2
    ll $PROJDEPLOY/picosystem/yarmico2-picosystem.uf2
else 
    echo "yarmico2.uf2 does not exist."
    pwd
    exit 1
fi


make clean
echo pico -----------------------------------------------------
pwd
cd ..
pwd




echo yarmico2 -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef HARDWARE_TARGET " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#define HARDWARE_TARGET HARDWARE_TARGET_VGABOARD " >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#define YARMICO_NATIVE_DISPLAY YARMICO_NATIVE_DISPLAY_160x120 // VGA board is 160x120 boarded widths" >> /home/dev/eclipse-workspace/yarmico2/build.h
echo "#define printf // " >> /home/dev/eclipse-workspace/yarmico2/build.h





cd build.vga
make clean
pwd
make -j1 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "yarmico2.uf2" ]; then
    echo "yarmico2.uf2 exists."
    cp yarmico2.uf2 $PROJDEPLOY/vgaboard/yarmico2-vgaboard.uf2
    ll $PROJDEPLOY/vgaboard/yarmico2-vgaboard.uf2
else 
    echo "yarmico2.uf2 does not exist."
    pwd
    exit 1
fi


make clean
echo pico -----------------------------------------------------
pwd
cd ..
pwd






echo linux  -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/yarmico2/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/yarmico2/build.h
#echo "#define printf // " >> /home/dev/eclipse-workspace/yarmico2/build.h

# default to SDL echo "#define HARDWARE_TARGET HARDWARE_TARGET_32BLIT_HANDHELD " >> /home/dev/eclipse-workspace/yarmico2/build.h

echo move build dir, instead of rebuilding everytime!


#cd audio
#./setupAudioFiles.sh 1
#cd ..

cd build.release
make clean
pwd
make -j1 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "yarmico2" ]; then
    echo "yarmico2 exists."
    cp yarmico2 $PROJDEPLOY/PC/linux/yarmico2.x86-x64
    ll $PROJDEPLOY/PC/linux/yarmico2.x86-x64
else 
    echo "yarmico2 does not exist."
    pwd
    exit 1
fi


make clean

pwd
echo linux  -----------------------------------------------------
cd ..

pwd

echo win -----------------------------------------------------

cd build.windows
pwd
make clean
make -j1

#echo will fail on metadata
#cp ../_metadata.cpp  .
#echo rebuild



echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "yarmico2.exe" ]; then
    echo "yarmico2 exists."   
	cp -a yarmico2.exe $PROJDEPLOY/PC/windows/yarmico2-x86-x64.exe
	ll $PROJDEPLOY/PC/windows/yarmico2-x86-x64.exe
else 
    echo "yarmico2 does not exist."
    pwd
    exit 1
fi


make clean

pwd
echo win -----------------------------------------------------
cd ..


pwd

echo html5 -----------------------------------------------------

cd build.html5
pwd
make clean 
make -j1   || true

echo build twice just in case first has a moment of weakness!
make -j1   || true

echo remove and redo with my link.txt file - custom web html
rm CMakeFiles/yarmico2.dir/yarmico2.c.*; rm yarmico2.*
cp  $PROJHOME/link.txt   $PROJHOME/build.html5/CMakeFiles/yarmico2.dir/
make -j1

if [ -f "yarmico2.html" ]; then
    echo "yarmico2 HTML exists."    
    cp yarmico2.* $PROJDEPLOY/HTML5
    mv $PROJDEPLOY/HTML5/yarmico2.html $PROJDEPLOY/HTML5/index.html
    ll $PROJDEPLOY/HTML5
else 
    echo "yarmico2 HTML does not exist."
    pwd
    exit 1
fi





make clean
pwd

echo html5 -----------------------------------------------------
cd ..





pwd

echo zipping up -----------------------------------------------------

cd $PROJHOME/..
now="yarmico2-linux-$(date +"%m_%d_%Y_%s").tar.gz"

echo "$now"

echo compressing...

# rm -rf /home/dev/eclipse-workspace/test/audio/include/*

# 7z a "$now" -xr~/eclipse-workspace/test/audio ~/eclipse-workspace/test
tar -cvpzf "$now" --exclude='test/audio'  --one-file-system yarmico2
mv "$now" /media/00-keep/scrolla-backup/


echo "" > $PROJHOME/build.h


