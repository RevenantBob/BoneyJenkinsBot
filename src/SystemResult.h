#pragma once

#include <string>
#include "Result.h"
#include "SystemError.h"

template <typename T>
using SystemResult = Result<T, SystemError>;

template <typename T>
SystemResult<T> SystemSuccess(const T value)
{
    return SystemResult<T>(value, {});
}

template <typename T>
inline SystemResult<T> SystemSuccessMove(T& value)
{
    return SystemResult<T>(std::move(value), {});
}


template <typename T>
inline SystemResult<T> SystemFailure(const SystemError& value)
{
    return SystemResult<T>({}, value);
}

template <typename T>
inline SystemResult<T> SystemFailure(const std::string& context, const std::string& message, uint32_t code)
{
    return SystemResult<T>({}, SystemError(context, message, code));
}

template <typename T>
inline SystemResult<T> SystemFailure(const std::string& context, const std::string& message)
{
    return SystemResult<T>({}, SystemError(context, message));
}

template <typename T>
inline SystemResult<T> SystemFailure(const std::string& context, uint32_t code)
{
    return SystemResult<T>({}, SystemError(context, code));
}

