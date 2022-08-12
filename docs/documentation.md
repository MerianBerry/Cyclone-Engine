# Lunar Documentation

This file will include the entirety of the documentation for Lunar because I don't want to make a website for this.

### *Contents*

1. [Types](#types)
2. [Non-Engine specific methods](#non-engine-specific-methods)
3. [Instance](#lunar-instance)
4. [Scenes](#scenes)
5. [Scene scripts](#scene-scripts)


## Types
1. [Lunar basic types](#lunar-basic-types)
2. [Vulkan types](#vulkan-types)

### Lunar basic types
```c++

//The typical ouput of all complex Lunar methods, the output of the method is stored in result.
//You can debug errors using the message & error_code outputs.
//get the result of the method by using .result on the method call
template<typename T>
struct Lresult
{
    string message;
    T result;
    Uint32 error_code = LUNAR_ERROR_SUCCESS;
};

//the classic lambdas. T is its return type, and _Types is the parameter types
template<class T>
using Lambda_vec = std::vector<std::function<T()>>;
template< typename T, typename ..._Types >
using Lambda = std::function< T(_Types ...) >;
//EXAMPLE
lunar::Lambda< int, float, float > example([=]( float x, float y )
{
    return float * float;
});

//this type holds type T as its main type, but is defined with a deleter Lambda. the lambda gets called when its deleter is called, or when callFunc() is called
//note: the lamdas return type is always void
template< typename T >
struct Dtype
{
    T x;
    Dtype( T _t, Lambda< void > _func )
    {
        x = _t;
        deleter = _func;
    }
    const void callFunc()
    {
        (deleter)();
    }
    ~Dtype()
    {
        (deleter)();
    }
    private:
    Lambda< void > deleter;
};
//EXAMPLE
lunar::Dtype< int > example( 5, lunar::Lambda< void >([=]
{
    printf("hello? NOOOO\n"); 
}));

//a shortening for std::chrono::steady_clock::time_point
typedef std::chrono::steady_clock::time_point SteadyTimePoint;

//a structure used by the time related lunar methods
struct times
{
    float milliseconds;
    float seconds;
    float minutes;
    float hours;
};

//a structure that contains time data that can be used by the stopwatch methods to keep track of time
struct StopWatch
{
    SteadyTimePoint start_time;
    SteadyTimePoint pause_time;
};

```

### Vulkan types

```c++


```

## Non-Engine specific methods

## Instance

## Scenes

## Scene scripts
