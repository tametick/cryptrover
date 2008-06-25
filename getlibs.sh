#!/bin/sh

linux() {
    echo "Linux" ;
    wget -P lib/ http://cryptrover.googlecode.com/files/libXCurses.a ;
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

