#include <stdio.h>
#include <stdlib.h>

#define MAX_SPEED 3

enum Direction{
    North,
    East,
    West,
    South
};

struct Position{
    int x;
    int y;
};

struct Node{
    struct Position pos;
    int speed;
    enum Direction heading;
};

struct ListNode
{
    struct Node node;
    struct ListNode *next;
};

int width = 0, height = 0;

void printMap(int map[height][width]){
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }   
}

int isValidMotion(int map[height][width], struct Position pos1, struct Position pos2){
    if(pos1.x > pos2.x){
        pos1.x += pos2.x;
        pos2.x -= pos1.x;
        pos1.x += pos2.x;
        pos2.x *= -1;
    }
    if(pos1.y > pos2.y){
        pos1.y += pos2.y;
        pos2.y -= pos1.y;
        pos1.y += pos2.y;
        pos2.y *= -1;
    }
    for (int i = pos1.x; i <= pos2.x; i++)
    {
        for (int j = pos1.y; j <= pos2.y; j++)
        {
            if(map[i][j]) return 0;
        }
        
    }
    return 1;
}

int isValidPos(struct Position pos){
    if(pos.x < height && pos.y < width && pos.x >= 0 && pos.y >= 0) return 1;
    return 0;
}

struct Position getResultantPosition(struct Position currPos, int speed, enum Direction heading, int horizontalMovement){
    if(heading % 2){
        return (struct Position){currPos.x + horizontalMovement, (2-heading)*speed + currPos.y};
    }else{
        return (struct Position){(heading-1)*speed + currPos.x, currPos.y + horizontalMovement};
    }
}

int getNeighbouringNodes(struct Node *currNode, struct Node neighbours[45], int map[height][width]){
    
    struct Position newPos;
    int counter = 0;
    int minSpeed = 0 > ((currNode->speed)-1) ? 0 : ((currNode->speed)-1);
    int maxSpeed = ((currNode->speed)+1) < MAX_SPEED ? ((currNode->speed)+1) : MAX_SPEED;
    for (int currSpeed = minSpeed; currSpeed <= maxSpeed; currSpeed++)
    {
        for (int i = -1; i <= 1; i++)
        {
            if(currSpeed == 0 || currSpeed == 1){
                for (int h = 0; h < 4; h++)
                {
                    newPos = getResultantPosition(currNode->pos, currSpeed, h, i);

                    if(isValidPos(newPos) && isValidMotion(map, currNode->pos, newPos)){
                        neighbours[counter] = (struct Node){newPos, currSpeed, h};
                        counter++;
                    }                }
                
            }else{
                newPos = getResultantPosition(currNode->pos, currSpeed, currNode->heading, i);

                if(isValidPos(newPos) && isValidMotion(map, currNode->pos, newPos)){
                    neighbours[counter] = (struct Node){newPos, currSpeed, currNode->heading};
                    counter++;
                }
            }

        }
        
    }
    return counter;
}

int compareNodes(struct Node *node1, struct Node *node2){
    if(node1->pos.x == node2->pos.x && node1->pos.y == node2->pos.y && node1->speed == node2->speed &&node1->heading == node2->heading){
        return 1;
    }else return 0;
}

int compareNodesWithList(int length, struct Node goals[length], struct Node *toCompare){
    for (int i = 0; i < length; i++)
    {
        if(compareNodes(toCompare, &goals[i])) return 1;
    }
    return 0;
    
}

int getHeuristic(struct Node *startNode, struct Node *goalNode, int ****hScoreMatrix){
    if(hScoreMatrix[startNode->pos.x][startNode->pos.y][startNode->speed][startNode->heading] != INT_MIN) return hScoreMatrix[startNode->pos.x][startNode->pos.y][startNode->speed][startNode->heading];
    int heuristic = ((goalNode->pos.x - startNode->pos.x)*(goalNode->pos.x - startNode->pos.x) + (goalNode->pos.y - startNode->pos.y)*(goalNode->pos.y - startNode->pos.y))/(1+(startNode->speed)*(startNode->speed));
    hScoreMatrix[startNode->pos.x][startNode->pos.y][startNode->speed][startNode->heading] = heuristic;
    return heuristic;
}

struct Node aStar(struct Node startNode, int map[height][width], int numGoals, struct Node goals[numGoals]){
    int ****gScore = malloc(height * sizeof(int***));
    int ****fScore = malloc(height * sizeof(int***));
    int ****hScore = malloc(height * sizeof(int***));
    struct Node cameFrom[height][width][MAX_SPEED+1][4];

    struct Node nullNode = {(struct Position){-1, -1}, 0, 0};

    for (int i = 0; i < height; i++)
    {
        gScore[i] = malloc(width * sizeof(int**));
        fScore[i] = malloc(width * sizeof(int**));
        hScore[i] = malloc(width * sizeof(int**));
        for (int j = 0; j < width; j++)
        {
            gScore[i][j] = malloc((MAX_SPEED+1) * sizeof(int*));
            fScore[i][j] = malloc((MAX_SPEED+1) * sizeof(int*));
            hScore[i][j] = malloc((MAX_SPEED+1) * sizeof(int*));
            for (int s = 0; s <= MAX_SPEED; s++)
            {
                gScore[i][j][s] = malloc(4 * sizeof(int*));
                fScore[i][j][s] = malloc(4 * sizeof(int*));
                hScore[i][j][s] = malloc(4 * sizeof(int*));
                for (int h = 0; h < 4; h++)
                {

                    gScore[i][j][s][h] = INT_MAX;
                    fScore[i][j][s][h] = INT_MAX;
                    hScore[i][j][s][h] = INT_MIN;
                    cameFrom[i][j][s][h] = nullNode;
                }
            } 
        }
    }

    gScore[startNode.pos.x][startNode.pos.y][0][0] = 0;
    map[startNode.pos.x][startNode.pos.y] = 0;
    
    struct ListNode *openSet;
    openSet = (struct ListNode *)malloc(sizeof(struct ListNode));
    openSet->node = startNode;
    openSet->next = NULL;
    int openSetLength = 1;

    while (openSetLength)
    {
        struct Node curr = openSet->node;
        if(compareNodesWithList(numGoals, goals, &curr)){
            struct Node reached = curr;
            
            printf("\nx = %d y = %d speed = %d heading = %d", curr.pos.x, curr.pos.y, curr.speed, curr.heading);
            map[curr.pos.x][curr.pos.y] = 3;
            
            int numSteps = 0;
            while(!compareNodes(&cameFrom[curr.pos.x][curr.pos.y][curr.speed][curr.heading], &nullNode)){
                if(compareNodes(&curr, &cameFrom[curr.pos.x][curr.pos.y][curr.speed][curr.heading])) break;
                curr = cameFrom[curr.pos.x][curr.pos.y][curr.speed][curr.heading];
                printf("\nx = %d y = %d speed = %d heading = %d", curr.pos.x, curr.pos.y, curr.speed, curr.heading);
                map[curr.pos.x][curr.pos.y] = 3;
                /*should ideally edit this in a copy to prevent future runs from getting stuck but this helps prevent backtracking and runs will be stuck only when starting in a 3 sided box*/
                numSteps++;
            }
            printf("\n\nNumber of Steps = %d\n\n", numSteps);
            printMap(map);
            return reached;
        }
        struct ListNode *toFree = openSet;
        openSet = openSet->next;
        free(toFree);
        openSetLength--;

        struct Node neighbours[45];
        int maxNeighbourIndex = getNeighbouringNodes(&curr, neighbours, map);

        for (int i = 0; i < maxNeighbourIndex; i++)
        {
            int tentativeGScore = gScore[curr.pos.x][curr.pos.y][curr.speed][curr.heading] + 1;
            if(tentativeGScore < gScore[neighbours[i].pos.x][neighbours[i].pos.y][neighbours[i].speed][neighbours[i].heading]){
                cameFrom[neighbours[i].pos.x][neighbours[i].pos.y][neighbours[i].speed][neighbours[i].heading] = curr;
                gScore[neighbours[i].pos.x][neighbours[i].pos.y][neighbours[i].speed][neighbours[i].heading] = tentativeGScore;
                int currFScore = tentativeGScore + getHeuristic(&neighbours[i], &goals[0], hScore);
                fScore[neighbours[i].pos.x][neighbours[i].pos.y][neighbours[i].speed][neighbours[i].heading] = currFScore;

                struct ListNode *currQueueElement;
                int addIndex = 0;
                if(openSet != NULL){
                    currQueueElement = openSet;
                    struct ListNode *prevQueueElement = NULL;
                    while (currQueueElement->next != NULL)
                    {
                        if(fScore[currQueueElement->node.pos.x][currQueueElement->node.pos.y][currQueueElement->node.speed][currQueueElement->node.heading] < currFScore){
                            addIndex++;
                        }
                        if(compareNodes(&currQueueElement->node, &neighbours[i]) && !compareNodes(&prevQueueElement->node, &nullNode)){
                            
                            if (prevQueueElement) {
                                prevQueueElement->next = currQueueElement->next;
                            } else {
                                openSet = currQueueElement->next;
                            }
                            free(currQueueElement);
                            openSetLength--;
                            break;
                        }
                        prevQueueElement = currQueueElement;
                        currQueueElement = currQueueElement->next;
                    }    
                }



                if(openSet != NULL){
                    struct ListNode *newElement = (struct ListNode *)malloc(sizeof(struct ListNode));
                    currQueueElement = openSet;
                    for(int j = 0; j < addIndex; j++){
                        currQueueElement = currQueueElement->next;
                    }
                    *newElement = (struct ListNode){neighbours[i], currQueueElement->next};
                    currQueueElement->next = newElement;
                }else{
                    openSet = (struct ListNode *)malloc(sizeof(struct ListNode));
                    *openSet = (struct ListNode){neighbours[i], NULL};
                }
                openSetLength++;
                

            }
        }
    }

}

void getCornerGoals(int heading, struct Node goals[6]){
    switch (heading)
    {
    case 0:
        goals[0] = (struct Node){(struct Position){3, 3}, 0, 1};
        goals[1] = (struct Node){(struct Position){3, 3}, 1, 1};
        goals[2] = (struct Node){(struct Position){2, 2}, 0, 1};
        goals[3] = (struct Node){(struct Position){2, 2}, 1, 1};
        goals[4] = (struct Node){(struct Position){1, 1}, 0, 1};
        goals[5] = (struct Node){(struct Position){1, 1}, 1, 1};
    break;
    
    case 1:
        goals[0] = (struct Node){(struct Position){3, width - 1 - 3}, 0, 2};
        goals[1] = (struct Node){(struct Position){3, width - 1 - 3}, 1, 2};
        goals[2] = (struct Node){(struct Position){2, width - 1 - 2}, 0, 2};
        goals[3] = (struct Node){(struct Position){2, width - 1 - 2}, 1, 2};
        goals[4] = (struct Node){(struct Position){1, width - 1 - 1}, 0, 2};
        goals[5] = (struct Node){(struct Position){1, width - 1 - 1}, 1, 2};
    break;

    case 2:
        goals[0] = (struct Node){(struct Position){height - 1 - 3, width - 1 - 3}, 0, 3};
        goals[1] = (struct Node){(struct Position){height - 1 - 3, width - 1 - 3}, 1, 3};
        goals[2] = (struct Node){(struct Position){height - 1 - 2, width - 1 - 2}, 0, 3};
        goals[3] = (struct Node){(struct Position){height - 1 - 2, width - 1 - 2}, 1, 3};
        goals[4] = (struct Node){(struct Position){height - 1 - 1, width - 1 - 1}, 0, 3};
        goals[5] = (struct Node){(struct Position){height - 1 - 1, width - 1 - 1}, 1, 3};
    break;
    
    case 3:
        goals[0] = (struct Node){(struct Position){height - 1 - 3, 3}, 0, 0};
        goals[1] = (struct Node){(struct Position){height - 1 - 3, 3}, 1, 0};
        goals[2] = (struct Node){(struct Position){height - 1 - 2, 2}, 0, 0};
        goals[3] = (struct Node){(struct Position){height - 1 - 2, 2}, 1, 0};
        goals[4] = (struct Node){(struct Position){height - 1 - 1, 1}, 0, 0};
        goals[5] = (struct Node){(struct Position){height - 1 - 1, 1}, 1, 0};
    break;
    
    default:
        break;
    }  
}

int main(){

    printf("Enter: <Width> <Height> (including padding) ");
    scanf("%d %d", &width, &height);
    
    int map[height][width];

    struct Position startPos;

    printf("Enter map matrix: \n");
    
    for(int i = 0; i < height; i++){
        for (int j = 0; j < width; j++)
        {
            scanf("%d", &(map[i][j]));
            if(map[i][j] == 3){
                startPos.x = i;
                startPos.y = j;
                map[i][j] = 0;
            }
        }
    }

    struct Node goals[6];
    struct Node reached;

    int currHeading = 0;
    if(startPos.x < 4 && startPos.y < width - 4) currHeading = 1;
    else if(startPos.x < height - 4 && startPos.y >= width - 4) currHeading = 2;
    else if(startPos.x >= height - 4 && startPos.y >= 4) currHeading = 3;

    getCornerGoals(currHeading, goals);
    reached = aStar((struct Node){startPos, 0, currHeading}, map, 6, goals);

    for (int i = 1; i <= 3; i++)
    {
        getCornerGoals((currHeading+i)%4, goals);
        reached = aStar(reached, map, 6, goals);
    }

    struct Node finalGoals[4] = {(struct Node){startPos, 0, 0},
                                 (struct Node){startPos, 0, 1},
                                 (struct Node){startPos, 0, 2},
                                 (struct Node){startPos, 0, 3}
    };

    map[startPos.x][startPos.y] = 0;
    aStar(reached, map, 4, finalGoals);
    

}
