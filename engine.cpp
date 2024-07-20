#include<iostream>
#include<iomanip>
#include<bits/stdc++.h>
#include<chrono>
#include<cstdlib>
using namespace std;
int bord[64]={
    8,6,10,6,12,8,6,6,
    7,7,6,6,6,7,4,7,
    6,6,9,6,7,6,6,3,
    11,6,10,6,0,6,6,6,
    6,6,6,7,6,6,6,6,
    6,6,2,6,6,2,6,6,
    0,0,0,6,6,0,0,0,
    1,6,6,6,5,6,6,1
};
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
const int buffer1=1550;
const int buffer2=300;
const int buffer3=300;
const int buffer4=1550;
//const int buffer5=500;
const int qBuffer=50;
const int pieceValues[13]={0,530,0,340,1000,10000,0,0,530,0,340,1000,10000};
int totalNodes=0;
int cutoffNodes=0;
const int moveArraySize=110;
const int alphaLimit=200;
const int alphaLimit1=1000;
const int pValues[64]={
    950,950,950,950,950,950,950,950,
    600,600,600,600,600,600,600,600,
    155,170,180,200,200,180,170,155,
    110,115,125,145,145,125,115,110,
    85,95,110,125,125,110,95,85,
    75,80,90,100,100,90,80,75,
    70,75,80,90,90,80,75,70,
    0,0,0,0,0,0,0,0
};
const int nValues[64]={
    250,270,280,290,290,280,270,250,
    270,280,300,310,310,300,280,270,
    280,300,320,330,330,320,300,280,
    290,310,330,340,340,330,310,290,
    290,310,330,340,340,330,310,290,
    280,300,320,330,330,320,300,280,
    270,280,300,310,310,300,280,270,
    250,270,280,290,290,280,270,250
};
const int sqValues[64]={
    5,6,7,8,8,7,6,5,
    6,8,9,10,10,9,8,6,
    7,9,11,12,12,11,9,7,
    8,10,12,14,14,12,10,8,
    8,10,12,14,14,12,10,8,
    7,9,11,12,12,11,9,7,
    6,8,9,10,10,9,8,6,
    5,6,7,8,8,7,6,5
};
const int wKmask=63<<4;
const int bKmask=63<<10;
const int aa[8]={25,15,10,7,5,3,2,1};
const char aq[13]={'p','r','n','b','q','k',' ','P','R','N','B','Q','K'};
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
        cout<<"  ";
        for(int j=0;j<8;j++){
            cout<<" ---";
        }
        cout<<endl<<8-i<<" ";
        for(int j=0;j<8;j++){
            cout<<"| "<<aq[board[8*i+j]]<<" ";
        }
        cout<<"|\n";
    }
    cout<<"  ";
    for(int j=0;j<8;j++){
        cout<<" ---";
    }
    cout<<"\n ";
    int z='a';
    for(int i=0;i<8;i++){
        cout<<"   "<<char(z);
        z++;
    }
    cout<<"\n";
    return;
};
void displayBoard1(){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            cout<<board[8*i+j]<<" ";
        }
        cout<<"\n";
    }
    return;
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
void printMoveArray1(const Move moveArray[90]){
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
class Board{
public:
    int pvArray[2*depth+1];
    Move moveArray[moveArraySize];
    int boardDepth;
    bool whiteToPlay;
    //bKing,wKing,wShort,wLong,bShort,bLong;
    int cFlagsAndKings=5071;
    int enPassant;
    int alpha;
    int beta;
    int wevl;
    int bevl;
    int staticEval;
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
            cout<<"a"<<wKing<<endl;
            cout<<cFlagsAndKings<<endl;
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
        int te=beta+buffer1-wevl-bevl-staticEval;
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=i-10;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 2 right, 1 up
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                    if(i<16){
                        if(board[i-8]==6){
                            int ec=pieceValues[4]-pValues[i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=-i+8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                            continue;
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
                        if(ec>te){
                            staticEval=INT_MAX;
                            return;
                        }
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
                        if(ec>te){
                            staticEval=INT_MAX;
                            return;
                        }
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=sqValues[j]-sqValues[i];
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=sqValues[j]-sqValues[i];
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=sqValues[j]-sqValues[i];
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=sqValues[j]-sqValues[i];
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];                          
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 up
                    j++;
                    if((j>=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 1 up
                    j++;
                    if((j>=0)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left
                    j+=6;
                    if((j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right
                    j+=2;
                    if((j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left, 1 down
                    j+=6;
                    if((j<64)&&(j%8!=7)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 down
                    j++;
                    if((j<64)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 1 down
                    j++;
                    if((j<64)&&(j%8!=0)&&(board[j]>5)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==7){
                                ec+=pValues[63-j];
                            }
                            else if(p==9){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
            }
        }
        // Store array size at 0 index
        wevl=df;
        if(arraySize==1){
            moveArray[0].all[0]=1;
            std::chrono::high_resolution_clock::time_point end=chrono::high_resolution_clock::now();
            double time_taken=chrono::duration_cast<chrono::microseconds>(end - start).count();
            time_taken *=1e-6;
            tt+=time_taken;
            tt1++;
            return;
        }
        moveArray[0].all[0]=arraySize;
        sort(moveArray+1,moveArray+arraySize,[](const Move &m1,const Move &m2){
            return m1.all[0]>m2.all[0];
        });
        // displayBoard();
        // printMoveArray(moveArray);
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
        int te=-alpha+buffer4+wevl+bevl+staticEval;
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
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
                            int ec=pieceValues[11]-pValues[63-i];
                            moveArray[arraySize].all[1]=i;
                            moveArray[arraySize].all[2]=-i-8;
                            moveArray[arraySize].all[0]=ec;
                            arraySize++;
                        }
                        continue;
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
                        if(ec>te){
                            staticEval=INT_MAX;
                            return;
                        }
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
                        if(ec>te){
                            staticEval=INT_MAX;
                            return;
                        }
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i])<<1;
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
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
                        moveArray[arraySize].all[0]=(sqValues[j]-sqValues[i]);
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
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 up
                    j++;
                    if((j>=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 1 up
                    j++;
                    if((j>=0)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left
                    j+=6;
                    if((j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right
                    j+=2;
                    if((j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 left, 1 down
                    j+=6;
                    if((j<64)&&(j%8!=7)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 down
                    j++;
                    if((j<64)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    // 1 right, 1 down
                    j++;
                    if((j<64)&&(j%8!=0)&&(board[j]<7)){
                        int p=board[j];
                        int ec=(sqValues[i]-sqValues[j]);
                        if(p!=6){
                            if(p==0){
                                ec+=pValues[j];
                            }
                            else if(p==2){
                                ec+=nValues[j];
                            }
                            ec+=pieceValues[p];
                            if(ec>te){
                                staticEval=INT_MAX;
                                return;
                            }
                        }
                        moveArray[arraySize].all[1]=i;
                        moveArray[arraySize].all[2]=j;
                        moveArray[arraySize].all[0]=ec;
                        arraySize++;
                    }
                    break;
                }
            }
        }
        // Store array size at 0 index
        bevl=-df;
        if(arraySize==1){
            moveArray[0].all[0]=1;
            return;
        }
        moveArray[0].all[0]=arraySize;
        sort(moveArray+1,moveArray+arraySize,[](const Move &m1,const Move &m2){
            return m1.all[0]>m2.all[0];
        });
        // printMoveArray(moveArray);
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
        if(a<0||b>64||a>64){
            displayBoard();
            cout<<a<<" "<<b<<endl;
        }
        if(b<0){
            board[-b]=4+7*(-b>15);
            board[a]=6;
        }
        else{
            board[b]=board[a];
            board[a]=6;
        }
    };
    // To copy Board
    int* copyBoardArray(){
        int* ft=new int[64];
        memcpy(ft,board,256);
        return ft;
    };
    // Minimax algorithm that returns PV array
    void miniMax(){
        // if(boardDepth<3){
        //     displayBoard1();
        // }
        totalNodes++;
        // Depth reached, return evaluation
        if(boardDepth==depth){
            pvArray[0]=staticEval+wevl+bevl;//+((depth%2==0)*wevl*2)+((depth%2==1)*bevl*2);
            return;
        }
        else if(whiteToPlay){
            pvArray[0]=-alphaLimit1;
            int maxValue=-alphaLimit;
            int bestMove=1;
            wMoveGen();
            if(staticEval==INT_MAX){
                pvArray[0]=alphaLimit1;
                return;
            }
            if(moveArray[0].all[0]==1){
                pvArray[0]=0;
                return;
            }
            int evalSum=staticEval+wevl+bevl;
            if(evalSum+moveArray[1].all[0]>beta+buffer1){
                cutoffNodes++;
                pvArray[0]=alphaLimit1;
                return;
            }
            //alpha=max(evalSum-buffer5,alpha);
            for(int i=1;i<moveArray[0].all[0];i++){
                totalNodes++;
                int cevl;
                int square1=moveArray[i].all[1];
                int square2=moveArray[i].all[2];
                int evalChange=moveArray[i].all[0];
                if(evalSum+evalChange<alpha-buffer3){
                    cutoffNodes++;
                    pvArray[0]=maxValue;
                    pvArray[1]=moveArray[bestMove].all[1];
                    pvArray[2]=moveArray[bestMove].all[2];
                    return;
                }
                int piece1,piece2;
                if(square2<0){
                    // to do
                    piece1=0;
                    piece2=6;
                }
                else{
                    piece1=board[square1];
                    piece2=board[square2];
                }
                if(boardDepth==depth-1){
                    if(evalChange>qBuffer){
                        //displayBoard();
                        Board newBoard=Board(*this);
                        newBoard.boardDepth=newBoard.boardDepth-1;
                        newBoard.wevl=wevl;
                        newBoard.bevl=bevl;
                        newBoard.staticEval=staticEval+evalChange;
                        makeMove(square1,square2);
                        //displayBoard();
                        newBoard.miniMax();
                        bevl=newBoard.bevl;
                        cevl=newBoard.pvArray[0];
                        if(square2<0){
                            // to do
                            square2=-square2;
                            board[square1]=piece1;
                            board[square2]=piece2;
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
                                pvArray[1]=moveArray[i].all[1];
                                pvArray[2]=moveArray[i].all[2];
                                return;
                            }
                            maxValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                    else{
                        if(depth%2==0){
                            if(evalSum>alpha){
                                pvArray[0]=evalSum;
                                pvArray[1]=moveArray[i].all[1];
                                pvArray[2]=moveArray[i].all[2];
                                return;
                            }
                            pvArray[0]=maxValue;
                            pvArray[1]=moveArray[bestMove].all[1];
                            pvArray[2]=moveArray[bestMove].all[2];
                            return;
                        }
                        if(evalSum+evalChange>alpha){
                            pvArray[0]=evalSum+evalChange;
                            pvArray[1]=moveArray[i].all[1];
                            pvArray[2]=moveArray[i].all[2];
                            return;
                        }
                        pvArray[0]=maxValue;
                        pvArray[1]=moveArray[bestMove].all[1];
                        pvArray[2]=moveArray[bestMove].all[2];
                        return;
                        // cevl=evalSum+evalChange;
                        // if(cevl>alpha){
                        //     alpha=cevl;
                        //     // Alpha-beta cutoff
                        //     if(alpha>=beta){
                        //         cutoffNodes++;
                        //         pvArray[0]=cevl;
                        //         return;
                        //     }
                        //     maxValue=cevl;
                        //     bestMove=i;
                        // }
                        // continue;
                    }
                }
                Board newBoard=Board(*this);
                newBoard.wevl=wevl;
                newBoard.bevl=bevl;
                newBoard.staticEval=staticEval+evalChange;
                makeMove(square1,square2);
                newBoard.miniMax();
                bevl=newBoard.bevl;
                cevl=newBoard.pvArray[0];
                // if(cevl==200){
                //     displayBoard();
                //     cout<<alpha<<" "<<beta<<endl;
                //     printPV(pvArray);
                // }
                if(square2<0){
                    // to do
                    square2=-square2;
                    board[square1]=piece1;
                    board[square2]=piece2;
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
                        pvArray[1]=moveArray[i].all[1];
                        pvArray[2]=moveArray[i].all[2];
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
            pvArray[0]=alphaLimit1;
            int minValue=alphaLimit;
            int bestMove=1;
            bMoveGen();
            if(staticEval==INT_MAX){
                pvArray[0]=-alphaLimit1;
                return;
            }
            if(moveArray[0].all[0]==1){
                pvArray[0]=0;
                return;
            }
            int evalSum=staticEval+wevl+bevl;
            if(evalSum-moveArray[1].all[0]<alpha-buffer4){
                cutoffNodes++;
                pvArray[0]=-alphaLimit1;
                return;
            }
            //beta=min(evalSum+buffer5,beta);
            for(int i=1;i<moveArray[0].all[0];i++){
                totalNodes++;
                int cevl;
                int square1=moveArray[i].all[1];
                int square2=moveArray[i].all[2];
                // if(square1>64||square2>64||square1<0||square2<0){
                //     displayBoard();
                //     cout<<square1<<" "<<square2<<endl;
                // }
                int evalChange=moveArray[i].all[0];
                if(evalSum-evalChange>beta+buffer3){
                    cutoffNodes++;
                    pvArray[0]=minValue;
                    pvArray[1]=moveArray[bestMove].all[1];
                    pvArray[2]=moveArray[bestMove].all[2];
                    return;
                }
                int piece1,piece2;
                // special moves
                if(square2<0){
                    // to do
                    piece1=7;
                    piece2=6;
                }
                else{
                    piece1=board[square1];
                    piece2=board[square2];
                }
                if(boardDepth==depth-1){
                    if(evalChange>qBuffer){
                        //displayBoard();
                        Board newBoard=Board(*this);
                        newBoard.boardDepth=newBoard.boardDepth-1;
                        newBoard.wevl=wevl;
                        newBoard.staticEval=staticEval-evalChange;
                        makeMove(square1,square2);
                        //displayBoard();
                        newBoard.miniMax();
                        wevl=newBoard.wevl;
                        bevl=newBoard.bevl;
                        cevl=newBoard.pvArray[0];
                        if(square2<0){
                            square2=-square2;
                            board[square1]=piece1;
                            board[square2]=piece2;
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
                                pvArray[1]=moveArray[i].all[1];
                                pvArray[2]=moveArray[i].all[2];
                                return;
                            }
                            minValue=cevl;
                            bestMove=i;
                        }
                        continue;
                    }
                    else{
                        if(depth%2==1){
                            if(evalSum-30<beta){
                                pvArray[0]=evalSum-30;
                                pvArray[1]=moveArray[i].all[1];
                                pvArray[2]=moveArray[i].all[2];
                                return;
                            }
                            pvArray[0]=minValue;
                            pvArray[1]=moveArray[bestMove].all[1];
                            pvArray[2]=moveArray[bestMove].all[2];
                            return;
                        }
                        if(evalSum-evalChange<beta){
                            pvArray[0]=evalSum-evalChange;
                            pvArray[1]=moveArray[i].all[1];
                            pvArray[2]=moveArray[i].all[2];
                            return;
                        }
                        pvArray[0]=minValue;
                        pvArray[1]=moveArray[bestMove].all[1];
                        pvArray[2]=moveArray[bestMove].all[2];
                        return;
                        // cevl=evalSum-evalChange;
                        // if(cevl<beta){
                        //     beta=cevl;
                        //     // Alpha-beta cutoff
                        //     if(alpha>=beta){
                        //         cutoffNodes++;
                        //         pvArray[0]=cevl;
                        //         return;
                        //     }
                        //     minValue=cevl;
                        //     bestMove=i;
                        // }
                        // continue;
                    }
                }
                Board newBoard=Board(*this);
                newBoard.wevl=wevl;
                newBoard.bevl=bevl;
                newBoard.staticEval=staticEval-evalChange;
                makeMove(square1,square2);
                newBoard.miniMax();
                wevl=newBoard.wevl;
                cevl=newBoard.pvArray[0];
                if(square2<0){
                    // to do
                    square2=-square2;
                    board[square1]=piece1;
                    board[square2]=piece2;
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
                        pvArray[1]=moveArray[i].all[1];
                        pvArray[2]=moveArray[i].all[2];
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
        boardDepth=0;
        whiteToPlay=true;
        cFlagsAndKings=5071;
        alpha=-alphaLimit;
        beta=alphaLimit;
        wevl=0;
        bevl=0;
        staticEval=0;
        // wevl=146;
        // bevl=-104;
        // staticEval=35;
        // alpha=-400;
        // beta=400;
    };
    // Copy constructor for child node
    Board(const Board &oldBoard){
        whiteToPlay=!oldBoard.whiteToPlay;
        boardDepth=oldBoard.boardDepth+1;
        cFlagsAndKings=oldBoard.cFlagsAndKings;
        if(oldBoard.cFlagsAndKings!=5071){
            cout<<"yo9"<<endl;
            displayBoard();
            printMoveArray1(oldBoard.moveArray);
        }
        alpha=oldBoard.alpha;
        beta=oldBoard.beta;
        staticEval=0;
    };
    ~Board(){

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
        int square1=pv[1];
        int square2=pv[2];
        int piece1=board[square1];
        int piece2=board[square2];
        initBoard.makeMove(pv[1],pv[2]);
        displayBoard();
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
        cout<<"ec "<<evalChange<<endl;
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
        square1=c;
        square2=d;
        piece1=board[square1];
        piece2=board[square2];
        initBoard.makeMove(c,d);
        displayBoard();
        evalChange=0;
        if(piece1==9){
            evalChange+=nValues[63-square2]-nValues[63-square1];
        }
        else if(piece1==7){
            evalChange+=pValues[63-square2]-pValues[63-square1];
        }
        if(piece2<6){
            if(piece2==0){
                evalChange+=pValues[square2];
            }
            else if(piece2==2){
                evalChange+=nValues[square2];
            }
            evalChange+=pieceValues[piece2];
        }
        initBoard.staticEval-=evalChange;
        cout<<"se "<<initBoard.staticEval<<endl;
        cout<<"uc "<<evalChange<<endl;
        cout<<initBoard.wevl<<" "<<initBoard.bevl<<endl;
        int ttl=initBoard.staticEval+initBoard.wevl+initBoard.bevl;
        initBoard.alpha=pv[0]-alphaLimit;
        initBoard.beta=pv[0]+alphaLimit;
        cout<<initBoard.alpha<<endl;
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
