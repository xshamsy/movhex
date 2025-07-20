#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct Nodo Nodo;
typedef Nodo * Lista;
typedef struct MovHex MovHex;
typedef struct info info;
typedef struct Vet Vet;
typedef struct coord coord;

struct MovHex {
    int valore;
    Lista rotte;
    Lista adiacenti;
};

struct info {
    int val;
    int x;
    int y;
};

struct Vet {
    info * A;
    int size;
};

struct coord {
    int x;
    int y;
};

struct Nodo {
    coord info;
    struct Nodo * prox;
};

int check(int val);
void adj(int x, int y, MovHex * M);
void change_cost (int x, int y, int v, float r, MovHex * M);

void cancellaNodo(Lista * testa, int x2, int y2);
int cercaNodo(Lista testa, int x2, int y2);
int contaLista(Lista testa);
void stampaLista(Lista testa);
void inserisciInTesta(Lista * testa, int x, int y);

void dijkstra(MovHex * M, int x, int y, Vet * Heap, int * indexHeap);
info HEAP_EXTRACT_MIN(Vet * Heap, int * indexHeap);
void swap(Vet * Heap, int * indexHeap, int i, int j);
void MIN_HEAPIFY(Vet * Heap, int * indexHeap, int i);

int col, rows;

int main() {
    if(scanf("init %d %d", &col, &rows)==2) {
        MovHex * M;
        M=(MovHex *) malloc(col*rows*sizeof(MovHex));
        int i, j;
        for(j=col-1; j>=0; j--) {
            for(i=0; i<rows; i++) {
                M[col*i+j].valore=1;
            }
        }

        M[col*(rows-4-1)+4].valore=10;

        printf("OK\n");

        int x, y, v,x2,y2;
        float r;
        /*char line[2];
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
            }*/

            Vet Heap;
            int * indexHeap;
            Heap.A = (info *) malloc(col*rows*sizeof(info));
            Heap.size = 0;
            indexHeap = (int *) malloc(col*rows*sizeof(int));
            for(i=0; i<rows; i++) {
                for(j=0; j<col; j++) {
                    Heap.A[col*i+j].val=-1;
                    Heap.A[col*i+j].x=j;
                    Heap.A[col*i+j].y=i;
                    indexHeap[col*(rows-i-1)+j]=col*i+j;
                }
            }
            

            for(i=0; i<rows; i++) {
                if((rows-i-1)%2==0)
                    printf("(%d) ", rows-i-1);
                else
                    printf("(%d)", rows-i-1);

                for(j=0; j<col; j++) {   
                    printf("%2d ", M[col*i+j].valore);
                }
                printf("\n");
            }
            printf("   ");
            for(i=0; i<col; i++) {
                printf("(%d)", i);
            }

            x=4;
            y=4;
            x2=0;
            y2=0;
            inserisciInTesta(&M[col*(rows-1-y)+x].rotte, x2, y2);

            dijkstra(M, 4, 4, &Heap, indexHeap);
            printf("\n\ndistanza minima da (4,4) a (0,0): %d", Heap.A[indexHeap[col*(rows-0-1)+0]].val);

            printf("\n");

                        

            
        
    }
}

void change_cost (int x, int y, int v, float r, MovHex * M) {
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

void adj(int x, int y, MovHex * M) {
    if(x>=0 && x<col && y>=0 && y<rows) {
        if(y%2==0) {
            if(x+1<col) {
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x+1, y);
                if(y-1>=0)
                    inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x+1, y-1);
                if(y+1<rows)
                    inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x+1, y+1);
            }
            if(x-1>=0) 
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x-1, y);

            if(y-1>=0)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x, y-1);
            if(y+1<rows)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x, y+1);
        }
        else {
            if(x-1>=0) {
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x-1, y);
                if(y-1>=0)
                    inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x-1, y-1);
                if(y+1<rows)
                    inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x-1, y+1);
            }
            if(x+1<col) 
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x+1, y);

            if(y-1>=0)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x, y-1);
            if(y+1<rows)
                inserisciInTesta(&M[col*(rows-y-1)+x].adiacenti, x, y+1);
        }
    }
           // stampaLista(M[col*(rows-y-1)+x].adiacenti);
}

void inserisciInTesta(Lista * testa, int x, int y) {
    Lista nuovo_nodo = (Nodo*) malloc(sizeof(Nodo));
    
    nuovo_nodo->info.x = x;
    nuovo_nodo->info.y= y;
    nuovo_nodo->prox = *testa;

    *testa = nuovo_nodo;
}

void stampaLista(Lista testa) {
    Nodo * ptr=testa;
    while(ptr!=NULL) {
        printf("(%d, %d) ", (ptr->info).x, (ptr->info).y);
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

void dijkstra(MovHex * M, int x, int y, Vet * Heap, int * indexHeap) {
    info u;
    int i, p;
    Nodo * ptr_adj;
    Nodo * ptr_rotte;

    i=indexHeap[col*(rows-1-y)+x];
    (*Heap).A[i].val=0;

    swap(Heap, indexHeap, i, 0);

    (*Heap).size=col*rows-1;

    while((*Heap).size!=0) {

        u=HEAP_EXTRACT_MIN(Heap, indexHeap);

        if(M[col*(rows-1-u.y)+u.x].valore!=0) { //le piastrelle 0 non hanno adiacenti
            adj(u.x, u.y, M);
            ptr_adj=M[col*(rows-1-u.y)+u.x].adiacenti;
            ptr_rotte=M[col*(rows-1-u.y)+u.x].rotte;
            while(ptr_adj!=NULL) {
                i=indexHeap[col*(rows-((ptr_adj->info).y)-1)+(ptr_adj->info).x];
                if(i<=(*Heap).size) {
                    if((*Heap).A[i].val==-1 || (*Heap).A[i].val>u.val+M[col*(rows-1-u.y)+u.x].valore) {
                        (*Heap).A[i].val=u.val+M[col*(rows-1-u.y)+u.x].valore;
                        if(i%2==0)
                                p=i/2-1;
                            else    
                                p=i/2;
                        while(i>0 && (((*Heap).A[p].val>(*Heap).A[i].val) || ((*Heap).A[p].val==-1))) {
                            swap(Heap, indexHeap, i, p);
                            i=p;
                            if(i%2==0)
                                p=i/2-1;
                            else    
                                p=i/2;
                        }
                    }
                }
                ptr_adj=(ptr_adj)->prox;
            }

            while(ptr_rotte!=NULL) {
                i=indexHeap[col*(rows-((ptr_rotte->info).y)-1)+(ptr_rotte->info).x];
                if(i<=(*Heap).size) {
                    if((*Heap).A[i].val==-1 || (*Heap).A[i].val>u.val+M[col*(rows-1-u.y)+u.x].valore) {
                        (*Heap).A[i].val=u.val+M[col*(rows-1-u.y)+u.x].valore;
                        if(i%2==0)
                                p=i/2-1;
                            else    
                                p=i/2;
                        while(i>0 && (((*Heap).A[p].val>(*Heap).A[i].val) || ((*Heap).A[p].val==-1))) {
                            swap(Heap, indexHeap, i, p);
                            i=p;
                            if(i%2==0)
                                p=i/2-1;
                            else    
                                p=i/2;
                        }
                    }
                }
                ptr_rotte=(ptr_rotte)->prox;
            }
        }
    }
}

info HEAP_EXTRACT_MIN(Vet * Heap, int * indexHeap) {
    info min;
    min=(*Heap).A[0];
    swap(Heap, indexHeap, 0, (*Heap).size);
    (*Heap).size=(*Heap).size-1;
    MIN_HEAPIFY(Heap, indexHeap, 0);
    return min;
}

void swap(Vet * Heap, int * indexHeap, int i, int j) {
    info tmp;
    tmp=(*Heap).A[i];
    (*Heap).A[i]=(*Heap).A[j];
    (*Heap).A[j]=tmp;
    indexHeap[col*(rows-(*Heap).A[i].y-1)+(*Heap).A[i].x]=i;
    indexHeap[col*(rows-(*Heap).A[j].y-1)+(*Heap).A[j].x]=j;
}

void MIN_HEAPIFY(Vet * Heap, int * indexHeap, int i) {
    int l, r, min;
    l=2*i+1;
    r=l+1;
    if(l<=(*Heap).size && (*Heap).A[l].val!=-1) {
        if((*Heap).A[i].val==-1 || (*Heap).A[l].val<(*Heap).A[i].val)
            min=l;
        else
            min=i;
    }
    else min=i;
    
    if(r<=(*Heap).size && (*Heap).A[r].val!=-1) {
        if((*Heap).A[min].val==-1 || (*Heap).A[r].val<(*Heap).A[min].val)
            min=r;
    }

    if(min!=i) {
        swap(Heap, indexHeap, i, min);
        MIN_HEAPIFY(Heap, indexHeap, min);
    }
}



