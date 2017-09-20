#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

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

  float get() {
    return *((float*)data);
  }
};

void test_float() {
  float f = Float(0, 124, 2097152).get();
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

  printf("f: %f\n", f.get());
  printf("f: %f\n", g.get());
  
  return 0;
}
