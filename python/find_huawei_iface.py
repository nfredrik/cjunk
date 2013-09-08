#!/usr/bin/python
# 
# Copyright (c) 2009, Slawek Ligus <root@ooz.ie>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
#  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 

"""find_huawei_iface.py finds active USB huawei interfaces."""

import dbus

BUS_NAME = 'org.freedesktop.Hal'
MGR_OBJ = '/org/freedesktop/Hal/Manager'
HAL_DEV_IFACE = 'org.freedesktop.Hal.Device'
HAL_MGR_IFACE = 'org.freedesktop.Hal.Manager'

bus = dbus.SystemBus()

def find_huawei_devices():
    """Find Huawei devices."""

    # Huawei vendor ID
    vendor_id = '12d1'
    hal_mgr_obj = bus.get_object(BUS_NAME, MGR_OBJ)
    hal_mgr = dbus.Interface(hal_mgr_obj, HAL_MGR_IFACE)
    all_dev = hal_mgr.FindDeviceByCapability('serial')
    devices = list()
    for device in all_dev:
        if vendor_id in device:
            devices.append(device)
    return devices

def get_hal_info(udi):
    """Return Huawei interface name and short description."""

    hal_dev = bus.get_object(BUS_NAME, udi)
    dev_property = hal_dev.GetProperty
    serial_port = dev_property('serial.device', dbus_interface=HAL_DEV_IFACE)
    info_product = dev_property('info.product', dbus_interface=HAL_DEV_IFACE)

    return info_product, serial_port

def main():
    """Find Serial interfaces for Huawei USB modems on a system."""

    devices = find_huawei_devices()
    if devices:
        l = len(devices)
        print '%s Huawei interface%s detected.' % (l, l > 1 and 's' or '')
        for dev in devices:
            print "%20s : %s" % get_hal_info(dev)
    else:
        print 'No Huawei devices found.'

if __name__ == '__main__':
    main()
