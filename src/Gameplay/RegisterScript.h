#pragma once
#include "ScriptFactory.h"
#include <memory>
#include <string>
#include <iostream>

#define REGISTER_SCRIPT(CLASS_NAME) \
    namespace { \
        struct CLASS_NAME##Registrator { \
            CLASS_NAME##Registrator() { \
                ScriptFactory::instance().registerScript(#CLASS_NAME, []() -> std::unique_ptr<Script> { \
                    return std::make_unique<CLASS_NAME>(); \
                }); \
            } \
        }; \
        static CLASS_NAME##Registrator global_##CLASS_NAME##Registrator; \
    }
