#pragma once
#include <string>

namespace TypeSafeInvocation_BadAlternative_MethodProxy
{
    typedef void InstanceType;
    typedef void (*InvokeMethodFunc)(InstanceType* base, InstanceType** const args);

    template <typename T>
    struct TFunctionTraits;

    template <typename TRet, typename... TArgs>
    struct TFunctionTraits<TRet(*)(TArgs...)>
    {
        using ReturnType = TRet;
        static constexpr size_t Arity = sizeof...(TArgs);
        using ArgsTuple = std::tuple<TArgs...>;
    };

    template <typename TRet, typename TClass, typename... TArgs>
    struct TFunctionTraits<TRet(TClass::*)(TArgs...)>
    {
        using ClassType = TClass;
        using ReturnType = TRet;
        static constexpr size_t Arity = sizeof...(TArgs);
        using ArgsTuple = std::tuple<TArgs...>;
    };

    template <typename TExpected, typename TActual, size_t TIdx>
    struct TTypeCheckHelper;

    template <typename TExpected, typename TActual>
    struct TTypeCheckHelper<TExpected, TActual, 0>
    {
        static constexpr bool value =
            std::is_convertible<
            typename std::tuple_element<0, TActual>::type,
            typename std::tuple_element<0, TExpected>::type
            >::value;
    };

    template <typename TExpected, typename TActual, size_t TIdx>
    struct TTypeCheckHelper
    {
        static constexpr bool value =
            std::is_convertible<
            typename std::tuple_element<TIdx, TActual>::type,
            typename std::tuple_element<TIdx, TExpected>::type
            >::value&& TTypeCheckHelper<TExpected, TActual, TIdx - 1>::value;
    };

    template <typename TMethodPtr>
    class TMethodProxy
    {
    private:
        using Traits = TFunctionTraits<TMethodPtr>;
        using ClassType = typename Traits::ClassType;
        using ReturnType = typename Traits::ReturnType;
        using ExpectedTuple = typename Traits::ArgsTuple;

    public:
        TMethodProxy(InvokeMethodFunc ptr)
            : m_methodPtr(ptr)
        {
        }
        template <typename... TActualArgs>
        static constexpr bool CheckArgTypes()
        {
            using ActualTuple = std::tuple<TActualArgs...>;

            if (sizeof...(TActualArgs) != Traits::Arity)
                return false;
            return CheckTypesNonRecursive<ExpectedTuple, ActualTuple>();
        }
        template <typename TE, typename TA>
        static constexpr bool CheckTypesNonRecursive()
        {
            return TTypeCheckHelper<TE, TA, std::tuple_size<TE>::value - 1>::value;
        }
        template <typename... TArgs>
        ReturnType Invoke(void* base, TArgs&&... args)
        {
            static_assert(CheckArgTypes<TArgs...>(),
                "Argument types do not match method signature");

            std::array<Niflect::InstanceType*, sizeof ...(TArgs)> argArray = { (&args)... };
            m_methodPtr(base, argArray.data());
        }

    private:
        InvokeMethodFunc m_methodPtr;
    };

    class CMyClass
    {
    public:
        void MyMethod0(int a, int b)
        {
            printf("%d, %d\n", a, b);
        }
        void MyMethod1(int a, std::string b)
        {
            printf("%d, %s\n", a, b.c_str());
        }
        void MyMethod2(int a, std::string& b)
        {
            printf("%d, %s\n", a, b.c_str());
        }
        void MyMethod3(int a, const std::string& b)
        {
            printf("%d, %s\n", a, b.c_str());
        }
    };

    static void MyWrapper0(InstanceType* base, InstanceType** const args)
    {
        auto& arg0 = *static_cast<int*>(args[0]);
        auto& arg1 = *static_cast<int*>(args[1]);
        static_cast<CMyClass*>(base)->MyMethod0(arg0, arg1);
    }
    static void MyWrapper1(InstanceType* base, InstanceType** const args)
    {
        auto& arg0 = *static_cast<int*>(args[0]);
        auto& arg1 = *static_cast<std::string*>(args[1]);
        static_cast<CMyClass*>(base)->MyMethod1(arg0, arg1);
    }
    static void MyWrapper2(InstanceType* base, InstanceType** const args)
    {
        auto& arg0 = *static_cast<int*>(args[0]);
        auto& arg1 = *static_cast<std::string*>(args[1]);
        static_cast<CMyClass*>(base)->MyMethod2(arg0, arg1);
    }
    static void MyWrapper3(InstanceType* base, InstanceType** const args)
    {
        auto& arg0 = *static_cast<int*>(args[0]);
        auto& arg1 = *static_cast<std::string*>(args[1]);
        static_cast<CMyClass*>(base)->MyMethod3(arg0, arg1);
    }

    static void DebugInvocations()
    {
        CMyClass obj;
        TMethodProxy<decltype(&CMyClass::MyMethod0)> method0(&MyWrapper0);
        method0.Invoke(&obj, 0, 1);

        TMethodProxy<decltype(&CMyClass::MyMethod1)> method1(&MyWrapper1);
        method1.Invoke(&obj, 1, std::string("nihao1"));

        TMethodProxy<decltype(&CMyClass::MyMethod2)> method2(&MyWrapper2);
        std::string func2_arg1("nihao2");
        method2.Invoke(&obj, 2, func2_arg1);

        TMethodProxy<decltype(&CMyClass::MyMethod3)> method3(&MyWrapper3);
        method3.Invoke(&obj, 3, std::string("nihao3"));

        //begin, “初始化”: 无法从“const char (*)[9]”转换为“_Ty”
        //TMethodProxy<decltype(&CMyClass::MyMethod3)> method3_1(&MyWrapper3);
        //method3_1.Invoke(&obj, 3, "nihao3_1");
        //end
    }
}