# Контейнеры

Все примеры в этой главе до сих пор использовали `boost::adjacency_list` для определения графов. В этом разделе представлены два других контейнера для графа, предусмотренных Boost.Graph: `boost::adjacency_matrix` и `boost::compressed_sparse_row_graph`.
***
####Заметка
Здесь недостает подключения в `boost/graph/adjacency_matrix.hpp` для Boost 1.56.0. Чтобы скомпилировать [Пример 31.15](#ex.graph_15) с Boost 1.56.0, подключите `boost/functional/hash.hpp` до `boost/graph/adjacency_matrix.hpp`.
***

<a name="ex.graph_15"/>
Пример 31.15 Графы и `boost::adjacency_matrix`
```c++
#include <boost/graph/adjacency_matrix.hpp>
#include <array>
#include <utility>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::adjacency_matrix<boost::undirectedS> graph;
  graph g{edges.begin(), edges.end(), 4};
}
```
`boost::adjacency_matrix` используется как `boost::adjacency_list` (так что смотри [Пример 31.15](#ex.graph_15)). Тем не менее, в `boost::adjacency_matrix` не используются два шаблонных параметра, передающих селекторы (те, что `boost::vecS` и `boost::setS`). `boost::adjacency_matrix` сохраняет граф в виде матрицы смежности, и внутренняя структура ее очень непонятна и написана жесткими программистами. Но вы можете расслабиться и будет достаточно думать о матрице смежности, как о двумерной таблице: таблица представляет собой квадрат с количеством строк и столбцов, равных числу вершин графа. Ребро создается путем маркировки ячейки, строка и столбец которой совпадают с начальной и конечной вершиной соответственно.

Такая внутренняя структура `boost::adjacency_matrix` обеспечивает быстрое добавление и удаление ребер. Но мы сильно теряем в  производительности памяти. Так что используйте `boost::adjacency_list`, когда число ребер небольшое по сравнению с числом вершин. Чем больше ребер, тем больше смысла использовать `boost::adjacency_matrix`.

<a name="ex.graph_16"/>
Пример 31.16 Графы с `boost::compressed_sparse_row_graph`
```c++
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <array>
#include <utility>

int main()
{
  enum { topLeft, topRight, bottomRight, bottomLeft };

  std::array<std::pair<int, int>, 4> edges{{
    std::make_pair(topLeft, topRight),
    std::make_pair(topRight, bottomRight),
    std::make_pair(bottomRight, bottomLeft),
    std::make_pair(bottomLeft, topLeft)
  }};

  typedef boost::compressed_sparse_row_graph<boost::bidirectionalS> graph;
  graph g{boost::edges_are_unsorted_multi_pass, edges.begin(),
    edges.end(), 4};
}
```

`boost::compressed_sparse_row_graph` используется таким же образом, как `boost::adjacency_list` и `boost::adjacency_matrix` (смотри [Пример 31.16](#ex.graph_16)). Наиболее важным отличием от них является то, что граф нельзя изменять. После того, как граф был создан, вершины и ребра нельзя добавить или удалить. Так что используйте `boost::compressed_sparse_row_graph` для неизменяемых графов.

`boost::compressed_sparse_row_graph` поддерживает только ориентированные графы. У ребер такого графа есть направления. Нельзя создать экземпляр `boost::compressed_sparse_row_graph` с шаблонным параметром `boost::undirectedS`.

Основное преимущество `boost::compressed_sparse_row_graph` - это низкое потребление памяти. `boost::compressed_sparse_row_graph` особенно полезен, если у вас огромный граф, с которым логично держать потребление памяти на низком уровне.
