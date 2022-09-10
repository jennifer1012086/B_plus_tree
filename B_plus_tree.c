#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ENTRY 4
#define MAX_PTR 5

int order;

typedef struct node
{
    int num;
    int type; // 0: interior node , 1: leaf

    struct node *ptr[MAX_PTR+1];  
    int entry[MAX_ENTRY+1];
    bool ISentry[MAX_ENTRY+1];

    struct node *parent;
} node;

/* SWAP */
int swap_entry( int *a, int *b );
int swap_key( node **a, node **b );

/* Compare for qsort */
int compare( const void *a, const void *b );

/* main functions */
node* initialize();
node* attach( node* root );
node* bulkload( node *root );
bool* lookup( node *root, int id );
node* insert( node *root, int input );
void delete(node* root, int id);

/* display the result */
void display( node *root );
void display_tree(node *root, int level);

/* String */
void add_space_before_newline( char input[] );
int string_parse( char buffer[], int input[] );
node* parse_attach_string( node* root, char buffer[]  );

/* Basic function */
node* create_node( int type );
node *create_new_level( node* left_interior, node* right_interior );
void link_parent( node *pos );
node* fill_in_leaf( int input[], int num );
int find_empty_ptr( node *pos );
int calculate_fill_in_num( int num );
node* find_right_most_interior_node(node *root);
node* find_insert_leaf( node *root, node *pos, int input );

/* Balance tree function */
node* bulkload_balance(node *root, int input[], int num);
void in_leaf_balance( node *leaf );
void in_interior_balance( node* interior );

node* move_up( node* root, node* left_interior, node* right_interior );
node* copyup(node* root, node* interior, node* right_leaf, int index );
node* split_leaf( node* root, node* leaf );
node* split_interior( node* root, node* interior );
node* insert_in_leaf( node* root, node* leaf, int input );


int swap_entry( int *a, int *b )
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int swap_key( node **a, node **b )
{
    node *tmp = *a;
    *a = *b;
    *b = tmp;
}

int compare( const void *a, const void *b )
{
    return *(int*)a - *(int*)b;
}

void add_space_before_newline( char input[] )
{
    char *ptr = input;
    
    while( *ptr != '\n' ) ptr++;
    *ptr = ' ';
    *(ptr+1) = '\n';
}

int string_parse( char buffer[], int input[] )
{
    char *ptr = buffer;
    char *qtr = buffer;
    int index = 0;

    while( *ptr != '\n' )
    {
        if( *ptr == ' ' )
        {
            input[index] = atoi(qtr);
            index++;
            qtr = ptr+1;
        }
        ptr++;
    }

    return index;
}

node* create_node( int type )
{
    // type = 1 : leaf
    // type = 0 : interior node

    node* tmp = malloc(sizeof(node));
    
    for(int i=0; i<5; i++)
    {
        tmp->ISentry[i] = false;
        tmp->entry[i] = -1;
    }
    
    for(int i=0; i<5; i++)
    {    
        tmp->ptr[i] = NULL; 
    }

    tmp->type = type;
    tmp->parent = NULL;

    return tmp;
}

int calculate_fill_in_num( int num )
{
    if( (num % (order*2)) < order && (num % (order*2)) != 0 && num > order )
    {
        return order;
    }
    else if( num < 2*order )
    {
        return num;
    }
    else
    {
        return order*2;
    }    
}

node* find_right_most_interior_node(node *root)
{
    node *interior = root;
    bool flag = true;

    while( flag )
    {        
        for(int i=MAX_PTR-1; i>=0; i--)
        {
            if( interior->ptr[i] != NULL && interior->ptr[i]->type == 0 )
            {
                interior = interior->ptr[i];
                break;
            }
            if( i == 0 )
            {
                flag = false;
            }
        }
    }

    return interior;
}

int find_empty_ptr( node *pos )
{
    for(int i=0; i<MAX_PTR; i++)
    {
        if( pos->ptr[i] == NULL )
        {
            return i;
        }
    }
}

node *create_new_level( node* left_interior, node* right_interior )
{
    node *tmp_root = create_node(0);

    tmp_root->ptr[0] = left_interior;   
    tmp_root->ptr[1] = right_interior;

    left_interior->parent = tmp_root;
    right_interior->parent = tmp_root;

    return tmp_root;

}

void link_parent( node *pos )
{
    for(int link=0; link<MAX_PTR; link++)
    {
        node* leaf = pos->ptr[link];
        
        if(leaf == NULL) continue;

        leaf->parent = pos;
    }
       
}

node* bulkload_balance(node *root, int input[], int num)
{
    qsort(input, num, sizeof(int), compare);

    int input_idx = 0;
    bool first_fill_in_leaf = true;

    while( input_idx < num  )
    {    
        int fill_in_num = calculate_fill_in_num( num-input_idx );
        node *leaf = fill_in_leaf(&input[input_idx], fill_in_num);
        node *interior = find_right_most_interior_node(root);
        
        // the first leaf don't need to copy up the first value 
        if( first_fill_in_leaf )
        {
            root = copyup(root, interior, leaf, -2);
            first_fill_in_leaf = false;
        }
        else
        {
            root = copyup(root, interior, leaf, -1);
        }
        
        link_parent(interior);
        input_idx += fill_in_num;
    }

    return root;

}

node* initialize()
{
    printf("Initializing...order = ");
    scanf("%d", &order); // global variable

    node *root = create_node(0);

    return root;
}

node* fill_in_leaf( int input[], int num )
{
    node* leaf = create_node(1);

    for(int i=0; i<num; i++)
    {
        leaf->ISentry[i] = true;
        leaf->entry[i] = input[i];
    }

    return leaf;
}

node* parse_attach_string( node* root, char buffer[]  )
{
    char *ptr = buffer;
    char *qtr = buffer;
    bool turn = false; // false: parse leaf, true: parse index
    int index = -2;
    int tmp[5];

    add_space_before_newline(buffer);
    
    while( *ptr != '\n' )
    {   
        // this turn parse the leaves
        if( turn == false && *ptr == ' ' && ( *(ptr+1) == ';' || *(ptr+1) == '\n' ) )
        {
            *(ptr+1) = '\n';
            int num = string_parse(qtr, tmp);

            node* leaf = fill_in_leaf(tmp, num);
            node* interior = find_right_most_interior_node(root);
            root = copyup(root, interior, leaf, index);

            turn = true;
            qtr = ptr+3;
            ptr += 2;
        }
        // this turn parse the single index
        else if( *ptr == ';' && turn == true )
        {
            index = atoi(qtr);
            turn = false;
            qtr = ptr+2;
        }
        
        ptr++;
    }

    return root;
}

node* attach( node* root )
{
    root = create_node(0);
    
    printf("Attaching...order =");
    scanf("%d", &order);
    printf("Nodes in inorder-like traversal =");
    
    char buffer[10000];
    
    getchar();
    fgets(buffer, 10000, stdin);

    root = parse_attach_string( root, buffer );
    
    return root;
}

node* bulkload( node *root )
{
    printf("Bulkloading... key sequence = ");
    
    char buffer[10005];
    int input[3000];
    char *ptr = buffer;

    getchar();
    fgets(buffer, 10000, stdin);

    add_space_before_newline(buffer);

    int num = string_parse(buffer, input);

    return bulkload_balance(root, input, num);
}

bool* lookup( node *root, int id )
{ 
    // leaf node : found or not found
    if( root->type == 1 )
    {
        for(int i=0; i<MAX_ENTRY; i++)
        {
            if( root->ISentry[i] == true && root->entry[i] == id )
            {
                return &(root->ISentry[i]);
            }
        }
        return NULL;
    }
    // interior node : keep recursing
    else
    {
        for(int i=0; i<MAX_PTR; i++)
        {
            if( root->ISentry[i] == true && root->entry[i] > id )
            {
                return lookup(root->ptr[i], id);
            }
            else if( root->ISentry[i] == false )
            {
                return lookup(root->ptr[i], id);
            }
        }
    }
}

void in_leaf_balance( node *leaf )
{   
    // balance the entry (sort)
    for(int i=0; i<MAX_ENTRY; i++)
    {
        if( leaf->ISentry[i] == true  )
        {
            if( leaf->entry[i] > leaf->entry[MAX_ENTRY] )
            {
                swap_entry( &(leaf->entry[i]), &(leaf->entry[MAX_ENTRY]) );
            }
        }
    }

    // make the entry continuous
    for(int i=0; i<MAX_ENTRY; i++)
    {
        if( leaf->ISentry[i] == false )
        {
            leaf->ISentry[i] = true;
            leaf->entry[i] = leaf->entry[MAX_ENTRY];
            leaf->ISentry[MAX_ENTRY] = false;
            break;
        }
    }

}

void in_interior_balance( node* interior )
{   
    // balance the interior node (sort)
    for(int i=0; i<MAX_ENTRY; i++)
    {
        if( interior->ISentry[i] == true )
        {
            if( interior->entry[i] > interior->entry[MAX_ENTRY] )
            {
                swap_entry( &(interior->entry[i]), &(interior->entry[MAX_ENTRY]) );
                swap_key( &(interior->ptr[i+1]), &(interior->ptr[MAX_ENTRY+1]) );
            }
        }
    }

    // make the entry continuous
    for(int i=0; i<MAX_ENTRY; i++)
    {    
        // if the leaf is the first leaf, no need to copy up the first value
        if( interior->ISentry[MAX_ENTRY] == true && interior->entry[MAX_ENTRY] == -2 )
        {
            interior->ptr[0] = interior->ptr[MAX_ENTRY+1];
            interior->ptr[MAX_ENTRY+1] = NULL;
            interior->ISentry[MAX_ENTRY] = false;
            break;
        }
        
        else if( interior->ISentry[i] == false )
        {
            interior->ISentry[i] = true;
            interior->entry[i] = interior->entry[MAX_ENTRY];
            interior->ISentry[MAX_ENTRY] = false;
            interior->ptr[i+1] = interior->ptr[MAX_ENTRY+1];
            interior->ptr[MAX_ENTRY+1] = NULL;
            break;
        }
        
    }

    link_parent(interior);    

}

node* move_up( node* root, node* left_interior, node* right_interior )
{   
    node* interior = left_interior->parent;

    if( interior == NULL )
    {
        interior = create_new_level(left_interior, right_interior);
    }

    interior->ISentry[MAX_ENTRY] = true;
    interior->entry[MAX_ENTRY] = left_interior->entry[ MAX_ENTRY/2 ];
    interior->ptr[MAX_ENTRY+1] = right_interior;
    left_interior->ISentry[ MAX_ENTRY/2 ] = false;

    in_interior_balance( interior );

    if( interior->ISentry[MAX_ENTRY] == false )
    {
        // trace to root and return
        while( interior->parent != NULL )
        {
            interior = interior->parent;
        }
        return interior;
    }
    else
    {
        return move_up(root, interior, split_interior(root, interior));
    }

}

node* copyup(node* root, node* interior, node* right_leaf, int index )
{
    interior->ptr[MAX_ENTRY+1] = right_leaf;
    interior->ISentry[MAX_ENTRY] = true;
    
    if( index == -1 ) // insert / bulkload
    {
        interior->entry[MAX_ENTRY] = right_leaf->entry[0];
    }
    else if( index == -2 ) // first leaf (no need to copy up the first value to interior node)
    {
        interior->entry[MAX_ENTRY] = -2;
    }
    else // attach
    {
        interior->entry[MAX_ENTRY] = index;
    }
    
    in_interior_balance( interior );

    if( interior->ISentry[MAX_ENTRY] == false )
    {
        // trace to root and return
        while( interior->parent != NULL )
        {
            interior = interior->parent;
        }
        return root;
    }
    else
    {
        return move_up( root, interior, split_interior(root, interior) );
    }

}

node* split_leaf( node* root, node* leaf )
{
    node *new_leaf = create_node(1);
    int idx = 0;

    for(int i=MAX_ENTRY/2; i<=MAX_ENTRY; i++)
    {
        new_leaf->ISentry[idx] = true;
        new_leaf->entry[idx] = leaf->entry[i];   
        leaf->ISentry[i] = false;
        idx++;
    }

    new_leaf->parent = leaf->parent;

    return new_leaf;

}

node* split_interior( node* root, node* interior )
{
    node *new_interior = create_node(0);
    int idx = 0;
    
    for(int i=MAX_ENTRY/2+1; i<=MAX_ENTRY; i++)
    {
        new_interior->ISentry[idx] = true;
        new_interior->entry[idx] = interior->entry[i];  
        interior->ISentry[i] = false;
        idx++;
    }

    idx = 0;

    for(int i=MAX_ENTRY/2; i<MAX_PTR; i++)
    {
        new_interior->ptr[idx] = interior->ptr[i+1];
        interior->ptr[i+1] = NULL;
        idx++;    
    }

    link_parent(new_interior);
    link_parent(interior);
    
    new_interior->parent = interior->parent;
    
    return new_interior;

}

node* insert_in_leaf( node* root, node* leaf, int input )
{
    leaf->ISentry[MAX_ENTRY] = true;
    leaf->entry[MAX_ENTRY] = input;
    in_leaf_balance( leaf );

    node* new_leaf = NULL;

    if( leaf->ISentry[MAX_ENTRY] != false )
    {
        new_leaf = split_leaf(root, leaf);
        root = copyup( root, leaf->parent, new_leaf, -1 );
    }

    return root;

}

node* find_insert_leaf( node *root, node *pos, int input )
{    
    // type == 1 : leaf
    if( pos->type == 1 )
    {
        return pos;
    }
    else
    {
        for(int i=0; i<MAX_ENTRY; i++)
        {
            if( pos->ISentry[i] == true && pos->entry[i] > input )
            {
                return find_insert_leaf(root, pos->ptr[i], input);
            }
            else if( pos->ISentry[i] == false )
            {
                if( pos->ptr[i] == NULL )
                {
                    pos->ptr[i] = create_node(1);
                    pos->ptr[i]->parent = pos;
                    
                    if( i == 0 )
                    {
                        root = copyup(root, pos, pos->ptr[i], -2);
                    }
                    else
                    {
                        root = copyup(root, pos, pos->ptr[i], -1);
                    }
                }
                return find_insert_leaf(root, pos->ptr[i], input);
            }
            else if( i == MAX_ENTRY-1 )
            {
                return find_insert_leaf(root, pos->ptr[i+1], input);
            }
        }
    }   
} 

node* insert( node *root, int input )
{
    node* leaf = find_insert_leaf(root, root, input);
    return insert_in_leaf(root, leaf, input);
}

void delete(node* root, int id)
{
    bool* data = lookup(root, id);
    
    if( data != NULL )
    {
        *data = false;
    }
    else
    {
        printf("ERROR: input key not found\n");
    }
    return;
}

void display( node *root )
{
    if( root->type == 0 )
    {
        for(int i=0; i<MAX_ENTRY+1; i++)
        {
            if( root->ptr[i] != NULL )
            {
                display( root->ptr[i] );
                
                if( root->ISentry[i] == true )
                {
                    printf("%d ; ", root->entry[i]);
                }
            }
        }
    }
    else
    {
        for(int i=0; i<MAX_ENTRY; i++)
        {
            if( root->ISentry[i] == true )
            {
                printf("%d ",root->entry[i]);
            }
        }
        printf("; ");
    }    
}

void display_tree(node *root, int level)
{
    if(root->type == 1) printf("<");
    
    for(int i=0; i<MAX_ENTRY; i++)
    {
        if( root->ISentry[i] )
            printf("%d ", root->entry[i]);
    }
    
    if(root->type == 1) printf(">");
    
    printf("\n");

    for(int i=0 ;i<MAX_ENTRY+1; i++)
    {
        if( root->ptr[i] != NULL )
        {
            for(int j=0; j<level; j++) printf("      ");
            display_tree(root->ptr[i], level+1);
        }
    }
}

int main()
{
    node *root = NULL;
    
    while(1)
    {
        printf("1) Initialize\t2) Attach\t3) Bulkload\t4) Lookup\n");
        printf("5) Insert\t6) Delete\t7) Display\t8) Quit\n");
        printf("> Select an operation:");
        
        int opt;
        scanf("%d", &opt);
        
        if( opt == 1 ) // Initialize
        {
            if( root != NULL )
            {
                printf("Delete the previous tree...\n");
                free(root);
                printf("Done\n");
                continue;
            }
            root = initialize();
        }
        else if( opt == 2 ) // Attach
        {
            if( root != NULL )
            {
                printf("Delete the previous tree...\n");
                free(root);
                printf("Done\n");
                continue;
            }
            root = attach(root);
        }
        else if( opt == 3 ) // Bulkload
        {
            if( root == NULL )
            {
                printf("Need to initialize first\n");
                continue;
            }
            root = bulkload( root );
        }
        else if( opt == 4 ) // Lookup
        {
            if( root == NULL )
            {
                printf("Need to initialize first\n");
                continue;
            }
            printf("Lookup for key = ");
            int id;
            scanf("%d", &id);
            if( lookup(root, id) != NULL ) printf("Found!\n");
            else printf("Not Found...\n");
        }
        else if( opt == 5 ) // Insert
        {
            if( root == NULL )
            {
                printf("Need to initialize first\n");
                continue;
            }
            int input;
            printf("Inserting...key =");
            scanf("%d", &input);
            root = insert(root, input);
        }
        else if( opt == 6 ) // Delete
        {
            if( root == NULL )
            {
                printf("Need to initialize first\n");
                continue;
            }
            printf("Deleting...key =");
            int input;
            scanf("%d", &input);
            delete(root, input);
        }
        else if( opt == 7 ) // Display
        {
            display(root);
            printf("\n\n");
            display_tree(root, 1);
            printf("\n");
        }
        else if( opt == 8 ) // Quit
        {
            break;
        }
        else
        {
            printf("Input error\n");
        }
    }
    return 0;
}