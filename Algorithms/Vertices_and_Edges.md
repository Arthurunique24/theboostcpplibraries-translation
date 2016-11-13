
# Вершины и рёбра

Граф состоит из вершин и ребер, соответственно, чтобы создать граф, нужно определить множество вершин и рёбер между ними. [Пример 31.1](#ex.graph_01) содержит первый простой граф, состоящий из четырёх вершин и не имеющий рёбер.

<a name="ex.graph_01"/>
Пример 31.1. Граф типа `boost::adjacency_list` с 4 вершинами.
```c++
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

int main()
{
  boost::adjacency_list<> g;

  boost::adjacency_list<>::vertex_descriptor v1 = boost::add_vertex(g);
  boost::adjacency_list<>::vertex_descriptor v2 = boost::add_vertex(g);
  boost::adjacency_list<>::vertex_descriptor v3 = boost::add_vertex(g);
  boost::adjacency_list<>::vertex_descriptor v4 = boost::add_vertex(g);

  std::cout << v1 << ", " << v2 << ", " << v3 << ", " << v4 << '\n';
}
```
Boost.Graph предоставляет три контейнера для задания графов. Самый важный контейнер `boost::adjacency_list`, который используется почти во всех примерах в этой главе. Чтобы использовать этот класс, нужно подключить заголовочный файл `boost/graph/adjacency_list.hpp`. Если вы хотите использовать другой контейнер, нужно будет подключить другой заголовочный файл. В Boost.Graph нет общего заголовочного файла, который позволил бы получить доступ ко всем классам и функциям. 

`boost::adjacency_list` шаблонный класс, который в [Примере 31.1](#ex.graph_01) создается с параметрами по умолчанию. Позже вы увидите, какие параметры можно передать в шаблон этого класса, определенного в `boost`. Кстати говоря, все классы и функции из Boost.Graph определены в этом пространстве имён.

`boost::add_vertex()` функция, добавляющая вершины графу. Чтобы добавить 4 вершины нашему графу, функция `boost::add_vertex()` должна быть вызвана 4 раза.

`boost::add_vertex()` самостоятельная функция, не принадлежащая классу. На самом деле в Boost.Graph очень много подобных самостоятельных функций, которые могли бы быть реализованы в качестве метода класса. Почему же так? Все дело в том, что Boost.Graph написан по большей части как библиотека обобщенного программирования, а не объектно-ориентированного.
`boost::add_vertex()` возвращает объект типа `boost::adjacency_list::vertex_descriptor`. Этот объект представляет собой новую добавленную вершину графа. Можно вывести объект вершины в стандартный поток вывода, как в [Примере 31.1](#ex.graph_01). В итоге будет выведено: `0, 1, 2, 3`.

Как видно в [Примере 31.1](#ex.graph_01), вершины графа определяются как положительные целые числа. Они являются индексами элементов вектора, который используется в `boost::adjacency_list`. Неудивительно, что  `boost::add_vertex()` возвращает 0, 1, 2 и 3, так как каждый вызов добавляет еще один элемент вектору. 

 `boost::adjacency_list` по умолчанию использует контейнер `std::vector` для хранения вершин. В нашем случае  `boost::adjacency_list::vertex_descriptor` это определение типа `std::size_t`. Так как могут быть использованы и другие контейнеры для хранения вершин, `boost::adjacency_list::vertex_descriptor` не обязательно всегда `std::size_t`.

<a name="ex.graph_02"/>
Пример 31.2. Доступ к вершинам с помощью `boost::vertices()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>

int main()
{
  boost::adjacency_list<> g;

  boost::add_vertex(g);
  boost::add_vertex(g);
  boost::add_vertex(g);
  boost::add_vertex(g);

  std::pair<boost::adjacency_list<>::vertex_iterator,
    boost::adjacency_list<>::vertex_iterator> vs = boost::vertices(g);

  std::copy(vs.first, vs.second,
    std::ostream_iterator<boost::adjacency_list<>::vertex_descriptor>{
      std::cout, "\n"});
}
```

Чтобы получить все вершины графа, нужно вызвать `boost::vertices()`. Эта функция возвращает два итератора типа `boost::adjacency_list::vertex_iterator`, которые относятся к начальным и конечным точкам. Итераторы возвращаются в виде `std::pair`. [Пример 31.2](#ex.graph_02) использует итераторы для вывода всех вершин в стандартный поток вывода. После компиляции снова получится `0, 1, 2, и 3`, так же, как и в предыдущем примере.

[Пример 31.3](#ex.graph_03) показывает как добавить вершины в граф и соединить их ребрами.

<a name="ex.graph_03"/>
Пример 31.3 Доступ к ребрам с помощью `boost::edges()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>

int main()
{
  boost::adjacency_list<> g;

  boost::adjacency_list<>::vertex_descriptor v1 = boost::add_vertex(g);
  boost::adjacency_list<>::vertex_descriptor v2 = boost::add_vertex(g);
  boost::add_vertex(g);
  boost::add_vertex(g);

  std::pair<boost::adjacency_list<>::edge_descriptor, bool> p =
    boost::add_edge(v1, v2, g);
  std::cout.setf(std::ios::boolalpha);
  std::cout << p.second << '\n';

  p = boost::add_edge(v1, v2, g);
  std::cout << p.second << '\n';

  p = boost::add_edge(v2, v1, g);
  std::cout << p.second << '\n';

  std::pair<boost::adjacency_list<>::edge_iterator,
    boost::adjacency_list<>::edge_iterator> es = boost::edges(g);

  std::copy(es.first, es.second,
    std::ostream_iterator<boost::adjacency_list<>::edge_descriptor>{
      std::cout, "\n"});
}
```

Итак, вызываем `boost::add_edge()`, чтобы соединить две вершины графа. Для этого нужно передать соответственные вершины и граф в качестве параметров. 

`boost::add_edge()` возвращает std::pair. **first** обеспечивает доступ к ребру. **second** является переменной типа `bool`, указывающей на то, было ли успешно добавлено это ребро. Если вы скомпилируете [Пример 31.3](#ex.graph_03), то увидите, что `p.second` равно true для каждого вызова `boost::add_edge()`, а также что с каждым вызовом добавляется новое ребро.

`boost::edges()` предоставляет доступ ко всем ребрам в графе. Как `boost::vertices()`, `boost::edges()` возвращает два итератора, указывающих на начальное и конечное ребро. [Пример 31.3](#ex.graph_03) выводит все ребра в стандартный поток вывода. В примере выведется `(0,1)`, `(0,1)` и `(1,0)`.

Вывод показывает, что у графа три ребра. Все они соединяют первые две вершины - те, что с индексами 0 и 1. Вывод также показывает, где начинаются и заканчиваются ребра. Два ребра начинаются в первой вершине, третье во второй. Направление ребер зависит от порядка параметров, переданных в `boost::add_edge()`.

Как видите, можно иметь несколько ребер между двумя одинаковыми вершинами. Но это свойство можно и отключить.

<a name="ex.graph_04"/>
Пример 31.4 `boost::adjacency_list` с переключателями
```c++
#include <boost/graph/adjacency_list.hpp>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>

int main()
{
  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g;

  boost::adjacency_list<>::vertex_descriptor v1 = boost::add_vertex(g);
  boost::adjacency_list<>::vertex_descriptor v2 = boost::add_vertex(g);
  boost::add_vertex(g);
  boost::add_vertex(g);

  std::pair<graph::edge_descriptor, bool> p =
    boost::add_edge(v1, v2, g);
  std::cout.setf(std::ios::boolalpha);
  std::cout << p.second << '\n';

  p = boost::add_edge(v1, v2, g);
  std::cout << p.second << '\n';

  p = boost::add_edge(v2, v1, g);
  std::cout << p.second << '\n';

  std::pair<graph::edge_iterator,
    graph::edge_iterator> es = boost::edges(g);

  std::copy(es.first, es.second,
    std::ostream_iterator<graph::edge_descriptor>{std::cout, "\n"});
}
```

[Пример 31.4](#ex.graph_04) не инстанциирует `boost::adjacency_list` с шаблонными параметрами по умолчанию. Три параметра, называемые селекторами, передаются в шаблон. По соглашению, имена селекторов заканчиваются на S. Эти селекторы определяют, какие типы будут использоваться в `boost::adjacency_list` для хранения вершин и ребер.

По умолчанию, `boost::adjacency_list` использует `std::vector` для хранения вершин и ребер. Из-за того, что мы передали `boost::setS` в качестве первого параметра в [Примере 31.4](#ex.graph_04), `std::set` используется в качестве контейнера для ребер. Поскольку `std::set` не поддерживает дубликатов, мы больше не сможем добавить уже имеющееся ребро, используя `boost::add_edge()`. Таким образом, пример выведет нам (0,1) только один раз.

Второй параметр шаблона говорит `boost::adjacency_list`, какой класс должен использоваться для хранения вершин. В [Примере 31.4](#ex.graph_04) передан `boost::vecS`. Это значение по умолчанию для второго параметра шаблона. Нам пришлось его указать, чтобы передать третий параметр шаблона.

Третий параметр шаблона определяет, будут ли у графа направленные ребра или нет. По умолчанию передается `boost::directedS`, что означает, что все ребра направленные и могут быть представлены в виде стрелок. Ребра могут пересекаться только в одном направлении.

`boost::undirectedS` используется в [Примере 31.4](#ex.graph_04). Этот селектор делает все ребра неориентированными, то есть все ребра можно пересечь в любом направлении. Не имеет значения, какая вершина является началом, а какая концом. Это еще одна причина, почему граф в [Примере 31.4](#ex.graph_04) содержит только одно ребро. Третий вызов функции `boost::add_edge()` меняет местами начальную и конечную вершины, но так как ребра у графа в этом примере ненаправленные, это ребро уже имеется и, следовательно, не добавляется.

В Boost.Graph есть еще много селекторов, в том числе `boost::listS`, `boost::mapS`, также `boost::hash_setS`. `boost::bidirectionalS` используется, чтобы ребра могли быть двунаправленными. Этот селектор похож на `boost::undirectedS`, но в этом случае начальная и конечная вершины важны. Используя `boost::bidirectionalS`в [Примере 31.4](#ex.graph_04), третий вызов `boost::add_edge()` добавит ребро в граф.

В [Примере 31.5](#ex.graph_05) показан более простой метод добавления вершин и ребер в граф.

<a name="ex.graph_05"/>
Пример 31.5 Создаем индексы автоматически с `boost::add_edge()` 
```c++
#include <boost/graph/adjacency_list.hpp>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <iostream>

int main()
{
  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g;

  enum { topLeft, topRight, bottomRight, bottomLeft };

  boost::add_edge(topLeft, topRight, g);
  boost::add_edge(topRight, bottomRight, g);
  boost::add_edge(bottomRight, bottomLeft, g);
  boost::add_edge(bottomLeft, topLeft, g);

  graph::edge_iterator it, end;
  std::tie(it, end) = boost::edges(g);
  std::copy(it, end,
    std::ostream_iterator<graph::edge_descriptor>{std::cout, "\n"});
}
```
[Пример 31.5](#ex.graph_05) задает граф, состоящий из четырех вершин. Вы можете визуализировать граф в виде карты с четырьмя вершинами, каждая из которых представлена точкой. Точки будут названы `topLeft`, `topRight`, `bottomRight` и `bottomLeft`. Поскольку имена заданы перечислением, каждое из них будет иметь числовое значение, которое используется в качестве индекса.

Граф можно задать и без вызова `boost::add_vertex()`. Boost.Graph добавляет недостающие вершины графу автоматически, если вершины, переданной `boost::add_edge()`, не существует. Многочисленные вызовы `boost::add_edge()` в [Примере 31.5](#ex.graph_05) не только определяют ребра, но и добавляют четыре вершины, необходимые ребрам графа.

Пожалуйста, заметьте как `std::tie()` используется для хранения итераторов, возвращенных в виде `std::pair` из `boost::edges()` в **it** и **end**. `std::tie()` - часть стандартной библиотеки с далекого C++11.

Граф в [Примере 31.5](#ex.graph_05) представляет собой карту с четырьмя вершинами. Чтобы перейти из верхнего левого угла в нижний правый угол, можно либо пересечь вершину в правом верхнем углу или же в левом нижнем углу. Ведь нет линии между противоположными вершинами. Таким образом, невозможно перейти непосредственно из верхнего левого угла в нижний правый угол. Все примеры в этой главе используют этот граф.

<a name="ex.graph_06"/>
Пример 31.6 `boost::adjacent_vertices()` и `boost::out_edges()`
```c++
#include <boost/graph/adjacency_list.hpp>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <iostream>

int main()
{
  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g;

  enum { topLeft, topRight, bottomRight, bottomLeft };

  boost::add_edge(topLeft, topRight, g);
  boost::add_edge(topRight, bottomRight, g);
  boost::add_edge(bottomRight, bottomLeft, g);
  boost::add_edge(bottomLeft, topLeft, g);

  graph::adjacency_iterator vit, vend;
  std::tie(vit, vend) = boost::adjacent_vertices(topLeft, g);
  std::copy(vit, vend,
    std::ostream_iterator<graph::vertex_descriptor>{std::cout, "\n"});

  graph::out_edge_iterator eit, eend;
  std::tie(eit, eend) = boost::out_edges(topLeft, g);
  std::for_each(eit, eend,
    [&g](graph::edge_descriptor it)
      { std::cout << boost::target(it, g) << '\n'; });
}
```

[Пример 31.6](#ex.graph_06) познакомит вас с функциями, необходимыми, чтобы получить дополнительную информацию о вершинах. `boost::adjacent_vertices()` возвращает пару итераторов, указывающих на вершины, с которыми соединена передаваемая вершина. Вызывайте `boost::out_edges()`, если хотите получить доступ ко всем исходящим ребрам от заданной вершины. `boost::in_edges()` получает доступ ко всем входящим ребрам. В случае неориентированного графа не имеет значения какую из функций вы вызовите.

`boost::target()` возвращает конечную вершину ребра. Начальная вершина вернется с `boost::source()`.

[Пример 31.6](#ex.graph_06) дважды выведет в стандартный поток вывода 1 и 3, индексы правой верхней и нижней левой вершин. `boost::adjacent_vertices()`, вызывается с переданным **topLeft** и выводит индексы правой верхней и левой нижней вершин. **topLeft** также передается `boost::out_edges()` для получения выходящих вершин. Поскольку `boost::target()` вызывается на каждом выходящем ребре с `std::for_each()`, индексы верхней правой вершины и нижней левой отображаются дважды.

[Пример 31.7](#ex.graph_07) показывает, как задать граф с `boost::adjacency_list` без вызова `boost::add_edge()`
 для каждого ребра.

<a name="ex.graph_07"/>
Пример 31.7 Инициализируем `boost::adjacency_list` ребрами
```c++
#include <boost/graph/adjacency_list.hpp>
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

  typedef boost::adjacency_list<boost::setS, boost::vecS,
    boost::undirectedS> graph;
  graph g{edges.begin(), edges.end(), 4};

  std::cout << boost::num_vertices(g) << '\n';
  std::cout << boost::num_edges(g) << '\n';

  g.clear();
}
```

Вы можете передать итераторы в конструктор `boost::adjacency_list`, ссылающиеся на объекты типа `std::pair<int, int>`, которые определяют ребра. Если вы передадите итераторы, также нужно будет передать третьим аргументом общее число вершин графа. Граф по крайней мере будет содержать вершины, необходимые для построения ребер. Третий аргумент позволяет вам добавить в граф вершины, не связанные ребрами с другими вершинами.

[Пример 31.7](#ex.graph_07) использует функции `boost::num_vertices()` и `boost::num_edges()`, которые возвращают количество вершин и ребер соответственно. Пример выведет `4` дважды.

[Пример 31.7](#ex.graph_07) вызывает `boost::adjacency_list::clear()`. Эта функция удаляет все вершины и ребра. Эта функция - член `boost::adjacency_list`, а не свободно стоящая функция.
