#!/bin/bash
# Determine OS platform
UNAME=$(uname | tr "[:upper:]" "[:lower:]")
# If Linux, try to determine specific distribution
if [ "$UNAME" == "linux" ]; then
    # If available, use LSB to identify distribution
    if [ -f /etc/lsb-release -o -d /etc/lsb-release.d ]; then
        export DISTRO=$(lsb_release -i | cut -d: -f2 | sed s/'^\t'//)
    # Otherwise, use release info file
    else
        export DISTRO=$(ls -d /etc/[A-Za-z]*[_-][rv]e[lr]* | grep -v "lsb" | cut -d'/' -f3 | cut -d'-' -f1 | cut -d'_' -f1)
    fi
fi
# For everything else (or if above failed), just use generic identifier
[ "$DISTRO" == "" ] && export DISTRO=$UNAME
unset UNAME

echo "Your Linux distro is: $DISTRO"
echo "Currently supported distros: Ubuntu, Arch"
echo "Installing dependencies: qt5base-dev, nasm, make, cmake, gcc, build-essential"

if [[ $DISTRO == *"ubuntu"* ]]; then
	sudo apt install -y nasm cmake make gcc build-essential qt5base-devel
fi

if [[ $DISTRO == *"arch"* ]]; then 
	sudo pacman -Sy nasm cmake make gcc base-devel qt
fi

echo "Dependecies installed!"

echo "To build the project run './buildAll.sh' bash script or execute following commands in this order:"
echo "$ cmake ."
echo "$ cd asm_src; make; cd .."
echo "$ cd c_src; make; cd .."
echo "$ make"

echo ""
echo "Then to run: $ ./projekt"
