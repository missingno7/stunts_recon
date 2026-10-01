#include "port_runtime.h"

static int16_t vector_intersection_component(int16_t first, int16_t second,
                                             uint16_t numerator,
                                             uint16_t denominator)
{
    int32_t product = (int32_t)(int16_t)(uint16_t)(first - second) *
                      (int32_t)(int16_t)numerator;
    int16_t signed_denominator = (int16_t)denominator;
    int32_t quotient;
    uint16_t result;

    if (signed_denominator == 0)
        port_guest_unwind("vector_op_unk divide by zero");
    quotient = product / signed_denominator;
    if (quotient < INT16_MIN || quotient > INT16_MAX)
        port_guest_unwind("vector_op_unk signed divide overflow");
    result = (uint16_t)(uint16_t)second + (uint16_t)(int16_t)quotient;
    return (int16_t)result;
}

/* Word-for-word semantic translation of vector_op_unk (load_23014),
   whose 85-byte extent and instruction anchors are verified by context.py.
   Keep the 16-bit SUB/SHR/IMUL/IDIV widths and write order from the decoded
   segment-012 body; Restunts' math.c version is a semantic lead, not a strict
   source match. */
void vector_op_unk(int16_t *first, int16_t *second, int16_t *output,
                   int16_t target_depth)
{
    uint16_t numerator;
    uint16_t denominator;

    output[2] = target_depth;
    numerator = (uint16_t)((uint16_t)target_depth - (uint16_t)second[2]);
    denominator = (uint16_t)((uint16_t)first[2] - (uint16_t)second[2]);
    if ((int16_t)denominator < 0) {
        numerator >>= 1;
        denominator >>= 1;
    }

    output[0] = vector_intersection_component(first[0], second[0],
                                              numerator, denominator);
    output[1] = vector_intersection_component(first[1], second[1],
                                              numerator, denominator);
}

static int16_t matrix_product_word(int16_t coefficient, int16_t component)
{
    uint32_t product = (uint32_t)((int32_t)coefficient * component);
    return (int16_t)(product << 2 >> 16);
}

static void matrix_row(const int16_t *input, const int16_t *matrix,
                       int16_t *output)
{
    /* MATRIX is column-major: _11,_21,_31,_12,... . The assembly
       stores the first product before reading the next input component;
       retain that order when the input and output vectors alias. */
    *output = matrix_product_word(matrix[0], input[0]);
    if (matrix[3] != 0 && input[1] != 0)
        *output = (int16_t)((uint16_t)*output +
                           (uint16_t)matrix_product_word(matrix[3], input[1]));
    if (matrix[6] != 0 && input[2] != 0)
        *output = (int16_t)((uint16_t)*output +
                           (uint16_t)matrix_product_word(matrix[6], input[2]));
}

/* Semantic C translation of asm/mat_multiply.ASM::_mat_multiply. The
   row/column write order preserves the guest routine's input/output aliasing. */
void mat_multiply(const int16_t *right_matrix, const int16_t *left_matrix,
                  int16_t *output_matrix)
{
    unsigned row;
    unsigned column;

    for (row = 0; row < 3u; ++row) {
        for (column = 0; column < 3u; ++column) {
            unsigned right_index = row * 3u;
            unsigned destination = row * 3u + column;
            /* The original writes each partial product immediately. An
               output cell may overlap an operand of this or a later cell. */
            output_matrix[destination] = matrix_product_word(
                right_matrix[right_index], left_matrix[column]);
            if (right_matrix[right_index + 1u] != 0 &&
                left_matrix[3u + column] != 0)
                output_matrix[destination] = (int16_t)(
                    (uint16_t)output_matrix[destination] +
                    (uint16_t)matrix_product_word(
                        right_matrix[right_index + 1u], left_matrix[3u + column]));
            if (right_matrix[right_index + 2u] != 0 &&
                left_matrix[6u + column] != 0)
                output_matrix[destination] = (int16_t)(
                    (uint16_t)output_matrix[destination] +
                    (uint16_t)matrix_product_word(
                        right_matrix[right_index + 2u], left_matrix[6u + column]));
        }
    }
}

/* Semantic translation of the mat_invert lead in src/restunts/c/math.c.
   context.py reports only PARTIAL_UNMAPPED evidence for this routine, so this
   port implementation is not claimed as a strict historical match. */
void mat_invert(int16_t *input_matrix, int16_t *output_matrix)
{
    int16_t temp;

    if (input_matrix == output_matrix) {
        temp = output_matrix[3];
        output_matrix[3] = output_matrix[1];
        output_matrix[1] = temp;
        temp = output_matrix[6];
        output_matrix[6] = output_matrix[2];
        output_matrix[2] = temp;
        temp = output_matrix[7];
        output_matrix[7] = output_matrix[5];
        output_matrix[5] = temp;
    } else {
        output_matrix[0] = input_matrix[0];
        output_matrix[1] = input_matrix[3];
        output_matrix[2] = input_matrix[6];
        output_matrix[3] = input_matrix[1];
        output_matrix[4] = input_matrix[4];
        output_matrix[5] = input_matrix[7];
        output_matrix[6] = input_matrix[2];
        output_matrix[7] = input_matrix[5];
        output_matrix[8] = input_matrix[8];
    }
}

/* C translation of _mat_vec in asm/font_matrix_shape_decode.ASM, lines
   235-379. Rows are stored to output in order; this preserves the original
   behavior when input and output vectors alias. */
void mat_vec(const int16_t *input, const int16_t *matrix,
             int16_t *output)
{
    if (input == NULL || matrix == NULL || output == NULL)
        return;
    matrix_row(input, matrix, output);
    matrix_row(input, matrix + 1, output + 1);
    matrix_row(input, matrix + 2, output + 2);
}
