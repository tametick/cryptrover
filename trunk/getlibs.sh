#!/bin/sh

linux() {
    echo "Linux" ;
    mkdir tmp ;
    cd tmp ;
    wget http://heanet.dl.sourceforge.net/sourceforge/pdcurses/PDCurses-3.3.tar.gz ;
    tar -xzf PDCurses-3.3.tar.gz ;
    cd PDCurses-3.3 ;
    ./configure
    cd x11 ;
    cp Makefile Makefile.backup ;
    sed s/\-O2/\-Os/ Makefile.backup > Makefile
    make ;
    cp libXCurses.a ../../../lib/
    cd ../../../
    rm -rf tmp/
}
windows() {
    echo "Windows" ;
    wget -P lib/ http://cryptrover.googlecode.com/files/pdcurses.a ;
}

while : 
echo "[l] Get pdcurses for linux"
echo "[w] Get pdcurses for windows"
echo "[q] Quit"
echo "----"
do
    echo -n "Enter your choice [l,w,q]: "
    read ch
    case $ch in
        l) linux ;;
        w) windows ;;
        q) echo "Quiting" ; exit 0 ;;
        *) echo "Invalid choice, please try again" ;;
    esac
    echo "----"
done

