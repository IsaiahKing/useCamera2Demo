//
// Created by menethis on 18-9-19.
//

#ifndef FACERECOGNITIONSDKPROJECT_JLXDEFINE_HPP
#define FACERECOGNITIONSDKPROJECT_JLXDEFINE_HPP

#include <array>
template <class T>
void safeRelease(T& buffer)
{
    if (buffer != NULL)
    {
        delete [] buffer;
        buffer = nullptr;
    }
}

namespace JLX{
    namespace MATH{
        union FloatRand {
            struct
            {
                unsigned long Frac:23;
                unsigned long Exp:8;
                unsigned long Signed:1;
            } BitArea;
            float Value;
        };

        float GetFloatRand();

        float custom_sqrt(float x);
    };
};

#endif //FACERECOGNITIONSDKPROJECT_JLXDEFINE_HPP
