#include <cstdio>
#include <cstdlib>
#include <cstring>

// https://gcc.gnu.org/onlinedocs/gcc-3.4.3/g77/Floating_002dpoint-Exception-Handling.html
#define _GNU_SOURCE 1
#include <fenv.h>

static void __attribute__ ((constructor)) trapfpe () {
  // Enable some exceptions.  At startup all exceptions are masked. 
  feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
}


class Float {  
private:
  char* data;

public:
  // https://ja.wikipedia.org/wiki/IEEE_754#32.E3.83.93.E3.83.83.E3.83.88.E5.8D.98.E7.B2.BE.E5.BA.A6
  Float(unsigned char sign, unsigned char exponent, unsigned int fraction) {
    data = (char*)malloc(4);
    memset(data, 0, 4);
    data[3] = (sign << 7) | (exponent >> 1);
    data[2] = ((exponent & 1) << 7) | ((fraction >> 16) & 0xff);
    data[1] = (fraction >> 8) & 0xff;
    data[0] = fraction & 0xff;
  }

  float to_float() {
    return *((float*)data);
  }
};

void test_float() {
  float f = Float(0, 124, 2097152).to_float();
  float answer = 0.15625;

  if (f != answer) {
    printf("BUG: f.get() returned %.10f, but should be %.10f\n", f, answer);
    exit(1);
  }
}

int main(){
  test_float();

  Float f = Float(0, 124, 2097152); // 0.15625
  Float g = Float(1, 124, 2097152); // -0.15625
  Float nan = Float(0, 255, 12345); // nan

  printf("f: %f\n", f.to_float());
  printf("g: %f\n", g.to_float());
  printf("nan: %f\n", nan.to_float()); // floating point exception

  return 0;
}
