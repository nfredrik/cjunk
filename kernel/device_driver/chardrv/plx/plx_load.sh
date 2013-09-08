#-----------------------------------------------------------------------------
#      Abstract     :  Loads the PLX module
#      Last Revision:  08-03-26
#-----------------------------------------------------------------------------


clear

#set -x

# Path to the driver node
path=/dev

# Check for debug version
#if [ "$1" != "" ]; then
#;B    debug=_dbg
#fi

# Registered name of driver
name="plx_drv"

# Name of module to load
module=${name}${debug}.ko

# Install the module
echo
echo "   *****************************************************************"
echo "   * NOTES:                                                        *"
echo "   *                                                               *"
echo "   *  You must be superuser, logged in as root, or have sufficient *"
echo "   *  rights to install modules or this script will not work.      *"
echo "   *                                                               *"
echo "   *  A warning regarding 'kernel tainting' is normal.  This is    *"
echo "   *  because the PLX driver is marked with a 'Proprietary'        *"
echo "   *  license tag, not GPL.  For more information, please refer    *"
echo "   *  to:                                                          *"
echo "   *        http://www.tux.org/lkml/#export-tainted                *"
echo "   *****************************************************************"
echo

echo "Installing module ($module)...."
/sbin/insmod ${module}
echo

# Get the major number
echo -n "Getting Module major number..... "
major=`cat /proc/devices | awk "\\$2==\"$name\" {print \\$1}"`

# Check if valid
if [ "$major" = "" ]; then
    echo "ERROR: Module not loaded or no device found"
    echo
    exit
fi

# Display Major ID
echo "Ok (MajorID = $major)"

# Create the device node path
echo -n "Creating device node path....... "
if [ -d ${path} ]; then
    echo "$path already exists"
else
    mkdir $path
    chmod 0777 $path
    echo "Ok ($path)"
fi

# Create the device nodes (up to 10 by default)
echo -n "Creating device nodes........... "
rm -f ${path}/${name}*
mknod ${path}/${name} c $major 255


#chmod 777 $path/*
echo "Ok ($path/$name)"

echo
echo Module load complete.
echo
echo
