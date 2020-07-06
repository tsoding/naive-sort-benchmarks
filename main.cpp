#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <stack>

const size_t MAX_VALUE   = 1'000'000;
const size_t XS_CAPACITY = 1'000'000'000;

template <typename T>
void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

template <typename T>
void bubble_sort(T *xs, size_t xs_size)
{
    for (size_t size = xs_size; size >= 2; --size) {
        for (size_t i = 0; i < size - 1; ++i) {
            if (xs[i] > xs[i + 1]) {
                swap(xs[i], xs[i + 1]);
            }
        }
    }
}

template <typename T>
size_t make_first_pivot(T *xs, size_t xs_size)
{
    size_t pivot = 0;
    for (size_t i = 1; i < xs_size; ++i) {
        if (xs[i] < xs[pivot]) {
            swap(xs[i], xs[pivot + 1]);
            swap(xs[pivot], xs[pivot + 1]);
            pivot += 1;
        }
    }
    return pivot;
}

template <typename T>
size_t make_random_pivot(T *xs, size_t xs_size)
{
    size_t pivot = rand() % xs_size;

    for (size_t i = 0; i < xs_size; ++i) {
        if (i < pivot) {
            if (xs[i] >= xs[pivot]) {
                swap(xs[i], xs[pivot - 1]);
                swap(xs[pivot], xs[pivot - 1]);
                pivot -= 1;
                i--;
            }
        } else if (i > pivot) {
            if (xs[i] < xs[pivot]) {
                swap(xs[i], xs[pivot + 1]);
                swap(xs[pivot], xs[pivot + 1]);
                pivot += 1;
            }
        }
    }

    return pivot;
}

template <typename T>
void quick_sort(T *xs, size_t xs_size)
{
    if (xs_size > 1) {
        size_t pivot = make_random_pivot(xs, xs_size);
        quick_sort(xs, pivot);
        quick_sort(xs + pivot + 1, xs_size - pivot - 1);
    }
}

const size_t STACK_INIT_CAPACITY = 256;

template <typename T>
struct Stack
{
    T *items;
    size_t capacity;
    size_t count;

    void push(T x)
    {
        if (items == NULL) {
            items = new T[STACK_INIT_CAPACITY];
            capacity = STACK_INIT_CAPACITY;
        }

        if (count >= capacity) {
            size_t new_capacity = capacity + (capacity >> 2);
            T *new_items = new T[new_capacity];
            for (size_t i = 0; i < capacity; ++i) {
                new_items[i] = items[i];
            }
            delete[] items;
            items = new_items;
            capacity = new_capacity;
        }

        items[count++] = x;
    }

    T pop()
    {
        assert(count > 0);
        return items[--count];
    }
};

template <typename T>
struct Slice
{
    T *base;
    size_t size;
};

constexpr int bit_width(int n)
{
    int res = 0;
    while (n > 0) {
        n /= 2;
        res += 1;
    }
    return res;
}

template <typename T>
void non_recursive_quick_sort(T *xs, const size_t xs_size)
{
    Stack<Slice<T>> stack = {};
    stack.push(Slice<T> {xs, xs_size});
    while (stack.count > 0) {
        auto slice = stack.pop();
        if (slice.size > 1) {
            size_t pivot = make_random_pivot(slice.base, slice.size);
            stack.push(Slice<T> {slice.base, pivot});
            stack.push(Slice<T> {slice.base + pivot + 1, slice.size - pivot - 1});
        }
    }
    delete[] stack.items;
}

template <typename T>
void adaptive_quick_sort(T *xs, size_t xs_size)
{
    if (xs_size > 15) {
        size_t pivot = make_random_pivot(xs, xs_size);
        adaptive_quick_sort(xs, pivot);
        adaptive_quick_sort(xs + pivot + 1, xs_size - pivot - 1);
    } else {
        bubble_sort(xs, xs_size);
    }
}

template <typename T>
void std_sort(T *xs, size_t xs_size)
{
    std::sort(xs, xs + xs_size);
}

template <typename T>
bool is_sorted(T *xs, size_t xs_size)
{
    if (xs_size >= 2) {
        for (size_t i = 0; i < xs_size - 1; ++i) {
            if (xs[i] > xs[i + 1]) {
                return false;
            }
        }
    }

    return true;
}

void generate_random_ints(int *xs, size_t xs_size, int max_value)
{
    for (size_t i = 0; i < xs_size; ++i) {
        xs[i] = rand() % max_value;
    }
}

void generate_sorted_ints(int *xs, size_t xs_size, int max_value)
{
    size_t step = (xs_size + max_value - 1) / max_value;
    for (size_t i = 0; i < xs_size; ++i) {
        xs[i] = (int) (i / step);
    }
}

void generate_reversed_ints(int *xs, size_t xs_size, int max_value)
{
    size_t step = (xs_size + max_value - 1) / max_value;
    for (size_t i = 0; i < xs_size; ++i) {
        xs[xs_size - i  - 1] = (int) (i / step);
    }
}

void print_array(int *xs, size_t xs_size)
{
    for (size_t i = 0; i < xs_size; ++i) {
        printf("%d ", xs[i]);
    }
    putc('\n', stdout);
}

void build_hist(int *xs, size_t xs_size,
                int *hist, size_t hist_size)
{
    memset(hist, 0, sizeof(*hist) * hist_size);
    for (size_t i = 0; i < xs_size; ++i) {
        assert((size_t) xs[i] < hist_size);
        hist[xs[i]] += 1;
    }
}

int xs[XS_CAPACITY];
int hist1[MAX_VALUE];
int hist2[MAX_VALUE];

struct Sort
{
    void (*run)(int *xs, size_t xs_size);
    const char *name;
};

struct Generator
{
    void (*run)(int *xs, size_t xs_size, int max_value);
    const char *name;
};

void test_bench(Sort sort, Generator generator, size_t n)
{
    size_t xs_size = 1;
    for (size_t i = 0; i < n; ++i, xs_size *= 10) {
        assert(xs_size <= XS_CAPACITY);
        printf("%s for %s of size %ld ... ", sort.name, generator.name, xs_size);
        fflush(stdout);

        generator.run(xs, xs_size, MAX_VALUE);
        build_hist(xs, xs_size, hist1, MAX_VALUE);
        clock_t begin = clock();
        sort.run(xs, xs_size);
        clock_t end = clock();
        build_hist(xs, xs_size, hist2, MAX_VALUE);

        if (!is_sorted(xs, xs_size)) {
            printf("IT IS NOT SORTED REEEEEEEEE!!!111\n");
            exit(1);
        }

        if (memcmp(hist1, hist2, MAX_VALUE) != 0) {
            printf("THE DATA IS CORRUPTED REEEEEE!!111\n");
            exit(1);
        }

        printf("%.6lf seconds\n", (double) (end - begin) / (double) CLOCKS_PER_SEC);
    }
}

Sort sorts[] = {
    {std_sort<int>, "std_sort"},
    {adaptive_quick_sort<int>, "adaptive_quick_sort"},
    {quick_sort<int>, "quick_sort"},
    {non_recursive_quick_sort<int>, "non_recursive_quick_sort"},
    {bubble_sort<int>, "bubble_sort"},
};
const size_t sorts_count = sizeof(sorts) / sizeof(sorts[0]);

Generator generators[] = {
    {generate_random_ints, "random_ints"},
    {generate_sorted_ints, "sorted_ints"},
    {generate_reversed_ints, "reversed_ints"},
};
const size_t generators_count = sizeof(generators) / sizeof(generators[0]);

int main()
{
    for (size_t i = 0; i < sorts_count; ++i) {
        for (size_t j = 0; j < generators_count; ++j) {
            test_bench(sorts[i], generators[j], 8);
        }
    }

    return 0;
}
