#include "gtest/gtest.h"
#include "../src/quadtree.h"

TEST(QuadtreeTest, Entidad_en_rango)
{
    QuadTree qt(Vector2(), 64.0f, 64.0f);
    Circulo circulo(Vector2(), 5.0f);
    Entidad entidad(&circulo);

    ASSERT_TRUE(qt.insertar(&entidad));
}