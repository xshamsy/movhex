#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* --- CONFIGURATION MACROS --- */
#define HASH_SIZE 300
#define MAX_HASH_PROBES 10
#define HASH_A 1
#define HASH_B 1

/* --- DATA STRUCTURES --- */

typedef struct HashCache HashCache;
typedef struct Node Node;
typedef Node * NodeList;
typedef struct HexCell HexCell;
typedef struct MinHeap MinHeap;
typedef struct PathState PathState;

/* 
 * HashCache represents a single entry in the memoization table.
 * It stores the source (x1, y1), destination (x2, y2), and the shortest 
 * path distance (val) calculated by Dijkstra to answer repeated queries in O(1).
 */
struct HashCache {
    __uint32_t val;
    __uint32_t x1;
    __uint32_t y1;
    __uint32_t x2;
    __uint32_t y2;
};

/* 
 * HexCell represents a physical tile on the hexagonal grid.
 * - air_routes: A dynamic linked list containing "teleport" paths to non-adjacent cells.
 * - weight: The standard traversal cost of this specific cell (clamped between 0 and 100).
 */
struct HexCell {
    NodeList air_routes;
    __uint8_t weight;
};

/* 
 * PathState tracks the state of a node during the execution of Dijkstra's algorithm.
 * - val: The current shortest accumulated distance from the starting node to this node.
 * - visited: A boolean flag (1 or 0) indicating if the node has been fully processed.
 */
struct PathState {
    __uint32_t val;
    unsigned visited;
};

/* 
 * MinHeap is a custom array-based priority queue used by Dijkstra.
 * It stores pointers to PathState objects to avoid copying large structs,
 * allowing O(log V) extraction of the node with the minimum distance.
 */
struct MinHeap {
    PathState ** ptr;
};

/* 
 * Node is a standard element of a singly linked list.
 * In this engine, it is primarily used to store the 1D index of destination 
 * cells for dynamic air routes (teleports).
 */
struct Node {
    __uint32_t info;
    struct Node * next;
};

/* --- FUNCTION PROTOTYPES --- */
int dijkstra(PathState * A, HexCell * M, int x, int y, int x2, int y2, MinHeap * Heap);
int checkBounds(int val);
void changeCost(int x, int y, int v, float r, HexCell * M);
void freeList(NodeList * l);
void deleteNode(NodeList * head, __uint32_t coord);
int searchNode(NodeList head, __uint32_t coord);
int countList(NodeList head);
void insertAtHead(NodeList * head, __uint32_t coord);
int extractMin(MinHeap * Heap, PathState * A);
void minHeapify(MinHeap * Heap, int i, PathState * A);
int searchCache(HashCache * v, int x1, int y1, int x2, int y2, HexCell * M, PathState * A, MinHeap * Heap);
void initCache(HashCache * v);
int hashFunction(int x1, int y1, int x2, int y2, int i);
void heapifyDown(int i, __uint32_t val, MinHeap * Heap, PathState * A);
int getNeighborsArray(int x, int y, int * neighbors, int base_index);

/* Global flags and grid dimensions */
unsigned cache_valid = 1; /* Flag used to enable O(1) geometric fast-path if grid is unaltered */
int col = -1;
int rows = -1;
int cache_miss_count = 1;
int heap_current_size;

/* 
 * MAIN FUNCTION: The Entry Point of the Application
 * 
 * Responsibilities:
 * 1. Initialize the global structures (Grid, Min-Heap, Hash Cache, State Array).
 * 2. Enter a continuous I/O loop to parse and process incoming commands character by character.
 * 3. Handle four main commands:
 *    - 'init': Allocates grid memory and resets states.
 *    - 'change_cost': Alters the traversal cost of an area and invalidates the cache.
 *    - 'toggle_air_route': Adds or removes direct links between distant nodes.
 *    - 'travel_cost': Computes the shortest path using either cache, fast-path math, or Dijkstra.
 * 4. Safely free all dynamically allocated memory upon exit to ensure zero memory leaks.
 */
int main() {
    HashCache * cache_table;
    MinHeap priority_queue;
    PathState * state_array = NULL;
    priority_queue.ptr = NULL;
    
    cache_table = (HashCache *) malloc(sizeof(HashCache) * HASH_SIZE);
    HexCell * grid = NULL;
    
    char c = 'p';
    int i = 0, j = 0;
    int v = 0, x = 0, y = 0, x2 = 0, y2 = 0;
    float r = 0;
    
    initCache(cache_table);
    c = getchar();
    
    while(c != EOF) {
        /* Command: Initialize the grid */
        if(c == 'i') {
            /* Free previously allocated air routes if re-initializing */
            if(col != -1) {
                for(i = 0; i < rows; i++) {
                    for(j = 0; j < col; j++) {
                        freeList(&grid[col * i + j].air_routes);
                    }
                }
            }
            
            /* Read dimensions. The 'i' was already consumed by getchar() */
            if(scanf("nit %d %d", &col, &rows) == 2) {
                cache_valid = 1; /* Grid is pristine, fast-math enabled */

                /* Safely deallocate old memory blocks before reallocating */
                if (priority_queue.ptr != NULL) {
                    free(priority_queue.ptr);
                    priority_queue.ptr = NULL;
                }
                if (grid != NULL) {
                    free(grid);
                    grid = NULL;
                }
                if (state_array != NULL) {
                    free(state_array);
                    state_array = NULL;
                }
                
                /* Allocate core structures based on new grid dimensions */
                priority_queue.ptr = (PathState **) malloc(col * rows * sizeof(PathState*));
                state_array = (PathState *) malloc(col * rows * sizeof(PathState));
                grid = (HexCell *) malloc(col * rows * sizeof(HexCell));

                /* Initialize each hex cell with a default weight of 1 */
                for(i = 0; i < rows; i++) {
                    for(j = 0; j < col; j++) {
                        grid[col * (rows - i - 1) + j].weight = 1;
                        grid[col * (rows - i - 1) + j].air_routes = NULL;
                    }
                }

                printf("OK\n");
            }
        }
        /* Command: Change traversal cost for a specific cell and its radius */
        else if(c == 'c') {
            if(scanf("hange_cost %d %d %d %f", &i, &j, &v, &r) == 4) {
                /* Validate input bounds and logical constraints */
                if(i >= 0 && i < col && j >= 0 && j < rows && r > 0 && v >= -10 && v <= 10) {
                    initCache(cache_table); /* Map changed, invalidate previous cached paths */
                    changeCost(i, j, v, r, grid); /* Apply the cost update radially */
                    cache_valid = 0; /* Map topology altered, disable fast-math */
                    printf("OK\n");
                }
                else {
                    printf("KO\n");
                }
            }
        }
        /* Command parsing for commands starting with 't' ('toggle' or 'travel') */
        else if(c == 't') {
            c = getchar();
            if(c == 'o') {
                if(scanf("ggle_air_route %d %d %d %d", &i, &j, &x2, &y2) == 4) {
                    
                    /* Strict boundary checks for both source and destination nodes */
                    if(i < 0 || i >= col || rows - j - 1 < 0 || rows - 1 - j >= rows || x2 < 0 || x2 >= col || rows - y2 - 1 < 0 || rows - 1 - y2 >= rows)
                        printf("KO\n");
                    /* If the route already exists, remove it */
                    else if(searchNode(grid[col * (rows - 1 - j) + i].air_routes, col * (rows - 1 - y2) + x2)) {
                        initCache(cache_table);
                        cache_valid = 0;
                        deleteNode(&grid[col * (rows - 1 - j) + i].air_routes, col * (rows - 1 - y2) + x2);
                        printf("OK\n");
                    }
                    /* Enforce maximum air routes limit per cell (max 5) */
                    else if(countList(grid[col * (rows - 1 - j) + i].air_routes) == 5) 
                        printf("KO\n");
                    /* If it doesn't exist and limit is not reached, add the new route */
                    else {
                        initCache(cache_table);
                        cache_valid = 0;
                        insertAtHead(&grid[col * (rows - 1 - j) + i].air_routes, col * (rows - 1 - y2) + x2);
                        printf("OK\n");
                    }
                }
            }
            /* Command: Calculate Shortest Travel Cost */
            else if(c == 'r') {
                if(scanf("avel_cost %d %d %d %d", &x, &y, &x2, &y2) == 4) {
                    /* Boundary checks for start and end coordinates */
                    if(x < 0 || x >= col || rows - y - 1 < 0 || rows - 1 - y >= rows || x2 < 0 || x2 >= col || rows - y2 - 1 < 0 || rows - 1 - y2 >= rows)
                        printf("-1\n");
                    /* Immediate zero cost if source equals destination */
                    else if(x == x2 && y == y2)
                        printf("0\n");
                    /* If starting cell is impassable (weight 0) */
                    else if(grid[col * (rows - 1 - y) + x].weight == 0)
                        printf("-1\n");
                    /* Immediate evaluation if an air route connects source and destination directly */
                    else if(searchNode(grid[col * (rows - 1 - y) + x].air_routes, col * (rows - 1 - y2) + x2)) 
                        printf("%d\n", grid[col * (rows - 1 - y) + x].weight);
                    /* O(1) Fast-Path: If map hasn't changed, use direct geometric distance calculations */
                    else if(cache_valid == 1) {
                        int d = 0, h = 0;
                        /* Complex mathematical reduction of hexagonal distances without iterating */
                        if(x == x2)
                            d = abs(y2 - y);
                        else if(y == y2)
                            d = abs(x - x2);
                        else if(x > x2 && d == 0) {
                            if(y > y2) {
                                d = y - y2;
                                h = y2 % 2 != 0 && y % 2 == 0 ? x - ((int)((y - y2) / 2) + 1) : x - ((int)((y - y2) / 2));
                                if(h >= 0 && h - x2 > 0)
                                    d = d + h - x2;
                            }
                            else {
                                d = y2 - y;
                                h = y2 % 2 != 0 && y % 2 == 0 ? x - ((int)((y2 - y) / 2) + 1) : x - ((int)((y2 - y) / 2));
                                if(h >= 0 && h - x2 > 0)
                                    d = d + h - x2;
                            }
                        }
                        else if(x2 > x) {
                            if(y2 > y) {
                                d = y2 - y;
                                h = y % 2 != 0 && y2 % 2 == 0 ? x2 - ((int)((y2 - y) / 2) + 1) : x2 - ((int)((y2 - y) / 2));
                                if(h >= 0 && h - x > 0)
                                    d = d + h - x;
                            }
                            else {
                                d = y - y2;
                                h = y % 2 != 0 && y2 % 2 == 0 ? x2 - ((int)((y - y2) / 2) + 1) : x2 - ((int)((y - y2) / 2));
                                if(h >= 0 && h - x > 0)                                     
                                    d = d + h - x; 
                            }
                        }
                        printf("%d\n", d);
                    }
                    /* If map was altered, trigger caching system (which in turn triggers Dijkstra on miss) */
                    else {
                        printf("%d\n", searchCache(cache_table, x, y, x2, y2, grid, state_array, &priority_queue));
                    }
                }
            }
        }  
        c = getchar();            
    }

    /* --- SHUTDOWN & CLEANUP SEQUENCE --- */
    /* Ensure no memory leaks by systematically freeing all dynamically allocated structures */
    if(col != -1) {
        for(i = 0; i < rows; i++) {
            for(j = 0; j < col; j++) {
                freeList(&grid[col * i + j].air_routes);
            }
        }
    }
        
    free(priority_queue.ptr);          
                
    if (grid != NULL) {
        free(grid);
        grid = NULL;
    }
    
    if (state_array != NULL) {
        free(state_array);
        state_array = NULL;
    }
    
    free(cache_table);
    cache_table = NULL;
    return 0;
}  


/* -------------------------------------------------------------------------- */
/*                        CACHE AND HASHING FUNCTIONS                         */
/* -------------------------------------------------------------------------- */

/* 
 * function: searchCache
 * 
 * Description:
 * Implements Memoization to prevent recalculating identical paths. It hashes the start and end coordinates.
 * If the computed hash index causes a collision (different coordinates, same hash), it uses quadratic 
 * probing up to MAX_HASH_PROBES. 
 * If a cache hit occurs, it returns the result in O(1). 
 * If a cache miss occurs, it invokes Dijkstra's algorithm, stores the result in the cache, and returns it.
 */
int searchCache(HashCache * v, int x1, int y1, int x2, int y2, HexCell * M, PathState * A, MinHeap * Heap) {
    int pos;
    int i = 0;
    
    pos = hashFunction(x1, y1, x2, y2, i);
    
    /* Quadratic probing loop to resolve collisions */
    while((v[pos].x1 != (__uint32_t)x1 || v[pos].y1 != (__uint32_t)y1 || v[pos].x2 != (__uint32_t)x2  || v[pos].y2 != (__uint32_t)y2) && (v[pos].val != (__uint32_t)-1) && (i <= MAX_HASH_PROBES)) {
        i++;
        pos = hashFunction(x1, y1, x2, y2, i);
    }
    
    /* Cache Miss (Collision limit reached) -> Run Dijkstra and overwrite */
    if(i > MAX_HASH_PROBES) {
        printf("[%d]", cache_miss_count);
        cache_miss_count++;
        pos = hashFunction(x1, y1, x2, y2, 0);
        v[pos].val = dijkstra(A, M, x1, y1, x2, y2, Heap); 
        v[pos].x1 = x1;
        v[pos].y1 = y1;
        v[pos].x2 = x2;
        v[pos].y2 = y2;
        return v[pos].val;
    }
    
    /* Cache Miss (Empty slot found) -> Run Dijkstra and cache result */
    if(v[pos].val == (__uint32_t)-1) {
        v[pos].val = dijkstra(A, M, x1, y1, x2, y2, Heap); 
        v[pos].x1 = x1;
        v[pos].y1 = y1;
        v[pos].x2 = x2;
        v[pos].y2 = y2;
        return v[pos].val;
    }

    /* Cache Hit -> Return immediately */
    return v[pos].val;
}    

/* 
 * function: initCache
 * 
 * Description:
 * Resets the entire Hash Table. Uses -1 (casted implicitly to __uint32_t max value) 
 * as a sentinel value to denote an empty slot. Called whenever map topology changes.
 */
void initCache(HashCache * v) {
    int i;
    for(i = 0; i < HASH_SIZE; i++) {
        v[i].val = -1;
        v[i].x1 = -1;
        v[i].y1 = -1;
        v[i].x2 = -1;
        v[i].y2 = -1;
    }
}

/* 
 * function: hashFunction
 * 
 * Description:
 * Generates an index for the HashCache. It flattens the 2D coordinates (x1, y1) into a 1D scalar
 * and applies polynomial hashing using HASH_A and HASH_B to handle quadratic probing (parameter 'i').
 */
int hashFunction(int x1, int y1, int x2, int y2, int i) {
    (void)x2; /* Destination coordinates intentionally omitted from base hash logic */
    (void)y2;
    return ((y1 * col + x1) + HASH_A * i + HASH_B * i * i) % HASH_SIZE;
}

/* -------------------------------------------------------------------------- */
/*                         HEXAGONAL GRID LOGIC                               */
/* -------------------------------------------------------------------------- */

/* 
 * function: changeCost
 * 
 * Description:
 * Modifies the traversal cost of a specific cell and its surrounding cells within a given radius 'r'.
 * The added cost 'v' decays linearly as the distance from the center increases (multiplier = (r-k)/r * v).
 * This function handles the complex boundary math of drawing concentric hexagons on an alternating grid.
 * It iteratively computes the inner/outer rings depending on whether the center Y-coordinate is even or odd.
 */
void changeCost(int x, int y, int v, float r, HexCell * M) {
    int j = 0;
    int k = 0, row_sx = 0, row_dx = 0, l = 0;
    float mul = 0;

    /* Update center cell */
    M[col * (rows - 1 - y) + x].weight = checkBounds(v + M[col * (rows - 1 - y) + x].weight);

    /* Logic branches heavily based on whether the row offset is odd or even */
    if(y % 2 != 0) {
        k = 1;
        /* Expand radially up to radius 'r' */
        while(k <= r) {
            /* Calculate decaying cost multiplier */
            mul = (r - k) / r * v;
            if(mul >= 0)
                mul = (int) mul;
            else {
                if((int) mul - mul != 0)
                    mul = mul - 1;
            }
            mul = (int) mul; 

            row_sx = 1;
            row_dx = 2;
            l = k;
            
            /* Apply lateral boundaries */
            if(x - k >= 0)
                M[col * (rows - y - 1) + x - k].weight = checkBounds(mul + M[col * (rows - y - 1) + x - k].weight);
            if(x + k < col) {
                if(y + 1 < rows)
                    M[col * (rows - y - 2) + x + k].weight = checkBounds(mul + M[col * (rows - y - 2) + x + k].weight);
                M[col * (rows - y) + x + k].weight = checkBounds(mul + M[col * (rows - y) + x + k].weight);
                M[col * (rows - y - 1) + x + k].weight = checkBounds(mul + M[col * (rows - y - 1) + x + k].weight);
            }
                
            /* Traverse and update top and bottom triangular caps of the hexagon ring */
            while(row_dx != k && row_sx != k) {   
                l--;
                if(x - l >= 0) {
                    if(y + row_sx < rows)
                        M[col * (rows - (y + row_sx) - 1) + x - l].weight = checkBounds(mul + M[col * (rows - (y + row_sx) - 1) + x - l].weight);
                    if(y + row_sx + 1 < rows)
                        M[col * (rows - (y + row_sx + 1) - 1) + x - l].weight = checkBounds(mul + M[col * (rows - (y + row_sx + 1) - 1) + x - l].weight);
                    if(y - row_sx >= 0)
                        M[col * (rows - 1 - (y - row_sx)) + x - l].weight = checkBounds(mul +  M[col * (rows - 1 - (y - row_sx)) + x - l].weight);
                    if(y - row_sx - 1 >= 0)
                        M[col * (rows - 1 - (y - row_sx - 1)) + x - l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_sx - 1)) + x - l].weight);
                }
                if(x + l < col) {
                    if(y + row_dx < rows)
                        M[col * (rows - 1 - (y + row_dx)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y + row_dx)) + x + l].weight);
                    if(y + row_dx + 1 < rows)
                        M[col * (rows - (y + row_dx + 1) - 1) + x + l].weight = checkBounds(mul + M[col * (rows - (y + row_dx + 1) - 1) + x + l].weight);
                    if(y - row_dx >= 0)
                        M[col * (rows - 1 - (y - row_dx)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_dx)) + x + l].weight);
                    if(y - row_dx - 1 >= 0)
                        M[col * (rows - 1 - (y - row_dx - 1)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_dx - 1)) + x + l].weight);
                }
                row_sx = row_sx + 2;
                row_dx = row_dx + 2;
            }

            /* Fill horizontal extremes of the current ring */
            if(row_sx == k) {
                j = 0;
                do {
                    if(x - l + 1 + j >= 0 && x - l + 1 + j < col) {
                        if(y + row_sx < rows)
                            M[col * (-(y + row_sx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y + row_sx) + rows - 1) + x - l + 1 + j].weight);
                        if(y - row_sx >= 0)
                            M[col * (-(y - row_sx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y - row_sx) + rows - 1) + x - l + 1 + j].weight);
                    }
                    j++;
                } while(j != 2 * l - 1);
            }
            if(row_dx == k) {
                j = 0;
                do {
                    if(x - l + 1 + j >= 0 && x - l + 1 + j < col) {
                        if(y + row_dx < rows)
                            M[col * (-(y + row_dx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y + row_dx) + rows - 1) + x - l + 1 + j].weight);

                        if(y - row_dx >= 0)
                            M[col * (-(y - row_dx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y - row_dx) + rows - 1) + x - l + 1 + j].weight);
                    }
                    j++;
                } while(j != 2 * l - 1);
                
                if(x - l + 1 >= 0 && x - l + 1 < col) {
                    if(y + row_dx - 1 < rows && y + row_dx - 1 >= 0)
                        M[col * (-(y + row_dx - 1) + rows - 1) + x - l + 1].weight = checkBounds(mul + M[col * (-(y + row_dx - 1) + rows - 1) + x - l + 1].weight);
                    if(y - row_dx + 1 >= 0 && y - row_dx + 1 < rows)
                        M[col * (-(y - row_dx + 1) + rows - 1) + x - l + 1].weight = checkBounds(mul + M[col * (-(y - row_dx + 1) + rows - 1) + x - l + 1].weight);
                }
            }
            k++;
        }
    }
    else {
        /* Even row geometry logic (symmetric counterpart to the odd branch) */
        k = 1;
        while(k <= r) {
            mul = (r - k) / r * v;
            if(mul >= 0)
                mul = (int) mul;
            else {
                if((int) mul - mul != 0)
                    mul = mul - 1;
            }

            mul = (int) mul;

            row_sx = 2;
            row_dx = 1;
            
            l = k;
            if(x - k >= 0){
                M[col * (rows - y - 1) + x - k].weight = checkBounds(mul + M[col * (rows - y - 1) + x - k].weight);
                if(y + 1 < rows)
                    M[col * (rows - y - 2) + x - k].weight = checkBounds(mul + M[col * (rows - y - 2) + x - k].weight);
                M[col * (rows - y) + x - k].weight = checkBounds(mul + M[col * (rows - y) + x - k].weight);
            }
            if(x + k < col)
                M[col * (rows - y - 1) + x + k].weight = checkBounds(mul + M[col * (rows - y - 1) + x + k].weight);
                
            while(row_dx != k && row_sx != k) {
                l--;
                if(x - l >= 0) {
                    if(y + row_sx < rows)
                        M[col * (rows - (y + row_sx) - 1) + x - l].weight = checkBounds(mul + M[col * (rows - (y + row_sx) - 1) + x - l].weight);
                    if(y + row_sx + 1 < rows)
                        M[col * (rows - (y + row_sx + 1) - 1) + x - l].weight = checkBounds(mul + M[col * (rows - (y + row_sx + 1) - 1) + x - l].weight);
                    if(y - row_sx >= 0)
                        M[col * (rows - 1 - (y - row_sx)) + x - l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_sx)) + x - l].weight);
                    if(y - row_sx - 1 >= 0)
                        M[col * (rows - 1 - (y - row_sx - 1)) + x - l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_sx - 1)) + x - l].weight);
                }
                if(x + l < col) {
                    if(y + row_dx < rows)
                        M[col * (rows - 1 - (y + row_dx)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y + row_dx)) + x + l].weight);
                    if(y + row_dx + 1 < rows)
                        M[col * (rows - (y + row_dx + 1) - 1) + x + l].weight = checkBounds(mul + M[col * (rows - (y + row_dx + 1) - 1) + x + l].weight);
                    if(y - row_dx >= 0)
                        M[col * (rows - 1 - (y - row_dx)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_dx)) + x + l].weight);
                    if(y - row_dx - 1 >= 0)
                        M[col * (rows - 1 - (y - row_dx - 1)) + x + l].weight = checkBounds(mul + M[col * (rows - 1 - (y - row_dx - 1)) + x + l].weight);
                }
                row_sx = row_sx + 2;
                row_dx = row_dx + 2;
            }

            if(row_dx == k) {
                j = 0;
                do {
                    if(x - l + 1 + j >= 0 && x - l + 1 + j < col) {
                        if(y + row_dx < rows)
                            M[col * (-(y + row_dx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y + row_dx) + rows - 1) + x - l + 1 + j].weight);
                        if(y - row_dx >= 0)
                            M[col * (-(y - row_dx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y - row_dx) + rows - 1) + x - l + 1 + j].weight);
                    }
                    j++;
                } while(j != 2 * l - 1);
            }
                
            if(row_sx == k) {
                j = 0;
                do {
                    if(x - l + 1 + j >= 0 && x - l + 1 + j < col) {
                        if(y + row_sx < rows)
                            M[col * (-(y + row_sx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y + row_sx) + rows - 1) + x - l + 1 + j].weight);
                        if(y - row_sx >= 0)
                            M[col * (-(y - row_sx) + rows - 1) + x - l + 1 + j].weight = checkBounds(mul + M[col * (-(y - row_sx) + rows - 1) + x - l + 1 + j].weight);
                    }
                    j++;
                } while(j != 2 * l - 1);
                
                if(x + l - 1 >= 0 && x + l - 1 < col) {
                    if(y + row_sx - 1 >= 0 && y + row_sx - 1 < rows)
                        M[col * (-(y + row_sx - 1) + rows - 1) + x + l - 1].weight = checkBounds(mul + M[col * (-(y + row_sx - 1) + rows - 1) + x + l - 1].weight);
                    if(y - row_sx + 1 >= 0 && y - row_sx + 1 < rows)
                        M[col * (-(y - row_sx + 1) + rows - 1) + x + l - 1].weight = checkBounds(mul + M[col * (-(y - row_sx + 1) + rows - 1) + x + l - 1].weight);
                }
            }
            k++;
        }
    }   
}

/* 
 * function: checkBounds
 * 
 * Description:
 * Clamps the calculated weight value of a hexagonal cell between 0 and 100 
 * as per algorithm constraints. 0 acts as a rigid wall (impassable).
 */
int checkBounds(int val) {
    if(val < 0)
        return 0;
    if(val > 100)
        return 100;
    return val;
}


/* -------------------------------------------------------------------------- */
/*                         LINKED LIST UTILITIES                              */
/* -------------------------------------------------------------------------- */

/* Dynamically allocates a new node and inserts it at the head of a list. O(1) time. */
void insertAtHead(NodeList * head, __uint32_t coord) {
    NodeList new_node = (Node*) malloc(sizeof(Node));
    new_node->info = coord;
    new_node->next = *head;
    *head = new_node;
}

/* Traverses the linked list to count its elements. Used to enforce the max 5 air routes limit. */
int countList(NodeList head) {
    Node * ptr = head;
    int count = 0;
    while(ptr != NULL) {
        count++;
        ptr = ptr->next;
    }
    return count;
}

/* Linear search through a linked list. Returns 1 if 'coord' is found, 0 otherwise. O(N) time. */
int searchNode(NodeList head, __uint32_t coord) {
    Node * ptr = head;
    while(ptr != NULL) {
        if((ptr->info) == coord) return 1;
        ptr = ptr->next;
    }
    return 0;
}

/* Safely removes a specific node from the linked list, freeing its memory to prevent leaks. */
void deleteNode(NodeList * head, __uint32_t coord) {
    Node * ptr = *head;
    if(ptr->info == coord) {
        *head = ptr->next;
        free(ptr);
        return;
    }
    else {
        while(ptr->next->info == coord)
            ptr = ptr->next;
        NodeList tmp = ptr->next;
        ptr->next = ptr->next->next;
        free(tmp);
    }
}


/* -------------------------------------------------------------------------- */
/*                       DIJKSTRA & MIN-HEAP IMPLEMENTATION                   */
/* -------------------------------------------------------------------------- */

/* 
 * function: getNeighborsArray
 * 
 * Description:
 * Computes the 1D indices of the 6 surrounding cells around a central hex cell (x,y).
 * A highly optimized function that writes directly to a static array pointer ('neighbors') 
 * instead of allocating dynamic lists. Crucial for eliminating malloc overhead in Dijkstra's inner loop.
 * It returns the total count of valid adjacent cells (handles grid borders safely).
 */
int getNeighborsArray(int x, int y, int * neighbors, int base_index) {
    int index = 0;

    if(y - 1 >= 0) {
        neighbors[index] = base_index + col;
        index++;
    }
    if(y + 1 < rows) {
        neighbors[index] = base_index - col;
        index++;
    }
    /* Neighbor logic shifts based on row alignment (even/odd) */
    if(y % 2 != 0) {
        if(x + 1 < col) {
            neighbors[index] = base_index + 1;
            index++;
            if(y - 1 >= 0) {
                neighbors[index] = base_index + col + 1;
                index++;
            }
            if(y + 1 < rows) {
                neighbors[index] = base_index - col + 1;
                index++;
            }
        }
        if(x - 1 >= 0) {
            neighbors[index] = base_index - 1;
            index++;
        } 
    }
    else {
        if(x - 1 >= 0) {
            neighbors[index] = base_index - 1;
            index++;
            if(y - 1 >= 0) {
                neighbors[index] = base_index + col - 1;
                index++;
            }
            if(y + 1 < rows) {
                neighbors[index] = base_index - col - 1;
                index++;
            }
        }
        if(x + 1 < col) {
            neighbors[index] = base_index + 1;
            index++;
        } 
    }
    return index; /* Return number of valid neighbors found */
}

/* 
 * function: dijkstra
 * 
 * Description:
 * Implements Dijkstra's Shortest Path algorithm optimized for performance.
 * 1. Resets the PathState tracking array (all costs initialized to -1 representing infinity).
 * 2. Processes nodes iteratively, extracting the minimum cost node via the custom Min-Heap.
 * 3. Relaxes standard edges (physical neighbors) retrieved via getNeighborsArray.
 * 4. Relaxes dynamic extra edges (air routes) retrieved via the Linked Lists.
 * 5. Safely terminates and returns the cost as soon as the destination is successfully extracted.
 */
int dijkstra(PathState * A, HexCell * M, int x, int y, int x2, int y2, MinHeap * Heap) {
    int u;
    int neighbors[6] = {0}; /* Pre-allocated static array to hold neighbor indices */
    int index = 0;
    __uint32_t coord;
    int i;
    PathState * temp;
    Node * ptr_routes;
    heap_current_size = 0;
    int destination;

    /* Initialize the path state tracker array */
    for(i = 0; i < rows * col; i++) {
        A[i].val = -1; /* Casted internally: -1 acts as an unsigned maximum (infinity) */
        A[i].visited = 0;  
    }

    u = col * (rows - 1 - y) + x;
    destination = col * (rows - 1 - y2) + x2;

    A[u].val = 0;
    A[u].visited = 1;

    /* Main relaxation loop */
    while(1) {
        if(M[u].weight != 0) { 
            x = u % col;
            y = (rows - 1) - (u / col);

            index = getNeighborsArray(x, y, neighbors, u);
            
            __uint32_t current_cost = A[u].val + M[u].weight;
            
            /* Edge Relaxation: Evaluate standard physical neighbors */
            for(i = 0; i < index; i++) {
                temp = &A[neighbors[i]];
                if((*temp).visited == 0) {
                    if((*temp).val == (__uint32_t)-1 || (*temp).val > current_cost) {
                        (*temp).val = current_cost;
                        
                        /* Insert the updated path into the Min-Heap */
                        (*Heap).ptr[heap_current_size] = temp;
                        heap_current_size++;

                        /* Fix heap property upwards (Bubble-Up) */
                        heapifyDown(heap_current_size - 1, A[(*Heap).ptr[heap_current_size - 1] - A].val, Heap, A);
                    }
                }
            }
            
            /* Edge Relaxation: Evaluate dynamic extra connections (teleports) */
            ptr_routes = M[u].air_routes;

            while(ptr_routes != NULL) {
                coord = (ptr_routes->info);
                temp = &A[coord];

                if((*temp).visited == 0) {
                    if((*temp).val == (__uint32_t)-1 || (*temp).val > current_cost) {
                        (*temp).val = current_cost;
                        (*Heap).ptr[heap_current_size] = temp;
                        heap_current_size++;

                        heapifyDown(heap_current_size - 1, A[(*Heap).ptr[heap_current_size - 1] - A].val, Heap, A);
                    }
                }
                ptr_routes = (ptr_routes)->next;
            }
        }

        /* Retrieve the next most promising node */
        u = extractMin(Heap, A);
        
        /* Stop conditions */
        if(u == -1) 
            return -1; /* Heap is empty and destination unreachable */
        
        if(u == destination)
            return A[u].val; /* Destination fully evaluated, shortest path found */
    }
    return A[u].val;
}

/* 
 * function: heapifyDown
 * 
 * Description:
 * Despite the name, this performs a "Bubble-Up" (Heap-Insert) operation. 
 * Starting from index 'i' (a newly inserted element at the bottom), it compares the element
 * with its parent node. If the element is smaller than the parent, they are swapped.
 * This continues up the tree until the Min-Heap property is satisfied.
 */
void heapifyDown(int i, __uint32_t val, MinHeap * Heap, PathState * A) {
    int p;
    PathState * temp;
    while(i > 0) {
        p = (i - 1) / 2; /* Calculate parent index */

        if(val < A[(*Heap).ptr[p] - A].val) {    
            temp = (*Heap).ptr[i];
            (*Heap).ptr[i] = (*Heap).ptr[p];
            (*Heap).ptr[p] = temp;

            i = p;
        }
        else return; /* Stop moving up if parent is already smaller */
    }
    return;
}

/* 
 * function: extractMin
 * 
 * Description:
 * Removes and returns the root element of the Min-Heap (the minimum cost node).
 * It features a "Lazy Deletion" approach: it skips over nodes that have already 
 * been visited, effectively cleaning up stale duplicate entries left in the array.
 * Once the root is removed, the last element replaces the root, and minHeapify is called.
 */
int extractMin(MinHeap * Heap, PathState * A) {
    int min;
    
    if(heap_current_size == 0) return -1;

    /* Lazy cleanup of stale pointers pointing to already visited nodes */
    while(A[(*Heap).ptr[0] - A].visited) {
        heap_current_size--;
        if(heap_current_size == 0) return -1;
        (*Heap).ptr[0] = (*Heap).ptr[heap_current_size];
    }

    min = (*Heap).ptr[0] - A;
    A[min].visited = 1; /* Mark node as fully evaluated */

    heap_current_size--;

    /* Move the bottom-most element to the root */
    (*Heap).ptr[0] = (*Heap).ptr[heap_current_size];

    /* Sink the new root down to maintain Min-Heap property */
    minHeapify(Heap, 0, A);
    return min;
}

/* 
 * function: minHeapify
 * 
 * Description:
 * Standard Top-Down Heapify (Bubble-Down). 
 * Starting from the root 'i', it compares the node with its left and right children.
 * It identifies the minimum among the three and swaps the parent downwards if needed,
 * repeating the process recursively (handled via loop) to enforce heap validity.
 */
void minHeapify(MinHeap * Heap, int i, PathState * A) {
    int l, min;
    PathState * tmp;
    
    while(1) {
        min = i;
        l = 2 * i + 1; /* Left child index */
        
        /* Check if left child is smaller than current minimum */
        if(l < heap_current_size) {
            if(A[(*Heap).ptr[l] - A].val < A[(*Heap).ptr[i] - A].val) 
                min = l;
        }
        else break; /* Node has no children, heap property satisfied */

        /* Check if right child is smaller than current minimum */
        if(l + 1 < heap_current_size) {
            if(A[(*Heap).ptr[l + 1] - A].val < A[(*Heap).ptr[min] - A].val)
                min = l + 1;
        }
        
        /* Swap with the smallest child and repeat down the tree */
        if(min != i) {
            tmp = (*Heap).ptr[i];
            (*Heap).ptr[i] = (*Heap).ptr[min];
            (*Heap).ptr[min] = tmp;

            i = min;
        }
        else break;
    } 
}

/* 
 * function: freeList
 * 
 * Description:
 * Iteratively frees every node in a Linked List to prevent memory leaks during grid teardowns.
 */
void freeList(NodeList * l) {
    Node *tmp;
    while ((*l) != NULL) {
        tmp = *l;
        *l = (*l)->next;
        free(tmp);
    }
    (*l) = NULL;
}
