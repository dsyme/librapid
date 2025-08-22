/*
 * I do not take credit for this code. It is taken from the following repository, and all
 * credit goes to the original author(s):
 *
 * https://github.com/NickStrupat/CacheLineSize
 *
 * Many thanks for making this code available and open source!
 *
 * The code has been modified slightly to better fit the needs of LibRapid, however, so the code
 * below is not identical to the original.
 */

#include <librapid/librapid.hpp>

#if defined(LIBRAPID_APPLE)

#    include <sys/sysctl.h>

namespace librapid {
    size_t cacheLineSize() {
        // Cache the result to avoid system call overhead on repeated calls
        static size_t cachedLineSize = 0;
        if (cachedLineSize != 0) {
            return cachedLineSize;
        }

        size_t lineSize = 64; // Default fallback
        size_t sizeOfLineSize = sizeof(lineSize);
        
        int result = sysctlbyname("hw.cachelinesize", &lineSize, &sizeOfLineSize, 0, 0);
        if (result == 0 && lineSize >= 32 && lineSize <= 256) {
            cachedLineSize = lineSize;
            return lineSize;
        }
        
        // Fallback to safe default
        cachedLineSize = 64;
        return 64;
    }
} // namespace librapid

#elif defined(LIBRAPID_WINDOWS) && !defined(LIBRAPID_NO_WINDOWS_H)

namespace librapid {
    size_t cacheLineSize() {
        // Cache the result to avoid system call overhead on repeated calls
        static size_t cachedLineSize = 0;
        if (cachedLineSize != 0) {
            return cachedLineSize;
        }

        size_t lineSize = 64; // Default fallback
        DWORD bufferSize = 0;

        // Get required buffer size
        GetLogicalProcessorInformation(0, &bufferSize);
        if (bufferSize == 0) {
            cachedLineSize = 64;
            return 64;
        }

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION *buffer = 
            (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(bufferSize);
        if (!buffer) {
            // Memory allocation failed - return safe default
            cachedLineSize = 64;
            return 64;
        }

        if (GetLogicalProcessorInformation(&buffer[0], &bufferSize)) {
            DWORD count = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            for (DWORD i = 0; i < count; ++i) {
                if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
                    lineSize = buffer[i].Cache.LineSize;
                    break;
                }
            }
        }

        free(buffer);
        cachedLineSize = lineSize;
        return lineSize;
    }
} // namespace librapid

#elif defined(LIBRAPID_LINUX)

namespace librapid {
    size_t cacheLineSize() {
        // Cache the result to avoid file system access on repeated calls
        static size_t cachedLineSize = 0;
        if (cachedLineSize != 0) {
            return cachedLineSize;
        }

        FILE *p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
        unsigned int lineSize = 64;
        if (p) {
            int result = fscanf(p, "%u", &lineSize);
            fclose(p);
            // Validate the read was successful and value is reasonable
            if (result == 1 && lineSize >= 32 && lineSize <= 256) {
                cachedLineSize = lineSize;
                return lineSize;
            }
        }
        
        // Fallback: try alternative sysfs path
        p = fopen("/sys/devices/system/cpu/cpu0/cache/index1/coherency_line_size", "r");
        if (p) {
            int result = fscanf(p, "%u", &lineSize);
            fclose(p);
            if (result == 1 && lineSize >= 32 && lineSize <= 256) {
                cachedLineSize = lineSize;
                return lineSize;
            }
        }
        
        // Final fallback to safe default
        cachedLineSize = 64;
        return 64;
    }
} // namespace librapid

#else

namespace librapid {
    size_t cacheLineSize() {
        // On unknown platforms, return 64
        return 64;
    }
} // namespace librapid

#endif