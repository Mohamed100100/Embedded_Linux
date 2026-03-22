




if [ -f "/home/gemy/.bashrc" ]; then
    echo "File exists :)"
    export HELLO=$HOSTNAME
    LOCAL=$USER
    gnome-terminal
else
    echo "File does not exist :("
fi

