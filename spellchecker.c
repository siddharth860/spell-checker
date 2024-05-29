#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10000

typedef struct Node {
    char* word;
    struct Node* next;
} Node;

typedef struct {
    Node** table;
} HashTable;

unsigned int hash(const char* word) {
    unsigned long hash = 5381;
    int c;
    while ((c = *word++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

HashTable* create_table() {
    HashTable* hashTable = malloc(sizeof(HashTable));
    hashTable->table = malloc(sizeof(Node*) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable->table[i] = NULL;
    }
    return hashTable;
}

void insert(HashTable* hashTable, const char* word) {
    unsigned int index = hash(word);
    Node* newNode = malloc(sizeof(Node));
    newNode->word = strdup(word);
    newNode->next = hashTable->table[index];
    hashTable->table[index] = newNode;
}

void load_dictionary(HashTable* hashTable, const char* dictionaryFile) {
    FILE* file = fopen(dictionaryFile, "r");
    if (!file) {
        fprintf(stderr, "Could not open dictionary file\n");
        return;
    }

    char word[256];
    while (fscanf(file, "%255s", word) != EOF) {
        insert(hashTable, word);
    }
    fclose(file);
}

int check_spelling(HashTable* hashTable, const char* word) {
    unsigned int index = hash(word);
    Node* node = hashTable->table[index];
    while (node) {
        if (strcmp(node->word, word) == 0) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}

void free_table(HashTable* hashTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* node = hashTable->table[i];
        while (node) {
            Node* temp = node;
            node = node->next;
            free(temp->word);
            free(temp);
        }
    }
    free(hashTable->table);
    free(hashTable);
}

int main() {
    HashTable* hashTable = create_table();
    load_dictionary(hashTable, "dictionary.txt");

    char word[256];
    printf("Enter a word to check (or type 'exit' to quit): ");
    while (scanf("%255s", word) != EOF) {
        if (strcmp(word, "exit") == 0) {
            break;
        }

        if (check_spelling(hashTable, word)) {
            printf("'%s' is spelled correctly.\n", word);
        } else {
            printf("'%s' is misspelled.\n", word);
        }

        printf("Enter a word to check (or type 'exit' to quit): ");
    }

    free_table(hashTable);
    return 0;
}
