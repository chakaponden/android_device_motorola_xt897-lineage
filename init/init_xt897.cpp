/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>
#include <android-base/logging.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void vendor_load_properties()
{
    std::string carrier, device, modelno, platform;
    char hardware_variant[92];
    FILE *fp;

    platform = android::base::GetProperty("ro.board.platform", "");
    if (platform != ANDROID_TARGET)
        return;

    modelno = android::base::GetProperty("ro.boot.modelno", "");
    carrier = android::base::GetProperty("ro.boot.carrier", "");
    fp = popen("/system/xbin/sed -n '/Hardware/,/Revision/p' /proc/cpuinfo | /system/xbin/cut -d ':' -f2 | /system/xbin/head -1", "r");
    fgets(hardware_variant, sizeof(hardware_variant), fp);
    pclose(fp);

    property_override("ro.product.device", "asanti_c");
    property_override("ro.product.model", "PHOTON Q");
    if (modelno == "XT897") {
        /* xt897 CSpire */
        property_override("ro.build.description", "asanti_c_cspire-user 4.1.2 9.8.2Q-122_XT897_FFW-7 8 release-keys");
        property_override("ro.build.fingerprint", "motorola/XT897_us_csp/asanti_c:4.1.2/9.8.2Q-122_XT897_FFW-7/8:user/release-keys");
        android::init::property_set("ro.cdma.home.operator.alpha", "Cspire");
        android::init::property_set("ro.cdma.home.operator.numeric", "311230");
    } else if (carrier == "sprint") {
        /* xt897 Sprint */
        property_override("ro.build.description", "XT897_us_spr-user 4.1.2 9.8.2Q-122_XT897_FFW-5 6 release-keys");
        property_override("ro.build.fingerprint", "motorola/XT897_us_spr/asanti_c:4.1.2/9.8.2Q-122_XT897_FFW-5/6:user/release-keys");
        android::init::property_set("ro.cdma.international.eri", "2,74,124,125,126,157,158,159,193,194,195,196,197,198,228,229,230,231,232,233,234,235");
        android::init::property_set("ro.cdma.home.operator.alpha", "Sprint");
        android::init::property_set("ro.cdma.home.operator.numeric", "310120");
        android::init::property_set("ro.com.google.clientidbase.ms", "android-sprint-us");
        android::init::property_set("ro.com.google.clientidbase.am", "android-sprint-us");
        android::init::property_set("ro.com.google.clientidbase.yt", "android-sprint-us");
    }

    device = android::base::GetProperty("ro.product.device", "");
    LOG(INFO) << "Found carrier id: " << carrier.c_str() << " "
              << "hardware: " << hardware_variant << " "
              << "model no: " << modelno.c_str() << " "
              << "Setting build properties for " << device.c_str() << " device";
}
