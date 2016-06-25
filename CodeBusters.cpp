#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>   
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

const int minMarginX=1131 ;
const int maxMarginX=16001-1131 ;
const int minMarginY=1131 ;
const int maxMarginY=9001-1131 ;
const int marginX=maxMarginX-minMarginX ;
const int marginY=maxMarginY-minMarginY ;
const double diagonalAngle=atan(marginX/marginY)*57.2958 ;
const double PI=3.14159265 ;
 
int entityId[40]; // buster id or ghost id
int x[40];
int y[40]; // position of this buster / ghost
int entityType[40]; // the team id if it is a buster, -1 if it is a ghost.
int state[40]; // For busters: 0=idle, 1=carrying a ghost.
int value[40]; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.

int entities; // the number of busters and ghosts visible to you            
int bustersPerPlayer; // the amount of busters you control
int ghostCount; // the amount of ghosts on the map
int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int nearestBuster;
int homeX ;
int homeY ;
int opsiteTeamId ;


int gotoX[10] ;
int gotoY[10] ;
int Bust[5] ;
int BustId[5] ;
int BustDist[5] ;
int lastBust[5] ;
int StunAbility[5] ;
int Stun[5] ;
int StunId[5] ;
int StunDist[5] ;
int arrived[5] ;

double optimalAngle ;

 
int main()
{
    srand(time(NULL)) ; 
    cerr << diagonalAngle  << endl ; 
    cin >> bustersPerPlayer; cin.ignore();
    cin >> ghostCount; cin.ignore();
    cin >> myTeamId; cin.ignore();
    
    
    
    //GET DIRRECTIONS
    optimalAngle = 90/(bustersPerPlayer+1) ;
    if (myTeamId==0)
    {
        homeX=minMarginX ; homeY=minMarginY ; opsiteTeamId=1 ;
        gotoX[0]=minMarginX ; gotoY[0]=maxMarginY ; 
        gotoX[bustersPerPlayer-1]=maxMarginX ; gotoY[bustersPerPlayer-1]=minMarginY ; 
    for (int i=1 ; i<bustersPerPlayer-1 ; i++){
        double angle=(i+1)*optimalAngle ;
        if (angle<diagonalAngle){
            gotoX[i]=tan(angle * PI / 180.0)*marginY ;
            gotoY[i]=maxMarginY ;
        }
        else if (angle>diagonalAngle)
        {
            gotoX[i]=maxMarginX ;
            gotoY[i]=tan((90-angle) * PI / 180.0)*marginX ;
        }
        else if (angle==diagonalAngle)
        {
            gotoX[i]=maxMarginX ;
            gotoY[i]=maxMarginY ;
        }
        // cerr << gotoX[i] << " " << gotoY[i] << endl ;
    }
    }
    else if (myTeamId==1)
    {
        homeX=maxMarginX ; homeY=maxMarginY ; opsiteTeamId=0 ;
        gotoX[0]=maxMarginX ; gotoY[0]=minMarginY ; 
        gotoX[bustersPerPlayer-1]=minMarginX ; gotoY[bustersPerPlayer-1]=maxMarginY ; 
    for (int i=1 ; i<bustersPerPlayer-1 ; i++){
        double angle=(i+1)*optimalAngle ;
        if (angle<diagonalAngle){
            gotoX[i]=maxMarginX-tan((angle) * PI / 180.0)*marginY ;
            gotoY[i]=minMarginY ;
        }
        else if (angle>diagonalAngle)
        {
            gotoX[i]=minMarginX ;
            gotoY[i]=tan((angle) * PI / 180.0)*marginX ;
        }
        else if (angle==diagonalAngle)
        {
            gotoX[i]=minMarginX ;
            gotoY[i]=minMarginY ;
        }
        // cerr << gotoX[i] << " " << gotoY[i] << endl ;
    }
    }
    
    
    
    // game loop
    while (1) {
        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++) {
            cin >> entityId[i] >> x[i] >> y[i] >> entityType[i] >> state[i] >> value[i]; cin.ignore();
            cerr << entityId[i] << " " << x[i] << " " << y[i] << " " << entityType[i] << " " << state[i] << " " << value[i] << " " << endl ;            
        }
        
        
        //ASSIGNING SHIT
        for (int i = 0 ; i<5 ; i++){
            Bust[i]=-1 ; 
            Stun[i]=-1 ;
            if (StunAbility[i]>0)
            {
                StunAbility[i]-- ; 
            }
            if (arrived[i]) 
            {
                if (myTeamId==0)
                {
                    gotoX[i] = rand()%6000+8000 ;
                    gotoY[i] = rand()%5000+1000 ;
                }
                else if (myTeamId==1)
                {
                    gotoX[i] = rand()%5000+1000 ;
                    gotoY[i] = rand()%6000+2000 ;
                }
                arrived[i] = 0 ;
            }
            
        }
        
        //PATH FIND
        for (int i=bustersPerPlayer ; i<entities ; i++)
        {
            if (entityType[i]==-1)
            {
                int min=99999 ;
                int minId ;
                int id=i ;
                for (int i=0 ; i<bustersPerPlayer ; i++) 
                    {
                        if (state[i]==0){
                            int dist = sqrt(pow(x[id]-x[i],2)+pow(y[id]-y[i],2)) ;
                            if (min>dist)
                            {
                                min = dist ; minId = i ;
                            }
                        }
                    }
                Bust[minId]=entityId[i] ;
                BustId[minId]=i ;
                BustDist[minId]=min ; 
            } 
            else if (entityType[i]==opsiteTeamId)
            {
                int min=99999 ;
                int minId ;
                int id=i ;
                for (int i=0 ; i<bustersPerPlayer ; i++) 
                    {
                        if (StunAbility[i]==0){
                            int dist = sqrt(pow(x[id]-x[i],2)+pow(y[id]-y[i],2)) ;
                            if (min>dist)
                            {
                                min = dist ; minId = i ;
                            }
                        }
                    }
                if ((min<1760) && (StunAbility[i]==0))
                    Stun[minId]=entityId[i] ;
                    StunId[minId]=i ;
                    StunDist[minId]=min ; 
            }
            // cerr << Bust[nearestBuster] << " " << endl ;
        }
        
        //OUTPUT
        for (int i = 0; i < bustersPerPlayer; i++) {
            
            
            if (state[i]==0) 
            {
                    if (Stun[i]!=-1)
                {
                    cout << "STUN " << Stun[i] << " Halt MF !!!" << endl ;
                    StunAbility[i]=20 ;
                }
                else if (Bust[i]!=-1)
                {
                    if (BustDist[i]<1760)
                    {
                        lastBust[i]=Bust[i] ;
                        cout << "BUST " << Bust[i] << " Suck you !!!" << endl ;
                    }
                    else 
                        cout << "MOVE " << x[BustId[i]] <<  " " << y[BustId[i]] << endl;
                }
                else
                {
                    cout << "MOVE " << gotoX[i] <<  " " << gotoY[i] << endl; // MOVE x y | BUST id | RELEASE
                    if ((x[i]==gotoX[i]) && (y[i]==gotoY[i]))
                        arrived[i]=1 ;
                }
            }
            else if (state[i]==1) 
            {
                if ((x[i]==homeX) && (y[i]==homeY))
                    cout << "RELEASE" << endl ;
                else
                    cout << "MOVE " << homeX <<  " " << homeY << endl; // MOVE x y | BUST id | RELEASE
            }
            else if (state[i]==2)
            {
                cout << "MOVE " << homeX <<  " " << homeY << " Oh shit !!!" << endl;
                
            }
            else if (state[i]==3)
            {
                cout << "BUST " << lastBust[i] << " Suck you !!!" << endl ;
            }
        }
    }
}












