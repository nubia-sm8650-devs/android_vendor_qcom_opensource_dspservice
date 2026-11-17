/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/vendor/qti/hardware/dsp/BnDspService.h>
#include <android-base/logging.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <errno.h>
#include <fcntl.h>

#include <cstdlib>
#include <string>

using ::aidl::vendor::qti::hardware::dsp::BnDspService;

namespace {

constexpr int32_t kCdspDomain = 3;
constexpr char kCdspNode[] = "/dev/fastrpc-cdsp";

class DspService : public BnDspService {
  public:
    ndk::ScopedAStatus openSession(int32_t domain, ndk::ScopedFileDescriptor* node) override {
        if (domain != kCdspDomain) {
            LOG(ERROR) << "Refused domain " << domain << " for pid " << AIBinder_getCallingPid();
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        int fd = open(kCdspNode, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
        if (fd < 0) {
            int error = errno;
            PLOG(ERROR) << "Failed to open " << kCdspNode;
            return ndk::ScopedAStatus::fromServiceSpecificError(error);
        }

        LOG(INFO) << "Opened " << kCdspNode << " for pid " << AIBinder_getCallingPid();
        *node = ndk::ScopedFileDescriptor(fd);
        return ndk::ScopedAStatus::ok();
    }
};

}  // namespace

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(4);
    ABinderProcess_startThreadPool();

    std::shared_ptr<DspService> service = ndk::SharedRefBase::make<DspService>();

    const std::string instance = std::string(DspService::descriptor) + "/default";
    binder_status_t status =
            AServiceManager_addService(service->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK);

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;
}
