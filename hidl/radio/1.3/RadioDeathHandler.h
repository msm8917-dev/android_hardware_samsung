#pragma once

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SehRadioDeathReceiver"
#include <log/log.h>

#include <hidl/HidlSupport.h>

namespace android {
namespace hardware {
namespace radio {
namespace V1_3 {
namespace implementation {

using ::android::hidl::base::V1_0::IBase;

template <class I>
struct RadioHandle {
  sp<I> *instanceRef;
  std::string name;
};

// Handles the case where any of radio binder dies.
template <class I>
struct SehRadioDeathRecipient : hidl_death_recipient {
  void serviceDied(uint64_t cookie, const android::wp<IBase> & /* who */) override {
    ALOGW("A radio binder has died");
    if (cookie > 0) {
      auto ref = reinterpret_cast<RadioHandle<I> *>(cookie);
      if (ref) {
        ALOGI("Name: '%s', invalidating local reference to it.", ref->name.c_str());
        if (ref->instanceRef) {
          ref->instanceRef->clear();
        } else {
          ALOGW("Cookie contains null pointer to the binder");
        }
        delete ref; // We no longer need it, instance ref is dead binder
      } else {
        ALOGE("Cookie is not null, casting it returns null?");
      }
    } else {
      // This is an unsigned 64bit int. Effectively cookie == 0
      ALOGE("Cookie is NULL, cannot obtain reference to binder");
    }
  }
};

} // namespace implementation
} // namespace V1_3
} // namespace radio
} // namespace hardware
} // namespace android
