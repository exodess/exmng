#!/bin/bash

echo "Starting the installation script..."
echo "Running the project build..."
make 2> /dev/null
if [[ $? -ne 0 ]]; then
	echo -e "\033[1m\033[31m""An error occured during the build, the installation crashed!" "\033[0m"
	if [[ -e *.o ]]; then
		rm *.o
	fi
	exit 1
fi
rm *.o

echo "The build completed succcessfully"
echo "Running the project installation..."

src="exmng"

file="$HOME/$src"

filerc="$HOME/.$(echo $SHELL | cut -f 3 -d "/")rc"

echo "alias $src='$file'" >> $filerc
source $filerc

echo -e "\033[1m\033[32m""The installation completed successfully!" "\033[0m"

echo -e "(Type \033[1m\033[97m source $filerc \033[0m or reload the console)"

exit 0
