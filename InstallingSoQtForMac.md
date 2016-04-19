# SoQt #

## Introduction ##

[SoQt](http://doc.coin3d.org/SoQt/) is a library that provides the glue between Systems in Motion's Coin high-level 3D visualization library and Trolltech's Qt 2D user interface library. This pages provides instruction regarding how to install SoQt in your computer.

Before installing SoQt, please, verify that Qt and Coin 3D are already installed and functioning on your PC. If they are not, install them firs by following the corresponding installation procedures InstallingQtForMac and [InstallingCoin3DForMac](InstallingCoin3DForMac.md).

## Details ##

Download the latest realese of SoQt, currently [SoQt-1.5.0](http://www.coin3d.org/lib/soqt/releases/1.5.0), from the [Coin3D](http://www.coin3d.org/) website at http://www.coin3d.org/lib/plonesoftwarecenter_view. Place it into the desktop.

Open a shell console and navigate to desktop and extract the archive by typing:
```
tar -xvf SoQt-1.5.0.tar
```

This will create a directory called _SoQt-1.5.0_ in your user directory. Using the shell navigate to just created SoQt-1.5.0 directory. First create release libraries by typing:
```
./configure --enable-debug=no  --enable-symbols=no --with-qt=/usr/local/Trolltech/Qt-4.7.0 --with-coin=/usr/local --without-framework
```

This will start the configuration process. Depending on your computer, this process may take some time to finish. When it does, install the simage libraries by typing:
```
make 
make install
```

At the end of the installation process, the libraries hab been installed at /usr/local directory. Now, [Marble](InstallingMarbleForMac.md) installation can be started.


[Mac Configuration](InstallingForMac.md) | [Wiki Home](http://code.google.com/p/tonatiuh/w/list)