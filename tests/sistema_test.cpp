#include "gtest/gtest.h"
#include "../src/sistema.h"

#include <math.h>

using namespace sistema;

class Particula_pos : public Particula
{
public:
    cr::Circulo m_circulo;

public:
    Particula_pos(float masa, Vector2 posicion, Vector2 velocidad, Vector2 fuerza, float coeficiente)
        : m_circulo(cr::Circulo(masa, posicion, .0f, .0f)), Particula(&m_circulo, velocidad, fuerza, coeficiente)
    {
    }

    Particula_pos(float masa, Vector2 velocidad, Vector2 fuerza, float coeficiente)
        : m_circulo(cr::Circulo(masa, Vector2(), .0f, .0f)), Particula(&m_circulo, velocidad, fuerza, coeficiente)
    {
    }

    Particula_pos()
        : m_circulo(cr::Circulo(-1.0f, Vector2(), .0f, .0f)), Particula(&m_circulo, Vector2(), Vector2(), -1.0f)
    {
    }

    Vector2 posicion_relativa(Particula_pos *particula)
    {
        return (particula->m_cuerpo->m_posicion - m_cuerpo->m_posicion);
    }

    void actualizar(float dt)
    {
        m_velocidad += (m_fuerza * dt) / m_cuerpo->m_masa;
        m_fuerza *= .0f;
    }
};

TEST(SistemaTest, Dos_particulas_y_el_piso_sin_velocidad_ninguna_sus_fuerzas_finales_son_cero)
{
    cr::Circulo posicion1(1.0f, Vector2(.0f, 3.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(2.0f, Vector2(.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(), Vector2(.0f, -20.0f), 1.0f);
    cr::Poligono<2> linea(-1.0f, {Vector2(1.0f, .0f), Vector2(-1.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea);

    Sistema sistema({&particula1, &particula2, &piso});

    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula2, &piso);
    sistema.agregar_interaccion(&piso, &particula2);
    sistema.agregar_interaccion(&particula2, &particula1);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2());
    ASSERT_EQ(particula2.m_velocidad, Vector2());
}

TEST(SistemaTest, Dos_particulas_y_el_piso_el_primero_con_velocidad_y_rebota_con_su_velocidad_invertida)
{
    cr::Circulo posicion1(1.0f, Vector2(.0f, 3.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(.0f, -10.0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(2.0f, Vector2(.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(), Vector2(.0f, -20.0f), 1.0f);
    cr::Poligono<2> linea(-1.0f, {Vector2(1.0f, .0f), Vector2(-1.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea);

    Sistema sistema({&particula1, &particula2, &piso});

    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula2, &piso);
    sistema.agregar_interaccion(&piso, &particula2);
    sistema.agregar_interaccion(&particula2, &particula1);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2(.0f, 7.777f));
    ASSERT_EQ(particula2.m_velocidad, Vector2(.0f, 4.444f));
}

TEST(SistemaTest, Dos_particulas_sobre_el_piso_una_tiene_velocidad_y_terminan_intercambiando_velocidades)
{
    cr::Circulo posicion1(1.0f, Vector2(-1.0f, 1.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(10.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(1.0f, Vector2(1.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(), Vector2(.0f, -10.0f), 1.0f);
    cr::Poligono<2> linea(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea);

    Sistema sistema({&particula1, &particula2, &piso});

    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula2, &particula1);
    sistema.agregar_interaccion(&particula1, &piso);
    sistema.agregar_interaccion(&piso, &particula1);
    sistema.agregar_interaccion(&particula2, &piso);
    sistema.agregar_interaccion(&piso, &particula2);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2());
    ASSERT_EQ(particula2.m_velocidad, Vector2(10.0f, .0f));
}

TEST(SistemaTest, Dos_particulas_sobre_el_piso_una_tienen_velocidades_y_hay_un_choque_elastico)
{
    cr::Circulo posicion1(1.0f, Vector2(-1.0f, 1.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(30.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(2.0f, Vector2(1.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(-30.0f, .0f), Vector2(.0f, -20.0f), 1.0f);
    cr::Poligono<2> linea(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea);

    Sistema sistema({&particula1, &particula2, &piso});

    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula2, &particula1);
    sistema.agregar_interaccion(&particula1, &piso);
    sistema.agregar_interaccion(&piso, &particula1);
    sistema.agregar_interaccion(&particula2, &piso);
    sistema.agregar_interaccion(&piso, &particula2);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2(-50.0f, .0f));
    ASSERT_EQ(particula2.m_velocidad, Vector2(10.0f, .0f));
}

TEST(SistemaTest, Particula_estando_en_una_esquina_y_una_velocidad_horizontal_rebota_contra_la_pared)
{
    cr::Circulo posicion(1.0f, Vector2(-1.0f, 1.0f), .0f, 1.0f);
    Particula particula = Particula(&posicion, Vector2(10.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Poligono<2> linea_piso(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea_piso);
    cr::Poligono<2> linea_pared(-1.0f, {Vector2(.0f, 10.0f), Vector2(.0f, -10.0f)});
    Particula pared = ParticulaEstatica(&linea_pared);

    Sistema sistema({&particula, &piso, &pared});

    sistema.agregar_interaccion(&particula, &piso);
    sistema.agregar_interaccion(&particula, &pared);
    sistema.agregar_interaccion(&pared, &particula);
    sistema.agregar_interaccion(&pared, &piso);
    sistema.agregar_interaccion(&piso, &particula);
    sistema.agregar_interaccion(&piso, &pared);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula, &piso, &pared})
        ((Particula_pos *)p)->actualizar(1.0f);

    particula.m_velocidad.imprimir();

    ASSERT_EQ(particula.m_velocidad, Vector2(-10.0f, .0f));
}

TEST(SistemaTest, Particula_choca_contra_el_piso_con_una_velocidad_y_rebota_terminando_con_la_velocidad_opuesta)
{
    cr::Circulo posicion(1.0f, Vector2(.0f, 1.0f), .0f, 1.0f);
    Particula particula = Particula(&posicion, Vector2(.0f, -10.0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Poligono<2> linea_piso(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea_piso);

    Sistema sistema({&particula, &piso});

    sistema.agregar_interaccion(&particula, &piso);
    sistema.agregar_interaccion(&piso, &particula);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula.m_velocidad, Vector2(.0f, 10.0f));
}

TEST(SistemaTest, Dos_particulas_con_velocidades_en_la_misma_direccion_la_que_esta_adelante_tiene_menor_velocidad)
{
    cr::Circulo posicion1(1.0f, Vector2(.0f, 3.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(.0f, 10.0f / 3.0f), Vector2(.0f, .0f), 1.0f);
    cr::Circulo posicion2(2.0f, Vector2(.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(.0f, 20.0f / 3.0f), Vector2(.0f, .0f), 1.0f);

    Sistema sistema({&particula1, &particula2});

    sistema.agregar_interaccion(&particula2, &particula1);
    sistema.agregar_interaccion(&particula1, &particula2);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2(.0f, 7.77f));
    ASSERT_EQ(particula2.m_velocidad, Vector2(.0f, 4.44f));
}

TEST(SistemaTest, Simular_el_pendulo_de_newton_con_todas_las_particulas_de_la_misma_masa)
{

    cr::Circulo posicion1(1.0f, Vector2(-4.0f, 1.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(10.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(1.0f, Vector2(-2.0f, 1.0f), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion3(1.0f, Vector2(.0f, 1.0f), .0f, 1.0f);
    Particula particula3 = Particula(&posicion3, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion4(1.0f, Vector2(2.0f, 1.0f), .0f, 1.0f);
    Particula particula4 = Particula(&posicion4, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion5(1.0f, Vector2(4.0f, 1.0f), .0f, 1.0f);
    Particula particula5 = Particula(&posicion5, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Poligono<2> linea_piso(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea_piso);

    Sistema sistema({&particula1, &particula2, &particula3, &particula4, &particula5, &piso});

    sistema.agregar_interaccion(&particula1, &piso);
    sistema.agregar_interaccion(&particula2, &piso);
    sistema.agregar_interaccion(&particula3, &piso);
    sistema.agregar_interaccion(&particula4, &piso);
    sistema.agregar_interaccion(&particula5, &piso);
    sistema.agregar_interaccion(&piso, &particula1);
    sistema.agregar_interaccion(&piso, &particula2);
    sistema.agregar_interaccion(&piso, &particula3);
    sistema.agregar_interaccion(&piso, &particula4);
    sistema.agregar_interaccion(&piso, &particula5);
    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula2, &particula1);
    sistema.agregar_interaccion(&particula2, &particula3);
    sistema.agregar_interaccion(&particula3, &particula2);
    sistema.agregar_interaccion(&particula3, &particula4);
    sistema.agregar_interaccion(&particula4, &particula3);
    sistema.agregar_interaccion(&particula4, &particula5);
    sistema.agregar_interaccion(&particula5, &particula4);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &particula3, &particula4, &particula5, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1.m_velocidad, Vector2(.0f, .0f));
    ASSERT_EQ(particula2.m_velocidad, Vector2(.0f, .0f));
    ASSERT_EQ(particula3.m_velocidad, Vector2(.0f, .0f));
    ASSERT_EQ(particula4.m_velocidad, Vector2(.0f, .0f));
    ASSERT_EQ(particula5.m_velocidad, Vector2(10.0f, .0f));
}

TEST(SistemaTest, Tres_particulas_con_las_fuerzas_de_peso_posicionadas_en_forma_de_piramide_las_de_abajo_se_mueven_para_los_costados)
{
    cr::Circulo posicion1(1.0f, Vector2(-1.0f, 1.0f), .0f, 1.0f);
    Particula particula1 = Particula(&posicion1, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion2(1.0f, Vector2(.0f, .99f + sqrt(3)), .0f, 1.0f);
    Particula particula2 = Particula(&posicion2, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Circulo posicion3(1.0f, Vector2(1.0f, 1.0f), .0f, 1.0f);
    Particula particula3 = Particula(&posicion3, Vector2(.0f, .0f), Vector2(.0f, -10.0f), 1.0f);
    cr::Poligono<2> linea_piso(-1.0f, {Vector2(10.0f, .0f), Vector2(-10.0f, .0f)});
    Particula piso = ParticulaEstatica(&linea_piso);

    Sistema sistema({&particula1, &particula2, &particula3, &piso});

    sistema.agregar_interaccion(&particula2, &particula1);
    sistema.agregar_interaccion(&particula2, &particula3);
    sistema.agregar_interaccion(&particula1, &particula2);
    sistema.agregar_interaccion(&particula3, &particula2);
    sistema.agregar_interaccion(&particula1, &particula3);
    sistema.agregar_interaccion(&particula3, &particula1);
    sistema.agregar_interaccion(&particula1, &piso);
    sistema.agregar_interaccion(&piso, &particula1);
    sistema.agregar_interaccion(&particula3, &piso);
    sistema.agregar_interaccion(&piso, &particula3);

    sistema.expandir_interacciones();
    for (Particula *p : {&particula1, &particula2, &particula3, &piso})
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1.m_velocidad.y, .0f, .01f);
    ASSERT_NEAR(particula2.m_velocidad.x, .0f, .01f);
    ASSERT_NEAR(particula3.m_velocidad.y, .0f, .01f);
    ASSERT_NEAR(particula1.m_velocidad.x, -1.0f * particula3.m_velocidad.x, .01f);
}

TEST(SistemaTest, Tres_particulas_sin_fuerzas_posicionadas_en_una_piramide_donde_la_de_mas_arriba_tiene_velocidad_se_mueven)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-0.2f, .0f), Vector2(.0f, .0f), Vector2(.0f, .0f), 1.0f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(.0f, 1.0f), Vector2(.0f, -10.0f), Vector2(.0f, .0f), 1.0f);
    Particula_pos *particula3 = new Particula_pos(1.0f, Vector2(.2f, .0f), Vector2(.0f, .0f), Vector2(.0f, .0f), 1.0f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);
    particulas.emplace_back(particula3);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    Vector2 dir_1_3 = particula1->posicion_relativa(particula3), dir_3_1 = particula3->posicion_relativa(particula1);
    Vector2 dir_2_3 = particula2->posicion_relativa(particula3), dir_3_2 = particula3->posicion_relativa(particula2);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);
    sistema.agregar_interaccion(particula2, particula3, dir_2_3);
    sistema.agregar_interaccion(particula3, particula2, dir_3_2);
    sistema.agregar_interaccion(particula1, particula3, dir_1_3);
    sistema.agregar_interaccion(particula3, particula1, dir_3_1);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    EXPECT_NEAR(particula2->m_velocidad.x, .0f, .01f);
    EXPECT_NEAR(particula1->m_velocidad.x, -1.0f * particula3->m_velocidad.x, .01f);
    EXPECT_NEAR(particula1->m_velocidad.y, particula3->m_velocidad.y, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Tres_particulas_sin_fuerzas_posicionadas_en_una_piramide_donde_la_de_mas_arriba_tiene_velocidad_se_mueven_hace_chochar_contra_el_piso)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-0.2f, .0f), Vector2(.0f, .0f), Vector2(.0f, .0f), 1.0f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(.0f, .22f), Vector2(.0f, -10.0f), Vector2(.0f, .0f), 1.0f);
    Particula_pos *particula3 = new Particula_pos(1.0f, Vector2(.2f, .0f), Vector2(.0f, .0f), Vector2(.0f, .0f), 1.0f);
    Particula *piso = new Particula_pos();

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);
    particulas.emplace_back(particula3);
    particulas.emplace_back(piso);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    Vector2 dir_1_3 = particula1->posicion_relativa(particula3), dir_3_1 = particula3->posicion_relativa(particula1);
    Vector2 dir_2_3 = particula2->posicion_relativa(particula3), dir_3_2 = particula3->posicion_relativa(particula2);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);
    sistema.agregar_interaccion(particula2, particula3, dir_2_3);
    sistema.agregar_interaccion(particula3, particula2, dir_3_2);
    sistema.agregar_interaccion(particula1, particula3, dir_1_3);
    sistema.agregar_interaccion(particula3, particula1, dir_3_1);

    Vector2 dir_abajo(.0f, -1.0f), dir_arriba(.0f, 1.0f);
    sistema.agregar_interaccion(particula1, piso, dir_abajo);
    sistema.agregar_interaccion(particula3, piso, dir_abajo);
    sistema.agregar_interaccion(piso, particula1, dir_arriba);
    sistema.agregar_interaccion(piso, particula3, dir_arriba);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    EXPECT_NEAR(particula2->m_velocidad.x, .0f, .01f);
    EXPECT_NEAR(particula1->m_velocidad.x, -1.0f * particula3->m_velocidad.x, .01f);
    EXPECT_NEAR(particula1->m_velocidad.y, particula3->m_velocidad.y, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_tiene_un_choque_inelastico_de_50_porciento)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .5f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .5f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, -.5f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, .5f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_tiene_un_choque_inelastico_de_25_porciento)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .25f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .25f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, -.25f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, .25f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_con_masas_diferentes_tiene_un_choque_inelastico_de_50_porciento)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .5f);
    Particula_pos *particula2 = new Particula_pos(2.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .5f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, -1.0f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, .0f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_con_masas_diferentes_tiene_un_choque_inelastico_de_25_porciento)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .3f);
    Particula_pos *particula2 = new Particula_pos(2.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .3f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, -.73333f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, -.13333f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_con_masas_diferentes_tiene_un_choque_plastico)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .0f);
    Particula_pos *particula2 = new Particula_pos(2.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .0f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, -.33333f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, -.33333f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_de_la_misma_masa_tiene_un_choque_plastico)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-1.0f, .0f), Vector2(1.0f, .0f), Vector2(.0f, .0f), .0f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(1.0f, .0f), Vector2(-1.0f, .0f), Vector2(.0f, .0f), .0f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_NEAR(particula1->m_velocidad.x, .0f, .01f);
    ASSERT_NEAR(particula2->m_velocidad.x, .0f, .01f);

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_de_la_misma_masa_tiene_un_choque_inelastico_de_50_porciento_en_dos_dimensiones)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-.21f, -.12), Vector2(1.0f, .0f), Vector2(.0f, .0f), .5f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(.0f, .2f), Vector2(.0f, -1.0f), Vector2(.0f, .0f), .5f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1->m_velocidad, Vector2(.43f, -.86f));
    ASSERT_EQ(particula2->m_velocidad, Vector2(.57f, -.13f));

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Dos_particulas_de_la_misma_masa_tiene_un_choque_inelastico_de_5_porciento_en_dos_dimensiones)
{
    std::vector<Particula *> particulas;
    Particula_pos *particula1 = new Particula_pos(1.0f, Vector2(-.21f, -.12), Vector2(1.0f, .0f), Vector2(.0f, .0f), .05f);
    Particula_pos *particula2 = new Particula_pos(1.0f, Vector2(.0f, .2f), Vector2(.0f, -1.0f), Vector2(.0f, .0f), .05f);

    particulas.emplace_back(particula1);
    particulas.emplace_back(particula2);

    Sistema sistema(particulas);

    Vector2 dir_1_2 = particula1->posicion_relativa(particula2), dir_2_1 = particula2->posicion_relativa(particula1);
    sistema.agregar_interaccion(particula2, particula1, dir_2_1);
    sistema.agregar_interaccion(particula1, particula2, dir_1_2);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula1->m_velocidad, Vector2(.6f, -.6f));
    ASSERT_EQ(particula2->m_velocidad, Vector2(.4f, -.39f));

    for (Particula *p : particulas)
        delete p;
}

TEST(SistemaTest, Una_particula_choca_contra_la_esquina_y_el_choque_es_plastico_haciendo_que_su_velocidad_sea_cero)
{
    std::vector<Particula *> particulas;
    Particula *particula = new Particula_pos(1.0f, Vector2(10.0f, -10.0f), Vector2(.0f, -10.0f), .0f);
    Particula *piso = new Particula_pos();
    Particula *pared = new Particula_pos();

    particulas.emplace_back(particula);
    particulas.emplace_back(piso);
    particulas.emplace_back(pared);

    Sistema sistema(particulas);

    Vector2 dir_arriba(.0f, 1.0f), dir_abajo(.0f, -1.0f), dir_derecha(1.0f, .0f), dir_izquierda(-1.0f, .0f);
    sistema.agregar_interaccion(particula, pared, dir_derecha);
    sistema.agregar_interaccion(pared, particula, dir_izquierda);
    sistema.agregar_interaccion(particula, piso, dir_abajo);
    sistema.agregar_interaccion(piso, particula, dir_arriba);

    sistema.expandir_interacciones();
    for (Particula *p : particulas)
        ((Particula_pos *)p)->actualizar(1.0f);

    ASSERT_EQ(particula->m_velocidad, Vector2());

    for (Particula *p : particulas)
        delete p;
}
