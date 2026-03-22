#!/bin/bash


mkdir -p ~/myDirectory/secondDirectory

touch ~/myDirectory/secondDirectory/myNotePaper

cp ~/myDirectory/secondDirectory/myNotePaper ~/myDirectory/myOldNotePaper

echo "DONE :) \n"
tree ~/myDirectory/
