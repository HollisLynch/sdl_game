#ifndef GAME_VECTOR2_H
#define GAME_VECTOR2_H

class Vector2 {
public:
    float x, y;

    float lerpf(float previous, float current, float delta) {
        return (1 - delta) * previous + delta * current;
    }

    Vector2(float x = 0, float y = 0) {
        this->x = x;
        this->y = y;
    }

    Vector2(const Vector2& other) {
        this->x = other.x;
        this->y = other.y;
    }

    Vector2 operator+(Vector2 b) {
        return Vector2(this->x + b.x, this->y + b.y);
    }

    Vector2 operator-(Vector2 b) {
        return Vector2(this->x - b.x, this->y - b.y);
    }

    Vector2 operator*(float b) {
        return Vector2(this->x * b, this->y * b);
    }

    float operator*(Vector2 b) {
        return (this->x * b.x) + (this->y * b.y);
    }

    Vector2 operator-() {
        return Vector2(-this->x, -this->y);
    }

    void operator+=(Vector2 b) {
        this->x = this->x + b.x;
        this->y = this->y + b.y;
    }

    Vector2 lerp(Vector2 other, float delta) {
        return Vector2(lerpf(other.x, this->x, delta), lerpf(other.y, this->y, delta));
    }

    Vector2 normalize() {
        float magSq = *this * *this;
        if (magSq < 0.000001) {
            return Vector2(1, 0);
        }

        float mag = sqrt(magSq);
        return Vector2(this->x / mag, this->y / mag);
    }
};

#endif
