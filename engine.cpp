#include<iostream>
#include<iomanip>
#include<bits/stdc++.h>
#include<chrono>
#include<cstdlib>
using namespace std;
int board[64]={
    8,9,10,11,12,10,9,8,
    7,7,7,7,7,7,7,7,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    0,0,0,0,0,0,0,0,
    1,2,3,4,5,3,2,1
};
double tt=0;
int tt1=0;
const int depth=6;
const int buffer1=1000;
const int buffer2=100;
const int buffer3=100;
const int buffer4=1000;
const int qBuffer=65;
const int pieceValues[13]={0,530,0,400,950,10000,0,0,530,0,400,950,10000};
int totalNodes=0;
int cutoffNodes=0;
const int moveArraySize=70;
const int alphaLimit=200;
const int pValues[64]={
    950,950,950,950,950,950,950,950,
    600,600,600,600,600,600,600,600,
    150,170,180,220,220,180,170,150,
    110,130,140,160,160,140,130,110,
    90,100,110,130,130,110,100,90,
    80,80,90,100,100,90,80,80,
    70,75,80,90,90,80,75,70,
    0,0,0,0,0,0,0,0
};
const int nValues[64]={
    240,260,280,290,290,280,270,240,
    260,280,300,310,310,300,280,260,
    280,300,320,330,330,320,300,280,
    290,310,330,350,350,330,310,290,
    290,310,330,350,350,330,310,290,
    280,300,320,330,330,320,300,280,
    260,280,300,310,310,300,280,260,
    240,260,280,290,290,280,260,240
};
const int wKmask=63<<4;
const int bKmask=63<<10;
const int aa[8]={17,13,10,7,5,3,2,1};
int allsq[4096];
struct Move{
    int all[3];
};
void xy(int a,int b){
    cout<<char(97+(a%8));
    cout<<8-int(a/8)<<"-";
    cout<<char(97+(b%8));
    cout<<8-int(b/8)<<" "<<endl;
}
void xx(Move* moveArray){
    int a=moveArray[0].all[1];
    cout<<char(97+(a%8));
    cout<<8-int(a/8)<<"-";
    a=moveArray[0].all[2];
    cout<<char(97+(a%8));
    cout<<8-int(a/8)<<" "<<endl;
};
void displayBoard(){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            cout<<board[8*i+j]<<" ";
        }
        cout<<"\n";
    }
    return;
};
class Board{
public:
    int* pvArray;
    int* sqValues;
    Move moveArray[moveArraySize];
    int boardDepth;
    bool whiteToPlay;
    //bKing,wKing,wShort,wLong,bShort,bLong;
    int cFlagsAndKings;
    int enPassant;
    int alpha;
    int beta;
    int wevl;
    int bevl;
    int staticEval;
    int pEval;
    // White Move Generator
    void wMoveGen(){
        std::chrono::high_resolution_clock::time_point start=chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        int arraySize=1;
        int df=0;
        int wKing=(cFlagsAndKings&wKmask)>>4;
        int wx=wKing%8;
        int wy=wKing/8;
        int bKing=(cFlagsAndKings&bKmask)>>10;
        int bx=bKing%8;
        int by=bKing/8;
        int loc2=wKing<<6;
        int loc1=bKing<<6;
        int tmp2=aa[max(wx,wy)];
        int tmp1=aa[max(bx,by)];
        if(allsq[loc2]!=tmp2){
            cout<<"a";
            for(int i=0;i<64;i++){
                int x=i%8;
                int y=i/8;
                allsq[loc2+i]=aa[max(abs(x-wx),abs(y-wy))];
            }
        }
        if(allsq[loc1]!=tmp1){
            cout<<"b"<<bKing;
            for(int i=0;i<64;i++){
                int x=i%8;
                int y=i/8;
                allsq[loc1+i]=aa[max(abs(x-bx),abs(y-by))];
            }
        }
        for(int i=0;i<64;i++){
            int squareValue=board[i];
            //Black pieces or Empty square
            if((squareValue>5)){
                continue;
            }
            int x=i%8;
            int y=i/8;
            switch(squareValue){
                // White Knight
                case 2:{
                    // 1 left, 2 up
                    df+=((allsq[loc1+i]<<1)+allsq[loc2+i])<<1;
                    if((i>15)&&(i%8!=0)&&(board[i-17]>5)){
                        int p=board[i-17];
                        int ec=nValues[i-17]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i+17];
                            }
                            else if(p==9){
                                ec+=nValues[i-17];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-17;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 up
                    if((i>15)&&(i%8!=7)&&(board[i-15]>5)){
                        int p=board[i-15];
                        int ec=nValues[i-15]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i+15];
                            }
                            else if(p==9){
                                ec+=nValues[i-15];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-15;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 left, 1 up
                    if((i>7)&&(i%8>1)&&(board[i-10]>5)){
                        int p=board[i-10];
                        int ec=nValues[i-10]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i+10];
                            }
                            else if(p==9){
                                ec+=nValues[i-10];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-10;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 up
                    if((i>7)&&(i%8<6)&&(board[i-6]>5)){
                        int p=board[i-6];
                        int ec=nValues[i-6]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i+6];
                            }
                            else if(p==9){
                                ec+=nValues[i-6];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-6;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 left, 1 down
                    if((i<56)&&(i%8>1)&&(board[i+6]>5)){
                        int p=board[i+6];
                        int ec=nValues[i+6]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i-6];
                            }
                            else if(p==9){
                                ec+=nValues[i+6];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+6;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 right, 1 down
                    if((i<56)&&(i%8<6)&&(board[i+10]>5)){
                        int p=board[i+10];
                        int ec=nValues[i+10]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i-10];
                            }
                            else if(p==9){
                                ec+=nValues[i+10];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+10;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left, 2 down
                    if((i<48)&&(i%8!=0)&&(board[i+15]>5)){
                        int p=board[i+15];
                        int ec=nValues[i+15]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i-15];
                            }
                            else if(p==9){
                                ec+=nValues[i+15];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+15;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 down
                    if((i<48)&&(i%8!=7)&&(board[i+17]>5)){
                        int p=board[i+17];
                        int ec=nValues[i+17]-nValues[i];
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-i-17];
                            }
                            else if(p==9){
                                ec+=nValues[i+17];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+17;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
                // White Pawn
                case 0:{
                    // Pawn promotion to be implemented
                    if(i<16){
                        if(board[i-8]==6){
                            int ec=pValues[i-8]-pValues[i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=i-8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                        }
                    }
                    // Normal pawn moves
                    else{
                        // 1 up
                        if(board[i-8]==6){
                            int ec=pValues[i-8]-pValues[i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=i-8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            // 2 up, set enPassant row
                            if((i>47)&&(board[i-16]==6)){
                                ec=pValues[i-16]-pValues[i];
                                moveArray[arraySize].all[1]=i;
                                moveArray[arraySize].all[2]=i-16;
                                moveArray[arraySize].all[0]=ec;
                                arraySize++;
                                //enp
                            }
                        }
                    }
                    // 1 up, 1 right, capture black
                    if((i%8!=7)&&(board[i-7]>6)){
                        int p=board[i-7];
                        int ec=pValues[i-7]-pValues[i];
                        if(p==7){
                            ec+=pValues[63-i+7];
                        }
                        else if(p==9){
                            ec+=nValues[i-7];
                        }
                        ec+=pieceValues[p];
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-7;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 up, 1 left, capture black
                    if((i%8!=0)&&(board[i-9]>6)){
                        int p=board[i-9];
                        int ec=pValues[i-9]-pValues[i];
                        if(p==7){
                            ec+=pValues[63-i+9];
                        }
                        else if(p==9){
                            ec+=nValues[i-9];
                        }
                        ec+=pieceValues[p];
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-9;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
                // White Rook
                case 1:{
                    // Up moves
                    int j=i-8;
                    while((j>=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j-=8;
                    }
                    // Down moves
                    j=i+8;
                    while((j<64)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j+=8;
                    }
                    // Left moves
                    j=i-1;
                    while(((j+8)%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j--;
                    }
                    // Right Moves
                    j=i+1;
                    while((j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j++;
                    }
                    break;
                }
                // White Bishop
                case 3:{
                    int j=i-9;
                    // Left and up, diagonal
                    while((j>=0)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j-=9;
                    }
                    // Right and up diagonal
                    j=i-7;
                    while((j>=0)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j-=7;
                    }
                    // Left and down diagonal
                    j=i+7;
                    while((j<64)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;
                        j+=7;
                    }
                    // Right and down diagonal
                    j=i+9;
                    while((j<64)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j+=9;
                    }
                    break;
                }
                // White Queen
                case 4:{
                    // Up moves
                    int j=i-8;
                    while((j>=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=8;
                    }
                    // Down moves
                    j=i+8;
                    while((j<64)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=8;
                    }
                    // Left moves
                    j=i-1;
                    while(((j+8)%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j--;
                    }
                    // Right moves
                    j=i+1;
                    while((j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j++;
                    }
                    // Left and up diagonal
                    j=i-9;
                    while((j>=0)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=9;
                    }
                    // Right and up diagonal
                    j=i-7;
                    while((j>=0)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=7;
                    }
                    // Left and down diagonal
                    j=i+7;
                    while((j<64)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=7;
                    }
                    // Right and down diagonal
                    j=i+9;
                    while((j<64)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        if(p>6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==7){
                                    ec+=pValues[63-j];
                                }
                                else if(p==9){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=9;
                    }
                    break;
                }
                // White King
                case 5:{
                    // 1 left, 1 up
                    int j=i-9;
                    if((j>=0)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 up
                    j++;
                    if((j>=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right, 1 up
                    j++;
                    if((j>=0)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 left
                    j+=6;
                    if((j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right
                    j+=2;
                    if((j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 left, 1 down
                    j+=6;
                    if((j<64)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 down
                    j++;
                    if((j<64)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right, 1 down
                    j++;
                    if((j<64)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    break;
                }
            }
        }
        // Store array size at 0 index
        wevl=df;
        moveArray[0].all[0]=arraySize;
        sort(moveArray+1,moveArray+arraySize-1,[](Move &m1,Move &m2){
            return m1.all[0]>m2.all[0];
        });
        std::chrono::high_resolution_clock::time_point end=chrono::high_resolution_clock::now();
        double time_taken=chrono::duration_cast<chrono::microseconds>(end - start).count();
        time_taken *=1e-6;
        tt+=time_taken;
        tt1++;
        return;
    };
    // Black Move Generator
    void bMoveGen(){
        int arraySize=1;
        int df=0;
        int wKing=(cFlagsAndKings&wKmask)>>4;
        int wx=wKing%8;
        int wy=wKing/8;
        int bKing=(cFlagsAndKings&bKmask)>>10;
        int bx=bKing%8;
        int by=bKing/8;
        int loc1=wKing<<6;
        int loc2=bKing<<6;
        int tmp1=aa[max(wx,wy)];
        int tmp2=aa[max(bx,by)];
        if(allsq[loc1]!=tmp1){
            for(int i=0;i<64;i++){
                int x=i%8;
                int y=i/8;
                allsq[loc1+i]=aa[max(abs(x-wx),abs(y-wy))];
            }
        }
        if(allsq[loc2]!=tmp2){
            for(int i=0;i<64;i++){
                int x=i%8;
                int y=i/8;
                allsq[loc1+i]=aa[max(abs(x-bx),abs(y-by))];
            }
        }
        for(int i=63;i>=0;i--){
            int squareValue=board[i];
            //White pieces or Empty square
            if((squareValue<7)){
                continue;
            }
            int x=i%8;
            int y=i/8;
            switch(squareValue){
                // Black Knight
                case 9:{
                    // 1 left, 2 up
                    df+=((allsq[loc1+i]<<1)+allsq[loc2+i])<<1;
                    if((i>15)&&(i%8!=0)&&(board[i-17]<7)){
                        int p=board[i-17];
                        int ec=nValues[i-17]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i-17];
                            }
                            else if(p==3){
                                ec+=nValues[i-17];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-17;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 up
                    if((i>15)&&(i%8!=7)&&(board[i-15]<7)){
                        int p=board[i-15];
                        int ec=nValues[i-15]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i-15];
                            }
                            else if(p==2){
                                ec+=nValues[i-15];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-15;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 left, 1 up
                    if((i>7)&&(i%8>1)&&(board[i-10]<7)){
                        int p=board[i-10];
                        int ec=nValues[i-10]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i-10];
                            }
                            else if(p==2){
                                ec+=nValues[i-10];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-10;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 up
                    if((i>7)&&(i%8<6)&&(board[i-6]<7)){
                        int p=board[i-6];
                        int ec=nValues[i-6]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i-6];
                            }
                            else if(p==2){
                                ec+=nValues[i-6];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-6;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 left, 1 down
                    if((i<56)&&(i%8>1)&&(board[i+6]<7)){
                        int p=board[i+6];
                        int ec=nValues[i+6]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i+6];
                            }
                            else if(p==2){
                                ec+=nValues[i+6];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+6;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 right, 1 down
                    if((i<56)&&(i%8<6)&&(board[i+10]<7)){
                        int p=board[i+10];
                        int ec=nValues[i+10]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i+10];
                            }
                            else if(p==2){
                                ec+=nValues[i+10];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+10;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left, 2 down
                    if((i<48)&&(i%8!=0)&&(board[i+15]<7)){
                        int p=board[i+15];
                        int ec=nValues[i+15]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i+15];
                            }
                            else if(p==2){
                                ec+=nValues[i+15];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+15;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 2 down
                    if((i<48)&&(i%8!=7)&&(board[i+17]<7)){
                        int p=board[i+17];
                        int ec=nValues[i+17]-nValues[i];
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[i+17];
                            }
                            else if(p==2){
                                ec+=nValues[i+17];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+17;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
                // Black Pawn
                case 7:{
                    // Pawn promotion to be implemented
                    if(i>47){
                        if(board[i+8]==6){
                            int ec=pValues[63-i-8]-pValues[63-i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=i+8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                        }
                    }
                    // Normal pawn moves
                    else{
                        // 1 down
                        if(board[i+8]==6){
                            int ec=pValues[63-i-8]-pValues[63-i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=i+8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            // 2 down, set enPassant row
                            if((i<16)&&(board[i+16]==6)){
                                ec=pValues[63-i-16]-pValues[63-i];
                                moveArray[arraySize].all[1]=i;
                                moveArray[arraySize].all[2]=i+16;
                                moveArray[arraySize].all[0]=ec;
                                arraySize++;
                                //enp
                            }
                        }
                    }
                    // 1 down, 1 right, capture white
                    if((i%8!=0)&&(board[i+7]<6)){
                        int p=board[i+7];
                        int ec=pValues[63-i-7]-pValues[63-i];
                        if(p==0){
                            ec+=pValues[i+7];
                        }
                        else if(p==2){
                            ec+=nValues[i+7];
                        }
                        ec+=pieceValues[p];
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+7;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 down, 1 left, capture white
                    if((i%8!=7)&&(board[i+9]<6)){
                        int p=board[i+9];
                        int ec=pValues[63-i-9]-pValues[63-i];
                        if(p==0){
                            ec+=pValues[i+9];
                        }
                        else if(p==2){
                            ec+=nValues[i+9];
                        }
                        ec+=pieceValues[p];
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i+9;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
                // Black Rook
                case 8:{
                    // Up moves
                    int j=i-8;
                    while((j>=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j-=8;
                    }
                    // Down moves
                    j=i+8;
                    while((j<64)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j+=8;
                    }
                    // Left moves
                    j=i-1;
                    while(((j+8)%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j--;
                    }
                    // Right Moves
                    j=i+1;
                    while((j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j++;
                    }
                    break;
                }
                // Black Bishop
                case 10:{
                    int j=i-9;
                    // Left and up, diagonal
                    while((j>=0)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j-=9;
                    }
                    // Right and up diagonal
                    j=i-7;
                    while((j>=0)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j-=7;
                    }
                    // Left and down diagonal
                    j=i+7;
                    while((j<64)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j+=7;
                    }
                    // Right and down diagonal
                    j=i+9;
                    while((j<64)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=((allsq[loc1+j]+(allsq[loc2+j]>>1))>>1)+10;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=1;
                        arraySize++;;
                        j+=9;
                    }
                    break;
                }
                // Black Queen
                case 11:{
                    // Up moves
                    int j=i-8;
                    while((j>=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=8;
                    }
                    // Down moves
                    j=i+8;
                    while((j<64)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=8;
                    }
                    // Left moves
                    j=i-1;
                    while(((j+8)%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j--;
                    }
                    // Right moves
                    j=i+1;
                    while((j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j++;
                    }
                    // Left and up diagonal
                    j=i-9;
                    while((j>=0)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=9;
                    }
                    // Right and up diagonal
                    j=i-7;
                    while((j>=0)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j-=7;
                    }
                    // Left and down diagonal
                    j=i+7;
                    while((j<64)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=7;
                    }
                    // Right and down diagonal
                    j=i+9;
                    while((j<64)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        if(p<6){
                            df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                            int ec=0;
                            if(p!=6){
                                if(p==0){
                                    ec+=pValues[j];
                                }
                                else if(p==2){
                                    ec+=nValues[j];
                                }
                                ec+=pieceValues[p];
                            }
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=j;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            break;
                        }
                        df+=(allsq[loc1+j]+(allsq[loc2+j]>>1))>>1;
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=0;
                        arraySize++;
                        j+=9;
                    }
                    break;
                }
                // Black King
                case 12:{
                    // 1 left, 1 up
                    int j=i-9;
                    if((j>=0)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 up
                    j++;
                    if((j>=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right, 1 up
                    j++;
                    if((j>=0)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 left
                    j+=6;
                    if((j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right
                    j+=2;
                    if((j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 left, 1 down
                    j+=6;
                    if((j<64)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 down
                    j++;
                    if((j<64)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    // 1 right, 1 down
                    j++;
                    if((j<64)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=0;
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec-1;
                        arraySize++;
                    }
                    break;
                }
            }
        }
        // Store array size at 0 index
        bevl=-df;
        moveArray[0].all[0]=arraySize;
        sort(moveArray+1,moveArray+arraySize-1,[](Move &m1,Move &m2){
            return m1.all[0]>m2.all[0];
        });
        return;
    };
    /*Board Evaluation Function    
    int boardEval(){
        int eval=0;
        for(int i=0;i<64;i++){
            switch(board[i]){
                case 0:{
                    eval+=pValues[i];
                    break;
                }
                case 1:{
                    eval+=530;
                    break;
                }
                case 2:{
                    eval+=nValues[i];
                    break;
                }
                case 3:{
                    eval+=330;
                    break;
                }
                case 4:{
                    eval+=950;
                    break;
                }
                case 5:{
                    eval+=10000;
                    break;
                }
                case 6:{
                    break;
                }
                case 7:{
                    eval-=pValues[63-i];
                    break;
                }
                case 8:{
                    eval-=530;
                    break;
                }
                case 9:{
                    eval-=nValues[63-i];
                    break;
                }
                case 10:{
                    eval-=330;
                    break;
                }
                case 11:{
                    eval-=950;
                    break;
                }
                case 12:{
                    eval-=10000;
                    break;
                }
            }
        }
        if(eval!=staticEval){
            cout<<"h";
        }
        return eval;
    };
    */
    // To make a move (pawn pro.,enP,castling left)
    void makeMove(int a,int b){
        board[b]=board[a];
        board[a]=6;
    };
    // To copy Board
    int* copyBoardArray(){
        int* ft=new int[64];
        memcpy(ft,board,256);
        return ft;
    };
    // Minimax algorithm that returns PV array
    void miniMax(){
        totalNodes++;
        // Depth reached, return evaluation
        if(boardDepth==depth){
            pvArray[0]=(staticEval+pEval)/2;//+((depth%2==0)*wevl*2)+((depth%2==1)*bevl*2);
            return;
        }
        else if(whiteToPlay){
            pvArray[0]=-alphaLimit;
            int maxValue=-alphaLimit;
            int bestMove=1;
            wMoveGen();
            int evalSum=staticEval;
            if(evalSum-alpha+buffer1<0){
                cutoffNodes++;
                return;
            }
            if(evalSum-beta-buffer2>0){
                cutoffNodes++;
                pvArray[0]=alphaLimit;
                return;
            }
            for(int i=1;i<moveArray[0].all[0];i++){
                totalNodes++;
                int cevl;
                int square1=moveArray[i].all[1];
                int square2=moveArray[i].all[2];
                int evalChange=moveArray[i].all[0];
                int piece1,piece2;
                if(square1<0){
                    // to do
                }
                else{
                    piece1=board[square1];
                    piece2=board[square2];
                }
                if(boardDepth==depth-1){
                    if(evalChange>qBuffer){
                        Board newBoard=Board(*this);
                        newBoard.boardDepth=newBoard.boardDepth-1;
                        newBoard.wevl=wevl;
                        newBoard.bevl=bevl;
                        newBoard.staticEval=staticEval+evalChange;
                        newBoard.pEval=staticEval;
                        makeMove(square1,square2);
                        newBoard.miniMax();
                        cevl=newBoard.pvArray[0];
                        if(square1<0){
                            // to do
                        }
                        else{
                            board[square1]=piece1;
                            board[square2]=piece2;
                        }
                        if(cevl>alpha){
                            alpha=cevl;
                            // Alpha-beta cutoff
                            if(alpha>=beta){
                                cutoffNodes++;
                                pvArray[0]=cevl;
                                return;
                            }
                            maxValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                    else{
                        cevl=staticEval+(evalChange)/2;
                        if(cevl>alpha){
                            alpha=cevl;
                            // Alpha-beta cutoff
                            if(alpha>=beta){
                                cutoffNodes++;
                                pvArray[0]=cevl;
                                return;
                            }
                            maxValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                }
                if(evalSum+evalChange<alpha-buffer3){
                    //cout<<staticEval<<" "<<evalChange<<" "<<wevl<<" "<<alpha<<endl;
                    cutoffNodes++;
                    continue;
                }
                if(evalSum+evalChange>beta+buffer4){
                    cutoffNodes++;
                    pvArray[0]=alphaLimit;
                    return;
                }
                Board newBoard=Board(*this);
                newBoard.wevl=wevl;
                newBoard.bevl=bevl;
                newBoard.staticEval=staticEval+evalChange;
                newBoard.pEval=staticEval;
                makeMove(square1,square2);
                newBoard.miniMax();
                cevl=newBoard.pvArray[0];
                if(square1<0){
                    // to do
                }
                else{
                    board[square1]=piece1;
                    board[square2]=piece2;
                }
                if(cevl>alpha){
                    alpha=cevl;
                    // Alpha-beta cutoff
                    if(alpha>=beta){
                        cutoffNodes++;
                        pvArray[0]=cevl;
                        return;
                    }
                    maxValue=cevl;
                    bestMove=i;
                    for(int i=3;i<2*(depth-boardDepth)+1;i++){
                        pvArray[i]=newBoard.pvArray[i-2];
                    }
                }
            }
            pvArray[0]=maxValue;
            pvArray[1]=moveArray[bestMove].all[1];
            pvArray[2]=moveArray[bestMove].all[2];
            return;
        }
        else{
            pvArray[0]=alphaLimit;
            int minValue=alphaLimit;
            int bestMove=1;
            bMoveGen();
            int evalSum=staticEval;
            if(evalSum-beta-buffer1>0){
                cutoffNodes++;
                return;
            }
            if(evalSum-alpha+buffer2<0){
                pvArray[0]=-alphaLimit;
                cutoffNodes++;
                return;
            }
            for(int i=1;i<moveArray[0].all[0];i++){
                totalNodes++;
                int cevl;
                int square1=moveArray[i].all[1];
                int square2=moveArray[i].all[2];
                int evalChange=moveArray[i].all[0];
                int piece1,piece2;
                // special moves
                if(square1<0){
                    // to do
                }
                else{
                    piece1=board[square1];
                    piece2=board[square2];
                }
                if(boardDepth==depth-1){
                    if(evalChange>qBuffer){
                        Board newBoard=Board(*this);
                        newBoard.boardDepth=newBoard.boardDepth-1;
                        newBoard.wevl=wevl;
                        newBoard.bevl=bevl;
                        newBoard.staticEval=staticEval-evalChange;
                        newBoard.pEval=staticEval;
                        makeMove(square1,square2);
                        newBoard.miniMax();
                        cevl=newBoard.pvArray[0];
                        if(square1<0){
                            // to do
                        }
                        else{
                            board[square1]=piece1;
                            board[square2]=piece2;
                        }
                        if(cevl<beta){
                            beta=cevl;
                            // Alpha-beta cutoff
                            if(alpha>=beta){
                                cutoffNodes++;
                                pvArray[0]=cevl;
                                return;
                            }
                            minValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                    else{
                        cevl=staticEval-(evalChange)/2;
                        if(cevl<beta){
                            beta=cevl;
                            // Alpha-beta cutoff
                            if(alpha>=beta){
                                cutoffNodes++;
                                pvArray[0]=cevl;
                                return;
                            }
                            minValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                }
                if(evalSum-evalChange>beta+buffer3){
                    pvArray[0]=alphaLimit;
                    cutoffNodes++;
                    continue;
                }
                if(evalSum-evalChange<alpha-buffer4){
                    cutoffNodes++;
                    pvArray[0]=-alphaLimit;
                    return;
                }
                Board newBoard=Board(*this);
                newBoard.wevl=wevl;
                newBoard.bevl=bevl;
                newBoard.staticEval=staticEval-evalChange;
                makeMove(square1,square2);
                newBoard.miniMax();
                cevl=newBoard.pvArray[0];
                if(square1<0){
                    // to do
                }
                else{
                    board[square1]=piece1;
                    board[square2]=piece2;
                }
                if(cevl<beta){
                    beta=cevl;
                    // Alpha-beta cutoff
                    if(alpha>=beta){
                        cutoffNodes++;
                        pvArray[0]=cevl;
                        return;
                    }
                    minValue=cevl;
                    bestMove=i;
                    for(int i=3;i<2*(depth-boardDepth)+1;i++){
                        pvArray[i]=newBoard.pvArray[i-2];
                    }
                }
            }
            pvArray[0]=minValue;
            pvArray[1]=moveArray[bestMove].all[1];
            pvArray[2]=moveArray[bestMove].all[2];
            return;
        }
    };
    // Constructor for starting position
    Board(){
        pvArray=new int[2*depth+1];
        boardDepth=0;
        whiteToPlay=true;
        cFlagsAndKings=15+(60<<4)+(4<<10);
        alpha=-alphaLimit;
        beta=alphaLimit;
        wevl=0;
        bevl=0;
        staticEval=0;
    };
    // Copy constructor for child node
    Board(Board &oldBoard){
        whiteToPlay=!oldBoard.whiteToPlay;
        boardDepth=oldBoard.boardDepth+1;
        pvArray=new int[2*(depth-boardDepth)+1];
        cFlagsAndKings=oldBoard.cFlagsAndKings;
        alpha=oldBoard.alpha;
        beta=oldBoard.beta;
        staticEval=0;
    };
    ~Board(){
        delete[] pvArray;
    };
};
// void randMove(int *moveArray){
//     int i=moveArray[0];
//     srand((unsigned) time(NULL));
//     i=rand()%(i/2);
//     i=i*2+1;
//     int a=i;
//     cout<<char(97+(a%8));
//     cout<<8-int(a/8)<<"-";
//     a=moveArray[i+1];
//     cout<<char(97+(a%8));
//     cout<<8-int(a/8)<<"\n";
// };
void printMoveArray(Move* moveArray){
    for(int i=1;i<moveArray[0].all[0];i++){
        int a=moveArray[i].all[1];
        cout<<char(97+(a%8));
        cout<<8-int(a/8)<<"-";
        a=moveArray[i].all[2];
        cout<<char(97+(a%8));
        cout<<8-int(a/8)<<" ";
    }
    cout<<"\n";
};
int dupliBoard[64]={
    8,9,10,11,12,10,9,8,
    7,7,7,7,7,7,7,7,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    0,0,0,0,0,0,0,0,
    1,2,3,4,5,3,2,1
};
void printPV(int* pvArray){
    for(int i=1;i<2*depth;i+=2){
        int a=pvArray[i];
        cout<<char(97+(a%8));
        cout<<8-int(a/8)<<"-";
        a=pvArray[i+1];
        cout<<char(97+(a%8));
        cout<<8-int(a/8)<<" ";
    }
    cout<<"\n";
    return;
};
void play(Board &initBoard){
    while(true){
        std::chrono::high_resolution_clock::time_point start=chrono::high_resolution_clock::now();
        ios_base::sync_with_stdio(false);
        initBoard.miniMax();
        std::chrono::high_resolution_clock::time_point end=chrono::high_resolution_clock::now();
        double time_taken=chrono::duration_cast<chrono::microseconds>(end - start).count();
        time_taken *=1e-6;
        cout<<"Time taken by program is : "<<fixed<<time_taken<<setprecision(6);
        cout<<" sec"<<"\n";
        cout<<"\n"<<totalNodes;
        cout<<"\n"<<cutoffNodes<<"\n"; 
        cout<<"eval";
        int *pv=initBoard.pvArray;
        cout<<pv[0]<<endl;
        initBoard.makeMove(pv[1],pv[2]);
        displayBoard();
        int square1=pv[1];
        int square2=pv[2];
        int piece1=board[square1];
        int piece2=board[square2];
        int evalChange=0;
        if(piece1==2){
            evalChange+=nValues[square2]-nValues[square1];
        }
        else if(piece1==0){
            evalChange+=pValues[square2]-pValues[square1];
        }
        if(piece2>6){
            if(piece2==7){
                evalChange+=pValues[63-square2];
            }
            else if(piece2==9){
                evalChange+=nValues[63-square2];
            }
            evalChange+=pieceValues[piece2];
        }
        initBoard.staticEval+=evalChange;
        printPV(pv);
        string s;
        cout<<"\nwanna pilay: ";
        cin>>s;
        if(s[0]=='q'){
            return;
        }
        int a=s[0]-'a';
        int b='8'-s[1];
        int c=b*8+a;
        a=s[3]-'a';
        b='8'-s[4];
        int d=b*8+a;
        initBoard.makeMove(c,d);
        displayBoard();
        evalChange=0;
        square1=c;
        square2=d;
        piece1=board[square1];
        piece2=board[square2];
        if(piece1==9){
            evalChange+=nValues[63-square2]-nValues[63-square1];
        }
        else if(piece1==7){
            evalChange+=pValues[63-square2]-pValues[63-square1];
        }
        if(piece2<6){
            if(piece2==1){
                evalChange+=pValues[square2];
            }
            else if(piece2==2){
                evalChange+=nValues[square2];
            }
            evalChange+=pieceValues[piece2];
        }
        initBoard.staticEval-=evalChange;
        initBoard.alpha=-alphaLimit;
        initBoard.beta=alphaLimit;
        initBoard.wevl=0;
        initBoard.bevl=0;
        totalNodes=0;
        cutoffNodes=0;
    }
    return;
};
int main(){
    // std::chrono::high_resolution_clock::time_point start=chrono::high_resolution_clock::now();
    // ios_base::sync_with_stdio(false);
    Board initBoard=Board();
    int loc2=60<<6;
    int wx=60%8;
    int wy=60/8;
    for(int i=0;i<64;i++){
        int x=i%8;
        int y=i/8;
        allsq[loc2+i]=aa[max(abs(x-wx),abs(y-wy))];
    }
    loc2=4<<6;
    wx=4%8;
    wy=4/8;
    for(int i=0;i<64;i++){
        int x=i%8;
        int y=i/8;
        allsq[loc2+i]=aa[max(abs(x-wx),abs(y-wy))];
    }
    // displayBoard();
    // cout<<"\nList of moves: ";
    // initBoard.wMoveGen();
    // printMoveArray(initBoard.moveArray);
    // displayBoard();
    // initBoard.miniMax();
    // std::chrono::high_resolution_clock::time_point end=chrono::high_resolution_clock::now();
    // double time_taken=chrono::duration_cast<chrono::microseconds>(end - start).count();
    // time_taken *=1e-6;
    // cout<<"Time taken by program is : "<<fixed<<time_taken<<setprecision(6);
    // cout<<" sec"<<"\n";
    // cout<<"Best move eval is: "<<initBoard.pvArray[0];
    // cout<<"\nPrincipal variation is: ";
    // printPV(initBoard.pvArray); 
    play(initBoard);
    // cout<<"by program is : "<<fixed<<tt<<setprecision(6);
    // cout<<" sec"<<"\n";
    // cout<<tt1<<endl;
    // cout<<totalNodes<<endl;
    // cout<<cutoffNodes<<endl;
    return 0;
}
