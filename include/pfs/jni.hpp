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

#include "error.hpp"
#include "i18n.hpp"
#include "numeric_cast.hpp"
#include <jni.h>
#include <array>
#include <cstdarg>
#include <cstdint>
#include <string>

namespace pfs {

// [Java VM Type Signatures](https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html#type_signatures)
//
// Type Signature            | Java Type
//-----------------------------------------
// Z                         | boolean
// B                         | byte
// C                         | char
// S                         | short
// I                         | int
// J                         | long
// F                         | float
// D                         | double
// L fully-qualified-class ; | fully-qualified-class
// [ type                    | type[]
// ( arg-types )             | ret-type method type
//
//
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPERIMENTAL
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace jni {

inline void check_argument (bool arg, char const * arg_name)
{
    if (arg == false) {
        throw pfs::error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::f_("bad {}", arg_name)
        };
    }
}

template <typename T>
inline void check_argument (T * arg, char const * arg_name)
{
    if (arg == nullptr) {
        throw pfs::error {
              std::make_error_code(std::errc::invalid_argument)
            , tr::f_("bad {}", arg_name)
        };
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// env
////////////////////////////////////////////////////////////////////////////////////////////////////
class env
{
public:
    static constexpr jint JNI_VERSION = JNI_VERSION_1_6;

private:
    JavaVM * _jvm {nullptr};
    JNIEnv * _env {nullptr};

public:
    env () = default;

    env (JavaVM * jvm)
        : _jvm(jvm)
    {
        check_argument(jvm, "Java VM");

        void * penv = nullptr;

        auto rc = _jvm->GetEnv(& penv, JNI_VERSION);

        auto jni_env = static_cast<JNIEnv *>(penv);

        if (rc == JNI_OK) {
            // Attached
        } else {
            if (rc == JNI_EDETACHED) {
                rc = _jvm->AttachCurrentThread(& jni_env, nullptr);

                if (rc != JNI_OK) {
                    throw pfs::error {
                          errc::jni_error
                        , tr::f_("JNI AttachCurrentThread() failure: result code={}", rc)
                    };
                }
            } else {
                throw pfs::error {
                    errc::jni_error
                    , tr::f_("JNI GetEnv() failure: result code={}", rc)
                };
            }
        }

        _env = jni_env;
    }

    ~env ()
    {}

public:
    operator bool () const noexcept
    {
        return _env != nullptr;
    }

    env attach ()
    {
        return env {_jvm};
    }

    JNIEnv * native () const noexcept
    {
        return _env;
    }

    bool handle_exception ()
    {
        jthrowable ex = _env->ExceptionOccurred();

        if (ex != nullptr) {
            _env->ExceptionDescribe();
            _env->ExceptionClear();
            return true;
        }

        return false;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// peer_class
////////////////////////////////////////////////////////////////////////////////////////////////////
class peer_class
{
    jclass _class {nullptr};
    std::string _class_name;

public:
    peer_class () = default;

    peer_class (env & env_ref, char const * class_name)
        : _class_name(class_name)
    {
        check_argument(env_ref, "Java environment");
        check_argument(class_name, "class name");

        _class = env_ref.native()->FindClass(_class_name.c_str());

        if (_class == nullptr) {
            env_ref.handle_exception();

            throw pfs::error {
                  errc::jni_error
                , tr::f_("Java class not found: {}", _class_name)
            };
        }

        _class = static_cast<jclass>(env_ref.native()->NewGlobalRef(_class));
    }

    void destroy (env & env_ref)
    {
        check_argument(env_ref, "Java environment");

        if (_class != nullptr) {
            env_ref.native()->DeleteGlobalRef(_class);
            _class = nullptr;
        }
    }

public:
    operator bool () const noexcept
    {
        return _class != nullptr;
    }

    jclass native () const noexcept
    {
        return _class;
    }

    std::string const & class_name () const noexcept
    {
        return _class_name;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// peer_object
////////////////////////////////////////////////////////////////////////////////////////////////////
class peer_object
{
    jobject _obj {nullptr};

public:
    peer_object () = default;

    peer_object (env & env_ref, jobject o)
    {
        check_argument(env_ref, "Java environment");

        if (o == nullptr) {
            throw pfs::error {
                  std::make_error_code(std::errc::invalid_argument)
                , tr::_("bad Java object")
            };
        }

        _obj = env_ref.native()->NewGlobalRef(o);

        if (_obj == nullptr) {
            env_ref.handle_exception();

            throw pfs::error {
                 errc::jni_error
               , tr::_("NewGlobalRef() failure")
            };
        }
    }

    void destroy (env & env_ref)
    {
        check_argument(env_ref, "Java environment");

        if (_obj != nullptr) {
            env_ref.native()->DeleteGlobalRef(_obj);
            _obj = nullptr;
        }
    }

public:
    operator bool () const noexcept
    {
        return _obj != nullptr;
    }

    jobject native () const noexcept
    {
        return _obj;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// peer_static_method
////////////////////////////////////////////////////////////////////////////////////////////////////
class peer_static_method
{
    env * _penv {nullptr};
    peer_class * _peer_class {nullptr};

public:
    peer_static_method (env & env_ref, peer_class & class_ref)
        : _penv(& env_ref)
        , _peer_class(& class_ref)
    {
        check_argument(env_ref, "Java environment");
        check_argument(class_ref, "peer class");
    }

private:
    jmethodID method_id (char const * method_name, char const * method_signature)
    {
        jmethodID result = _penv->native()->GetStaticMethodID(_peer_class->native(), method_name, method_signature);

        if (result == nullptr) {
            _penv->handle_exception();

            throw pfs::error {
                  errc::jni_error
                , tr::f_("Java static method not found for class {}: {} ", method_name, _peer_class->class_name())
            };
        }

        return result;
    }

public:
    void invoke_v (char const * method_name, char const * method_signature, va_list args)
    {
        jmethodID method = method_id(method_name, method_signature);
        _penv->native()->CallStaticVoidMethodV(_peer_class->native(), method, args);
    }

    void invoke (char const * method_name, char const * method_signature, ...)
    {
        va_list args;
        va_start(args, method_signature);
        invoke_v(method_name, method_signature, args);
        va_end(args);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// peer_method
////////////////////////////////////////////////////////////////////////////////////////////////////
class peer_method
{
    env * _penv {nullptr};
    peer_class * _peer_class {nullptr};
    peer_object * _peer_object {nullptr};

public:
    peer_method (env & env_ref, peer_class & class_ref, peer_object & obj_ref)
        : _penv(& env_ref)
        , _peer_class(& class_ref)
        , _peer_object(& obj_ref)
    {
        check_argument(env_ref, "Java environment");
        check_argument(class_ref, "peer class");
        check_argument(obj_ref, "peer object");
    }

private:
    jmethodID method_id (char const * method_name, char const * method_signature)
    {
        jmethodID result = _penv->native()->GetMethodID(_peer_class->native(), method_name, method_signature);

        if (result == nullptr) {
            _penv->handle_exception();

            throw pfs::error {
                  errc::jni_error
                , tr::f_("Java method not found for class {}: {}", method_name, _peer_class->class_name())
            };
        }

        return result;
    }

public:
    void invoke_v (char const * method_name, char const * method_signature, va_list args)
    {
        jmethodID method = method_id(method_name, method_signature);
        _penv->native()->CallVoidMethodV(_peer_object->native(), method, args);
    }

    void invoke (char const * method_name, char const * method_signature, ...)
    {
        va_list args;
        va_start(args, method_signature);
        invoke_v(method_name, method_signature, args);
        va_end(args);
    }
};

} // namespace jni

} // namespace pfs

#endif
