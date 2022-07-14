#include <iostream>
#include <queue>
#include <string>

#include <emscripten/fiber.h>


using namespace std;


struct fiber;
static fiber *this_fiber;
static fiber *scheduler;


bool const debug = false;


struct fiber
{
  string             name;
  emscripten_fiber_t ctx;
  char               async_stack[65536];
  char               c_stack[1048576];

  fiber(string name_) : name(name_)
    { emscripten_fiber_init_from_current_context(&ctx, async_stack, sizeof(async_stack));
      this_fiber = this; }

  fiber(string name_, em_arg_callback_func f) : name(name_)
    { emscripten_fiber_init(&ctx, f, nullptr, c_stack, sizeof(c_stack), async_stack, sizeof(async_stack)); }

  void swap(fiber &g)
    { if (debug) cout << "swapping from " << this_fiber->name << " to " << g.name << endl;
      this_fiber = &g;
      emscripten_fiber_swap(&ctx, &g.ctx); }

  static void switch_to(fiber &g) { this_fiber->swap(g); }
};


template <typename T>
struct stream
{
  deque<T> xs;
  int      capacity;
  bool     omega = false;

  stream(int c) : capacity(c) {}

  inline bool head_ready() const { return !omega && xs.size() < capacity; }
  inline bool tail_ready() const { return !omega && xs.size() > 0; }
  inline T&&  pop()
    {
      while (!tail_ready())
      {
        if (debug) cout << "stream.pop switching to scheduler" << endl;
        fiber::switch_to(*scheduler);
      }
      T&& x = std::move(xs[0]);
      xs.pop_front();
      return std::move(x);
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready())
      {
        if (debug) cout << "stream<< switching to scheduler" << endl;
        fiber::switch_to(*scheduler);
      }
      xs.push_back(x);
      return *this;
    }
};


static stream<int> s1(4);
static stream<int> s2(4);


int
main()
{
  fiber *f1 = new fiber("f1", [](void* _) -> void {
    cout << "f1 started" << endl;
    for (int i = 0;; ++i)
    {
      cout << "f1 emitting " << i << endl;
      s1 << i;
    }
    fiber::switch_to(*scheduler);
  });

  fiber *f2 = new fiber("f2", [](void* _) -> void {
    cout << "f2 started" << endl;
    for (int t = 0;;)
    {
      cout << "f2 requesting" << endl;
      t += s1.pop();
      cout << "f2 sending " << t << endl;
      s2 << t;
    }
    fiber::switch_to(*scheduler);
  });

  fiber *f3 = new fiber("f3", [](void* _) -> void {
    cout << "f3 started" << endl;
    for (int i = 0; i < 10; ++i) cout << s2.pop() << endl;
    s2.omega = true;
    fiber::switch_to(*scheduler);
  });

  scheduler = new fiber("main");
  cout << "scheduler loop starting" << endl;
  while (!s2.omega)
  {
    cout << "s1: " << s1.head_ready() << "/" << s1.tail_ready() << "/" << s1.omega << endl;
    cout << "s2: " << s2.head_ready() << "/" << s2.tail_ready() << "/" << s2.omega << endl;

    if (s1.head_ready())                    fiber::switch_to(*f1);
    if (s1.tail_ready() && s2.head_ready()) fiber::switch_to(*f2);
    if (s2.tail_ready())                    fiber::switch_to(*f3);
  }

  cout << "all done" << endl;

  return 0;
}
