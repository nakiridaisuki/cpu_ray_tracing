#pragma once

#include <glm/glm.hpp>

struct Complex{
    union {
        float a;
        float real;
    };
    union {
        float b;
        float image;
    };
    
    Complex(float a, float b): a(a), b(b) {}
    Complex(float a): a(a), b(0) {}
};


inline Complex operator+(Complex lhs, Complex rhs) {
    return {lhs.a + rhs.a, lhs.b + rhs.b};
}

inline Complex operator-(Complex lhs, Complex rhs) {
    return {lhs.a - rhs.a, lhs.b - rhs.b};
}

inline Complex operator*(Complex lhs, Complex rhs) {
    return {lhs.a * rhs.a - lhs.b * rhs.b, lhs.a * rhs.b + lhs.b * rhs.a};
}
inline Complex operator*(Complex lhs, float rhs) {
    return {lhs.a * rhs, lhs.b * rhs};
}

inline Complex operator/(Complex lhs, Complex rhs) {
    float inv = 1.f / (rhs.a*rhs.a + rhs.b*rhs.b);
    return {(lhs.a*rhs.a + lhs.b*rhs.b) * inv, (lhs.b*rhs.a - lhs.a*rhs.b) * inv};
}
inline Complex operator/(Complex lhs, float rhs) {
    return {lhs.a / rhs, lhs.b / rhs};
}

inline float norm(Complex rhs) {
    return { glm::sqrt(rhs.a*rhs.a + rhs.b*rhs.b) };
}

inline Complex sqrt(Complex rhs) {
    float n = norm(rhs);
    float r = glm::sqrt((n + rhs.a) * 0.5f);
    float i = glm::sqrt((n - rhs.a) * 0.5f);
    if(rhs.b < 0){
        i = -i;
    }
    return { r, i };
}