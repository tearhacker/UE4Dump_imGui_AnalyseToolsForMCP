#include <math.h>

#ifndef GAMEHELPER_VECTORSTRUCT_H
#define GAMEHELPER_VECTORSTRUCT_H

struct My_Vector2 {
    float x;
    float y;

    inline My_Vector2() : x(0), y(0) {};

    inline My_Vector2(const float x, const float y) : x(x), y(y) {};


    inline My_Vector2 operator+(const My_Vector2 &other) const {
        return My_Vector2(x + other.x, y + other.y);
    }

    inline My_Vector2 operator+(const float other) const {
        return My_Vector2(x + other, y + other);
    }

    inline My_Vector2 operator-(const My_Vector2 &other) const {
        return My_Vector2(x - other.x, y - other.y);
    }

    inline My_Vector2 operator-(const float other) const {
        return My_Vector2(x - other, y - other);
    }

    inline My_Vector2 operator*(const My_Vector2 &other) const {
        return My_Vector2(x * other.x, y * other.y);
    }

    inline My_Vector2 operator*(const float value) const {
        return My_Vector2(x * value, y * value);
    }

    inline My_Vector2 operator/(const My_Vector2 &other) const {
        if (other.x != 0 && other.y != 0) {
            return My_Vector2(x / other.x, y / other.y);
        }
        return My_Vector2();
    }

    inline My_Vector2 operator/(const float value) const {
        if (value != 0) {
            return My_Vector2(x / value, y / value);
        }
        return My_Vector2();
    }

    inline My_Vector2 operator-() const {
        return My_Vector2(-x, -y);
    }

    inline My_Vector2 &operator+=(const My_Vector2 &other) {
        x += other.x;
        y += other.y;
        return *this;
    }


    inline My_Vector2 &operator-=(const My_Vector2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline My_Vector2 &operator+=(const float value) {
        x += value;
        y += value;
        return *this;
    }

    inline My_Vector2 &operator-=(const float value) {
        x -= value;
        y -= value;
        return *this;
    }

    inline My_Vector2 &operator*=(const float value) {
        x *= value;
        y *= value;
        return *this;
    }

    inline My_Vector2 &operator*=(const My_Vector2 &other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    inline My_Vector2 &operator/=(const float &value) {
        x /= value;
        y /= value;
        return *this;
    }

    inline My_Vector2 &operator=(const My_Vector2 &other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    inline bool operator==(const My_Vector2 &other) const {
        return x == other.x && y == other.y;
    }

    inline bool operator!=(const My_Vector2 &other) const {
        return x != other.x || y != other.y;
    }

    inline float operator[](int index) const {
        return (&x)[index];
    }

    inline float &operator[](int index) {
        return (&x)[index];
    }

    inline bool NotHaveZero() {
        return x != 0 && y != 0;
    }

    inline void zero() {
        x = y = 0;
    }

    inline float length() {
        return sqrt(x * x + y * y);
    }

};

struct My_Vector3 {
    float x;
    float y;
    float z;

    inline My_Vector3() : x(0), y(0), z(0) {}

    inline My_Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    inline  explicit My_Vector3(float value) : x(value), y(value), z(value) {}

    inline My_Vector3 operator+(const My_Vector3 &other) const {
        return My_Vector3(x + other.x, y + other.y, z + other.z);
    }

    inline My_Vector3 operator+(const float other) const {
        return My_Vector3(x + other, y + other, z + other);
    }

    inline My_Vector3 operator-(const My_Vector3 &other) const {
        return My_Vector3(x - other.x, y - other.y, z - other.z);
    }

    inline My_Vector3 operator-(const float other) const {
        return My_Vector3(x - other, y - other, z - other);
    }

    inline My_Vector3 operator*(const My_Vector3 &other) const {
        return My_Vector3(x * other.x, y * other.y, z * other.z);
    }

    inline My_Vector3 operator*(const float value) const {
        return My_Vector3(x * value, y * value, z * value);
    }

    inline My_Vector3 operator/(const float value) const {
        if (value != 0) {
            return My_Vector3(x / value, y / value, z / value);
        }
        return My_Vector3();
    }

    inline My_Vector3 operator-() const {
        return My_Vector3(-x, -y, -z);
    }

    inline My_Vector3 &operator+=(const My_Vector3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    inline My_Vector3 &operator-=(const My_Vector3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    inline My_Vector3 &operator+=(const float value) {
        x += value;
        y += value;
        z += value;
        return *this;
    }

    inline My_Vector3 &operator-=(const float value) {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }

    inline My_Vector3 &operator*=(const float value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    inline My_Vector3 &operator*=(const My_Vector3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    inline My_Vector3 &operator/=(const float &value) {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    inline My_Vector3 &operator=(const My_Vector3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    inline bool operator==(const My_Vector3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    inline bool operator!=(const My_Vector3 &other) const {
        return x != other.x || y != other.y || z != other.z;
    }

    inline float operator[](int index) const {
        return (&x)[index];
    }

    inline float &operator[](int index) {
        return (&x)[index];
    }

    inline void Zero() {
        x = y = z = 0;
    }

    inline float length() const {
        return sqrt(x * x + y * y + z * z);
    }

    inline bool isValid() const {
        return x != 0 && y != 0 && z != 0 && !isnan(x) && !isnan(y) && !isnan(z);
    }

    static inline float dot(const My_Vector3 &a, const My_Vector3 &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static inline bool inRange(const My_Vector3 &target, const My_Vector3 &min, const My_Vector3 &max) {
        return target.x > min.x && target.x < max.x && target.y > min.y && target.y < max.y &&
               target.z > min.z && target.z < max.z;
    }
};

struct My_Vector4 {
    float x;
    float y;
    float z;
    float w;

    inline My_Vector4() : x(0), y(0), z(0), w(0) {}

    inline My_Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    inline explicit My_Vector4(float value) : x(value), y(value), z(value), w(value) {}

    inline My_Vector4 operator+(const My_Vector4 &other) const {
        return My_Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline My_Vector4 operator+(const float other) const {
        return My_Vector4(x + other, y + other, z + other, w + other);
    }

    inline My_Vector4 operator-(const My_Vector4 &other) const {
        return My_Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline My_Vector4 operator-(const float other) const {
        return My_Vector4(x - other, y - other, z - other, w - other);
    }

    inline My_Vector4 operator*(const float value) const {
        return My_Vector4(x * value, y * value, z * value, w * value);
    }

    inline My_Vector4 operator*(const My_Vector4 &other) const {
        return My_Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    inline My_Vector4 operator/(const float value) const {
        if (value != 0) {
            return My_Vector4(x / value, y / value, z / value, w / value);
        }
        return My_Vector4();
    }


    inline My_Vector4 operator-() const {
        return My_Vector4(-x, -y, -z, -w);
    }

    inline My_Vector4 &operator+=(const My_Vector4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline My_Vector4 &operator-=(const My_Vector4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline My_Vector4 &operator+=(const float value) {
        x += value;
        y += value;
        z += value;
        w += value;
        return *this;
    }

    inline My_Vector4 &operator-=(const float value) {
        x -= value;
        y -= value;
        z -= value;
        w -= value;
        return *this;
    }

    inline My_Vector4 &operator*=(const float value) {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }

    inline My_Vector4 &operator*=(const My_Vector4 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    inline My_Vector4 &operator/=(const float value) {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }

    inline My_Vector4 &operator=(const My_Vector4 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    inline bool operator==(const My_Vector4 &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    inline bool operator!=(const My_Vector4 &other) const {
        return x != other.x || y != other.y || z != other.z || w != other.w;
    }

    inline float operator[](int index) const {
        return (&x)[index];
    }

    inline float &operator[](int index) {
        return (&x)[index];
    }

    inline void Zero() {
        x = y = z = w = 0;
    }

    inline bool NotHaveZero() {
        return x != 0 && y != 0 && z != 0 && w != 0;
    }

    inline float length() {
        return sqrt(x * x + y * y + z * z + w * w);
    }

};

inline My_Vector4 vec4_mult(const My_Vector4 &vec, const My_Vector4 &vec2) {
    return vec * vec2;
}

inline My_Vector4 vec4_piu(const My_Vector4 &vec, const My_Vector4 &vec2) {
    return vec + vec2;
}

inline My_Vector4 vec4_meno(const My_Vector4 &vec, const My_Vector4 &vec2) {
    return vec - vec2;
}

inline float q2djl(const My_Vector2 &vec, const My_Vector2 &vec2) {
    return (vec - vec2).length();
}

inline float q3djl(const My_Vector3 &vec, const My_Vector3 &vec2) {
    return (vec - vec2).length();
}

inline bool isInRange(const My_Vector2 &target, const My_Vector2 &min, const My_Vector2 &max) {
    return target.x > min.x && target.x < max.x && target.y > min.y && target.y < max.y;
}

inline bool isInRange(const My_Vector3 &target, const My_Vector3 &min, const My_Vector3 &max) {
    return target.x > min.x && target.x < max.x && target.y > min.y && target.y < max.y && target.z > min.z &&
           target.z < max.z;
}

struct _Int2 {
    int x;
    int y;
};

struct _Int3 {
    int x;
    int y;
    int z;
};


#endif