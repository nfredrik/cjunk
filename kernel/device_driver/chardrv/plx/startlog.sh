#-----------------------------------------------------------------------------
#
#      File         :  startlog
#      Abstract     :  Set up a terminal window to capture module debug output
#      Last Revision:  08-03-26
#      SDK Version  :  3.50
#
#-----------------------------------------------------------------------------

clear

echo
echo "   ****************************************************"
echo "   * NOTE:  You must be superuser, logged in as root, *"
echo "   *        or have sufficient rights to kill/start   *"
echo "   *        daemons and view log messages.  This      *"
echo "   *        script will not work, otherwise.          *"
echo "   ****************************************************"
echo

# Stop kernel log daemon if running
if [ -f /var/run/klogd.pid ]; then
    echo Stopping current klogd...
    kill -s SIGTERM `cat /var/run/klogd.pid`
    # Delay to let daemon halt
    sleep 1
fi

# Stop current daemon if running
if [ -f /var/run/syslogd.pid ]; then
    echo Stopping current syslogd...
    kill -s SIGTERM `cat /var/run/syslogd.pid`
    # Delay to let daemon halt
    sleep 1
fi

# Start new daemon
echo Starting new syslogd...
/sbin/syslogd

echo Starting log to console...
echo ">>>>>"
echo
cat /proc/kmsg
