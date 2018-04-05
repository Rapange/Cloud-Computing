#include <iostream>
#include <unordered_map>
#include <fstream>
#include <windows.h>
#include <thread>
#include <mutex>
#include <utility>
#include <vector>
#define CHUNK_SIZE  32*1024

using namespace std;
typedef long long ll;

mutex mtx;

ll all_words[41238] = {0};

unordered_map<string, ll> word_count;
unordered_map<string, unsigned int> word_order;


struct Node
{
    Node* m_children[27];
    string m_word;
    ll m_cont;
    bool visited;
    Node()
    {
        m_cont = 0;
        for(unsigned int i = 0; i < 27; i++)
        {
            m_children[i] = NULL;
            visited = false;
        }
    }
};

struct PatTree
{
    Node* m_head, *m_current;

    PatTree()
    {
        m_head = m_current = new Node();
    }
    inline void addLetter(char letter)
    {
        unsigned int l_index;
        l_index = (int)letter - 97;

        if(!m_current->m_children[l_index])
        {
            m_current->m_children[l_index] = new Node();
        }
        m_current = m_current->m_children[l_index];
    }
    inline void addWord(char word[32])
    {
        m_current->m_cont++;
        m_current->m_word = word;
        m_current = m_head;

    }
};



struct MyThread
{
    PatTree pat;
    thread thr;
    char word[27];
    MyThread()
    {

    }
    void process_chunk_t(char chunk[CHUNK_SIZE], unsigned int start, unsigned int ending)
    {
        unsigned short int word_tam = 0, pos = 0;
        for(unsigned int i = start; i < ending; i++)
        {
            //cout<<"processing "<<(int)chunk[i]<<endl;
            if((int)chunk[i] == 0) continue;
            if(chunk[i] == ' ')
            {
                //pat.addWord();
            }
            else
            {
                pat.addLetter(chunk[i]);
            }

        }
        if(word_tam > 0)
        {
            //pat.addWord();
        }

    }
};


PatTree p_all;
PatTree p_alls[8];
unordered_map<string, ll> word_counts[8];
//PatTree p_1, p_2;

inline void traverseOne(unsigned int& i)
{
    Node* start = p_alls[i].m_head;
    vector<Node*> my_stack;
    while(!my_stack.empty())
    {
        start = my_stack.back();
        my_stack.pop_back();
        start->visited = true;

        if(start->m_cont > 0) word_count[start->m_word]+= start->m_cont;
        for(unsigned int j = 0; j < 27; j++)
        {
            if(start->m_children[j] && !start->visited) my_stack.push_back(start->m_children[j]);
        }
    }
}


void join_counts_seq()
{
    for(unsigned int i = 1; i < 8; i++)
    {
        for(unordered_map<string,ll>::iterator it = word_counts[i].begin(); it != word_counts[i].end(); it++)
        {
            word_counts[0][it->first] += it->second;
        }
    }

}

void join_counts(unsigned int dest, unsigned int org)
{
    for(unordered_map<string,ll>::iterator it = word_counts[org].begin(); it != word_counts[org].end(); it++)
    {
        word_counts[dest][it->first] += it->second;
    }
}

thread pool[8];
inline void join_all()
{
    unsigned int num_threads = 4;
    while(num_threads > 0)
    {
        for(unsigned int i = 0; i < num_threads; i++)
        {
            pool[i] = thread(join_counts, i, i+num_threads);
        }
        for(unsigned int i = 0; i < num_threads; i++) pool[i].join();
        num_threads /= 2;
    }

}

void traverseAll()
{
    for(unsigned int i = 0; i < 8; i++)
    {
        traverseOne(i);
    }
}

void process_chunk_ts(char chunk[CHUNK_SIZE], unsigned int start, unsigned int ending, unsigned int id)
    {
        unsigned short int word_tam = 0;
        char word[33];
        for(unsigned int i = start; i < ending; i++)
        {
            if(chunk[i] == ' ')
            {
                word[word_tam] = '\0';
                p_alls[id].addWord(word);

                word_tam = 0;
            }
            else
            {
                word[word_tam] = chunk[i];
                word_tam++;
                p_alls[id].addLetter(chunk[i]);
            }

        }
    }

    void process_chunk_ts_2(char chunk[CHUNK_SIZE], unsigned int start, unsigned int ending, unsigned int id)
    {
        unsigned short int word_tam = 0;
        char word[33];
        for(unsigned int i = start; i < ending; i++)
        {
            if(chunk[i] == ' ')
            {
                word[word_tam] = '\0';
                word_counts[id][word]++;

                word_tam = 0;
            }
            else
            {
                word[word_tam] = chunk[i];
                word_tam++;
            }

        }
    }

void fillWordOrder(string file_name)
{
    string word;
    unsigned int cont = 0;
    ifstream file(file_name);
    while(file >> word) word_order[word] = cont++;
    cout<<cont<<endl;
}

void process_chunk(char chunk[CHUNK_SIZE], unsigned int start, unsigned int ending)
{
    char word[27];
    unsigned short int word_tam = 0, pos = 0;
    for(unsigned int i = start; i < ending; i++)
    {
        if(chunk[i] == ' ')
        {
            /*word[word_tam] = '\0';
            pos = word_order[word];


            mutexes[pos].lock();
            all_words[pos]++;
            mutexes[pos].unlock();
            word_tam = 0;*/
            word_tam = 0;
            //p_all.addWord();
        }
        else
        {
            /*word[word_tam] = chunk[i];
            word_tam++;*/

            word_tam++;
            p_all.addLetter(chunk[i]);
        }

    }
    if(word_tam > 0)
    {
        word_tam = 0;
        //p_all.addWord();
        /*word[word_tam] = '\0';
        pos = word_order[word];

            mutexes[pos].lock();
            all_words[pos]++;
            mutexes[pos].unlock();*/
    }
}



int main()
{
    //fillWordOrder("dictionary.txt");
    //string word;


    /*HANDLE h_file = CreateFile("words.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
    HANDLE map_file = CreateFileMapping(h_file, NULL, PAGE_READWRITE | SEC_RESERVE, 0, 0, 0);
    char* map_ptr = (char*) MapViewOfFile(map_file, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 2);

    cout<<map_ptr[0]<<endl;
    cout<<map_ptr[1]<<endl;
    UnmapViewOfFile(map_ptr);
    CloseHandle(map_file);
    CloseHandle(h_file);*/

    ifstream file("words2.txt");
    //FILE *file = fopen("words.txt", "r");
    ofstream result_file;

    //char chunk[CHUNK_SIZE + 32], next_chunk[CHUNK_SIZE + 32];
    char chunks[9][CHUNK_SIZE + 32];
    unsigned int i = 0, j, start = 0, next_start = 0;
    file.read(chunks[i],CHUNK_SIZE);
    while(file.read(chunks[i+1], CHUNK_SIZE)) //32768
    {

        //cout<<i<<endl;
        next_start = 0;
        if(chunks[i][CHUNK_SIZE - 1] != ' ')
        {
            while(chunks[i+1][next_start] != ' ')
            {
                chunks[i][CHUNK_SIZE + next_start] = chunks[i+1][next_start];
                next_start++;
            }
            chunks[i][CHUNK_SIZE + next_start] = chunks[i+1][next_start];
            next_start++;
        }

        //process_chunk(chunk, start, CHUNK_SIZE + next_start);
        pool[i] = thread(process_chunk_ts_2, chunks[i], start, CHUNK_SIZE + next_start, i);

        i++;

        swap(next_start, start);
        if(i == 8)
        {
            for(i = 0;i < 8; i++) pool[i].join();
            swap(chunks[i], chunks[8]);
            i = 0;
        }

        //swap(chunk, next_chunk);
        //t.join();
        //cout<<"finished"<<endl;
    }

    pool[i] = thread(process_chunk_ts_2, chunks[i], start, CHUNK_SIZE + next_start, i);
    i++;
    for(j = 0;j < i; j++) pool[j].join();

    /*while(file >> word) word_count[word]++;*/

    join_all();
    //join_counts_seq();

    result_file.open("result.txt");
    for(unordered_map<string, ll>::iterator it = word_counts[0].begin(); it != word_counts[0].end(); it++)
    {
        result_file<<it->first<<" "<<it->second<<'\n';
    }

    file.close();
    result_file.close();
    return 0;
}
