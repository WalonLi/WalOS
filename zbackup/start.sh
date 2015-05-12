#
#   Provide by Walon Li
#   2015/03/29
#
#   start.sh

#!/bin/sh
make clean
make
sudo make copy
bochs

