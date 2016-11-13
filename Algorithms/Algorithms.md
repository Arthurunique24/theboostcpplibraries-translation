# Алгоритмы

Алгоритмы из Boost.Graph напоминают алгоритмы из стандартной библиотеки - они универсальные и очень гибкие. Тем не менее, не всегда сразу понятно, как ими пользоваться.

<a name="ex.graph_08"/>
Пример 31.8 Обход графа в ширину с `boost::breadth_first_search()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <iterator>
#include <algorithm>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g{edges.begin(), edges.end(), 4};

  boost::array<int, 4> distances{{0}};

  boost::breadth_first_search(g, topLeft,
    boost::visitor(
      boost::make_bfs_visitor(
        boost::record_distances(distances.begin(),
          boost::on_tree_edge{}))));

  std::copy(distances.begin(), distances.end(),
    std::ostream_iterator<int>{std::cout, "\n"});
}
```

[Пример 31.8](#ex.graph_08) использует алгоритм `boost::breadth_first_search()` для обхода графа в ширину. Алгоритм начинается в вершине, переданной в качестве второго параметра. Сперва посещаются все вершины, доступные прямо из переданной.

`boost::breadth_first_search()` не возвращает конкретного результата. Алгоритм просто обходит вершины. Собираются ли данные или хранятся где-то при обходе зависит от *посетителей*, переданных в `boost::breadth_first_search()`.

Посетители - это объекты, функции-члены которых вызываются при посещении вершины. Передавая посетителя в алгоритм типа `boost::breadth_first_search()`, вы сами решаете, что должно произойти при посещении вершины. Посетители - это словно функциональные объекты, которые мы передаем в алгоритмы стандартной библиотеки.

[Пример 31.8](#ex.graph_08) использует посетителя, который записывает расстояния. Расстояние в нашем понимании - это количество ребер, которое нужно пройти, чтобы попасть из одной вершины в другую, начиная с вершины, переданной в `boost::breadth_first_search()` в качестве второго аргумента. Boost.Graph обеспечивает вспомогательную функцию `boost::record_distances()` для создания посетителя. *Карта свойств* и *тег* также должны быть переданы.

Карты свойств хранят описания свойств вершин или ребер. Boost.Graph описывает концепцию карт свойств. Так как указатель или итератор берется как начало карты свойств, нам не важно знать деталей работы карт свойств. В [Примере 31.8](#ex.graph_08) указатель на начало массива **distances** передается в виде `distances.begin()` в `boost::record_distances()`. Этого вполне достаточно, чтобы использовать массив **distances** в качестве карты свойств. Тем не менее, важно, чтобы размер массива был не меньше количества вершин графа. В конце концов, расстояние до каждой вершины графа должно быть сохранено.

Пожалуйста, обратите внимание, что *distances* основан на `boost::array`, а не на `std::array`. Использование `std::array` приведет к ошибке компиляции.

В зависимости от алгоритма, существуют различные события. Второй передаваемый параметр для `boost::record_distances()` определяет о каких событиях посетитель должен быть уведомлен. Boost.Graph определяет теги, которые являются пустыми классами, чтобы дать событиям имена. Тег `boost::on_tree_edge` в [Примере 31.8](#ex.graph_08) указывает, что расстояние должно быть записано, когда была найден новая вершина.

События зависят от алгоритма. Нужно будет проверить алгоритмы в документации, чтобы выяснить, какие события поддерживаются и какие теги можно использовать.

Посетитель в примере создан независимым алгоритмом `boost::record_distances()`, так что вы можете использовать `boost::record_distances()` и с другими алгоритмами. Адаптер используется для связывания алгоритма и посетителя. [Пример 31.8](#ex.graph_08)) вызывает `boost::make_bfs_visitor()`, чтобы создать адаптер. Эта вспомогательная функция возвращает посетителя с помощью алгоритма `boost::breadth_first_search()`. Этот посетитель определяет функции-члены, которым соответствуют события, которые поддерживает алгоритм. Например, посетитель, возвращаемый `boost::make_bfs_visitor()` определяет функцию член `tree_edge()`. Если посетитель, который определен с тегом `boost::on_tree_edge`, передается в `boost::make_bfs_visitor()` (как сделано в [Примере 31.8](#ex.graph_08)), то посетитель получит уведомление, когда вызовется `tree_edge()`. Это позволит вам использовать посетителей с различными алгоритмами без необходимости определять все функции-члены, которые требуют алгоритмы.  

Адаптер, возвращаемый `boost::make_bfs_visitor()` не может быть передан непосредственно в `boost::breadth_first_search()`. Он должен быть обернут в `boost::visitor()`, а затем передан в качестве третьего аргумента.

Есть два варианта алгоритмов, как `boost::breadth_first_search()`. Один из вариантов предполагает, что каждый аргумент, который алгоритм поддерживает, будет передан. Другой вариант поддерживает что-то подобное только с объявленными аргументами. Как правило, проще использовать второй вариант, так как логично передать только те аргументы, в которых вы заинтересованы. Многие аргументы могут быть не переданы, так как алгоритм будет использовать значения по умолчанию.

[Пример 31.8](#ex.graph_08) использует второй вариант алгоритма `boost::breadth_first_search()`, то есть он будет ожидать только объявленные аргументы. Первые два параметра это граф и начальная вершина, которые обязательны. Третий параметр может быть почти всем. В [Примере 31.8](#ex.graph_08) должен быть передан посетитель. Чтобы это все работало, адаптер, возвращаемый `boost::make_bfs_visitor()` объявлен используя `boost::visitor()`. Теперь ясно, что третий агумент посетитель. В следующих примерах вы увидите, каким образом другие аргументы передаются по имени в `boost::breadth_first_search()`.

[Пример 31.8](#ex.graph_08) выводит числа 0, 1, 2 и 1. Это расстояния до всех вершин из верхней левой вершины. Правая верхняя вершина - та, что с индексом 1 - только в одном шагу. Нижняя правая - с индексом 2 - находится в двух шагах. Чтобы попасть в нижнюю левую - с индексом 3 - снова требуется только один шаг. Число 0, которое выведено первым, относится к верхней левой вершине. Поскольку это начальная вершина, которая была передана в `boost::breadth_first_search()`, расстояние от нее до самой себя и равно 0.

`boost::breadth_first_search()` не записывает элементы в массив, он просто увеличивает уже хранящиеся значения. Таким образом, сперва нужно инициализировать все элементы в массиве **distances**.

[Пример 31.9](#ex.graph_09) показывает, как можно найти кратчайший путь.

<a name="ex.graph_09"/>
Пример 31.9 Ищем пути с `boost::breadth_first_search()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <algorithm>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g{edges.begin(), edges.end(), 4};

  boost::array<int, 4> predecessors;
  predecessors[bottomRight] = bottomRight;

  boost::breadth_first_search(g, bottomRight,
    boost::visitor(
      boost::make_bfs_visitor(
        boost::record_predecessors(predecessors.begin(),
          boost::on_tree_edge{}))));

  int p = topLeft;
  while (p != bottomRight)
  {
    std::cout << p << '\n';
    p = predecessors[p];
  }
  std::cout << p << '\n';
}
```

[Пример 31.9](#ex.graph_09) выведет 0, 1 и 2. Это самый короткий путь из левого верхнего угла в правый нижний. Путь выбран через верхнюю правую вершину, хотя путь по нижней левой будет таким же по длине.

`boost::breadth_first_search()` используется снова - на этот раз чтобы найти кратчайший путь. Как вы уже знаете, этот алгоритм просто посещает вершины. Чтобы получить кратчайший путь, должен быть использован соответствующий посетитель. [Пример 31.9](#ex.graph_09) вызывает `boost::record_predecessors()` для этого.

 `boost::record_predecessors()` возвращает посетителя для хранения предка каждой вершины. Всякий раз, когда `boost::breadth_first_search()` посещает новую вершину, предыдущая вершина сохраняется в карте свойств, передающейся в  `boost::record_predecessors()`. `boost::breadth_first_search()` посещает вершины изнутри и находит самый короткий путь - начиная с вершины, переданной в качестве второго параметра в `boost::breadth_first_search()`. [Пример 31.9](#ex.graph_09) находит кратчайшие пути из всех вершин графа к правой нижней вершине.

После выполнения `boost::breadth_first_search()`, карта свойств **predecessors** содержит предка каждой вершины. Чтобы найти первую вершину при движении из верхнего левого угла в нижний правый угол, получают доступ к элементу с индексом 0 - индексом верхней левой вершины - в **predecessors**. Найденное значение в **predecessors** равно 1, что означает, что следующая вершина находится в правом верхнем углу. Обращение к **predecessors** с индексом 1 вернет следующую вершину. В [Примере 31.9](#ex.graph_09) это нижняя правая вершина - та, что с индексом 2. Таким образом итеративно можно найти все вершины в огромных графах, переходя от начальной вершины к конечной.

<a name="ex.graph_10"/>
Пример 31.10 Ищем пути вместе с `boost::breadth_first_search()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <algorithm>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g{edges.begin(), edges.end(), 4};

  boost::array<int, 4> distances{{0}};
  boost::array<int, 4> predecessors;
  predecessors[bottomRight] = bottomRight;

  boost::breadth_first_search(g, bottomRight,
    boost::visitor(
      boost::make_bfs_visitor(
        std::make_pair(
          boost::record_distances(distances.begin(),
            boost::on_tree_edge()),
          boost::record_predecessors(predecessors.begin(),
            boost::on_tree_edge{})))));

  std::for_each(distances.begin(), distances.end(),
    [](int d){ std::cout << d << '\n'; });

  int p = topLeft;
  while (p != bottomRight)
  {
    std::cout << p << '\n';
    p = predecessors[p];
  }
  std::cout << p << '\n';
}
```
[Пример 31.10](#ex.graph_10) показывает, как использовать `boost::breadth_first_search()` с двумя посетителями. Для использования двух посетителей, необходимо поместить их в пару с помощью `std::make_pair()`. Если требуется больше двух посетителей, пары должны быть вложены. [Пример 31.10](#ex.graph_10) делает то же самое, что и [Пример 31.8](#ex.graph_08) и [Пример 31.9](#ex.graph_09) вместе.

`boost::breadth_first_search()` можно использовать только когда все ребра равновесные. Фактически это означает, что время, необходимое, чтобы пересечь любое ребро всегда одинаково. Если ребра взвешенные, причем каждое ребро может иметь разный вес, то вам придется использовать другой алгоритм, чтобы найти кратчайший путь.

<a name="ex.graph_11"/>
Пример 31.11 Ищем кратчайшие пусти с `dijkstra_shortest_paths()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::adjacency_list<boost::listS, boost::vecS,
    boost::undirectedS, boost::no_property,
    boost::property<boost::edge_weight_t, int>> graph;

  std::array<int, 4> weights{{2, 1, 1, 1}};

  graph g{edges.begin(), edges.end(), weights.begin(), 4};

  boost::array<int, 4> directions;
  boost::dijkstra_shortest_paths(g, bottomRight,
    boost::predecessor_map(directions.begin()));

  int p = topLeft;
  while (p != bottomRight)
  {
    std::cout << p << '\n';
    p = directions[p];
  }
  std::cout << p << '\n';
}
```

[Пример 31.11](#ex.graph_11) использует `boost::dijkstra_shortest_paths()`, чтобы найти кратчайший путь в нижний правый угол. Этот алгоритм используется при взвешенных ребрах. В [Примере 31.11](#ex.graph_11) путь от верхнего левого угла в верхний правый в два раза дольше, чем любой другой. 

Перед тем как использовать `boost::dijkstra_shortest_paths()`, ребрам должны быть назначены веса. В примере это сделано при помощи массива **weights**. Элементы в массиве соответствуют ребрам в графе. Поскольку ребро, соединяющее верхний левый угол и верхний правый угол является первым, первый элемент **weights** по значению вдвое больше, чем все остальные.

Для создания соответствия между ребрами и их весами, итератор первого элемента массива **weights** передается в качестве третьего параметра в конструктор графа. Этот третий аргумент может быть использован и для инициализации свойств ребер. Однако работает это только когда свойства ребер были определены.

[Пример 31.11](#ex.graph_11) передает дополнительные параметры шаблона в `boost::adjacency_list`. Четвертый и пятый параметр шаблона определяет есть ли у ребер и вершин свойства и указывает, что это за свойства. Вы можете назначить свойства и ребрам, и вершинам.

По умолчанию, `boost::adjacency_list` использует `boost::no_property`, а это значит, что ни вершины, ни ребра не имеют свойств. В [Примере 31.11](#ex.graph_11) `boost::no_property` передается в качестве четвертого параметра, чтобы не задавать свойства вершинам. Пятый параметр использует `boost::property` для определения пакетного свойства (*bundled property*).

 Пакетные свойства - свойства, которые хранятся внутри графа. Т.к. можно определить несколько пакетных свойств, `boost::property` принимает тег, чтобы определить каждое свойство. Boost.Graph предоставляет некоторые теги, такие как `boost::edge_weight_t`, чтобы определить часто используемые свойства, которые автоматически распознаются и используются алгоритмами. Второй параметр шаблона, который передается в `boost::property` - это тип свойства. В [Примере 31.11](#ex.graph_11) у веса тип `int`.

[Пример 31.11](#ex.graph_11) работает, потому что `boost::dijkstra_shortest_paths()` автоматически использует пакетное свойство типа `boost::edge_weight_t`.

Обратите внимание, что в `boost::dijkstra_shortest_paths()` не передается ни один посетитель. Этот алгоритм не просто посещает вершины. Он ищет кратчайшие пути - вот почему он и называется `boost::dijkstra_shortest_paths()`. Не нужно думать о событиях или посетителях. Вам надо только передать контейнер для хранения предков каждой вершины. Если вы используете такой вариант функции `boost::dijkstra_shortest_paths()`, который принимает уже объявленные параметры, как описано в [Примере 31.11](#ex.graph_11), передайте контейнер с `boost::predecessor_map()`. Это вспомогательная функция, которая принимает указатель или итератор на начало массива.

[Пример 31.11](#ex.graph_11) выведет 0, 3 и 2: самый короткий путь из левого верхнего угла в правый нижний ведет через нижнюю левую вершину. Путь через верхнюю правую вершину больше.

<a name="ex.graph_12"/>
Пример 31.12 Пользовательские свойства в `dijkstra_shortest_paths()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  struct edge_properties
  {
    int weight;
  };

  typedef boost::adjacency_list<boost::listS, boost::vecS,
    boost::undirectedS, boost::no_property,
    edge_properties> graph;

  graph g{edges.begin(), edges.end(), 4};

  graph::edge_iterator it, end;
  boost::tie(it, end) = boost::edges(g);
  g[*it].weight = 2;
  g[*++it].weight = 1;
  g[*++it].weight = 1;
  g[*++it].weight = 1;

  boost::array<int, 4> directions;
  boost::dijkstra_shortest_paths(g, bottomRight,
    boost::predecessor_map(directions.begin()).
    weight_map(boost::get(&edge_properties::weight, g)));

  int p = topLeft;
  while (p != bottomRight)
  {
    std::cout << p << '\n';
    p = directions[p];
  }
  std::cout << p << '\n';
}
```
[Пример 31.12](#ex.graph_12) работает как предыдущий и выводит те же цифры, но он использует определенный пользователем класс, `edge_properties`, а не свойства по умолчанию.

`edge_properties` определяет переменную **weight** для хранения веса ребра. Если требуются другие свойства, можно добавить дополнительные члены класса.

Вы можете получить доступ к определяемым пользователем свойствам, если используете дескриптор ребер в качестве индекса для графа. Таким образом, граф ведет себя как массив. Вы получаете дескрипторы от итераторов ребер, которые получите из `boost::edges()`. Таким путем вес может быть задан каждому ребру.

Чтобы `boost::dijkstra_shortest_paths()` понимал, что веса хранятся в **weight**, в `edge_properties` должен быть передан другой объявленный параметр. Это делается с помощью `weight_map()`. Обратите внимание, что `weight_map()` функция-член объекта, возвращаемого `boost::predecessor_map()`. Есть также свободно стоящая функция `boost::weight_map()`. Если вам необходимо передать несколько объявленных шаблонных параметров, вы должны вызвать функцию-член для первого параметра (того, что возвращен свободно стоящей функцией). Таким образом все параметры будут упакованы в один объект, который затем передается в алгоритм.

Чтобы сказать `boost::dijkstra_shortest_paths()`, что **weight** в `edge_properties` содержит веса, передается указатель на это свойство. Но он не передается в `weight_map()` напрямую. Вместо этого он передается в объект, созданный `boost::get()`. Теперь вызов завершен, и `boost::dijkstra_shortest_paths()` знает, какое свойство позволяет получить вес ребра.

<a name="ex.graph_13"/>
Пример 31.13 Инициализация пользовательских свойств при определении графа
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <iostream>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  struct edge_properties
  {
    int weight;
  };

  typedef boost::adjacency_list<boost::listS, boost::vecS,
    boost::undirectedS, boost::no_property,
    edge_properties> graph;

  boost::array<edge_properties, 4> props{{2, 1, 1, 1}};

  graph g{edges.begin(), edges.end(), props.begin(), 4};

  boost::array<int, 4> directions;
  boost::dijkstra_shortest_paths(g, bottomRight,
    boost::predecessor_map(directions.begin()).
    weight_map(boost::get(&edge_properties::weight, g)));

  int p = topLeft;
  while (p != bottomRight)
  {
    std::cout << p << '\n';
    p = directions[p];
  }
  std::cout << p << '\n';
}
```

Есть возможность инициализировать пользовательские свойства при определении графа. Вам нужно только передать в качестве третьего аргумента конструктора `boost::adjacency_list` итератор типа пользовательского свойства. Таким образом вам не нужно получать доступ к свойствам ребер через дескрипторы. [Пример 31.13](#ex.graph_13) работает как предыдущий пример и выводит тот же самый результат.

<a name="ex.graph_14"/>
Пример 31.14 Случайные пути с `random_spanning_tree()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/array.hpp>
#include <array>
#include <utility>
#include <random>
#include <iostream>
#include <ctime>
#include <cstdint>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  struct edge_properties
  {
    int weight;
  };

  typedef boost::adjacency_list<boost::listS, boost::vecS,
    boost::undirectedS> graph;

  graph g{edges.begin(), edges.end(), 4};

  boost::array<int, 4> predecessors;

  std::mt19937 gen{static_cast<uint32_t>(std::time(0))};
  boost::random_spanning_tree(g, gen,
    boost::predecessor_map(predecessors.begin()).
    root_vertex(bottomLeft));

  int p = topRight;
  while (p != -1)
  {
    std::cout << p << '\n';
    p = predecessors[p];
  }
}
```
Алгоритм, показанный в [Примере 31.14](#ex.graph_14) находит случайные пути. `boost::random_spanning_tree()` похож на `boost::dijkstra_shortest_paths()`. Он возвращает предшественников вершин в контейнер, который передается с `boost::predecessor_map`. В отличие от `boost::dijkstra_shortest_paths()`, начальная вершина не передается непосредственно в качестве аргумента в `boost::random_spanning_tree()`. Она должна быть передана как уже объявленный аргумент. Вот почему `root_vertex()` вызывается на объекте типа `boost::predecessor_map`. [Пример 31.14](#ex.graph_14) находит случайные пути к нижней левой вершине.

Поскольку `boost::random_spanning_tree()` ищет случайный путь, генератор случайных чисел должен быть передан в качестве второго аргумента. [Пример 31.14](#ex.graph_14) использует `std::mt19937`, часть стандартной библиотеки с C ++ 11. Вы также можете использовать генератор случайных чисел из Boost.Random.

[Пример 31.14](#ex.graph_14) выведет либо 1, 0 и 3 или 1, 2 и 3. 1 - правая верхняя вершина, 3 - левая нижняя вершина. Есть только два возможных пути от верхней правой вершины к нижней левой: через верхнюю левую или через нижнюю правую. `boost::random_spanning_tree()` должен вернуть один из этих вариантов.
