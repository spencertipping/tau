#include <iostream>

using namespace std;

int (*f)(int) = [](int x) { return x + 1; };
int (*g)(int) = [](int x) { return x + 1; };

int main()
{
  cout << (void*)f << endl;
  cout << (void*)g << endl;
  return 0;
}
