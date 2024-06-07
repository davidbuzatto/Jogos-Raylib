//
// Created by David on 06/06/2024.
//
#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "Sujeira.h"

typedef struct Antifinha {
    
    Vector2 pos;
    Vector2 vel;
    Vector2 velPulo;
    float atrito;
    float elasticidade;
    Texture2D textura;

    int quantidadeSujeira;
    int quantidadeMaximaSujeira;
    Sujeira *sujeira;

    bool emArraste;
    Vector2 pressOffset;
    Vector2 posicaoAnterior;

} Antifinha;

Antifinha *criarAntifinha();
void destruirAntifinha( Antifinha *antifinha );
void desenharAntifinha( Antifinha *antifinha );
void atualizarAntifinha( Antifinha *antifinha, Vector2 *gravidade );
void criarSujeiraAntifinha( Antifinha *antifinha, Vector2 pos, Vector2 dim );
bool interceptaAntifinha( Antifinha *antifinha, Vector2 pos );
void pularAntifinha( Antifinha *antifinha );
