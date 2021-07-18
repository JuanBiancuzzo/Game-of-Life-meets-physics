#include "circulo.h"

using namespace cr;

Circulo::Circulo(Vector2 posicion, float radio)
    : CuerpoRigido(posicion), m_radio(radio)
{
}

Circulo::Circulo(float masa, Vector2 posicion, float rotacion, float radio)
    : CuerpoRigido(masa, posicion, rotacion), m_radio(radio)
{
}

Vector2 Circulo::punto_soporte(Vector2 dir)
{
    return m_posicion + dir.normal() * m_radio;
}

CuerpoRigido *Circulo::copia(Vector2 posicion)
{
    return new Circulo(m_masa, m_posicion + posicion, m_rotacion, m_radio);
}

float Circulo::calcular_inercia()
{
    return .5f * m_masa * m_radio * m_radio;
}