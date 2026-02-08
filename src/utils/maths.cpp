int pow(int i, int exponent) {
  int out = 1;
  while (exponent) {
    exponent--;
    out *= i;
  }
  return out;
}
