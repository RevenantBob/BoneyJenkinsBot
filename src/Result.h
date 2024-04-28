#pragma once
#include <functional>
#include <optional>
#include "Format.h"

struct Unit
{
    bool operator <(const Unit& other)
    {
        return false;
    }
    bool operator >(const Unit& other)
    {
        return false;
    }
    bool operator <=(const Unit& other)
    {
        return true;
    }
    bool operator >=(const Unit& other)
    {
        return true;
    }
    bool operator ==(const Unit& other)
    {
        return true;
    }

    bool operator !=(const Unit& other)
    {
        return false;
    }
};

template <typename T, typename E>
class Result
{
public:
    Result()
    {
    }

    // If the result type and error type are the same, how do we tell the constructor is doing error or success?
    // So provide null for the value not set.
    Result(const std::optional<T>& result, const std::optional<E>& error = {})
    {
        if (result.has_value())
        {
            if (error.has_value())
            {
                throw std::exception("both result and error set");
            }

            m_bResult = true;
            m_Result = result;
        }
        else
        {
            if (error.has_value())
            {
                m_bResult = false;
                m_Error = error;
            }

            // if nothing set, treat as default constructor
        }
    }
    Result(std::optional<T>&& result, std::optional<E>&& error = {})
    {
        if (result.has_value())
        {
            if (error.has_value())
            {
                throw std::exception("both result and error set");
            }

            m_bResult = true;
            m_Result = std::move(result);
        }
        else
        {
            if (error.has_value())
            {
                m_bResult = false;
                m_Error = std::move(error);
            }

            // if nothing set, treat as default constructor
        }
    }

    Result(const Result<T, E>& other)
        : m_bResult(other.m_bResult)
        , m_Result(other.m_Result)
        , m_Error(other.m_Error)
    {
    }

    Result(Result<T, E>&& other) noexcept
    {
        m_bResult = other.m_bResult;

        if (m_bResult.has_value())
        {
            if (other.m_Error.has_value())
            {
                m_Error = std::move(other.m_Error);
            }
            else
            {
                m_Error.reset();
            }

            if (other.m_Result.has_value())
            {
                m_Result = std::move(other.m_Result);
            }
            else
            {
                m_Result.reset();
            }
        }
        else
        {
            m_Error.reset();
            m_Result.reset();
        }
    }
public:
    operator bool() const
    {
        if (!m_bResult.has_value())
        {
            throw new std::exception("attempt to access a null result");
        }

        return m_bResult.value();
    }

    Result<T, E>& operator =(const Result<T, E>& result)
    {
        m_bResult = result.m_bResult;
        m_Result = result.m_Result;
        m_Error = result.m_Error;

        return *this;
    }

    void Iter(std::function<void(T)> result, std::function<void(E)> error)
    {
        if (m_bResult.value())
        {
            result(m_Result.value);
        }
        else
        {
            error(m_Error.value);
        }
    }

    bool IsError() const {
        return !m_bResult.value();
    }

    bool IsResult() const {
        return m_bResult.value();
    }

    const T& Value() const {
        return m_Result.value();
    }

    const E& Error() const {
        return m_Error.value();
    }

    // *Ref operations are so move constuctors can be called
    T& ValueRef() {
        return m_Result.value();
    }

    E& ErrorRef() {
        return m_Error.value();
    }

    static Result<T, E> NewResult(const T value)
    {
        Result<T, E> result;

        result.m_bResult = true;
        result.m_Result = value;

        return result;
    }

    static Result<T, E> NewResultRef(const T& value)
    {
        Result<T, E> result;

        result.m_bResult = true;
        result.m_Result = value;

        return result;
    }

    static Result<T, E> NewResultMove(T& value)
    {
        Result<T, E> result;

        result.m_bResult = true;
        result.m_Result = std::move(value);

        return result;
    }
    static Result<T, E> NewError(const E value)
    {
        Result<T, E> result;

        result.m_bResult = false;
        result.m_Error = value;

        return result;
    }

    static Result<T, E> NewErrorRef(const E& value)
    {
        Result<T, E> result;

        result.m_bResult = false;
        result.m_Error = value;

        return result;
    }
    static Result<T, E> NewErrorMove(E& value)
    {
        Result<T, E> result;

        result.m_bResult = false;
        result.m_Error = std::move(value);

        return result;
    }

    /// <summary>
    /// Bind a new result from a success.
    /// </summary>
    /// <param name="func">Function called to convert result to a new result.</param>
    /// <returns>New result value which replaces previous success.</returns>
    template <typename R>
    Result<R, E> Bind(std::function<Result<R, E>(const T& success)> func)
    {
        if (!m_bResult.has_value())
        {
            throw std::exception("result not set");
        }


        if (m_bResult.value() == true)
        {
            return func(m_Result.value());
        }
        else
        {
            return Result<R, E>({}, m_Error);
        }
    }

    /// <summary>
    /// Map success value to a new success value.
    /// </summary>
    /// <typeparam name="R"></typeparam>
    /// <param name="func"></param>
    /// <returns></returns>
    template <typename R>
    Result<R, E> Map(std::function<R(const T& success)> func)
    {
        if (!m_bResult.has_value())
        {
            throw std::exception("result not set");
        }

        std::optional<R> result;

        if (m_bResult.value() == true)
        {
            result = func(m_Result.value());
        }

        return Result<R, E>(result, m_Error);
    }

    /// <summary>
    /// Map error value to a new error value.
    /// </summary>
    /// <typeparam name="R"></typeparam>
    /// <param name="func"></param>
    /// <returns></returns>
    template <typename R>
    Result<T, R> MapError(std::function<R(const E& error)> func)
    {
        if (!m_bResult.has_value())
        {
            throw std::exception("result not set");
        }

        std::optional<R> result;

        if (m_bResult.value() == false)
        {
            result = func(m_Error.value());
        }

        return Result<T, R>(m_Result, result);
    }

    T Default(std::function<T(const E& error)> func)
    {
        if (!m_bResult.has_value())
        {
            throw std::exception("result not set");
        }

        if (m_bResult.value() == false)
        {
            return func(m_Error.value());
        }
        else
        {
            return m_Result.value();
        }
    }
    T DefaultValue(T value)
    {
        if (!m_bResult.has_value())
        {
            throw std::exception("result not set");
        }

        if (m_bResult.value() == false)
        {
            return value;
        }
        else
        {
            return m_Result.value();
        }
    }
protected:
    std::optional<bool> m_bResult;
    std::optional<T> m_Result;
    std::optional<E> m_Error;
};
