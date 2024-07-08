////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2024.07.08 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#if __ANDROID__

#include "numeric_cast.hpp"
#include <jni.h>
#include <array>
#include <cstdint>
#include <string>

namespace pfs {

// References:
// 1. https://developer.android.com/training/articles/perf-jni#utf-8-and-utf-16-strings:~:text=stack-allocated%20buffer
/**
 * ByteArrayContainer requirements:
 *      * implements default constructor
 *      * contains ByteArrayContainer::size_type definition
 *      * implements constructor ByteArrayContainer(char const * begin, char const * last);
 *
 * Suitable types from standard C++ library:
 *      * std::string
 *      * std::vector<char>
 */
template <typename ByteArrayContainer, jint BUFFER_SIZE = 1024>
ByteArrayContainer jbyte_array_to (JNIEnv * env, jbyteArray bytes, bool fast = false)
{
    static jbyte static_buf[BUFFER_SIZE];

    if (bytes == nullptr)
        return ByteArrayContainer{};

    ByteArrayContainer result;
    auto size = pfs::numeric_cast<typename ByteArrayContainer::size_type>(env->GetArrayLength(bytes));

    if (size == 0)
        return ByteArrayContainer{};

    if (fast) {
        jboolean isCopy;
        jbyte * buf = static_cast<jbyte *>(env->GetPrimitiveArrayCritical(bytes, nullptr));
        result = ByteArrayContainer(reinterpret_cast<char const *>(buf)
            , reinterpret_cast<char const *>(buf) + size);
        env->ReleasePrimitiveArrayCritical(bytes, buf, JNI_ABORT);
        return result;
    } else {
        if (size <= BUFFER_SIZE) {
            env->GetByteArrayRegion(bytes, 0, size, static_buf);
            result = ByteArrayContainer(reinterpret_cast<char const *>(static_buf)
                , pfs::numeric_cast<typename ByteArrayContainer::size_type>(size));
        } else {
            jboolean isCopy;
            jbyte * buf = env->GetByteArrayElements(bytes, nullptr);
            result = ByteArrayContainer(reinterpret_cast<char const *>(buf)
                , reinterpret_cast<char const *>(buf) + size);
            env->ReleaseByteArrayElements(bytes, buf, JNI_ABORT);
        }
    }

    return result;
}

} // namespace pfs

#endif
