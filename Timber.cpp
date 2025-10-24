// PROGRAMA --> Videojoc SFML
// AUTOR --> Pablo Abad Ortega
// DATA --> 17/10/2025

#include <SFML/Graphics.hpp> // Inclou la funcionalitat de gràfics de SFML
#include <SFML/Audio.hpp> // Inclou la funcionalitat d'àudio i so de SFML
#include <sstream> // Inclou la llibreria per a manipulació de streams de strings (cadenes)
#include <ctime> // Inclou la llibreria per a gestionar temps i data
#include <iostream> // Inclou la llibreria per entrada i sortida estàndard (cout i cin)
using namespace sf; // Utilitza l'espai de noms 'sf' per no haver d'escriure "sf::" sempre


// ENUMERACIONS DE LES CONSTANTS GLOBALS

const int NUM_CLOUDS = 3; // numero de nuvols
const float timeBarStartWidth = 400; // longitud de la barra de temps
const float timeBarHeight = 80; // alçada de la barra de temps
const int NUM_BRANCHES = 6; // numero de branques de l'arbre
enum class Side {LEFT, RIGHT, NONE}; // posicio del jugador i la branca
const float AXE_POSITION_LEFT = 700; //posicio de la destral a l'esquerra
const float AXE_POSITION_RIGHT = 1075; // posicio de la destral a la dreta


// ESTRUCTURA NPC (ABELLA I NÚVOLS)

struct NPC // estructura base del NPC
{
    Sprite sprite; // grafic del persontgae
    bool active; // esta a la pantalla si o no?
    float speed; // velocitat de moviment 
    int maxHeight; // alcada maxima que pot tenir de la Y
    int maxSpeed; // volcitat maxima 
    int sentit; // direccio del moviment
    
    float posicioInicialX; // posicio X quant s'actualitzi

    NPC(Texture& texture, int maxHeight_, int maxSpeed_, int sentit_, float posicioInicialX_) // Constructor NPC
        :sprite(texture), active(false), speed(0), // Inicialització de membres
        maxHeight(maxHeight_), maxSpeed(maxSpeed_), // Assignació de límits
        sentit(sentit_), posicioInicialX(posicioInicialX_) // Assignació de direcció
    {
    }
};


// PROTOTIPS DE LES FUNCIONS

void updateNPC(NPC&, float); // actualitza la posicio del NPC
void updateBranchSprites(Side[], Sprite[]); // dibuixa les branques
void updateBranches(Side branchPositions[], int seed); // mou les branques

// FUNCIÓ PRINCIPAL DEL JOC

int main()
    {
        // CREAR ELS ELEMENTS VISUALS DEL JOC
        
        VideoMode vm({1920, 1080}); // creo un objecte que es diu "VideoMode" amb una resolució de 1920x1080
        RenderWindow window(vm, "Timber!!!", State::Fullscreen); // crea la finestra principal, utilitzant el mode de vídeo i un títol en pantalla completa
        Texture textureBackground("graphics/background.png"); // carrego la imatge de fons des d'un fitxer
        Sprite spriteBackground(textureBackground); // creo un sprite per el fons utilitzant la textura carregada
        Texture texturePlayer("graphics/player.png"); // carrego la imatge del jugador des d'un fitxer a una textura
        Sprite spritePlayer(texturePlayer); // creo un esprite per al jugador utilitzant 
        spritePlayer.setPosition({580, 720}); // poso la posició inicial del esprite del jugador 
        Texture textureTree1("graphics/tree.png"); // carrego la imatge del primer arbre des d'un fitxer 
        Sprite spriteTree1(textureTree1); // creo un esprite per al primer arbre 
        spriteTree1.setPosition({810, 0}); // poso la posició inicial de l'esprite del primer arbre
        Texture textureRIP("graphics/rip.png"); // carrego la imatge del primer arbre des d'un fitxer 
        Sprite spriteRIP(textureRIP); // creo un esprite per al primer arbre 
        Texture textureLOG("graphics/log.png"); // carrego la imatge del primer arbre des d'un fitxer 
        Sprite spriteLOG(textureLOG); // creo un esprite per al primer arbre 

        
        // ENTITATS MÒBILS (ABELLA + NÚVOLS)
        
        Texture textureBee("graphics/bee.png"); // carrega la textura de la abella
        Texture textureCloud("graphics/cloud.png"); // carrega la textura dels nubols
        NPC bee(textureBee, 500, 400, -1, 2000); // crea l'objecte de la abella

        NPC clouds[NUM_CLOUDS] = // array per els nuvols
        {
        NPC(textureCloud, 100, 200, 1, -200), // Nubol 1
        NPC(textureCloud, 250, 200, 1, -200), // Nubol 2
        NPC(textureCloud, 500, 200, 1, -200) // Nubol 3
        };


        // TEXTOS I MARCADOR
        
        Font font("fonts/KOMIKAP_.ttf"); // carrega la font del text
        Text messageText(font); // crea el text per el missatge
        messageText.setString("Clica ENTER per començar!"); // Defineix el missatge
        messageText.setCharacterSize(75); // Mida del text
        messageText.setFillColor(Color::White); // Color blanc
        FloatRect textRect = messageText.getLocalBounds(); // Obté les vores
        messageText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
             textRect.position.y + textRect.size.y / 2.0f}); // Estableix origen al centre
        messageText.setPosition({1920 / 2, 1080 / 2}); // Posiciona al centre
        Text scoreText(font); // Crea text puntuació
        scoreText.setCharacterSize(100); // Mida de la puntuació
        scoreText.setString("Score= 0"); // Puntuació inicial
        scoreText.setFillColor(Color::White); // Color blanc
        scoreText.setPosition({20, 20}); // Posició superior esquerra


        // VARIABLES DEL JOC
        
        bool paused = true; // esta el joc en pausa?
        Clock clock; // variable per el rellotge del joc
        int score = 0; // puntuacio del jugador
        float timeRemaining = 6.0f; // temps de joc que queda
        RectangleShape timeBar(Vector2f(timeBarStartWidth, timeBarHeight)); // Creo la barra de temps
        timeBar.setFillColor(Color::Blue); // Color de la barra
        timeBar.setPosition({1920 / 2 - timeBarStartWidth / 2, 980}); // Posició barra
        float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining; // Factor de reducció
        bool logActive = false; // Tronc en moviment?
        float logSpeedX = 1000; // Velocitat X del tronc
        float logSpeedY = -1500; // Velocitat Y del tronc


        // CONFIGURAR BRANQUES
        
        Texture textureBranch("graphics/branch.png"); // Carrega textura branca
        Side branchPositions[NUM_BRANCHES]; // Array de posicions
        Sprite branches[NUM_BRANCHES] = // Array d'sprites
        {
            Sprite(textureBranch), Sprite(textureBranch), Sprite(textureBranch),
            Sprite(textureBranch), Sprite(textureBranch), Sprite(textureBranch) // Creació dels sprites
        };
        for (int i = 0; i < NUM_BRANCHES; i++) // Bucle inicialització
        {
            branches[i].setTexture(textureBranch); // Assigna la textura
            branches[i].setPosition({-2000, -2000}); // Posiciona fora pantalla
            // establir l'origen del esprite al centre
            // I el podem girar sense canviar la seva posició
            branches[i].setOrigin({220, 20}); // Defineix el punt d'origen
            branchPositions[i] = Side::LEFT; // Posició inicial (esquerra)
        }


        // BUCLE PRINCIPAL PER EL JOC
       
        while (window.isOpen()) // Bucle principal joc
        {

            // GESTIÓ DELS EVENTS

            while (const std::optional event = window.pollEvent()) // Processa tots events
            {
                if (event->is<Event::Closed>()) window.close(); // Tanca la finestra
                if (const auto* key = event->getIf<Event::KeyPressed>()) // Detecció de tecla
                {
                    if (key->scancode == Keyboard::Scancode::Escape) // Tecla ESC
                        window.close(); // Tanca la finestra
                    if (key->scancode == Keyboard::Scancode::Enter) // Tecla ENTER
                    {
                        paused = false; // Inicia el joc
                        score = 0; // Reinicia puntuació
                        timeRemaining = 6; // Reinicia el temps

                        // Gestionar la configuració d'un joc nou

                        for (int i = 0; i < NUM_BRANCHES; i++) // Bucle reinici branques
                            branchPositions[i] = Side::NONE; // Neteja les branques
                        spriteRIP.setPosition({675, 2000}); // Amaga sprite RIP
                        spritePlayer.setPosition({675, 660}); // Posició inicial jugador
                    }

                    // Detecció del tall del jugador

                    if (key->scancode == Keyboard::Scancode::Left) // Tecla esquerra
                    {

                        // Assegurarse de que el jugador estigui a la dreta

                        playerSIDE = Side::RIGHT; // Jugador dreta
                        score++; // Incrementa puntuació
                        // afegir a la quantitat de temps restant
                        timeRemaining += (2 / score) + .15; // Afegeix temps
                        spriteAXE.setPosition({AXE_POSITION_RIGHT,spriteAXE.getPosition().y}); // Mou destral
                        spritePlayer.setPosition({1200, 720}); // Mou jugador dreta
                        updateBranches(branchPositions, score); // Mou les branques

                        // fer volar el tronc cap a l'esquerra

                        spriteLOG.setPosition({810, 720}); // Posició inicial tronc
                        logSpeedX = -5000; // Velocitat tronc esquerra
                        logActive = true; // Activa el tronc
                    }
                }
            }


            // ACTUALITZACIÓ DEL JOC
           
            if (!paused) // Si el joc no està en pausa
            {
                Time dt = clock.restart(); // Mesura el Delta Time
                timeRemaining -= dt.asSeconds(); // Redueix temps restant
                timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, 80)); // Ajusta barra de temps
                if (timeRemaining <= 0) // El temps s'ha acabat
                {
                    paused = true; // Pausa el joc
                    messageText.setString("FORA DE TEMPS!!"); // Missatge de fi de temps
                    textRect = messageText.getLocalBounds(); // Recalcula vores
                    messageText.setOrigin({(textRect.position.x + (textRect.size.x / 2.0f)),
                        (textRect.position.y + (textRect.size.y / 2.0f))}); // Centra missatge
                }

                // ENTITATS MOBILS

                updateNPC(clouds[0], dt.asSeconds()); // Actualitza núvol 1
                updateNPC(clouds[1], dt.asSeconds()); // Actualitza núvol 2
                updateNPC(clouds[2], dt.asSeconds()); // Actualitza núvol 3
                updateNPC(bee, dt.asSeconds()); // Actualitza l'abella
                updateBranchSprites(branchPositions, branches); // Posa les branques

                // gestionar la mort del jugador

                if (branchPositions[5] == playerSIDE) // Colisiona amb branca
                {
                    paused = true; // Pausa el joc (mort)
                    spriteRIP.setPosition({525, 760}); // Mostra sprite RIP
                    spritePlayer.setPosition({2000, 660}); // Amaga el jugador
                    messageText.setString("SQUISHED!"); // Missatge de mort
                    textRect = messageText.getLocalBounds(); // Recalcula les vores de la pantalla
                    messageText.setOrigin({(textRect.position.x + (textRect.size.x / 2.0f)),
                         (textRect.position.y + (textRect.size.y / 2.0f))}); // Centra el missatge
                }
                std::stringstream ss; // Crea stream de string
                ss << "Score = " << score; // Prepara el text per la puntuació
                scoreText.setString(ss.str()); // reinicia el text 
            }


            // DIBUIX

            window.clear(); // Neteja la finestra
            window.draw(spriteBackground); // Dibuixa el fons
            for (int i = 0; i < NUM_CLOUDS; i++) // Bucle dels núvols
            {
                window.draw(clouds[i].sprite); // Dibuixa cada núvol
            }
            window.draw(spriteTree1); // Dibuixa l'arbre
            window.draw(spritePlayer); // Dibuixa el jugador
            window.draw(bee.sprite); // Dibuixa l'abella
            window.draw(scoreText); // Dibuixa la puntuació
            window.draw(timeBar); // Dibuixa la barra de temps
            if (paused) window.draw(messageText); // Dibuixa missatge de pausa
            window.display(); // ensenya el frame
        } 
        return 0; // s'acaba el programa
    
};

void updateNPC(NPC& npc, float dt) // actualitza la lògica del NPC
{
    if (!npc.active) // Si està inactiu
    {
        npc.speed = (rand() % npc.maxSpeed) * npc.sentit; // defineix la nova velocitat
        float height = rand() % npc.maxHeight; // genera una alçada aleatòria
        npc.sprite.setPosition({ npc.posicioInicialX, height }); // Posició per el reinici
        npc.active = true; // Activa el moviment
    }
    else // Si està actiu (moviment)
    {
        npc.sprite.setPosition( // Mou lesprite
            {
        npc.sprite.getPosition().x + npc.speed * dt, // Moviment de la X 
        npc.sprite.getPosition().y // Manté posició de la Y
            });
        if (npc.sprite.getPosition().x < -200 || // Fora de pantalla esquerra
            npc.sprite.getPosition().x > 2000) // Fora de pantalla dreta
            npc.active = false; // deixa inactiu el NPC
    }
}

void updateBranchSprites(Side branchPositions[], Sprite branches[]) // actualitza els esprites de les branques
{
    for (int i = 0; i < NUM_BRANCHES; i++) // Bucle sobre les branques
    {
        float height = i * 150; // Calcula alçada de Y
        if (branchPositions[i] == Side::LEFT) // si la branca esta a la esquerra
        {
            branches[i].setPosition({610, height}); // es posiciona a l'esquerra
            branches[i].setRotation(degrees(180)); // es girarà 180 graus
        }
        else if (branchPositions[i] == Side::RIGHT) // si la branca esta a la dreta
        {
            branches[i].setPosition({1330, height}); // es posiciona a la dreta
            branches[i].setRotation(degrees(0)); // no té rotació
        }
        else // Si no hi ha cap branca 
        {
            branches[i].setPosition({3000, height}); // s'amaga fora de la pantalla
        }
    }
}
void updateBranches(Side branchPositions[], int seed) // actualitza el model de les branques
{
    for (int j = NUM_BRANCHES - 1; j > 0; j--) // desplaça l'array
        branchPositions[j] = branchPositions[j - 1]; // branca s'avança
    srand((int)time(0) + seed); // nova llavor aleatòria
    int r = rand() % 5; // genero un numero aleatori entre (0-4)
    switch (r) { // determino la nova branca
    case 0:
        branchPositions[0] = Side::LEFT; // branca de la esquerra
        break;
    case 1:
        branchPositions[0] = Side::RIGHT; // branca de la dreta
        break;
    default:
        branchPositions[0] = Side::NONE; // cap branca (comú)
        break;
    }
}