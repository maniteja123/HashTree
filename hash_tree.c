#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HASH 3
#define MAX 3 // this means max of 3 in a node
#define N 3
int map[10];

struct itemset* build_itemsets(FILE *fp, int t);
void print_itemsets(struct itemset *it, int t);
struct node* create_node(int val, int level);
int hash(int val);
void build(struct node *root, struct itemset *it, int level);
void print_item(struct itemset *it);
int is_leaf(struct node *root);
void print_tree(struct node *root, int level);
void print_support(struct node *root, int level);
struct transaction* build_transactions(FILE *fp, int t);
void support(struct node *root, struct transaction *tr, int size, int level);
int contains(struct node *root, struct transaction *tr);
void print_transactions(struct transaction *tr, int t);

struct node{
    int val; // the hash value : val <= HASH
    int level; // the level in the tree : level <= N
    int thres; // the number of item sets in the node : thres <= MAX
    int sup; // the support count of the item set
    struct node* child[HASH]; // the child nodes
    struct itemset* item[MAX]; // the item sets in the node
};

struct itemset{
    int *items;
    int n;
};

struct transaction{
    int *elements;
    int n;
};

struct itemset* build_itemsets(FILE *fp, int t){
    int i, j;
    struct itemset *it = malloc(sizeof(struct itemset)*t);
    for(i=0; i<t; i++){
        fscanf(fp,"%d",&(it[i].n));
        it[i].items = malloc(sizeof(int)*(it[i].n));
        for(j=0; j<it[i].n; j++)
            fscanf(fp,"%d",&(it[i].items[j]));
    }
    close(fp);
    return it;
}

void print_itemsets(struct itemset *it, int t){
    int i, j;
    printf("%d\n",t);
    for(i=0; i<t; i++){
        printf("%d: ",it[i].n);
        for(j=0; j<it[i].n; j++)
            printf("%d ",it[i].items[j]);
        printf("\n");
    }
}

void print_transactions(struct transaction *tr, int t){
    int i, j;
    printf("%d\n",t);
    for(i=0; i<t; i++){
        printf("%d: ",tr[i].n);
        for(j=0; j<tr[i].n; j++)
            printf("%d ",tr[i].elements[j]);
        printf("\n");
    }
}

struct node* create_node(int val, int level){
    int i;
    struct node *new = malloc(sizeof(struct node));
    new->val = val;
    new->thres = 0;
    new->level = level;
    new->sup = 0 ;
    for(i=0; i<HASH; i++)
        new->child[i] = NULL;
    for(i=0; i<MAX; i++)
        new->item[i] = NULL;
    return new;
}

int hash(int val){
    int v = val%HASH;
    if(v==0) return HASH;
    return v;
}

void build(struct node *root, struct itemset *it, int level){
    struct node *temp;
    int i, j;
    for(i=0; i<HASH; i++){
        if(hash(it->items[level])==root->child[i]->val ){
            if(is_leaf(root->child[i])){
                temp = root->child[i];
                if(temp->thres < MAX){
                    temp->item[temp->thres] = it;
                    (temp->thres)++;
                }else{
                    for(j=0; j<HASH; j++)
                        temp->child[j] = create_node(j+1, level+1);
                    for(j=0;j<MAX; j++)
                        build(temp, temp->item[j], level+1);
                    build(temp, it, level+1);
                    for(j=0; j<MAX; j++)
                        temp->item[j] = NULL;
                    temp->thres=0;
                }
            }else{
                build(root->child[i], it, level+1);
            }
        }
    }
}

void print_item(struct itemset *it){
    int i, n;
    n = it->n;
    for(i=0; i<n; i++)
        printf("%d\t",it->items[i]);
}

void print_trans(struct transaction *tr){
    int i, n;
    n = tr->n;
    for(i=0; i<n; i++)
        printf("%d\t",tr->elements[i]);
    printf("\n");
}

int is_leaf(struct node *root){
    int i;
    for(i=0; i<HASH; i++){
        if(root->child[i] != NULL){
            return 0;
        }
    }
    return 1;
}

void print_tree(struct node *root, int level){
    int i, j;
    if(level<=N && is_leaf(root)){
        for(i=0;i<root->thres;i++){
            for(j=0;j<level;j++)
                printf("\t");
            printf("%d:\t",root->val);
            print_item(root->item[i]);
            printf("(%d)\n",root->sup);
        }
    }else if(!is_leaf(root)){
        for(i=0;i<HASH;i++)
            print_tree(root->child[i], level+1);
    }
}

void print_support(struct node *root, int level){
    int i, j;
    if(level<=N && is_leaf(root)){
        for(j=0;j<level;j++)
            printf("\t");
        printf("%d:\t",root->val);
        printf("(%d)\n",root->sup);
    }else if(!is_leaf(root)){
        for(i=0;i<HASH;i++)
            print_support(root->child[i], level+1);
    }
}

struct transaction* build_transactions(FILE *fp, int t){
    int i, j;
    struct transaction *tr = malloc(sizeof(struct transaction)*t);
    for(i=0; i<t; i++){
        fscanf(fp,"%d",&(tr[i].n));
        tr[i].elements = malloc(sizeof(int)*(tr[i].n));
        for(j=0;j<tr[i].n;j++)
            fscanf(fp,"%d",&(tr[i].elements[j]));
    }
    close(fp);
    return tr;
}

int contains(struct node *root, struct transaction *tr) {
    int i, j;
    int flag;
    for(i=0; i<root->thres; i++){
        flag = 1;
        for(j=0; j<N; j++){
            if(map[(root->item[i])->items[j]] != 1) flag = 0;
        }
        if(flag) return 1;
    }
    return 0;
}

void support(struct node *root, struct transaction *tr, int size, int level){
    int i,j;
    struct nod *temp;
    for(i=level; i<size && i<level+N; i++){
        for(j=0; j<HASH; j++){
            if(hash(tr->elements[i])==root->child[j]->val){
                //printf("node %d %d %d %d\n", i, level, tr->elements[i], is_leaf(root->child[j]));
                if(is_leaf(root->child[j])){
                    //printf("leaf %d %d %d\n", i, level, contains(root->child[j], tr));
                    if(contains(root->child[j], tr)) (root->child[j]->sup)++;
                }else{
                    support(root->child[j], tr, size, level+1);
                }
            }
        }
    }
}

void main(){
    int t, p, i, j;
    struct node *root;
    root = create_node(-1, 0);
    for(i=0; i<HASH; i++)
        root->child[i] = create_node(i+1, 1);
    FILE *fp= fopen("input.txt","r");
    fscanf(fp,"%d",&t);
    printf("itemsets : %d\n",t);
    close(fp);
    struct itemset *it = build_itemsets(fp, t); // build an array of item sets
    int pres[t];
    //print_itemsets(it, t); // print the item sets for debugging
    for(i=0; i<t; i++)
        build(root, &it[i], 0); // insert each item in the hash tree
    print_tree(root, 0);
    fp= fopen("trans.txt","r");
    fscanf(fp,"%d", &p);
    printf("transactions : %d\n",p);
    struct transaction *tr = build_transactions(fp, p); // build an array of item sets
    //print_transactions(tr, p); // print the transactions for debugging
    for(i=0; i<p; i++) {
        for(j=0; j<t; j++) pres[i] = 0;
        for(j=0; j<10; j++) map[j]=0;
        for(j=0; j<tr[i].n; j++) map[tr->elements[j]]++;
        support(root, &tr[i], tr[i].n, 0); // run each transaction through the hash tree
    }
    print_support(root, 0);
    return;
}
