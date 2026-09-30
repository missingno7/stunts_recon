#include "port_runtime.h"

/* Data words initialized alongside _set_projection in
   asm/projection_vector_window.ASM. These stay 16-bit because the game uses
   word-sized projection coordinates and fixed-point scales. */
int16_t projection_half_width = 160;
int16_t projection_half_height = 100;
int16_t projection_left_origin;
int16_t projection_top_origin;
int16_t projection_center_x = 160;
int16_t projection_center_y = 100;
int16_t projection_x_scale;
int16_t projection_y_scale;
int16_t projection_x_angle;
int16_t projection_y_angle;
int16_t projectiondata5 = 160;
int16_t projectiondata8 = 100;
int16_t projectiondata9;
int16_t projectiondata10;

static int16_t projection_add_center(uint16_t value, int16_t center)
{
    uint16_t center_word = (uint16_t)center;
    uint16_t sum = (uint16_t)(value + center_word);
    uint16_t same_sign = (uint16_t)~(value ^ center_word);
    uint16_t changed_sign = (uint16_t)(value ^ sum);
    if ((same_sign & changed_sign & 0x8000u) != 0)
        return (int16_t)((int16_t)sum < 0 ? 0x7d00 : 0x8300);
    return (int16_t)sum;
}

static int16_t projection_x_axis(int16_t coordinate, int16_t depth)
{
    uint16_t magnitude = (uint16_t)coordinate;
    uint32_t product;
    uint16_t low;
    uint16_t high;
    uint16_t threshold;
    uint16_t quotient;
    uint16_t scaled;

    if (coordinate < 0)
        magnitude = (uint16_t)(0u - magnitude);
    product = (uint32_t)magnitude * (uint16_t)projectiondata9;
    low = (uint16_t)product;
    high = (uint16_t)(product >> 16);
    threshold = (uint16_t)(high << 1);
    if ((int16_t)low < 0)
        ++threshold;
    if (depth <= (int16_t)threshold)
        return (int16_t)(coordinate < 0 ? 0x8300 : 0x7d00);

    quotient = (uint16_t)(product / (uint16_t)depth);
    scaled = coordinate < 0 ? (uint16_t)(0u - quotient) : quotient;
    return projection_add_center(scaled, projectiondata5);
}

static int16_t projection_y_axis(int16_t coordinate, int16_t depth)
{
    uint16_t magnitude = (uint16_t)coordinate;
    uint32_t product;
    uint16_t low;
    uint16_t high;
    uint16_t threshold;
    uint16_t quotient;
    uint16_t scaled;

    if (coordinate < 0)
        magnitude = (uint16_t)(0u - magnitude);
    product = (uint32_t)magnitude * (uint16_t)projectiondata10;
    low = (uint16_t)product;
    high = (uint16_t)(product >> 16);
    threshold = (uint16_t)(high << 1);
    if ((int16_t)low < 0)
        ++threshold;
    if (depth <= (int16_t)threshold)
        return (int16_t)((int16_t)low < 0 ? 0x7d00 : 0x8300);

    quotient = (uint16_t)(product / (uint16_t)depth);
    scaled = coordinate < 0 ? quotient : (uint16_t)(0u - quotient);
    return projection_add_center(scaled, projectiondata8);
}

extern int16_t polang(int16_t y, int16_t scale);

static uint16_t angle_from_degrees_word(uint16_t degrees)
{
    uint32_t dividend = (uint32_t)degrees << 11;
    uint32_t quotient = dividend / 360u;
    if (quotient > 0xFFFFu)
        port_guest_unwind("set_projection 16-bit angle division overflow");
    return (uint16_t)(quotient >> 1);
}

static uint16_t projection_scale(uint16_t angle, uint16_t half_extent)
{
    uint16_t cosine = (uint16_t)cosfast(angle);
    uint16_t sine = (uint16_t)sinfast(angle);
    uint32_t dividend = (uint32_t)cosine * half_extent;
    if (sine == 0)
        port_guest_unwind("set_projection divided by zero sine");
    return (uint16_t)(dividend / sine);
}

/* Semantic C translation of asm/projection_vector_window.ASM:_set_projection.
   The current setup_intro caller passes (40, 40, 320, 200). */
void set_projection(int16_t x_angle, int16_t y_angle,
                    int16_t width, int16_t height)
{
    uint16_t x_word = (uint16_t)x_angle;
    uint16_t y_word = (uint16_t)y_angle;
    uint16_t width_word = (uint16_t)width;
    uint16_t height_word = (uint16_t)height;
    uint16_t eighth;
    uint16_t scale;

    projection_x_angle = (int16_t)angle_from_degrees_word(x_word);
    projection_y_angle = (int16_t)angle_from_degrees_word(y_word);
    projection_half_width = (int16_t)(width_word >> 1);
    projection_center_x = (int16_t)((uint16_t)projection_left_origin +
                                    (uint16_t)projection_half_width);
    projection_half_height = (int16_t)(height_word >> 1);
    projection_center_y = (int16_t)((uint16_t)projection_top_origin +
                                    (uint16_t)projection_half_height);
    projection_x_scale = (int16_t)projection_scale(
        (uint16_t)projection_x_angle, (uint16_t)projection_half_width);
    projectiondata5 = projection_center_x;
    projectiondata9 = projection_x_scale;

    if (projection_y_angle != 0) {
        projection_y_scale = (int16_t)projection_scale(
            (uint16_t)projection_y_angle, (uint16_t)projection_half_height);
        projectiondata8 = projection_center_y;
        projectiondata10 = projection_y_scale;
        return;
    }

    scale = (uint16_t)projection_x_scale;
    eighth = (uint16_t)(scale >> 3);
    projection_y_scale = (int16_t)(uint16_t)(scale - eighth - (eighth >> 1));
    projectiondata8 = projection_center_y;
    projectiondata10 = projection_y_scale;
    projection_y_angle = polang(projection_half_height, projection_y_scale);
}

/* Semantic C translation of vector_to_point (load_223d9), whose context.py
   assembly has verified instruction boundaries but no strict acceptance recipe. */
void vector_to_point(const int16_t *vector, int16_t *point)
{
    int16_t depth = vector[2];
    if (depth <= 0) {
        point[0] = (int16_t)0x8000u;
        point[1] = (int16_t)0x8000u;
        return;
    }
    point[0] = projection_x_axis(vector[0], depth);
    point[1] = projection_y_axis(vector[1], depth);
}
