#ifndef N
#define N 10000
#endif

double a[N], b[N], c[N], d[N];

// Do NOT modify code above

/*
 * For each question function, if its for-loop is free 
 * of data-dependency issue, then add OpenMP directives 
 * to parallelize it; else skip that function.
 */
//yes
void question1() {
  #pragma omp parallel for
  for (int i=0; i<N; i++)
    a[i] = i * 1.0;
}
//yes
void question2() {
  #pragma omp parallel for
  for (int i=1; i<N; i++)
    b[i] = a[i-1] - a[i] + i*1.0;
}
//yes
void question3() {
  #pragma omp parallel for
  for (int i=0; i<N; i++)
    c[i] = (c[i]+1.0) * b[i];
}
//no
void question4() {
  for (int i=1; i<N; i++)
    d[i] = d[i-1] + c[i-1];
}
//yes
void question5() {
  #pragma omp parallel for
  for (int i=1; i<N; i++)
    a[i] = b[i] + (b[i-1]++);
}
//yes
void question6() {
  #pragma omp parallel for
  for (int i=1; i<N; i++) {
    a[i] = b[i];
    b[i] = c[i];
    c[i] = d[i];
    d[i] = a[i];
  }
}
//no?
void question7() {
  for (int i=1; i<N-1; i++) {
    a[i] = b[i-1];
    b[i+1] = c[i];
    c[i] = d[i+1];
    d[i-1] = a[i];
  }
}

// Do NOT modify code below

void init() {
#pragma omp parallel for shared(a, b, c, d) num_threads(40)
  for (int i=0; i<N; i++)
    a[i] =  b[i] = c[i] = d[i] = 0.0;
}

int main () {
  init();
  question1();
  question2();
  question3();
  question4();
  question5();
  question6();
  question7();
}
