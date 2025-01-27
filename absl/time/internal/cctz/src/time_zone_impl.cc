// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   https://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#include "time_zone_impl.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "time_zone_fixed.h"

namespace absl {
inline namespace lts_2019_08_08 {
namespace time_internal {
namespace cctz {

namespace {

// time_zone::Impls are linked into a map to support fast lookup by name.
using TimeZoneImplByName =
    std::unordered_map<std::string, const time_zone::Impl*>;
TimeZoneImplByName* time_zone_map = nullptr;

#if !defined(ESP8266)
// Mutual exclusion for time_zone_map.
std::mutex time_zone_mutex;
#endif

}  // namespace

time_zone time_zone::Impl::UTC() {
  return time_zone(UTCImpl());
}

bool time_zone::Impl::LoadTimeZone(const std::string& name, time_zone* tz) {
  const time_zone::Impl* const utc_impl = UTCImpl();

  // First check for UTC (which is never a key in time_zone_map).
  auto offset = seconds::zero();
  if (FixedOffsetFromName(name, &offset) && offset == seconds::zero()) {
    *tz = time_zone(utc_impl);
    return true;
  }

  // Then check, under a shared lock, whether the time zone has already
  // been loaded. This is the common path. TODO: Move to shared_mutex.
  {
    #if !defined(ESP8266)
    std::lock_guard<std::mutex> lock(time_zone_mutex);
    #endif
    if (time_zone_map != nullptr) {
      TimeZoneImplByName::const_iterator itr = time_zone_map->find(name);
      if (itr != time_zone_map->end()) {
        *tz = time_zone(itr->second);
        return itr->second != utc_impl;
      }
    }
  }

  // Now check again, under an exclusive lock.
  #if !defined(ESP8266)
  std::lock_guard<std::mutex> lock(time_zone_mutex);
  #endif
  if (time_zone_map == nullptr) time_zone_map = new TimeZoneImplByName;
  const Impl*& impl = (*time_zone_map)[name];
  if (impl == nullptr) {
    // The first thread in loads the new time zone.
    Impl* new_impl = new Impl(name);
    new_impl->zone_ = TimeZoneIf::Load(new_impl->name_);
    if (new_impl->zone_ == nullptr) {
      delete new_impl;  // free the nascent Impl
      impl = utc_impl;  // and fallback to UTC
    } else {
      impl = new_impl;  // install new time zone
    }
  }
  *tz = time_zone(impl);
  return impl != utc_impl;
}

void time_zone::Impl::ClearTimeZoneMapTestOnly() {
  #if !defined(ESP8266)
  std::lock_guard<std::mutex> lock(time_zone_mutex);
  #endif
  if (time_zone_map != nullptr) {
    // Existing time_zone::Impl* entries are in the wild, so we simply
    // leak them.  Future requests will result in reloading the data.
    time_zone_map->clear();
  }
}

time_zone::Impl::Impl(const std::string& name) : name_(name) {}

const time_zone::Impl* time_zone::Impl::UTCImpl() {
  static Impl* utc_impl = [] {
    Impl* impl = new Impl("UTC");
    impl->zone_ = TimeZoneIf::Load(impl->name_);  // never fails
    return impl;
  }();
  return utc_impl;
}

}  // namespace cctz
}  // namespace time_internal
}  // inline namespace lts_2019_08_08
}  // namespace absl
