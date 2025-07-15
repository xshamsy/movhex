#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

typedef struct Nodo Nodo;
typedef Nodo * Lista;
typedef struct MovHex MovHex;

struct MovHex {
    int valore;
    int distanza;
    int x;
    int y;
    Lista rotte;
    Lista adiacenti;
};

struct Nodo {
    MovHex info;
    struct Nodo * prox;
};

void cancellaNodo(Lista * testa, int x2, int y2);
int cercaNodo(Lista testa, int x2, int y2);
int contaLista(Lista testa);
void adj(int x, int y, int col, int rows, MovHex * M);
void stampaLista(Lista testa);
void change_cost (int x, int y, int v, float r, MovHex * M, int rows, int col);

int check(int val);
void inserisciInTesta(Lista * testa, MovHex nuovo_dato);

int main() {
    srand(time(NULL));
    int col, rows;
    if(scanf("init %d %d", &col, &rows)==2) {
        MovHex * M;
        M=(MovHex *) malloc(col*rows*sizeof(MovHex));
        int i, j;
        for(i=0; i<rows; i++) {
            for(j=0; j<col; j++) {
                M[col*(rows-i-1)+j].valore=1;
                M[col*(rows-i-1)+j].x=j;
                M[col*(rows-i-1)+j].y=i;
            }
        }
        
        //M[col*(rows-6-1)+10].valore=5;   // M[2][3]
        printf("OK\n");

        int x, y, v,x2,y2;
        float r;
        char line[2];
        while(1) {
            if(scanf("\nchange_cost %d %d %d %f", &x, &y, &v, &r)) {
                if(x>=0 && x<col && y>=0 && y<rows && r>0 && v>=-10 && v<=10) {
                    change_cost(x,y,v,r, M, rows, col);
                    printf("OK\n");
                }
                else
                    printf("KO\n");
            }
            if(scanf("\ntoggle_air_route %d %d %d %d", &x, &y, &x2, &y2)) {
                if(cercaNodo(M[col*(rows-1-y)+x].rotte, x2, y2)) {
                    cancellaNodo(&M[col*(rows-1-y)+x].rotte, x2, y2);
                    printf("OK\n");

                }
                else if(contaLista(M[col*(rows-1-y)+x].rotte)==5) printf("KO\n");
                else {
                    inserisciInTesta(&M[col*(rows-1-y)+x].rotte, M[col*(rows-1-y2)+x2]);
                    printf("OK\n");
                }
                //stampaLista(M[col*(rows-1-y)+x].rotte);
            }
        }
        
        

        //adj(3,8,col,rows,M);

        printf("\n");

        for(i=0; i<rows; i++) {
            if((rows-i-1)%2==0)
                printf(" ");
            for(j=0; j<col; j++) {
                printf("%2d ", M[col*i+j].valore);
            }
            printf("\n");
        }
    }
}

void change_cost (int x, int y, int v, float r, MovHex * M, int rows, int col) {
    int j;
    int k, row_sx, row_dx, l;
    M[col*(rows-1-y)+x].valore=check(v+M[col*(rows-1-y)+x].valore);

    if(y%2==0) {
            k=1;
            while(k<=r) {
                row_sx=1;
                row_dx=2;
                
                l=k;
                if(x-k >=0)
                    M[col*(rows-y-1)+x-k].valore=check((r-k)/r * v + M[col*(rows-y-1)+x-k].valore);
                if(x+k < col) {
                    if(y-1>=0)
                        M[col*(rows-y-2)+x+k].valore=check((r-k)/r * v + M[col*(rows-y-2)+x+k].valore);
                    if(y+1<rows)
                        M[col*(rows-y)+x+k].valore=check((r-k)/r * v + M[col*(rows-y)+x+k].valore);
                    M[col*(rows-y-1)+x+k].valore=check((r-k)/r * v + M[col*(rows-y-1)+x+k].valore);
                }
                

                while(row_dx!=k && row_sx!=k) {   
                    l--;
                    if(x-l >=0) {
                        if(y+row_sx < rows)
                            M[col*(rows-(y+row_sx)-1)+x-l].valore=check((r-k)/r * v + M[col*(rows-(y+row_sx)-1)+x-l].valore);
                        if(y+row_sx+1 < rows)
                            M[col*(rows-(y+row_sx+1)-1)+x-l].valore=check((r-k)/r * v + M[col*(rows-(y+row_sx+1)-1)+x-l].valore);
                        if(y-row_sx>=0)
                            M[col*(rows-1-(y-row_sx))+x-l].valore=check((r-k)/r * v +  M[col*(rows-1-(y-row_sx))+x-l].valore);
                        if(y-row_sx-1>=0)
                            M[col*(rows-1-(y-row_sx-1))+x-l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_sx-1))+x-l].valore);

                    }
                    if(x+l<col) {
                        if(y+row_dx < rows)
                            M[col*(rows-1-(y+row_dx))+x+l].valore=check((r-k)/r * v + M[col*(rows-1-(y+row_dx))+x+l].valore);
                        if(y+row_dx+1<rows)
                            M[col*(rows-(y+row_dx+1)-1)+x+l].valore=check((r-k)/r * v + M[col*(rows-(y+row_dx+1)-1)+x+l].valore);
                        if(y-row_dx>=0)
                            M[col*(rows-1-(y-row_dx))+x+l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_dx))+x+l].valore);
                        if(y-row_dx-1>=0)
                            M[col*(rows-1-(y-row_dx-1))+x+l].valore=check((r-k)/r * v +M[col*(rows-1-(y-row_dx-1))+x+l].valore);
                    }
                    row_sx=row_sx+2;
                    row_dx=row_dx+2;
                }

                if(row_sx==k) {
                    j=0;
                    do{
                        if(x-l+1+j >=0 && x-l+1+j<col) {
                            if(y+row_sx<rows)
                                M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore);
                            if(y-row_sx>=0)
                                M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore);
                        }
                        j++;
                    }while(j!=2*l-1);
                }
                if(row_dx==k) {
                    j=0;
                    do{
                        if(x-l+1+j >=0 && x-l+1+j<col) {
                            if(y+row_dx<rows)
                                M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore);

                            if(y-row_dx>=0)
                                M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore);
                        }
                        j++;
                    }while(j!=2*l-1);
                    if(x-l+1 >=0 && x-l+1<col) {
                        if(y+row_dx-1 < rows && y+row_dx-1>=0)
                            M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore=check((r-k)/r * v + M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore);
                        if(y-row_dx+1 >=0 && y-row_dx+1<rows)
                            M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore=check((r-k)/r * v + M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore);
                    }
                }
                k++;
            }
        
    }
    else {
            k=1;
            while(k<=r) {
                row_sx=2;
                row_dx=1;
            
                l=k;
                if(x-k >=0){
                    M[col*(rows-y-1)+x-k].valore=check((r-k)/r * v + M[col*(rows-y-1)+x-k].valore);
                    if(y-1>=0)
                        M[col*(rows-y-2)+x-k].valore=check((r-k)/r * v + M[col*(rows-y-2)+x-k].valore);
                    if(y+1<rows)
                        M[col*(rows-y)+x-k].valore=check((r-k)/r * v + M[col*(rows-y)+x-k].valore);
                }
                if(x+k < col)
                    M[col*(rows-y-1)+x+k].valore=check((r-k)/r * v + M[col*(rows-y-1)+x+k].valore);
                
                while(row_dx!=k && row_sx!=k) {
                    l--;
                    if(x-l>=0) {
                        if(y+row_sx<rows)
                            M[col*(rows-(y+row_sx)-1)+x-l].valore=check((r-k)/r * v + M[col*(rows-(y+row_sx)-1)+x-l].valore);
                        if(y+row_sx+1<rows)
                            M[col*(rows-(y+row_sx+1)-1)+x-l].valore=check((r-k)/r * v + M[col*(rows-(y+row_sx+1)-1)+x-l].valore);
                        if(y-row_sx>=0)
                            M[col*(rows-1-(y-row_sx))+x-l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_sx))+x-l].valore);
                        if(y-row_sx-1>=0)
                            M[col*(rows-1-(y-row_sx-1))+x-l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_sx-1))+x-l].valore);
                    }
                    if(x+l<col) {
                        if(y+row_dx<rows)
                            M[col*(rows-1-(y+row_dx))+x+l].valore=check((r-k)/r * v + M[col*(rows-1-(y+row_dx))+x+l].valore);
                        if(y+row_dx+1<rows)
                            M[col*(rows-(y+row_dx+1)-1)+x+l].valore=check((r-k)/r * v + M[col*(rows-(y+row_dx+1)-1)+x+l].valore);
                        if(y-row_dx>=0)
                            M[col*(rows-1-(y-row_dx))+x+l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_dx))+x+l].valore);
                        if(y-row_dx-1>=0)
                            M[col*(rows-1-(y-row_dx-1))+x+l].valore=check((r-k)/r * v + M[col*(rows-1-(y-row_dx-1))+x+l].valore);
                    }
                    row_sx=row_sx+2;
                    row_dx=row_dx+2;
                }

                
                if(row_dx==k) {
                    j=0;
                    do{
                        if(x-l+1+j>=0 && x-l+1+j<col) {
                            if(y+row_dx<rows)
                                M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore);
                            if(y-row_dx>=0)
                                M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore);
                        }
                        j++;
                    }while(j!=2*l-1);
                }
                
                if(row_sx==k) {
                    j=0;
                    do{
                        if(x-l+1+j>=0 && x-l+1+j<col) {
                            if(y+row_sx<rows)
                                M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore);
                            if(y-row_sx>=0)
                                M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=check((r-k)/r * v + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore);
                        }
                        j++;
                    }while(j!=2*l-1);
                    if(x+l-1>=0 && x+l-1<col) {
                        if(y+row_sx-1>=0 && y+row_sx-1<rows)
                            M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore=check((r-k)/r * v + M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore);
                        if(y-row_sx+1>=0 && y-row_sx+1<rows)
                            M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore=check((r-k)/r * v + M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore);
                    }
                }
                k++;
            }
        
    }   
}

int check(int val) {

    if(val<0)
        return 0;
    if(val>100)
        return 100;
    return val;
}

void adj(int x, int y, int col, int rows, MovHex * M) {
    if(y%2==0) {
        if(x+1<col) {
            inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, M[col*(rows-y-1)+(x+1)]);
            if(y-1>=0)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, M[col*(rows-y-2)+(x+1)]);
            if(y+1>=0)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti,M[col*(rows-y)+(x+1)]);
        }
        if(x-1>=0) 
            inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti,M[col*(rows-1-y)+(x-1)]);

        if(x-1>=0)
            inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti,M[col*(rows-y-2)+x]);
        if(x+1<rows)
            inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti,M[col*(rows-y)+x]);
    }
    stampaLista(M[col*(rows-y-1)+x].adiacenti);
}



void inserisciInTesta(Lista * testa, MovHex nuovo_dato) {
    Lista nuovo_nodo = (Nodo*) malloc(sizeof(Nodo));
    
    nuovo_nodo->info = nuovo_dato;

    nuovo_nodo->prox = *testa;

    *testa = nuovo_nodo;
}

void stampaLista(Lista testa) {
    Nodo * ptr=testa;
    while(ptr!=NULL) {
        printf("%d ", (ptr->info).valore);
        ptr=ptr->prox;
    }
}

int contaLista(Lista testa) {
    Nodo * ptr=testa;
    int conta=0;
    while(ptr!=NULL) {
        conta++;
        ptr=ptr->prox;
    }
    return conta;
}

int cercaNodo(Lista testa, int x2, int y2) {
    Nodo * ptr=testa;
    
    while(ptr!=NULL) {
        if((ptr->info).x == x2 && (ptr->info).y== y2) return 1;
        ptr=ptr->prox;
    }
    return 0;
}

void cancellaNodo(Lista * testa, int x2, int y2) {
    Nodo * ptr=*testa;
    if(ptr->info.x == x2 && ptr->info.y==y2) {
        *testa=ptr->prox;
        free(ptr);
        return;
    }
    else {
        while(ptr->prox->info.x != x2 && ptr->prox->info.y != y2)
            ptr=ptr->prox;
        Lista tmp = ptr->prox;
        ptr->prox = ptr->prox->prox;
        free(tmp);
    }
}
