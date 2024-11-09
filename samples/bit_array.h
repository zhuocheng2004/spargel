#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * bit_array - an array of boolean values
 *
 * Only finitely many bits are non-zero.
 */
class bit_array {
 private:
  /*
   * We store the bit array as a contiguous sequence of segements
   * for manipulation efficiency.
   */
  typedef uint64_t segment_t;
  const int segment_length = 8;        // sizeof(segment_t)
  const int segment_length_shift = 3;  // 8 = 2^3
  const segment_t segment_mask = segment_length - 1;

  // number of segments allocated
  int size;

  // the array of segments
  segment_t* data;

 public:
  bit_array() : bit_array(0U) {}

  // copy constructor
  bit_array(const bit_array& that) {
    size = that.size;
    data = (segment_t*)malloc(size * segment_length);
    memcpy(data, that.data, size * segment_length);
  }

  bit_array(bit_array&& that) = default;

  bit_array(const uint64_t n) {
    size = 1;
    data = (segment_t*)malloc(1 * segment_length);
    data[0] = n;
  }

  ~bit_array() { free(data); }

  int get_size() const { return size * segment_length; }

  bit_array& operator=(const bit_array& that) {
    size = that.size;
    data = (segment_t*)realloc(data, size * segment_length);
    memcpy(data, that.data, size * segment_length);

    return *this;
  }

  bit_array& operator=(const uint64_t n) {
    size = 1;
    data = (segment_t*)realloc(data, 1 * segment_length);
    data[0] = n;

    return *this;
  }

  // reset the bit array to zero
  void clear() { *this = 0; }

  bool operator[](int index) const {
    int pos = index >> segment_length_shift;
    if (pos >= size) return false;
    int index2 = index & segment_mask;
    return (data[pos] & ((segment_t)1U << index2)) != 0;
  }

  void set_bit(int index) {
    int pos = index >> segment_length_shift;
    if (pos >= size) {
      int old_size = size, new_size = size;
      while (2 * pos >= new_size) new_size *= 2;
      size = new_size;
      data = (segment_t*)realloc(data, new_size * segment_length);
      memset(&data[old_size], 0, (new_size - old_size) * segment_length);
    }
    int index2 = index & segment_mask;
    data[pos] |= ((segment_t)1U << index2);
  }

  void clear_bit(int index) {
    int pos = index >> segment_length_shift;
    if (pos >= size) return;
    int index2 = index & segment_mask;
    data[pos] &= ~((segment_t)1U << index2);
  }

  bool operator==(const bit_array& that) const {
    if (this == &that) return true;

    int common_size = size < that.size ? size : that.size;

    int pos = 0;

    // common
    for (; pos < common_size; pos++) {
      if (data[pos] != that.data[pos]) return false;
    }

    if (size < that.size) {
      // "that" is longer
      for (; pos < that.size; pos++) {
        if (that.data[pos] != 0) return false;
      }
    } else {
      // "this" is longer or of equal size
      for (; pos < size; pos++) {
        if (data[pos] != 0) return false;
      }
    }

    return true;
  }

  // an internal order
  bool operator>(const bit_array& that) const {
    if (this == &that) return false;

    int common_size = size < that.size ? size : that.size;

    int pos = 0;

    // common
    for (; pos < common_size; pos++) {
      if (data[pos] <= that.data[pos]) return false;
    }

    if (size < that.size) {
      // When "that" is longer, we require the additional segments are all
      // zeros.
      for (; pos < that.size; pos++) {
        if (that.data[pos] != 0) return false;
      }
    }

    return true;
  }

  // an internal order
  bool operator<(const bit_array& that) const {
    if (this == &that) return false;

    int common_size = size < that.size ? size : that.size;

    int pos = 0;

    // common
    for (; pos < common_size; pos++) {
      if (data[pos] >= that.data[pos]) return false;
    }

    if (size < that.size) {
      // When "this" is longer, we require the additional segments are all
      // zeros.
      for (; pos < that.size; pos++) {
        if (data[pos] != 0) return false;
      }
    }

    return true;
  }

  bool operator==(const uint64_t n) const {
    if (data[0] != n) return false;

    for (int pos = 1; pos < size; pos++) {
      if (data[pos] != 0) return false;
    }

    return true;
  }

  /*
   * We say x lies over y, if for any index >= 0, y[index] is set => x[index] is
   * set.
   */
  bool lies_over(const bit_array& that) const {
    int common_size = size < that.size ? size : that.size;

    int pos = 0;

    // common
    for (; pos < common_size; pos++) {
      if ((data[pos] & that.data[pos]) != that.data[pos]) return false;
    }

    if (size < that.size) {
      // When "that" is longer, we require the additional segments are all
      // zeros.
      for (; pos < that.size; pos++) {
        if (that.data[pos] != 0) return false;
      }
    }

    return true;
  }

  /*
   * We say x lies below y, if for any index >= 0, x[index] is set => y[index]
   * is set.
   */
  bool lies_below(const bit_array& that) const {
    int common_size = size < that.size ? size : that.size;

    int pos = 0;

    // common
    for (; pos < common_size; pos++) {
      if ((data[pos] & that.data[pos]) != data[pos]) return false;
    }

    if (size > that.size) {
      // When "this" is longer, we require the additional segments are all
      // zeros.
      for (; pos < size; pos++) {
        if (data[pos] != 0) return false;
      }
    }

    return true;
  }
};

#endif
