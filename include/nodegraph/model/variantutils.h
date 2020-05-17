#pragma once

namespace MUtils
{

template<class T>
inline float var_is_real(const T& a)
{ 
    return std::holds_alternative<float>(a) || std::holds_alternative<double>(a); 
}

template<class T>
inline float var_is_string(const T& a)
{
    return std::holds_alternative<std::string>(a);
}

template<class T>
inline float var_is_integer(const T& a)
{
    return std::holds_alternative<std::int64_t>(a); 
}

template<class T>
inline double var_as_double(const T& a)
{
    if (std::holds_alternative<double>(a))
        return std::get<double>(a);
    else if (std::holds_alternative<float>(a))
        return (double)std::get<float>(a);
    else if (std::holds_alternative<int64_t>(a))
        return (double)std::get<int64_t>(a);
    assert(!"?");
    return 0.0;
}

template<class T>
inline float var_as_float(const T& a)
{
    if (std::holds_alternative<double>(a))
        return (float)std::get<double>(a);
    else if (std::holds_alternative<float>(a))
        return std::get<float>(a);
    else if (std::holds_alternative<int64_t>(a))
        return (float)std::get<int64_t>(a);
    assert(!"?");
    return 0.0f;
}

template<class T>
inline int64_t var_as_integer(const T& a)
{
    if (std::holds_alternative<double>(a))
        return (int64_t)std::get<double>(a);
    else if (std::holds_alternative<float>(a))
        return (int64_t)std::get<float>(a);
    else if (std::holds_alternative<int64_t>(a))
        return std::get<int64_t>(a);
    assert(!"?");
    return 0;
}

template<class T>
inline std::string var_as_string(const T& a)
{
    if (std::holds_alternative<std::string>(a))
        return std::get<std::string>(a);
    assert(!"?");
    return "";
}

} // MUtils
