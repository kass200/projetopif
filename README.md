# Jogo Pac-Man em Haskell

Este projeto é desenvolvido por alunos Cesar School, e é pertencente a disciplina de PIF (Programação Imperativa e Funcional). Desenvolveremos uma recriação do clássico jogo **Pac-Man** com personalizações, desenvolvido em **Haskell**.

## Equipe

| Nome Completo           | GitHub                  |
|-------------------------|-------------------------|
| João de Medeiros Andrade| [joca-000](https://github.com/joca-000) |
| Karoline Santiago       | [kas200](https://github.com/kas200)     |
| Eduardo Américo         | [EduardoAmericoo](https://github.com/EduardoAmericoo) |
| Igor Soares             | [IgorSoaresss](https://github.com/IgorSoaresss) |

## Descrição do Projeto

Nosso jogo, o PASCIIMAN, segue a estrutura básica do Pac-Man, onde o jogador controla um personagem que se move em um labirinto, devorando pontos e escapando dos fantasmas. Você deve coletar todos os pontos espalhados pelo mapa e entrar por um portal para poder passar de nível.

A cada nível, o mapa vai ficando mais complexo, aumentando de tamanho e com lugares mais difíceis de acessar. Você encontrará também novas espécies de fantasmas, mas eles não são tão perigosos, então vai ficar tudo bem... Desde que você não encontre o- Na verdade, apenas ler seria decepcionante, então você terá que jogar e ver por conta própria. 

Este projeto visa explorar a linguagem Haskell, abordando tópicos como:

- **Movimentação do personagem** no labirinto
- **Interatividade** entre o jogador e os fantasmas
- **Pontuação** e progresso no jogo
- **Renderização** básica em Haskell

## Mecânicas de movimentação

Por enquanto, temos apenas a movimentação básica, ou seja, a locomoção.

W - Mover para cima  
A - Mover para esquerda  
S - Mover para baixo  
D - Mover para direita  

## Estrutura do Projeto

```plaintext
.
├── src
│   ├── Main.hs               # Arquivo principal do jogo
│   ├── GameLogic.hs          # Lógica central do jogo
│   ├── Player.hs             # Comportamento e estado do jogador
│   ├── Ghost.hs              # Comportamento dos fantasmas
│   └── Board.hs              # Configuração e renderização do labirinto
├── assets
│   ├── sprites               # Sprites e ícones
│   └── sounds                # Sons e música do jogo
├── README.md                 # Documentação do projeto
└── LICENSE                   # Licença do projeto


### Requisitos
- Haskell 
- Biblioteca SDL2
- Joystick (USB)
