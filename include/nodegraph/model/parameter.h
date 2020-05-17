#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace NodeGraph
{

// Data that flows between nodes, of generic type
class IFlowData;
class IControlData;

enum class ParameterType
{
    None,
    Float,
    Double,
    Int64,
    Bool,
    String,
    FlowData,
    ControlData
};

struct ParameterValue
{
    union
    {
        float fVal;
        double dVal;
        int64_t iVal;
        bool bVal;
        IFlowData* pFVal;
        IControlData* pCVal;
    };
    std::string sVal;
    ParameterType type = ParameterType::None;

    ~ParameterValue()
    {
    }

    ParameterValue()
    {
    }

    ParameterValue(const ParameterValue& val)
    {
        type = val.type;
        switch (val.type)
        {
        case ParameterType::Float:
            fVal = val.fVal;
            break;
        case ParameterType::Double:
            dVal = val.dVal;
            break;
        case ParameterType::Int64:
            iVal = val.iVal;
            break;
        case ParameterType::Bool:
            bVal = val.bVal;
            break;
        case ParameterType::String:
            sVal = val.sVal;
            break;
        case ParameterType::FlowData:
            pFVal = val.pFVal;
            break;
        case ParameterType::ControlData:
            pCVal = val.pCVal;
            break;
        default:
            break;
        }
    }

    bool operator==(const ParameterValue& val)
    {
        if (type != val.type)
            return false;

        switch (val.type)
        {
        case ParameterType::Float:
            return fVal == val.fVal;
            break;
        case ParameterType::Double:
            return dVal == val.dVal;
            break;
        case ParameterType::Int64:
            return iVal == val.iVal;
            break;
        case ParameterType::Bool:
            return bVal == val.bVal;
            break;
        case ParameterType::String:
            return sVal == val.sVal;
            break;
        case ParameterType::FlowData:
            return pFVal == val.pFVal;
            break;
        case ParameterType::ControlData:
            return pCVal == val.pCVal;
            break;
        default:
            break;
        }
        return false;
    }

    bool operator==(const float& val)
    {
        if (type != ParameterType::Float)
            return false;
        return fVal == val;
    }
    bool operator==(const double& val)
    {
        if (type != ParameterType::Double)
            return false;
        return dVal == val;
    }
    bool operator==(const int64_t& val)
    {
        if (type != ParameterType::Int64)
            return false;
        return iVal == val;
    }
    bool operator==(const bool& val)
    {
        if (type != ParameterType::Bool)
            return false;
        return bVal == val;
    }
    bool operator==(const std::string& val)
    {
        if (type != ParameterType::String)
            return false;
        return sVal == val;
    }
    bool operator==(const IFlowData* const& val)
    {
        if (type != ParameterType::FlowData)
            return false;
        return pFVal == val;
    }
    bool operator==(const IControlData* const& val)
    {
        if (type != ParameterType::ControlData)
            return false;
        return pCVal == val;
    }

    bool operator!=(const float& val)
    {
        return !operator==(val);
    }
    bool operator!=(const double& val)
    {
        return !operator==(val);
    }
    bool operator!=(const int64_t& val)
    {
        return !operator==(val);
    }
    bool operator!=(const bool& val)
    {
        return !operator==(val);
    }
    bool operator!=(const std::string& val)
    {
        return !operator==(val);
    }
    bool operator!=(const IFlowData* const& val)
    {
        return !operator==(val);
    }
    bool operator!=(const IControlData* const& val)
    {
        return !operator==(val);
    }

    explicit ParameterValue(const float& f)
    {
        fVal = f;
        type = ParameterType::Float;
    }

    explicit ParameterValue(const double& f)
    {
        dVal = f;
        type = ParameterType::Double;
    }
    explicit ParameterValue(const int64_t& i)
    {
        iVal = i;
        type = ParameterType::Int64;
    }
    explicit ParameterValue(const bool& b)
    {
        bVal = b;
        type = ParameterType::Bool;
    }
    explicit ParameterValue(const std::string& str)
    {
        sVal = str;
        type = ParameterType::String;
    }
    explicit ParameterValue(IFlowData* const& ptr)
    {
        pFVal = ptr;
        type = ParameterType::FlowData;
    }
    explicit ParameterValue(IControlData* const& ptr)
    {
        pCVal = ptr;
        type = ParameterType::ControlData;
    }

    float operator=(const float& f)
    {
        if (type == ParameterType::None)
        {
            type = ParameterType::Float;
        }
        if (type != ParameterType::Float)
        {
            throw std::invalid_argument("Not a float!");
        }
        fVal = f;
        return fVal;
    }
    double operator=(const double& f)
    {
        if (type == ParameterType::None)
        {
            type = ParameterType::Double;
        }
        if (type != ParameterType::Double)
        {
            throw std::invalid_argument("Not a double!");
        }
        dVal = f;
        return dVal;
    }
    int64_t operator=(const int64_t& v)
    {
        if (type == ParameterType::None)
        {
            type = ParameterType::Int64;
        }
        if (type != ParameterType::Int64)
        {
            throw std::invalid_argument("Not an int64!");
        }
        iVal = v;
        return iVal;
    }
    bool operator=(const bool& v)
    {
        if (type == ParameterType::None)
        {
            type = ParameterType::Bool;
        }
        if (type != ParameterType::Bool)
        {
            throw std::invalid_argument("Not a bool!");
        }
        bVal = v;
        return bVal;
    }
    std::string& operator=(const std::string& v)
    {
        if (type == ParameterType::None)
        {
            type = ParameterType::String;
        }
        if (type != ParameterType::String)
        {
            throw std::invalid_argument("Not a string!");
        }
        sVal = v;
        return sVal;
    }

    IFlowData* operator=(IFlowData* const& v)
    {
        if (type == ParameterType::FlowData)
        {
            type = ParameterType::FlowData;
        }
        if (type != ParameterType::FlowData)
        {
            throw std::invalid_argument("Not flow data!");
        }
        pFVal = v;
        return pFVal;
    }

    IControlData* operator=(IControlData* const& v)
    {
        if (type == ParameterType::ControlData)
        {
            type = ParameterType::ControlData;
        }
        if (type != ParameterType::ControlData)
        {
            throw std::invalid_argument("Not control data!");
        }
        pCVal = v;
        return pCVal;
    }

    explicit operator float() const
    {
        if (type != ParameterType::Float)
        {
            throw std::invalid_argument("Not a float!");
        }
        return fVal;
    }
    explicit operator double() const
    {
        if (type != ParameterType::Double)
        {
            throw std::invalid_argument("Not a double!");
        }
        return dVal;
    }
    explicit operator int64_t() const
    {
        if (type != ParameterType::Double)
        {
            throw std::invalid_argument("Not an int!");
        }
        return iVal;
    }
    explicit operator std::string() const
    {
        if (type != ParameterType::String)
        {
            throw std::invalid_argument("Not a string!");
        }
        return sVal;
    }
    explicit operator IFlowData*() const
    {
        if (type != ParameterType::FlowData)
        {
            throw std::invalid_argument("Not flow data!");
        }
        return pFVal;
    }
    explicit operator IControlData*() const
    {
        if (type != ParameterType::ControlData)
        {
            throw std::invalid_argument("Not flow data!");
        }
        return pCVal;
    }

    template <class T>
    T To() const
    {
        if (type == ParameterType::Double)
        {
            return (T)dVal;
        }
        else if (type == ParameterType::Float)
        {
            return (T)fVal;
        }
        else if (type == ParameterType::Int64)
        {
            return (T)iVal;
        }
        else if (type == ParameterType::Bool)
        {
            return (T)bVal;
        }
        else
        {
            assert(!"no supported?");
        }
        T v{};
        return v;
    };

    template <class T>
    void SetFrom(const T& value)
    {
        if (type == ParameterType::Double)
        {
            value = (T)dVal;
        }
        else if (type == ParameterType::Float)
        {
            value = (T)fVal;
        }
        else if (type == ParameterType::Int64)
        {
            value = (T)iVal;
        }
        else if (type == ParameterType::Bool)
        {
            value = (T)bVal;
        }
        else if (type == ParameterType::String)
        {
            value = (T)sVal;
        }
        else if (type == ParameterType::FlowData)
        {
            value = (T)pFVal;
        }
        else if (type == ParameterType::ControlData)
        {
            value = (T)pCVal;
        }
        else
        {
            assert(!"no supported?");
        }
    }
};

enum class ParameterUI
{
    None,
    Slider,
    Knob,
    Button,
    Text,
    Custom
};

enum class ParameterDisplayType
{
    None,
    Value,
    Percentage,
    Custom
};

namespace ParameterFlags
{
enum
{
    None = (0),
    ReadOnly = (1 << 0)
};
}

struct ParameterAttributes
{
    // Fixed types once set
    ParameterValue min;
    ParameterValue max;
    ParameterValue origin;
    ParameterValue step;
    ParameterValue thumb;

    ParameterUI ui = ParameterUI::None;
    ParameterDisplayType displayType = ParameterDisplayType::Custom;
    std::string postFix;
    uint32_t flags = ParameterFlags::None;
    std::vector<std::string> labels;
    float taper = 1.0f;

    ParameterAttributes()
    {
    }

    template <class T>
    ParameterAttributes(ParameterUI _ui, const T& _min = T(0), const T& _max = T(1))
    {
        ui = _ui;
        SetRanges(_min, _max);
    }

    template <class T>
    void SetRanges(const T& _min = T(0), const T& _max = T(1), const T& _origin = T(0), const T& _step = T(0))
    {
        min = _min;
        max = _max;
        origin = _origin;
        step = _step;
        thumb = _step;
    }

    void SetRanges(const int64_t& _min = 0, const int64_t& _max = 100, const int64_t& _origin = 0, const int64_t& _step = 1)
    {
        min = _min;
        max = _max;
        origin = _origin;
        step = _step;
        thumb = _step;
    }
};

// A parameter is a variant type that can also lerp
class Parameter
{
public:
    Parameter()
    {
    }

    explicit Parameter(float val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
        Set(val, true);
    }

    explicit Parameter(double val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
        Set(val, true);
    }

    explicit Parameter(int64_t val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
        Set(val, true);
    }

    explicit Parameter(bool val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
        Set(val, true);
        m_attributes.ui = ParameterUI::Button;
    }

    explicit Parameter(const std::string& val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
        Set(val, true);
        m_attributes.ui = ParameterUI::Text;
    }

    explicit Parameter(IFlowData* val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
    }

    explicit Parameter(IControlData* val, const ParameterAttributes& attrib = ParameterAttributes{})
        : m_value(val)
        , m_initValue(val)
        , m_attributes(attrib)
    {
    }

    void SetAttributes(const ParameterAttributes& attributes)
    {
        m_attributes = attributes;
    }

    ParameterAttributes& GetAttributes()
    {
        return m_attributes;
    }

    // Get a value that isn't a flow data
    template <class T>
    T To() const
    {
        if (m_value.type == ParameterType::FlowData)
        {
            throw std::invalid_argument("Can't request flow data with GetValue");
        }
        else if (m_value.type == ParameterType::ControlData)
        {
            throw std::invalid_argument("Can't request control data with GetValue");
        }
        return m_value.To<T>();
    }

    virtual IFlowData* GetFlowData() const
    {
        if (m_value.type != ParameterType::FlowData)
        {
            throw std::invalid_argument("Not flow data!");
        }
        return m_value.pFVal;
    }

    virtual IControlData* GetControlData() const
    {
        if (m_value.type != ParameterType::ControlData)
        {
            throw std::invalid_argument("Not control data!");
        }
        return m_value.pCVal;
    }

    // Update the current value of the parameter
    ParameterValue Update(uint64_t tick)
    {
        m_currentTick = tick;

        if (m_endValue == m_value)
        {
            return m_value;
        }

        m_generation++;

        if (m_value.type == ParameterType::String || m_value.type == ParameterType::FlowData || m_value.type == ParameterType::ControlData)
        {
            m_endValue = m_value;
            return m_value;
        }

        float frac = m_lerpTicks != 0 ? ((float)(tick - m_startTick) / m_lerpTicks) : 1.0f;
        frac = std::min(frac, 1.0f);
        frac = std::max(frac, 0.0f);
        if (frac <= 1.0f)
        {
            if (m_value.type == ParameterType::Float)
            {
                m_value = m_startValue.fVal + (m_endValue.fVal - m_startValue.fVal) * frac;
                if (std::abs(m_value.fVal - m_endValue.fVal) <= std::numeric_limits<float>::epsilon())
                {
                    m_value = m_endValue;
                }
            }
            else if (m_value.type == ParameterType::Double)
            {
                m_value = m_startValue.dVal + (m_endValue.dVal - m_startValue.dVal) * frac;
                if (std::abs(m_value.dVal - m_endValue.dVal) <= std::numeric_limits<float>::epsilon())
                {
                    m_value = m_endValue;
                }
            }
            else if (m_value.type == ParameterType::Int64)
            {
                m_value = int64_t(m_startValue.iVal + (m_endValue.iVal - m_startValue.iVal) * frac);
            }
            else
            {
                // Can't lerp here.  Might be fun to lerp string ;)
                m_value = m_endValue;
            }
        }

        return m_value;
    }

    ParameterType GetType() const
    {
        return m_value.type;
    }

    void SetLerpSamples(uint64_t lerpTicks)
    {
        m_lerpTicks = lerpTicks;
    }

    template <class T>
    inline T To()
    {
        return m_value.To<T>();
    }

    template <class T>
    void SetFrom(const T& value)
    {
        if (m_value.type == ParameterType::Double)
        {
            Set<double>(double(value));
        }
        else if (m_value.type == ParameterType::Float)
        {
            Set<float>((float)value);
        }
        else if (m_value.type == ParameterType::Int64)
        {
            Set<int64_t>((int64_t)value);
        }
        else if (m_value.type == ParameterType::Bool)
        {
            Set<bool>(value ? true : false);
        }
        else
        {
            assert(!"no supported?");
        }
    }

    template <class T>
    void Set(const T& val, bool immediate = false)
    {
        if (m_endValue == val)
        {
            // No need to update
            return;
        }

        m_generation++;

        // Always immediate
        if (m_value.type == ParameterType::FlowData || m_value.type == ParameterType::ControlData || m_value.type == ParameterType::String)
        {
            m_value = val;
            return;
        }

        m_endValue = val;
        if (immediate)
        {
            m_startValue = val;
            m_value = val;
            m_startTick = 0;
        }
        else
        {
            if (m_value.type == ParameterType::None)
            {
                m_value = val;
            }
            // m_value stays where it is
            m_startValue = m_value;
            m_startTick = m_currentTick;
            m_endValue = val;
        }
    }

    uint64_t GetGeneration() const
    {
        return m_generation;
    }

    const ParameterValue& GetInitValue() const
    {
        return m_initValue;
    }

    // Note can be greater than 1 if the value is out of bounds
    double Normalized()
    {
        auto min = m_attributes.min.To<double>();
        auto max = m_attributes.max.To<double>();

        if (m_attributes.taper == 1.0f)
        {
            return (To<double>() - min) / (max - min);
        }
        // Taper == 1 is linear
        return std::pow(((To<double>() - min) / (max - min)), (1.0 / m_attributes.taper));
    }

    double NormalizedStep() const
    {
        auto min = m_attributes.min.To<double>();
        auto max = m_attributes.max.To<double>();
        return std::abs(m_attributes.step.To<double>() / (max - min));
    }

    double NormalizedOrigin() const
    {
        auto min = m_attributes.min.To<double>();
        auto max = m_attributes.max.To<double>();

        if (m_attributes.taper == 1.0f)
        {
            return std::clamp((m_attributes.origin.To<double>() - min) / (max - min), 0.0, 1.0);
        }
        // Taper == 1 is linear
        return std::clamp(std::pow((((double)m_attributes.origin.To<double>() - min) / (max - min)), (1.0 / m_attributes.taper)), 0.0, 1.0);
    }

    void SetFromNormalized(double val)
    {
        auto min = m_attributes.min.To<double>();
        auto max = m_attributes.max.To<double>();

        val = std::clamp(val, 0.0, 1.0);
        if (m_attributes.taper == 1.0f)
        {
            SetFrom<double>(min + (max - min) * val);
        }

        // algebraic taper
        SetFrom<double>(min + (max - min) * std::pow(val, m_attributes.taper));
    }

protected:
    // We lerp between start/end and output value
    ParameterValue m_value;
    ParameterValue m_endValue;
    ParameterValue m_startValue;

    ParameterValue m_initValue;

    // Settings for how to display
    ParameterAttributes m_attributes;

    // How many ticks to lerp
    int64_t m_lerpTicks = 0;

    // Starting tick for a new lerp
    int64_t m_startTick = 0;

    // Where we are now
    int64_t m_currentTick = 0;

    uint64_t m_generation = 0;
};

} // namespace NodeGraph