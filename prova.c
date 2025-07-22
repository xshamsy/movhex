// riga 258
// change_cost 23 97 7 89

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
void liberaLista(Lista * l);
void cancellaNodo(Lista * testa, int x2, int y2);
int cercaNodo(Lista testa, int x2, int y2);
int contaLista(Lista testa);
void stampaLista(Lista testa);
void inserisciInTesta(Lista * testa, int x, int y);

int dijkstra(MovHex * M, int x, int y, int x2, int y2, Vet * Heap, int * indexHeap);
info HEAP_EXTRACT_MIN(Vet * Heap, int * indexHeap);
void swap(Vet * Heap, int * indexHeap, int i, int j);
void MIN_HEAPIFY(Vet * Heap, int * indexHeap, int i);

int col=-1;
int rows=-1;

int main() {
    MovHex * M=NULL;
    Vet Heap;
    Heap.A=NULL;
    int * indexHeap;
    indexHeap=NULL;
    Heap.size = 0;
    char c='p';
    int i=0, j=0;
    int v=0, x=0, y=0, x2=0, y2=0;
    float r=0;
    c=getchar();
    while(c!=EOF) {
        if(c=='i') {

            if(col!=-1) {
                for(i=0; i<rows; i++) {
                    for(j=0; j<col; j++) {
                        liberaLista(&M[col*i+j].rotte);
                    }
                }
            }
        
            if(scanf("nit %d %d", &col, &rows)==2) {
                if (M != NULL) {
                    free(M);
                    M = NULL;
                }
                if (Heap.A != NULL) {
                    free(Heap.A);
                    Heap.A=NULL;
                }
                if (indexHeap != NULL) {
                    free(indexHeap);
                    indexHeap=NULL;
                }
                    
                M=(MovHex *) malloc(col*rows*sizeof(MovHex));
                for(i=0; i<rows; i++) {
                    for(j=0; j<col; j++) {
                        M[col*i+j].valore=1;
                        M[col*i+j].rotte=NULL;
                        M[col*i+j].adiacenti=NULL;
                    }
                }
                printf("OK\n");
            }
        }
        else if(c=='c') {
            if(scanf("hange_cost %d %d %d %f", &i, &j, &v, &r)==4) {
                if(i>=0 && i<col && j>=0 && j<rows && r>0 && v>=-10 && v<=10) {
                    change_cost(i,j,v,r, M);
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
                    else if(cercaNodo(M[col*(rows-1-j)+i].rotte, x2, y2)) {
                        cancellaNodo(&M[col*(rows-1-j)+i].rotte, x2, y2);
                        stampaLista(M[col*(rows-1-j)+i].rotte);
                        printf("OK\n");
                    }
                    else if(contaLista(M[col*(rows-1-j)+i].rotte)==5) printf("KO\n");
                    else {
                        inserisciInTesta(&M[col*(rows-1-j)+i].rotte, x2, y2);
                        stampaLista(M[col*(rows-1-j)+i].rotte);
                        printf("OK\n");
                    }
                }
            }
        
            else if(c=='r') {
                if(scanf("avel_cost %d %d %d %d", &x, &y, &x2, &y2)==4) {
                    if(x<0 || x>=col || rows-y-1<0 || rows-1-y>=rows || x2<0 || x2>=col || rows-y2-1<0 || rows-1-y2>=rows)
                        printf("-1\n");
                    else if(M[col*(rows-1-y)+x].valore==0)
                        printf("-1\n");
                    else if(cercaNodo(M[col*(rows-1-y)+x].rotte, x2, y2)) 
                        printf("%d\n", M[col*(rows-1-y)+x].valore);
                    else if(cercaNodo(M[col*(rows-1-y)+x].adiacenti, x2, y2))
                        printf("1\n");
                    else {
                        Heap.A = (info *) malloc(col*rows*sizeof(info));
                        indexHeap = (int *) malloc(col*rows*sizeof(int));
                        for(i=0; i<rows; i++) {
                            for(j=0; j<col; j++) {
                                Heap.A[col*i+j].val=-1;
                                Heap.A[col*i+j].x=j;
                                Heap.A[col*i+j].y=i;
                                indexHeap[col*(rows-i-1)+j]=col*i+j;
                            }
                        }
                        printf("%d\n", dijkstra(M, x, y, x2, y2, &Heap, indexHeap));
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
            }
        }
    }
        
    if (M != NULL) {
        free(M);
        M = NULL;
    }
    if (Heap.A != NULL) {
        free(Heap.A);
        Heap.A=NULL;
    }
    if (indexHeap != NULL) {
        free(indexHeap);
        indexHeap=NULL;
    }
        
}    

void change_cost (int x, int y, int v, float r, MovHex * M) {
    int j=0;
    int k=0, row_sx=0, row_dx=0, l=0;
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
                if(y+1<rows)
                    M[col*(rows-y-2)+x+k].valore=check((r-k)/r * v + M[col*(rows-y-2)+x+k].valore);
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
                if(y+1<rows)
                    M[col*(rows-y-2)+x-k].valore=check((r-k)/r * v + M[col*(rows-y-2)+x-k].valore);
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

int dijkstra(MovHex * M, int x, int y, int x2, int y2, Vet * Heap, int * indexHeap) {
    info u;
    int i, p,q=-1;
    Nodo * ptr_adj;
    Nodo * ptr_rotte;

    
    i=indexHeap[col*(rows-1-y)+x];

    (*Heap).A[i].val=0;

    
    swap(Heap, indexHeap, i, 0);

    (*Heap).size=col*rows-1;

    while((*Heap).size!=-1 && q==-1) {
        u=HEAP_EXTRACT_MIN(Heap, indexHeap);
        if(u.x==x2 && u.y==y2)
            q=u.val;
        
        if(M[col*(rows-1-u.y)+u.x].valore!=0) { //le piastrelle 0 non hanno adiacenti
            adj(u.x, u.y, M);
            ptr_adj=M[col*(rows-1-u.y)+u.x].adiacenti;
            ptr_rotte=M[col*(rows-1-u.y)+u.x].rotte;
            while(ptr_adj!=NULL) {
                i=indexHeap[col*(rows-((ptr_adj->info).y)-1)+(ptr_adj->info).x];
                if(i<=(*Heap).size && (*Heap).A[i].x == (ptr_adj->info).x && (*Heap).A[i].y == (ptr_adj->info).y) {
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

            liberaLista(&M[col*(rows-1-u.y)+u.x].adiacenti);



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
    /*int j;
    for(i=0; i<rows; i++) {
                if((rows-i-1)%2==0)
                    printf("(%d) ", rows-i-1);
                else
                    printf("(%d)", rows-i-1);

                for(j=0; j<col; j++) {   
                    printf("%2d ", indexHeap[col*i+j]);
                }
                printf("\n");
            }
    for(i=0; i<=rows*col-1; i++)
            printf("[%d] (x: %d, y: %d) ", (*Heap).A[i].val, (*Heap).A[i].x, (*Heap).A[i].y);*/
               
    return q;
}



info HEAP_EXTRACT_MIN(Vet * Heap, int * indexHeap) {
    info min;
    min=(*Heap).A[0];
    /*(*Heap).A[0]=(*Heap).A[(*Heap).size];
    indexHeap[col*(rows-(*Heap).A[0].y-1)+(*Heap).A[0].x]=0;
    if(temp==0) {
        varx=col-1;
        vary=rows-1;
    }
    else {
        if(varx-1>=0) 
            varx--;
        else {
            varx=col-1;
            vary--;
        }
    }


    (*Heap).A[(*Heap).size].val=-1;
    (*Heap).A[(*Heap).size].x=varx;
    (*Heap).A[(*Heap).size].y=vary;
    indexHeap[col*(rows-min.y-1)+min.x]=col*min.y+min.x;*/

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


void liberaLista(Lista * l) {
    Nodo *tmp;
    while ((*l)!=NULL) {
        tmp=*l;
        *l=(*l)->prox;
        free(tmp);
    }
    (*l)=NULL;
}

