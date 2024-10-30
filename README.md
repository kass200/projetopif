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

O jogo segue a estrutura básica do Pac-Man, com o jogador controlando um personagem que se move em um labirinto, devorando pontos e escapando dos fantasmas. Este projeto visa explorar a linguagem Haskell, abordando tópicos como:

- **Movimentação do personagem** no labirinto
- **Interatividade** entre o jogador e os fantasmas
- **Pontuação** e progresso no jogo
- **Renderização** básica em Haskell

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
