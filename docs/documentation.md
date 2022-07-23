# Lunar Documentation

This file will include the entirety of the documentation for Lunar because I don't want to make a website for this.

### *Contents*

1. [Types](#types)
2. [Non-Engine specific methods](#non-engine-specific-methods)
3. [Instance](#lunar-instance)
4. [Scenes](#scenes)
5. [Scene scripts](#scene-scripts)


## Types

```c++

//The typical ouput of all complex Lunar methods, the output of the method is stored in result. You can also debug errors using the message & error_code outputs
template<typename T>
struct Lresult
{
    string message;
    T result;
    Uint32 error_code = LUNAR_ERROR_SUCCESS;
};

```

## Non-Engine specific methods

## Instance

## Scenes

## Scene scripts
