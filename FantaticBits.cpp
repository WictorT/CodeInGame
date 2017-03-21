#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

#define SIGN(a, b) ((a > b) ? 1 : -1) 

const double WIZARDS_FRICTION = 0.75;
const double BLUDGERS_FRICTION = 0.9;
const double SNAFFLES_FRICTION = 0.75;
const int MAX_TURNS = 200;
const int POLE_RADIUS = 300;
const int WIZZARD_RADIUS = 400;
const int DISTANCE_BETWEEN_POLES = 4000;
const int TURNS_TO_PREDICT = 3;
const int MAX_X = 16000;
const int MAX_Y = 7500;
const int WIZZARDS_PER_TEAM = 2;
const int BLUDGER_COUNT = 2;
const int SPELL_COUNT = 4;
const int TYPES_COUNT = 4;
const int MAX_SHUFFLE_COUNT = 20;
const int MAX_ENTITY_COUNT = 30;
const int MAX_MAGIC_POINTS = 100;
const int FLIPENDO_ERROR = 900;
const int FLIPENDO_ALLOWED_SPEED = 1500;
const int TEAM_X_QUOTIEN[2] = {1, -1};
const int POLE_X[2][2] = {{1 ,1}, {MAX_X ,MAX_X}}; //[team id][pole]
const int POLE_Y[2][2] = {{1750 , 5250}, {1750 , 5250}}; //[team id][pole]
const int NUMBER_OF_TEAMS = 2;
const string ENTITY_TYPES[4] = {"WIZARD", "OPPONENT_WIZARD", "SNAFFLE", "BLUDGER"};
const int ENTITY_RADIUS[4] = {};
const string SPELL_TYPES[4] = {"OBLIVIATE", "PETRIFICUS", "ACCIO", "FLIPENDO"};

class Entity
{
public:
    int id; // entity identifier    
    int idId; // array identifier
    string type; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
    int typeId;
    int x; // position
    int y; // position
    int vx; // velocity
    int vy; // velocity
    int state; // 1 if the wizard is holding a Snaffle, 0 otherwise

    Entity(){};
    Entity(int id, int idId, string type, int typeId, int x, int y, int vx, int vy, int state) {};

    double DistTo(Entity e);
};

class Snuffle: public Entity
{
public:
    bool isTaken;

    Snuffle(): Entity(){};    
    Snuffle(int id, int idId, string type, int typeId, int x, int y, int vx, int vy, int state, bool isTaken):
        Entity(id, idId, type, typeId, x, y, vx, vy, state){};
};

class Wizzard: public Entity
{
public:
    int targetSnaffleIdId;
    double distToClosestBall;
    double turnsToClosestBall;

    Wizzard(): Entity(){};
    Wizzard(int id, int idId, string type, int typeId, int x, int y, int vx, int vy, int state):
        Entity(id, idId, type, typeId, x, y, vx, vy, state){};
    
};

class Bludger: public Entity
{
public:

    Bludger(): Entity(){};
    Bludger(int id, int idId, string type, int typeId, int x, int y, int vx, int vy, int state):
        Entity(id, idId, type, typeId, x, y, vx, vy, state){};
};

class Spell
{
public:
    string type;
    int typeId;
    int cost;
    int duration;

    Spell() {};
    Spell(string _type, int _typeId, int _cost, int _duration);

    void SpellIt(int id);
};

std::vector<Entity> entity(MAX_ENTITY_COUNT, Entity());

vector< vector<Snuffle> > snuffle(MAX_TURNS, vector<Snuffle>(MAX_SHUFFLE_COUNT)); // [turn_number][snuffle]
vector< vector< vector<Wizzard> > > wizzard(MAX_TURNS, vector< vector<Wizzard> > (NUMBER_OF_TEAMS, vector<Wizzard> (WIZZARDS_PER_TEAM))); //[team_id][wizzard]
vector< vector<Bludger> > bludger(MAX_TURNS, vector<Bludger>(BLUDGER_COUNT)); // [turn_number][bludger]
vector<Spell> spell(SPELL_COUNT, Spell());

int thisTurn;
int myTeamId;
int enemyTeamId;

int entitiesCount;
int snufflesCount;
int bludgersCount;
int wizzardsCount[2];
int magicPoints[2];
int yOffset[4];

double Entity::DistTo(Entity e)
{
    return pow(pow(x - e.x, 2) + pow(y - e.y, 2), 0.5);
};

Spell::Spell(string _type, int _typeId, int _cost, int _duration)
{
    type = _type;
    typeId = _typeId;
    cost = _cost;
    duration = _duration;
}

void Spell::SpellIt(int id)
{
    cout << type << " " << id << endl;
    magicPoints[myTeamId] -= cost;
}

void Init();
void ReadTurnData();
void ChooseBestSnuffles();
bool SpellFlippendo(Wizzard wizzard);
bool PetrifyFlippendedSnuffle(Wizzard wizzard);
int  ClosestToInterval(int x, double a, double b);
int  GetEntityTypeByName(string type);
bool SpellAccio(Wizzard wizzard);
void PredictSnufflesUntil(int untilTurn);
void SetOffSet();

int main()
{
    Init();
    // game loop
    while (1) {
        // turn Initialization

        ReadTurnData();
        ChooseBestSnuffles();
        PredictSnufflesUntil(thisTurn + 15);
        SetOffSet();

        for (int i = 0; i < 2; i++) {
            if (wizzard[thisTurn][myTeamId][i].state == 0)
            {          

                // if (magicPoints[myTeamId] >= spell[3].cost && SpellFlippendo(wizzard[thisTurn][myTeamId][i]))
                // {
                // }      
                // else if (magicPoints[myTeamId] >= spell[2].cost && SpellAccio(wizzard[thisTurn][myTeamId][i]))
                // {
                // }   
                // // else if (magicPoints[myTeamId] >= spell[0].cost && thisTurn < 10)
                // // {
                // //      spell[0].SpellIt(bludger[thisTurn][myTeamId].id);
                // // } 
                // else if (magicPoints[myTeamId] >= spell[1].cost && PetrifyFlippendedSnuffle(wizzard[thisTurn][myTeamId][i]))
                // {
                // }
                // else 
                // {                
                cout << "MOVE " 
                    << (int)(snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].x + 
                        snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].vx +
                        snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].vx * 0.75f - 
                        wizzard[thisTurn][myTeamId][i].vx) << " "
                    << (int)(snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].y + 
                        snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].vy +
                        snuffle[thisTurn][wizzard[thisTurn][myTeamId][i].targetSnaffleIdId].vy * 0.75f - 
                        wizzard[thisTurn][myTeamId][i].vy) // yOffset[wizzard[thisTurn][myTeamId][i].id] 
                        << " "
                    << "150" << endl;
                // }
            }
            else if (wizzard[thisTurn][myTeamId][i].state == 1)
            {                
                cout << "THROW " 
                    << POLE_X[enemyTeamId][0] << " "
                    << ClosestToInterval(wizzard[thisTurn][myTeamId][i].y, POLE_Y[enemyTeamId][0] + POLE_RADIUS * 5.0f,
                                                                 POLE_Y[enemyTeamId][1] - POLE_RADIUS * 5.0f) << " "
                    << "500"
                << endl;
            }
        }

        magicPoints[myTeamId] = min(magicPoints[myTeamId] + 1, MAX_MAGIC_POINTS);
        magicPoints[enemyTeamId] = min(magicPoints[myTeamId] + 1, MAX_MAGIC_POINTS);
        cerr << "Energy: " << magicPoints[myTeamId] << endl;
        thisTurn++;        
    }
}

void SetOffSet()
{    
    // if (thisTurn > 10){
    //     yOffset[0] = 0; yOffset[1] = 0; yOffset[2] = 0; yOffset[3] = 0; 
    // }
    // else{
    //     yOffset[0] = (MAX_Y - snuffle[thisTurn][wizzard[thisTurn][0][0].targetSnaffleIdId].y) / 9;
    //     yOffset[1] = (snuffle[thisTurn][wizzard[thisTurn][0][1].targetSnaffleIdId].y) / 9;
    //     yOffset[2] = (snuffle[thisTurn][wizzard[thisTurn][1][0].targetSnaffleIdId].y) / 9;
    //     yOffset[3] = (MAX_Y - snuffle[thisTurn][wizzard[thisTurn][1][1].targetSnaffleIdId].y) / 9;
    // }
}

bool SpellAccio(Wizzard wizzard)
{
    if (snufflesCount < 3 && ((snuffle[thisTurn][0].x < wizzard.x && myTeamId == 0) || (snuffle[thisTurn][0].x > wizzard.x && myTeamId == 1))) 
    {
        spell[2].SpellIt(snuffle[thisTurn][wizzard.targetSnaffleIdId].id);
        return true;
    }
    return false;
}

int GetEntityTypeByName(string type)
{
    for (int i = 0; i < TYPES_COUNT; i++)
    {
        if (ENTITY_TYPES[i] == type) 
            return i;
    }
}

bool PetrifyFlippendedSnuffle(Wizzard wizzard)
{
    for (int snuffleI = 0; snuffleI < snufflesCount; snuffleI++)
    {
        if (snuffle[thisTurn][snuffleI].vx * TEAM_X_QUOTIEN[enemyTeamId] > FLIPENDO_ALLOWED_SPEED)
        {
            spell[1].SpellIt(snuffle[thisTurn][snuffleI].id);

            return true;
        }
    }

    return false;
}

bool SpellFlippendo(Wizzard wizzard)
{
    for (int snuffleI = 0; snuffleI < snufflesCount; snuffleI++)
    {
        if ((snuffle[thisTurn][snuffleI].x > wizzard.x && myTeamId == 0) ||
            (snuffle[thisTurn][snuffleI].x < wizzard.x && myTeamId == 1))
        {
            double m = 1.0f * (wizzard.y - snuffle[thisTurn][snuffleI].y) / (wizzard.x - snuffle[thisTurn][snuffleI].x);
            double b = m * -wizzard.x + wizzard.y;
            double resultY = (m * POLE_X[enemyTeamId][0] + b);

            // cerr << m << " " << snuffle[thisTurn][snuffleI].id << " " << resultY << endl;            
            if (resultY < POLE_Y[enemyTeamId][1] - FLIPENDO_ERROR && 
                resultY > POLE_Y[enemyTeamId][0] + FLIPENDO_ERROR &&
                wizzard.DistTo(snuffle[thisTurn][snuffleI]) < 5000) 
            // if (resultY < POLE_Y[enemyTeamId][1] + (wizzard.vy * + snuffle[thisTurn][snuffleI].vy) * 3 && 
            //     resultY > POLE_Y[enemyTeamId][0] + (wizzard.vy * + snuffle[thisTurn][snuffleI].vy) * 3 &&
            //     wizzard.DistTo(snuffle[thisTurn][snuffleI]) < 5000) 
            {
                spell[3].SpellIt(snuffle[thisTurn][snuffleI].id);
                return true;
            }
            // else if ((resultY < 0 || resultY > MAX_Y) && 
            //             ((wizzard.x > MAX_X / 2 && myTeamId == 0) || (wizzard.x < MAX_X / 2 && myTeamId == 1))) 
            // {
            //     float wallX = -b / m;
            //     m = -m;
            //     b = m * -wallX;
            //     resultY = m * POLE_X[enemyTeamId][0] + b;
            //     if (resultY < POLE_Y[enemyTeamId][1] - FLIPENDO_ERROR && resultY > POLE_Y[enemyTeamId][0] + FLIPENDO_ERROR) 
            //     {
            //         spell[3].SpellIt(snuffle[thisTurn][snuffleI].id);
            //         return true;
            //     }
            // }
        }
    }
    return false;
}

int ClosestToInterval(int x, double a, double b)
{
    cerr << x << " " << a << " "  << b << endl;
    if (x <= a)
        return a;
    else if (x >= b)
        return b;
    else 
        return x;
}

void Init()
{
    cin >> myTeamId; cin.ignore();
    if (!myTeamId)
        enemyTeamId = 1;

    spell[0] = Spell(SPELL_TYPES[0], 0, 5, 3);
    spell[1] = Spell(SPELL_TYPES[1], 1, 10, 1);
    spell[2] = Spell(SPELL_TYPES[2], 2, 20, 6);
    spell[3] = Spell(SPELL_TYPES[3], 3, 20, 3);
}

void ReadTurnData()
{
    entitiesCount = 0;
    snufflesCount = 0;
    bludgersCount = 0;
    wizzardsCount[myTeamId] = 0;
    wizzardsCount[enemyTeamId] = 0;

    int a,b,c,d;
    cin >> a >> b >> c >> d;
    cin >> entitiesCount; cin.ignore() ;
    for (int i = 0; i < entitiesCount; i++) {
        int entityId; // entity identifier
        string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
        int x; // position
        int y; // position
        int vx; // velocity
        int vy; // velocity
        int state; // 1 if the wizard is holding a Snaffle, 0 otherwise

        cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();

            entity[i].id = entityId;
            entity[i].type = entityType;
            entity[i].typeId = GetEntityTypeByName(entityType);
            entity[i].x = x;
            entity[i].y = y;
            entity[i].vx = vx;
            entity[i].vy = vy;

        if (entityType == ENTITY_TYPES[0]) 
        {
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].id = entityId;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].type = entityType;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].typeId = 0;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].x = x;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].y = y;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].vx = vx;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].vy = vy;
            wizzard[thisTurn][myTeamId][wizzardsCount[myTeamId]].state = state;
            wizzardsCount[myTeamId]++;

        }             
        else if (entityType == ENTITY_TYPES[1]) 
        {
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].id = entityId;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].type = entityType;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].typeId = 1;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].x = x;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].y = y;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].vx = vx;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].vy = vy;
            wizzard[thisTurn][enemyTeamId][wizzardsCount[enemyTeamId]].state = state;
            wizzardsCount[enemyTeamId]++;
        }     
        else if (entityType == ENTITY_TYPES[2]) 
        {
            snuffle[thisTurn][snufflesCount].id = entityId;
            snuffle[thisTurn][snufflesCount].idId = snufflesCount;
            snuffle[thisTurn][snufflesCount].type = entityType;
            snuffle[thisTurn][snufflesCount].typeId = 2;
            snuffle[thisTurn][snufflesCount].x = x;
            snuffle[thisTurn][snufflesCount].y = y;
            snuffle[thisTurn][snufflesCount].vx = vx;
            snuffle[thisTurn][snufflesCount].vy = vy;
            snuffle[thisTurn][snufflesCount].state = state;
            snuffle[thisTurn][snufflesCount].isTaken = 0;
            snufflesCount++;
        }  
        else if (entityType == ENTITY_TYPES[3]) 
        {           
            bludger[thisTurn][bludgersCount].id = entityId;
            bludger[thisTurn][bludgersCount].type = entityType;
            bludger[thisTurn][bludgersCount].typeId = 3;
            bludger[thisTurn][bludgersCount].x = x;
            bludger[thisTurn][bludgersCount].y = y;
            bludger[thisTurn][bludgersCount].vx = vx;
            bludger[thisTurn][bludgersCount].vy = vy;
            bludger[thisTurn][bludgersCount].state = state;
            bludgersCount++;
        }
        cerr << entityId << " "
            << entityType << " "
            << x << " "
            << y << " "
            << vx << " "
            << vy << " "
            << state << endl;
    }
}

void ChooseBestSnuffles()
{
    for (int teamId = 0; teamId < NUMBER_OF_TEAMS; teamId++)
    {
        double shortestDistance = 99999.0f; // really?!!!
        if (snufflesCount == 1) 
        {            
            wizzard[thisTurn][teamId][0].targetSnaffleIdId = snuffle[thisTurn][0].idId;
            wizzard[thisTurn][teamId][1].targetSnaffleIdId = snuffle[thisTurn][0].idId;
        }
        else 
        {
            for (int snuffleI = 0; snuffleI < snufflesCount; snuffleI++)
            {
                for (int snuffleJ = 0; snuffleJ < snufflesCount; snuffleJ++)
                {
                    if (snuffleJ != snuffleI)
                    {
                        double d1 = wizzard[thisTurn][teamId][0].DistTo(snuffle[thisTurn][snuffleI]) /** (abs(snuffle[thisTurn][snuffleI].x - POLE_X[myTeamId][0] + MAX_X * 1.7f) / MAX_X)*/;
                        double d2 = wizzard[thisTurn][teamId][1].DistTo(snuffle[thisTurn][snuffleJ]) /** (abs(snuffle[thisTurn][snuffleJ].x - POLE_X[myTeamId][0] + MAX_X * 1.7f) / MAX_X)*/;
                        double distance = d1 + d2;

                        // cerr << wizzard[thisTurn][teamId][0].id << " " << snuffle[thisTurn][snuffleI].id << " " << distance << endl;            
                        // cerr << "    " << wizzard[thisTurn][teamId][1].id << " " << snuffle[thisTurn][snuffleJ].id << " " << distance << endl;

                        if (distance < shortestDistance)
                        {
                            shortestDistance = distance;
                            wizzard[thisTurn][teamId][0].targetSnaffleIdId = snuffle[thisTurn][snuffleI].idId;
                            wizzard[thisTurn][teamId][1].targetSnaffleIdId = snuffle[thisTurn][snuffleJ].idId;
                        }
                    }
                }
            }
        }
    }
}

void ChooseBestSnufflesByTurns()
{
    for (int teamId = 0; teamId < NUMBER_OF_TEAMS; teamId++)
    {
        double lesserTurns = 99999.0f; // really?!!!
        if (snufflesCount == 1) 
        {            
            wizzard[thisTurn][teamId][0].targetSnaffleIdId = snuffle[thisTurn][0].idId;
            wizzard[thisTurn][teamId][1].targetSnaffleIdId = snuffle[thisTurn][0].idId;
        }
        else 
        {
            for (int snuffleI = 0; snuffleI < snufflesCount; snuffleI++)
            {
                for (int snuffleJ = 0; snuffleJ < snufflesCount; snuffleJ++)
                {
                    if (snuffleJ != snuffleI || snufflesCount == 1)
                    {
                        float turnsToReach[2];
                        turnsToReach[0] = wizzard[thisTurn][teamId][0].DistTo(snuffle[thisTurn][snuffleI]) / 400 + 
                            wizzard[thisTurn][teamId][0].vx / 40 +
                            wizzard[thisTurn][teamId][0].vy / 40;
                        turnsToReach[1] = wizzard[thisTurn][teamId][1].DistTo(snuffle[thisTurn][snuffleI]) / 400 + 
                            wizzard[thisTurn][teamId][1].vx / 40 +
                            wizzard[thisTurn][teamId][1].vy / 40;
                        double totalTurnsToReach = turnsToReach[0] + turnsToReach[1];
                        if (totalTurnsToReach < lesserTurns)
                        {
                            lesserTurns = totalTurnsToReach;
                            wizzard[thisTurn][teamId][0].targetSnaffleIdId = snuffle[thisTurn][snuffleI].idId;
                            wizzard[thisTurn][teamId][1].targetSnaffleIdId = snuffle[thisTurn][snuffleJ].idId;
                            wizzard[thisTurn][teamId][0].turnsToClosestBall = turnsToReach[0];
                            wizzard[thisTurn][teamId][1].turnsToClosestBall = turnsToReach[1];
                        }
                    }
                }
            }
        }
    }
}

void PredictSnufflesUntil(int untilTurn)
{
    for (int turn = thisTurn + 1; turn < untilTurn; turn++)
    {
        for (int snuffleI = 0; snuffleI < snufflesCount; snuffleI++)
        {
            snuffle[turn][snuffleI].x = snuffle[turn - 1][snuffleI].x + snuffle[turn - 1][snuffleI].vx;
            snuffle[turn][snuffleI].y = snuffle[turn - 1][snuffleI].y + snuffle[turn - 1][snuffleI].vy;
            snuffle[turn][snuffleI].vx = snuffle[turn - 1][snuffleI].vx * SNAFFLES_FRICTION;
            snuffle[turn][snuffleI].vy = snuffle[turn - 1][snuffleI].vy * SNAFFLES_FRICTION;
        }
    }
}
