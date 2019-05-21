#include <iostream>
#include <list>
#include <set>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

template <typename C>
struct container_wrapper {
  C const& container;
};

template <typename C>
container_wrapper<C> wrap_container(C const& container)
{
  return { container };
}

template <
  typename C,
  typename = std::void_t<
    decltype(std::declval<std::ostream>() << *std::cbegin(std::declval<C>()))>
>
std::ostream& operator<<(std::ostream& s, container_wrapper<C> const& wrapper)
{
  for (auto const& x : wrapper.container)
    s << x << " ";

  return s;
}

template <typename ContainerT, typename FactoryT, typename = std::void_t<> >
struct make_container_helper {};

template <typename ContainerT, typename FactoryT>
struct make_container_helper<
  ContainerT,
  FactoryT,
  std::void_t<
    decltype(std::declval<ContainerT>() << std::declval<FactoryT>()()),
    decltype(std::declval<ContainerT>() << " ")
  >
>
{
    static ContainerT make_container(int const n, FactoryT item_factory)
    {
        ContainerT result;
        for (auto i = 0; i != n; ++i)
        {
            result << item_factory() << " ";
        }

        return result;
    }
};

template <typename ContainerT, typename FactoryT>
struct make_container_helper<
  ContainerT,
  FactoryT,
  std::void_t<decltype(std::declval<ContainerT>().insert(std::declval<FactoryT>()()))> >
{
    static ContainerT make_container(int const n, FactoryT item_factory)
    {
        ContainerT result;
        for (auto i = 0; i != n; ++i)
        {
            result.insert(item_factory());
        }

        return result;
    }
};

template <typename ContainerT, typename FactoryT>
struct make_container_helper<
  ContainerT,
  FactoryT,
  std::void_t<decltype(std::declval<ContainerT>().push_back(std::declval<FactoryT>()()))> >
{
    static ContainerT make_container(int const n, FactoryT item_factory)
    {
        ContainerT result;
        for (auto i = 0; i != n; ++i)
        {
            result.push_back(item_factory());
        }

        return result;
    }
};

template <typename ContainerT, typename FactoryT>
ContainerT make_container(int const n, FactoryT item_factory)
{
    return make_container_helper<ContainerT, FactoryT>::make_container(n, item_factory);
}



template <typename ContainerT>
void test()
{
    auto v = make_container<ContainerT>(9, []() { return 9; });

    for (auto const& x : v)
    {
        std::cout << x << std::endl;
    }
}

int main()
{
  int count = 0;
  auto const int_ascending_generator = [&count]() { return count++; };

  std::cout
    // fill using push_back()
    << wrap_container(make_container<std::vector<int> >(
        9, int_ascending_generator))
    << std::endl
    // fill using push_back()
    << wrap_container(make_container<std::list<int> >(
        9, int_ascending_generator))
    << std::endl
    // fill using insert()
    << wrap_container(make_container<std::set<int> >(
        9, int_ascending_generator))
    << std::endl
    // fill using operator<<
    << make_container<std::ostringstream>(
        9, int_ascending_generator).str()
    << std::endl
    ;

    return 0;
}
