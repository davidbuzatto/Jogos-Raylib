//
// Created by David on 06/06/2024.
//
#include <stdlib.h>

#include "include/Antifinha.h"
#include "raylib.h"

Color coresSujeira[] = {
    { 134, 55, 0, 255 },
    { 145, 50, 2, 255 },
    { 177, 64, 5, 255 },
    { 211, 87, 38, 255 }
};

Antifinha *criarAntifinha() {

    Antifinha *antifinha = (Antifinha*) malloc( sizeof( Antifinha ) );
    antifinha->pos = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 };
    antifinha->vel = (Vector2){ 300, 300 };
    antifinha->velPulo = (Vector2){ 0, -800 };
    antifinha->atrito = 0.99f;
    antifinha->elasticidade = 0.9f;
    antifinha->textura = LoadTexture( "resources/images/antifinha30.png" );

    antifinha->quantidadeSujeira = 0;
    antifinha->quantidadeMaximaSujeira = 200;
    antifinha->sujeira = (Sujeira*) malloc( sizeof( Sujeira ) * antifinha->quantidadeMaximaSujeira );

    antifinha->emArraste = false;
    antifinha->pressOffset = (Vector2){ 0 };
    antifinha->posicaoAnterior = (Vector2){ 0 };

    return antifinha;

}

void destruirAntifinha( Antifinha *antifinha ) {
    UnloadTexture( antifinha->textura );
    free( antifinha->sujeira );
    free( antifinha );
}

void desenharAntifinha( Antifinha *antifinha ) {
    
    for ( int i = 0; i < antifinha->quantidadeSujeira; i++ ) {
        desenharSujeira( &antifinha->sujeira[i%antifinha->quantidadeMaximaSujeira] );
    }

    DrawTextureV( antifinha->textura, antifinha->pos, WHITE );

}

void atualizarAntifinha( Antifinha *antifinha, Vector2 *gravidade ) {

    if ( !antifinha->emArraste ) {

        float delta = GetFrameTime();

        antifinha->pos.x += antifinha->vel.x * delta;
        antifinha->pos.y += antifinha->vel.y * delta;

        if ( antifinha->pos.x + antifinha->textura.width > GetScreenWidth() ) {
            antifinha->pos.x = GetScreenWidth() - antifinha->textura.width;
            antifinha->vel.x = -antifinha->vel.x * antifinha->elasticidade;
            criarSujeiraAntifinha( 
                antifinha, 
                (Vector2) { 
                    antifinha->pos.x + antifinha->textura.width, 
                    antifinha->pos.y + antifinha->textura.height / 2
                },
                (Vector2) { 20, antifinha->textura.height * 0.5f }
            );
        } else if ( antifinha->pos.x < 0 ) {
            antifinha->pos.x = 0;
            antifinha->vel.x = -antifinha->vel.x * antifinha->elasticidade;
            criarSujeiraAntifinha( 
                antifinha, 
                (Vector2) { 
                    antifinha->pos.x, 
                    antifinha->pos.y + antifinha->textura.height / 2
                },
                (Vector2) { 20, antifinha->textura.height * 0.5f }
            );
        }

        if ( antifinha->pos.y + antifinha->textura.height > GetScreenHeight() ) {
            antifinha->pos.y = GetScreenHeight() - antifinha->textura.height;
            antifinha->vel.y = -antifinha->vel.y * antifinha->elasticidade;
            criarSujeiraAntifinha( 
                antifinha, 
                (Vector2) { 
                    antifinha->pos.x + antifinha->textura.width / 2, 
                    antifinha->pos.y + antifinha->textura.height
                },
                (Vector2) { antifinha->textura.width * 0.5f, 20 }
            );
        } else if ( antifinha->pos.y < 0 ) {
            antifinha->pos.y = 0;
            antifinha->vel.y = -antifinha->vel.y * antifinha->elasticidade;
            criarSujeiraAntifinha( 
                antifinha, 
                (Vector2) { 
                    antifinha->pos.x + antifinha->textura.width / 2, 
                    antifinha->pos.y
                },
                (Vector2) { antifinha->textura.width * 0.5f, 20 }
            );
        }

        antifinha->vel.x = antifinha->vel.x * antifinha->atrito;
        antifinha->vel.y = antifinha->vel.y * antifinha->atrito + gravidade->y;

    } else {

        Vector2 mousePos = GetMousePosition();
        antifinha->pos.x = mousePos.x - antifinha->pressOffset.x;
        antifinha->pos.y = mousePos.y - antifinha->pressOffset.y;

        antifinha->vel.x = ( antifinha->pos.x - antifinha->posicaoAnterior.x ) * 100;
        antifinha->vel.y = ( antifinha->pos.y - antifinha->posicaoAnterior.y ) * 100;

        antifinha->posicaoAnterior = antifinha->pos;

    }

}

void criarSujeiraAntifinha( Antifinha *antifinha, Vector2 pos, Vector2 dim ) {

    antifinha->sujeira[antifinha->quantidadeSujeira%antifinha->quantidadeMaximaSujeira] = (Sujeira){
        .pos = pos,
        .dim = dim,
        .cor = coresSujeira[GetRandomValue(0, 3)]
    };
    antifinha->quantidadeSujeira++;

}

bool interceptaAntifinha( Antifinha *antifinha, Vector2 pos ) {
    return CheckCollisionPointCircle(
        pos,
        (Vector2){ 
            antifinha->pos.x + antifinha->textura.width / 2, 
            antifinha->pos.y + antifinha->textura.height / 2
        }, 
        antifinha->textura.width > antifinha->textura.height ? antifinha->textura.height : antifinha->textura.width );
}

void pularAntifinha( Antifinha *antifinha ) {
    antifinha->vel.y = antifinha->velPulo.y;
}