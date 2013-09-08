#-----------------------------------------------------------------------------
#
#      File         :  modunload
#      Abstract     :  Unloads the PLX module
#      Last Revision:  08-03-26
#
#-----------------------------------------------------------------------------

clear

# Remove comment to display script commands as it runs
#set -x

# Path to the driver nodes
path=/dev





# Check for debug version
if [ "$1" != "" ]; then
    debug=_dbg
fi

# Registered name of driver
name="plx_drv"

# Name of module to unload
module=${name}${debug}.ko


echo
echo "   ****************************************************"
echo "   * NOTE:  You must be superuser, logged in as root, *"
echo "   *        or have sufficient rights to remove       *"
echo "   *        modules or this script will not work.     *"
echo "   ****************************************************"
echo
echo
echo -n "Clear existing device nodes..... "
rm -f $path/${name}
echo "Ok (${path}/${name})"

# Delete the directory only if empty
#if [ -d ${path} ]; then
#    echo -n "Delete device node path......... "
#    rmdir --ignore-fail-on-non-empty ${path}
#    echo "Ok (${path})"
#fi

echo -n "Remove module................... "
#/sbin/rmmod $module
/sbin/rmmod $name
echo "Ok ($module)"

echo
echo Module unload complete.
echo
echo
