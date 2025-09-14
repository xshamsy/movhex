#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define DIM_HASH 300
#define i_HASH 10
#define a 1
#define b 1

typedef struct hash hash;
typedef struct Nodo Nodo;
typedef Nodo * Lista;
typedef struct MovHex MovHex;
typedef struct Vet Vet;
typedef struct Mov Mov;


struct hash {
    __uint32_t val;
    __uint32_t x1;
    __uint32_t y1;
    __uint32_t x2;
    __uint32_t y2;
};

struct MovHex {
    Lista rotte;
    __uint8_t valore;
};

struct Mov {
    __uint32_t val;
    unsigned visitato;
};

struct Vet {
    Mov ** ptr;
};

struct Nodo {
    __uint32_t info;
    struct Nodo * prox;
};
int dijkstra(Mov * A, MovHex * M, int x, int y, int x2, int y2, Vet * Heap);
int check(int val);
void adiacenti(Lista * adj, int x, int y, MovHex * M);
void change_cost (int x, int y, int v, float r, MovHex * M);
void liberaLista(Lista * l);
void cancellaNodo(Lista * testa, __uint32_t coord);
int cercaNodo(Lista testa, __uint32_t coord);
int contaLista(Lista testa);
void stampaLista(Lista testa);
void inserisciInTesta(Lista * testa, __uint32_t coord);
int dijkstra(Mov * A, MovHex * M, int x, int y, int x2, int y2, Vet * Heap);
int HEAP_EXTRACT_MIN(Vet * Heap, Mov * A);
void MIN_HEAPIFY(Vet * Heap, int i, Mov * A);
int ricerca(hash * v, int x1, int y1, int x2, int y2, MovHex * M, Mov * A, Vet * Heap);
void inizializza(hash * v);
//void stampa(hash * v);
int h(int x1, int y1, int x2, int y2, int i);
void HEAPIFY_DOWN (int i, int val, Vet * Heap, Mov * A);
int adj(int x, int y, int * adiacenti, int risultato);

unsigned dijk=1;

int col=-1;
int rows=-1;

int main() {
    hash * vet;
    Vet Heap;
    Mov * A=NULL;
    Heap.ptr = NULL;
    vet = (hash *) malloc(sizeof(hash) * DIM_HASH);
    MovHex * M=NULL;
    char c='p';
    int i=0, j=0;
    int v=0, x=0, y=0, x2=0, y2=0;
    float r=0;
    inizializza(vet);
    c=getchar();
    while(c!=EOF) {

        if(c=='i') {

            if(col!=-1) {
                for(i=0; i<rows; i++) {
                    for(j=0; j<col; j++) {
                        liberaLista(&M[col*i+j].rotte);
                        //liberaLista(&M[col*i+j].adiacenti);
                    }
                }
            }
        
            if(scanf("nit %d %d", &col, &rows)==2) {
               dijk=1;

                if (Heap.ptr != NULL) {
                    free(Heap.ptr);
                    Heap.ptr=NULL;
                }
                
                 if (M != NULL) {
                    free(M);
                    M = NULL;
                }

                if (A != NULL) {
                    free(A);
                    A = NULL;
                }
                
                Heap.ptr = (Mov **) malloc(col*rows*sizeof(Mov*));
                A = (Mov *) malloc(col*rows*sizeof(Mov));
                M = (MovHex *) malloc(col*rows*sizeof(MovHex));

                for(i=0; i<rows; i++) {
                    for(j=0; j<col; j++) {
                        M[col*(rows-i-1)+j].valore=1;
                        M[col*(rows-i-1)+j].rotte=NULL;
                    }
                }

                printf("OK\n");
            }
        }
        else if(c=='c') {
            if(scanf("hange_cost %d %d %d %f", &i, &j, &v, &r)==4) {
                if(i>=0 && i<col && j>=0 && j<rows && r>0 && v>=-10 && v<=10) {
                    inizializza(vet);
                    change_cost(i,j,v,r, M);
                    dijk=0;
                    printf("OK\n");
                }
                else
                    printf("KO\n");
            }
        }
        else if(c=='t') {
            c=getchar();
            if(c=='o') {
                if(scanf("ggle_air_route %d %d %d %d", &i, &j, &x2, &y2)==4) {
                    
                    if(i<0 || i>=col || rows-j-1<0 || rows-1-j>=rows || x2<0 || x2>=col || rows-y2-1<0 || rows-1-y2>=rows)
                        printf("KO\n");
                    else if(cercaNodo(M[col*(rows-1-j)+i].rotte, col*(rows-1-y2)+x2)) {
                        inizializza(vet);
                        dijk=0;
                        cancellaNodo(&M[col*(rows-1-j)+i].rotte, col*(rows-1-y2)+x2);
                        printf("OK\n");
                    }
                    else if(contaLista(M[col*(rows-1-j)+i].rotte)==5) printf("KO\n");
                    else {
                        inizializza(vet);
                        dijk=0;
                        inserisciInTesta(&M[col*(rows-1-j)+i].rotte, col*(rows-1-y2)+x2);
                        printf("OK\n");
                    }
                }
            }
        
            else if(c=='r') {
                if(scanf("avel_cost %d %d %d %d", &x, &y, &x2, &y2)==4) {
                    if(x<0 || x>=col || rows-y-1<0 || rows-1-y>=rows || x2<0 || x2>=col || rows-y2-1<0 || rows-1-y2>=rows)
                        printf("-1\n");
                    else if(x==x2 && y==y2)
                        printf("0\n");
                    else if(M[col*(rows-1-y)+x].valore==0)
                        printf("-1\n");
                    else if(cercaNodo(M[col*(rows-1-y)+x].rotte, col*(rows-1-y2)+x2)) 
                        printf("%d\n", M[col*(rows-1-y)+x].valore);
                    else if(dijk==1) {
                        int d=0, h=0;
                        if(x==x2)
                            d=abs(y2-y);
                        else if(y==y2)
                            d=abs(x-x2);
                        else if(x>x2 && d==0) {
                            if(y>y2) {
                                d=y-y2;
                                h=y2%2!=0 && y%2==0 ? x-((int)((y-y2)/2)+1) : x-((int)((y-y2)/2));
                                if(h>=0 && h-x2>0)
                                    d=d+h-x2;
                            }
                            else{
                                d=y2-y;
                                h=y2%2!=0 && y%2==0 ? x-((int)((y2-y)/2)+1) : x-((int)((y2-y)/2));
                                if(h>=0 && h-x2>0)
                                    d=d+h-x2;
                            }
                        }
                        else if(x2>x) {
                            if(y2>y) {
                                d=y2-y;
                                h=y%2!=0 && y2%2==0 ? x2-((int)((y2-y)/2)+1) : x2-((int)((y2-y)/2));
                                if(h>=0 && h-x>0)
                                    d=d+h-x;
                            }
                            else{
                                d=y-y2;
                                h=y%2!=0 && y2%2==0 ? x2-((int)((y-y2)/2)+1) : x2-((int)((y-y2)/2));
                                if(h>=0 && h-x>0)                                    
                                    d=d+h-x; 
                            }
                        }
                        printf("%d\n", d);
                    }
                    else {
                        printf("%d\n", ricerca(vet, x, y, x2, y2, M, A, &Heap));
                    }
                }
            }
        }  
        c=getchar();            
    }

    if(col!=-1) {
        for(i=0; i<rows; i++) {
            for(j=0; j<col; j++) {
                liberaLista(&M[col*i+j].rotte);
                //liberaLista(&M[col*i+j].adiacenti);
            }
        }
    }
        
  
    free(Heap.ptr);          
                
    if (M != NULL) {
        free(M);
        M = NULL;
    }
    free(vet);
    vet=NULL;
        
}  

int count=1;

int ricerca(hash * v, int x1, int y1, int x2, int y2, MovHex * M, Mov * A, Vet * Heap) {
    int pos;
    int i=0;
    pos=h(x1,y1,x2,y2,i);
    while((v[pos].x1!=x1 || v[pos].y1!=y1 || v[pos].x2!=x2  || v[pos].y2!=y2) && (v[pos].val != -1) && (i<=i_HASH)) {
        i++;
        pos=h(x1,y1,x2,y2,i);
    }
    
    if(i>i_HASH) {
        printf("[%d]", count);
        count++;
        pos=h(x1,y1,x2,y2,0);
        v[pos].val=dijkstra(A, M, x1, y1, x2, y2, Heap); 
        v[pos].x1=x1;
        v[pos].y1=y1;
        v[pos].x2=x2;
        v[pos].y2=y2;
        return v[pos].val;
    }
    if(v[pos].val==-1) {
        v[pos].val=dijkstra(A, M, x1, y1, x2, y2, Heap); 
        v[pos].x1=x1;
        v[pos].y1=y1;
        v[pos].x2=x2;
        v[pos].y2=y2;
        return v[pos].val;
    }

    return v[pos].val;
}    

/*void stampa(hash * v) {
    int i;
    for(i=0; i<DIM_HASH; i++) {
        printf("[%d]", v[i].val);
    }
    printf("\n");
}*/

void inizializza(hash * v) {
    int i;
    for(i=0; i<DIM_HASH; i++) {
        v[i].val=-1;
        v[i].x1=-1;
        v[i].y1=-1;
        v[i].x2=-1;
        v[i].y2=-1;
    }
}

int h(int x1, int y1, int x2, int y2, int i) {
    return ((y1*col+x1)+a*i+b*i*i)%DIM_HASH;
}

void change_cost (int x, int y, int v, float r, MovHex * M) {
    int j=0;
    int k=0, row_sx=0, row_dx=0, l=0;
        float mul=0;

    M[col*(rows-1-y)+x].valore=check(v+M[col*(rows-1-y)+x].valore);

    if(y%2!=0) {
        k=1;
        while(k<=r) {
            mul=(r-k)/r *v;
            if(mul>=0)
                mul= (int) mul;
            else {
                if((int) mul - mul != 0)
                    mul=mul-1;
            }
                
            mul= (int) mul; 

            row_sx=1;
            row_dx=2;
            
            l=k;
            if(x-k >=0)
                M[col*(rows-y-1)+x-k].valore=check(mul + M[col*(rows-y-1)+x-k].valore);
            if(x+k < col) {
                if(y+1<rows)
                    M[col*(rows-y-2)+x+k].valore=check(mul + M[col*(rows-y-2)+x+k].valore);
                M[col*(rows-y)+x+k].valore=check(mul + M[col*(rows-y)+x+k].valore);
                M[col*(rows-y-1)+x+k].valore=check(mul + M[col*(rows-y-1)+x+k].valore);
            }
                

            while(row_dx!=k && row_sx!=k) {   
                l--;
                if(x-l >=0) {
                    if(y+row_sx < rows)
                        M[col*(rows-(y+row_sx)-1)+x-l].valore=check(mul + M[col*(rows-(y+row_sx)-1)+x-l].valore);
                    if(y+row_sx+1 < rows)
                        M[col*(rows-(y+row_sx+1)-1)+x-l].valore=check(mul + M[col*(rows-(y+row_sx+1)-1)+x-l].valore);
                    if(y-row_sx>=0)
                        M[col*(rows-1-(y-row_sx))+x-l].valore=check(mul +  M[col*(rows-1-(y-row_sx))+x-l].valore);
                    if(y-row_sx-1>=0)
                        M[col*(rows-1-(y-row_sx-1))+x-l].valore=check(mul + M[col*(rows-1-(y-row_sx-1))+x-l].valore);
                }
                if(x+l<col) {
                    if(y+row_dx < rows)
                        M[col*(rows-1-(y+row_dx))+x+l].valore=check(mul + M[col*(rows-1-(y+row_dx))+x+l].valore);
                    if(y+row_dx+1<rows)
                        M[col*(rows-(y+row_dx+1)-1)+x+l].valore=check(mul + M[col*(rows-(y+row_dx+1)-1)+x+l].valore);
                    if(y-row_dx>=0)
                        M[col*(rows-1-(y-row_dx))+x+l].valore=check(mul + M[col*(rows-1-(y-row_dx))+x+l].valore);
                    if(y-row_dx-1>=0)
                        M[col*(rows-1-(y-row_dx-1))+x+l].valore=check(mul +M[col*(rows-1-(y-row_dx-1))+x+l].valore);
                }
                row_sx=row_sx+2;
                row_dx=row_dx+2;
            }

            if(row_sx==k) {
                j=0;
                do{
                    if(x-l+1+j >=0 && x-l+1+j<col) {
                        if(y+row_sx<rows)
                            M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore);
                        if(y-row_sx>=0)
                            M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore);
                    }
                    j++;
                }while(j!=2*l-1);
            }
            if(row_dx==k) {
                j=0;
                do{
                    if(x-l+1+j >=0 && x-l+1+j<col) {
                        if(y+row_dx<rows)
                            M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore);

                        if(y-row_dx>=0)
                            M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore);
                    }
                    j++;
                }while(j!=2*l-1);
                if(x-l+1 >=0 && x-l+1<col) {
                    if(y+row_dx-1 < rows && y+row_dx-1>=0)
                        M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore=check(mul + M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore);
                    if(y-row_dx+1 >=0 && y-row_dx+1<rows)
                        M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore=check(mul + M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore);
                }
            }
            k++;
        }
    }
    else {
        k=1;
        while(k<=r) {
            mul=(r-k)/r * v;
            if(mul>=0)
                mul= (int) mul;
            else {
                if((int) mul - mul != 0)
                    mul=mul-1;
            }

            mul=(int) mul;

            row_sx=2;
            row_dx=1;
            
            l=k;
            if(x-k >=0){
                M[col*(rows-y-1)+x-k].valore=check(mul + M[col*(rows-y-1)+x-k].valore);
                if(y+1<rows)
                    M[col*(rows-y-2)+x-k].valore=check(mul + M[col*(rows-y-2)+x-k].valore);
                M[col*(rows-y)+x-k].valore=check(mul + M[col*(rows-y)+x-k].valore);
            }
            if(x+k < col)
                M[col*(rows-y-1)+x+k].valore=check(mul + M[col*(rows-y-1)+x+k].valore);
                
            while(row_dx!=k && row_sx!=k) {
                l--;
                if(x-l>=0) {
                    if(y+row_sx<rows)
                        M[col*(rows-(y+row_sx)-1)+x-l].valore=check(mul + M[col*(rows-(y+row_sx)-1)+x-l].valore);
                    if(y+row_sx+1<rows)
                        M[col*(rows-(y+row_sx+1)-1)+x-l].valore=check(mul + M[col*(rows-(y+row_sx+1)-1)+x-l].valore);
                    if(y-row_sx>=0)
                        M[col*(rows-1-(y-row_sx))+x-l].valore=check(mul + M[col*(rows-1-(y-row_sx))+x-l].valore);
                    if(y-row_sx-1>=0)
                        M[col*(rows-1-(y-row_sx-1))+x-l].valore=check(mul + M[col*(rows-1-(y-row_sx-1))+x-l].valore);
                }
                if(x+l<col) {
                    if(y+row_dx<rows)
                        M[col*(rows-1-(y+row_dx))+x+l].valore=check(mul + M[col*(rows-1-(y+row_dx))+x+l].valore);
                    if(y+row_dx+1<rows)
                        M[col*(rows-(y+row_dx+1)-1)+x+l].valore=check(mul + M[col*(rows-(y+row_dx+1)-1)+x+l].valore);
                    if(y-row_dx>=0)
                        M[col*(rows-1-(y-row_dx))+x+l].valore=check(mul + M[col*(rows-1-(y-row_dx))+x+l].valore);
                    if(y-row_dx-1>=0)
                        M[col*(rows-1-(y-row_dx-1))+x+l].valore=check(mul + M[col*(rows-1-(y-row_dx-1))+x+l].valore);
                }
                row_sx=row_sx+2;
                row_dx=row_dx+2;
            }

            if(row_dx==k) {
                j=0;
                do{
                    if(x-l+1+j>=0 && x-l+1+j<col) {
                        if(y+row_dx<rows)
                            M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore);
                        if(y-row_dx>=0)
                            M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore);
                    }
                    j++;
                }while(j!=2*l-1);
            }
                
            if(row_sx==k) {
                j=0;
                do{
                    if(x-l+1+j>=0 && x-l+1+j<col) {
                        if(y+row_sx<rows)
                            M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore);
                        if(y-row_sx>=0)
                            M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=check(mul + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore);
                    }
                    j++;
                }while(j!=2*l-1);
                if(x+l-1>=0 && x+l-1<col) {
                    if(y+row_sx-1>=0 && y+row_sx-1<rows)
                        M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore=check(mul + M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore);
                    if(y-row_sx+1>=0 && y-row_sx+1<rows)
                        M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore=check(mul + M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore);
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

void adiacenti(Lista * adj, int x, int y, MovHex * M) {
    if(x>=0 && x<col && y>=0 && y<rows) {
        if(y%2!=0) {
            if(x+1<col) {
                inserisciInTesta(adj, col*(rows-y-1)+x+1);
                if(y-1>=0)
                    inserisciInTesta(adj, col*(rows-(y-1)-1)+x+1);
                if(y+1<rows)
                    inserisciInTesta(adj, col*(rows-(y+1)-1)+x+1);
            }
            if(x-1>=0) 
                inserisciInTesta(adj, col*(rows-y-1)+x-1);

            if(y-1>=0)
                inserisciInTesta(adj, col*(rows-(y-1)-1)+x);
            if(y+1<rows)
                inserisciInTesta(adj, col*(rows-(y+1)-1)+x);
        }
        else {
            if(x-1>=0) {
                inserisciInTesta(adj, col*(rows-y-1)+x-1);
                if(y-1>=0)
                    inserisciInTesta(adj, col*(rows-(y-1)-1)+x-1);
                if(y+1<rows)
                    inserisciInTesta(adj, col*(rows-(y+1)-1)+x-1);
            }
            if(x+1<col) 
                inserisciInTesta(adj, col*(rows-y-1)+x+1);

            if(y-1>=0)
                inserisciInTesta(adj, col*(rows-(y-1)-1)+x);
            if(y+1<rows)
                inserisciInTesta(adj, col*(rows-(y+1)-1)+x);
        }
    }
}

void inserisciInTesta(Lista * testa, __uint32_t coord) {
    Lista nuovo_nodo = (Nodo*) malloc(sizeof(Nodo));
    
    nuovo_nodo->info = coord;
    nuovo_nodo->prox = *testa;

    *testa = nuovo_nodo;
}

void stampaLista(Lista testa) {
    Nodo * ptr=testa;
    while(ptr!=NULL) {
        printf("(%d, %d) ", (ptr->info)%col, rows-1-(ptr->info)/col);
        ptr=ptr->prox;
    }
    printf("\n");
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

int cercaNodo(Lista testa, __uint32_t coord) {
    Nodo * ptr=testa;
    
    while(ptr!=NULL) {
        if((ptr->info)==coord) return 1;
        ptr=ptr->prox;
    }
    return 0;
}

void cancellaNodo(Lista * testa, __uint32_t coord) {
    Nodo * ptr=*testa;
    if(ptr->info==coord) {
        *testa=ptr->prox;
        free(ptr);
        return;
    }
    else {
        while(ptr->prox->info==coord)
            ptr=ptr->prox;
        Lista tmp = ptr->prox;
        ptr->prox = ptr->prox->prox;
        free(tmp);
    }
}

int pos_current;


int adj(int x, int y, int * adiacenti, int risultato) {

    int index=0;

    if(y-1>=0) {
                adiacenti[index]=risultato+col;
                index++;
            }
            if(y+1<rows) {
                adiacenti[index]=risultato-col;
                index++;
            }
            if(y%2!=0) {
                if(x+1<col) {
                    adiacenti[index]=risultato+1;
                    index++;
                    if(y-1>=0) {
                        adiacenti[index]=risultato+col+1;
                        index++;
                    }
                    if(y+1<rows) {
                        adiacenti[index]=risultato-col+1;
                        index++;
                    }
                }
                if(x-1>=0) {
                    adiacenti[index]=risultato-1;
                    index++;
                } 
            }
            else {
                 if(x-1>=0) {
                    adiacenti[index]=risultato-1;
                    index++;
                    if(y-1>=0) {
                        adiacenti[index]=risultato+col-1;
                        index++;
                    }
                    if(y+1<rows) {
                        adiacenti[index]=risultato-col-1;
                        index++;
                    }
                }
                if(x+1<col) {
                    adiacenti[index]=risultato+1;
                    index++;
                } 
            }
        return index;
}

int dijkstra(Mov * A, MovHex * M, int x, int y, int x2, int y2, Vet * Heap) {
    int u;
    int adiacenti[6]={0};
    int index=0;
    __uint32_t coord;
    int i;
    Mov * temp;
    Nodo * ptr_rotte;
    pos_current=0;
    int destinazione;


    for(i=0; i<rows*col; i++) {
        A[i].val=-1;
        A[i].visitato=0;  
    }

    u=col*(rows-1-y)+x;
    destinazione=col*(rows-1-y2)+x2;

    A[u].val=0;
    A[u].visitato=1;


    while(1) {
        if(M[u].valore!=0) { 
            x=u%col;
            y=(rows-1)-(u/col);

            index=adj(x, y, adiacenti, u);

            
            int valore;
            valore=A[u].val+M[u].valore;
            
            for(i=0; i<index; i++) {
                temp=&A[adiacenti[i]];
                if((*temp).visitato==0) {
                    if((*temp).val==-1 || (*temp).val>valore) {
                        (*temp).val=valore;
                        (*Heap).ptr[pos_current]=temp;
                        pos_current++;

                        HEAPIFY_DOWN(pos_current-1,A[(*Heap).ptr[pos_current-1]-A].val,Heap,A);
                    }
                }
            }
            
            ptr_rotte=M[u].rotte;

            //stampaLista(ptr_adj);
        

            while(ptr_rotte!=NULL) {
                coord=(ptr_rotte->info);
                temp=&A[coord];

                if((*temp).visitato==0) {
                    if((*temp).val==-1 || (*temp).val>valore) {
                        (*temp).val=valore;
                        (*Heap).ptr[pos_current]=temp;
                        pos_current++;

                        HEAPIFY_DOWN(pos_current-1,A[(*Heap).ptr[pos_current-1]-A].val,Heap,A);
                    }
                }

                ptr_rotte=(ptr_rotte)->prox;
            }
        }

        u=HEAP_EXTRACT_MIN(Heap, A);
        if(u==-1) 
            return -1;
        
        if(u==destinazione)
            return A[u].val;
        
    }
    return A[u].val;
}

void HEAPIFY_DOWN (int i, int val, Vet * Heap, Mov * A) {
    int p;
    Mov * temp;
    while(i>0) {
        p=(i-1)/2;

        if(val < A[(*Heap).ptr[p]-A].val) {
                
        temp=(*Heap).ptr[i];
        (*Heap).ptr[i]=(*Heap).ptr[p];
        (*Heap).ptr[p]=temp;

        i=p;
        }
        else return;
    }
    return;
}

int HEAP_EXTRACT_MIN(Vet * Heap, Mov * A) {
    int min;
    if(pos_current==0) return -1;

    while(A[(*Heap).ptr[0]-A].visitato) {
        pos_current--;
        if(pos_current==0) return -1;
        (*Heap).ptr[0]=(*Heap).ptr[pos_current];
    }

    min=(*Heap).ptr[0]-A;
    A[min].visitato=1;

    pos_current--;

    (*Heap).ptr[0]=(*Heap).ptr[pos_current];

    MIN_HEAPIFY(Heap, 0, A);
    return min;
}

void MIN_HEAPIFY(Vet * Heap, int i, Mov * A) {
    int l, min;
    Mov * tmp;
    
    while(1) {
        min=i;
        l=2*i+1;
        //r=l+1;
        
        if(l<pos_current) {
            if(A[(*Heap).ptr[l]-A].val<A[(*Heap).ptr[i]-A].val) 
                min=l;
        }

        else break;

        if(l+1<pos_current) {
            if(A[(*Heap).ptr[l+1]-A].val<A[(*Heap).ptr[min]-A].val)
                min=l+1;
        }
        
        
        if(min!=i) {

            tmp=(*Heap).ptr[i];
            (*Heap).ptr[i]=(*Heap).ptr[min];
            (*Heap).ptr[min]=tmp;

            i=min;
        }
        else break;
    } 

}

void liberaLista(Lista * l) {
    Nodo *tmp;
    while ((*l)!=NULL) {
        tmp=*l;
        *l=(*l)->prox;
        free(tmp);
    }
    (*l)=NULL;
}