#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


typedef struct prova {
    int valore;
    int distanza;
}ciao;

void change_cost (int x, int y, int v, float r, ciao * M, int rows, int col);


int main() {
    srand(time(NULL));
    ciao * M;
    int col, rows, i, j;
    int x, y, v;
    float r;
    col=40;
    rows=30;
    M=(ciao *) malloc(col*rows*sizeof(ciao));
    for(i=0; i<rows; i++) {
        for(j=0; j<col; j++) {
         M[col*i+j].valore=1;
        }
    }

    //M[col*(rows-6-1)+10].valore=5;   // M[2][3]

    if(scanf("change_cost %d %d %d %f", &x, &y, &v, &r)==4) {
        change_cost(x,y,v,r, M, rows, col);
    }
            printf("\n");

    for(i=0; i<rows; i++) {
        if(i==0 || i%2!=0)
            printf(" ");
        for(j=0; j<col; j++) {
            printf("%2d ", M[col*i+j].valore);
        }
        printf("\n");
    }
}

void change_cost (int x, int y, int v, float r, ciao * M, int rows, int col) {
    int j;
    int k, row_sx, row_dx, l;

    M[col*(rows-1-y)+x].valore=v+M[col*(rows-1-y)+x].valore;

    if(y%2==0) {
        if(r>=1) {
            M[col*(rows-y)+x].valore=(r-1)/r * v + M[col*(rows-y)+x].valore;
            M[col*(rows-y-2)+x].valore=(r-1)/r * v + M[col*(rows-y-2)+x].valore;
            M[col*(rows-1-y)+(x+1)].valore=(r-1)/r * v + M[col*(rows-1-y)+(x+1)].valore;
            M[col*(rows-y-2)+(x+1)].valore=(r-1)/r * v + M[col*(rows-y-2)+(x+1)].valore;
            M[col*(rows-y)+(x+1)].valore=(r-1)/r * v + M[col*(rows-y)+(x+1)].valore;
            M[col*(rows-1-y)+(x-1)].valore=(r-1)/r * v + M[col*(rows-1-y)+(x-1)].valore;
        }
        if(r>=2) {
            M[col*(rows-y-3)+(x)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x)].valore;
            M[col*(rows-y+1)+(x)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x)].valore;
            M[col*(rows-y-1)+(x+2)].valore=(r-2)/r * v + M[col*(rows-y-1)+(x+2)].valore;
            M[col*(rows-y-2)+(x+2)].valore=(r-2)/r * v + M[col*(rows-y-2)+(x+2)].valore;
            M[col*(rows-y)+(x+2)].valore=(r-2)/r * v + M[col*(rows-y)+(x+2)].valore;
            M[col*(rows-y)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y)+(x-1)].valore;
            M[col*(rows-y-2)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y-2)+(x-1)].valore;
            M[col*(rows-y-1)+(x-2)].valore=(r-2)/r * v + M[col*(rows-y-1)+(x-2)].valore;
            M[col*(rows-y+1)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x-1)].valore; 
            M[col*(rows-y-3)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x-1)].valore;
            M[col*(rows-y-3)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x+1)].valore;
            M[col*(rows-y+1)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x+1)].valore;
        }
        if(r>=3) {
            k=3;
            while(k<=r) {
                row_sx=1;
                row_dx=2;
                
                l=k;
                M[col*(rows-y-1)+x-k].valore=(r-k)/r * v + M[col*(rows-y-1)+x-k].valore;
                M[col*(rows-y-2)+x+k].valore=(r-k)/r * v + M[col*(rows-y-2)+x+k].valore;
                M[col*(rows-y)+x+k].valore=(r-k)/r * v + M[col*(rows-y)+x+k].valore;
                M[col*(rows-y-1)+x+k].valore=(r-k)/r * v + M[col*(rows-y-1)+x+k].valore;

                while(row_dx!=k && row_sx!=k) {   
                    l--;
                    M[col*(rows-(y+row_sx)-1)+x-l].valore=(r-k)/r * v + M[col*(rows-(y+row_sx)-1)+x-l].valore;
                    M[col*(rows-(y+row_sx+1)-1)+x-l].valore=(r-k)/r * v + M[col*(rows-(y+row_sx+1)-1)+x-l].valore;
                    M[col*(rows-1-(y+row_dx))+x+l].valore=(r-k)/r * v + M[col*(rows-1-(y+row_dx))+x+l].valore;
                    M[col*(rows-(y+row_dx+1)-1)+x+l].valore=(r-k)/r * v + M[col*(rows-(y+row_dx+1)-1)+x+l].valore;
                    M[col*(rows-1-(y-row_sx))+x-l].valore=(r-k)/r * v +  M[col*(rows-1-(y-row_sx))+x-l].valore;
                    M[col*(rows-1-(y-row_sx-1))+x-l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_sx-1))+x-l].valore;
                    M[col*(rows-1-(y-row_dx))+x+l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_dx))+x+l].valore;
                    M[col*(rows-1-(y-row_dx-1))+x+l].valore=(r-k)/r * v +M[col*(rows-1-(y-row_dx-1))+x+l].valore;
                    row_sx=row_sx+2;
                    row_dx=row_dx+2;
                }

                if(row_sx==k) {
                    j=0;
                    do{
                        M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore;
                        M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore;
                        j++;
                    }while(j!=2*l-1);
                }
                if(row_dx==k) {
                    j=0;
                    do{
                        M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore;
                        M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore;
                        j++;
                    }while(j!=2*l);
                    M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore=(r-k)/r * v + M[col*(-(y+row_dx-1)+rows-1)+x-l+1].valore;
                    M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore=(r-k)/r * v + M[col*(-(y-row_dx+1)+rows-1)+x-l+1].valore;
                }
                k++;
            }
        }
    }
    
    else {
        if(r>=1) {
            M[col*(rows-y)+x].valore=(r-1)/r * v + M[col*(rows-y)+x].valore;
            M[col*(rows-y-2)+x].valore=(r-1)/r * v + M[col*(rows-y-2)+x].valore;
            M[col*(rows-1-y)+(x-1)].valore=(r-1)/r * v + M[col*(rows-1-y)+(x-1)].valore;
            M[col*(rows-y-2)+(x-1)].valore=(r-1)/r * v + M[col*(rows-y-2)+(x-1)].valore;
            M[col*(rows-y)+(x-1)].valore=(r-1)/r * v + M[col*(rows-y)+(x-1)].valore;
            M[col*(rows-1-y)+(x+1)].valore=(r-1)/r * v + M[col*(rows-1-y)+(x+1)].valore;  
        }
    
        if(r>=2) {
            M[col*(rows-y-3)+(x)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x)].valore;
            M[col*(rows-y+1)+(x)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x)].valore;
            M[col*(rows-y-1)+(x-2)].valore=(r-2)/r * v + M[col*(rows-y-1)+(x-2)].valore;
            M[col*(rows-y-2)+(x-2)].valore=(r-2)/r * v + M[col*(rows-y-2)+(x-2)].valore;
            M[col*(rows-y)+(x-2)].valore=(r-2)/r * v + M[col*(rows-y)+(x-2)].valore;
            M[col*(rows-y)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y)+(x+1)].valore;
            M[col*(rows-y-2)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y-2)+(x+1)].valore;
            M[col*(rows-y-1)+(x+2)].valore=(r-2)/r * v + M[col*(rows-y-1)+(x+2)].valore;
            M[col*(rows-y+1)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x+1)].valore; 
            M[col*(rows-y-3)+(x+1)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x+1)].valore;
            M[col*(rows-y-3)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y-3)+(x-1)].valore;
            M[col*(rows-y+1)+(x-1)].valore=(r-2)/r * v + M[col*(rows-y+1)+(x-1)].valore;
        }
        
        if(r>=3) {
            k=3;
            while(k<=r) {
                row_sx=2;
                row_dx=1;
            
                l=k;
                M[col*(rows-y-1)+x-k].valore=(r-k)/r * v + M[col*(rows-y-1)+x-k].valore;
                M[col*(rows-y-2)+x-k].valore=(r-k)/r * v + M[col*(rows-y-2)+x-k].valore;
                M[col*(rows-y)+x-k].valore=(r-k)/r * v + M[col*(rows-y)+x-k].valore;
                M[col*(rows-y-1)+x+k].valore=(r-k)/r * v + M[col*(rows-y-1)+x+k].valore;
                while(row_dx!=k && row_sx!=k) {
                    l--;
                    M[col*(rows-(y+row_sx)-1)+x-l].valore=(r-k)/r * v + M[col*(rows-(y+row_sx)-1)+x-l].valore;
                    M[col*(rows-(y+row_sx+1)-1)+x-l].valore=(r-k)/r * v + M[col*(rows-(y+row_sx+1)-1)+x-l].valore;
                    M[col*(rows-1-(y+row_dx))+x+l].valore=(r-k)/r * v + M[col*(rows-1-(y+row_dx))+x+l].valore;
                    M[col*(rows-(y+row_dx+1)-1)+x+l].valore=(r-k)/r * v + M[col*(rows-(y+row_dx+1)-1)+x+l].valore;
                    M[col*(rows-1-(y-row_sx))+x-l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_sx))+x-l].valore;
                    M[col*(rows-1-(y-row_sx-1))+x-l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_sx-1))+x-l].valore;
                    M[col*(rows-1-(y-row_dx))+x+l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_dx))+x+l].valore;
                    M[col*(rows-1-(y-row_dx-1))+x+l].valore=(r-k)/r * v + M[col*(rows-1-(y-row_dx-1))+x+l].valore;

                    row_sx=row_sx+2;
                    row_dx=row_dx+2;
                }

                
                if(row_dx==k) {
                    j=0;
                    do{
                        M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y+row_dx)+rows-1)+x-l+1+j].valore;
                        M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y-row_dx)+rows-1)+x-l+1+j].valore;
                        j++;
                    }while(j!=2*l-1);
                }
                
                if(row_sx==k) {
                    j=0;
                    do{
                        M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y+row_sx)+rows-1)+x-l+1+j].valore;
                        M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore=(r-k)/r * v + M[col*(-(y-row_sx)+rows-1)+x-l+1+j].valore;
                        j++;
                    }while(j!=2*l-1);
                    M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore=(r-k)/r * v + M[col*(-(y+row_sx-1)+rows-1)+x+l-1].valore;
                    M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore=(r-k)/r * v + M[col*(-(y-row_sx+1)+rows-1)+x+l-1].valore;
                }
                k++;
            }
        }
    }   
}

