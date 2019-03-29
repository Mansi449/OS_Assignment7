#include <thread>
#include <mutex>
#include <cstdint>
#include <iostream>
#include <utility>
#include <ctime>
#include <string>
#include <memory>

using namespace std;

#define TOBACO 1
#define PAPER 2
#define MATCH 3

/*
    use the lpthread flag with gcc to compile this code
    ~$ g++ q3.cpp -lpthread
*/

mutex smoke, check;

bool should_stop = false;

struct smoker
{
    bool has_tobaco;
    bool has_paper;
    bool has_match;

    smoker(int t, int p, int m):
        has_tobaco(t),
        has_paper(p),
        has_match(m) {}
};

pair<int, int> generate_two_products()
{
    srand(time(0));
    int num = rand() % 3 + 1;

    /*
        generate 2 products according to num to complete the ingredients needed for smoking
    */
    switch (num)
    {
        case TOBACO:
            return make_pair(PAPER, MATCH);
        
        case PAPER:
            return make_pair(TOBACO, MATCH);

        case MATCH:
            return make_pair(PAPER, TOBACO);
    }
}

void start_smoking(shared_ptr<smoker> s)
{
    string str = "";
    if(s -> has_tobaco)    str += "smoker with tobaco";
    if(s -> has_paper)    str += "smoker with paper";
    if(s -> has_match)    str += "smoker with match";
    cout << str << " is now smoking.." << endl;
    this_thread::sleep_for(chrono::milliseconds(100));
}

void create_for_tobacco(
    shared_ptr<smoker> s,
    shared_ptr<int> prod1,
    shared_ptr<int> prod2,
    shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                lock_guard<mutex> lock(check);
                if((*prod1 == MATCH || *prod1 == PAPER) 
                    && 
                    ((*prod2 == MATCH || *prod2 == PAPER)))
                {
                    lock_guard<mutex> lock(smoke);
                    start_smoking(s);
                }
            }
            *change = true;
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}

void create_for_paper(
    shared_ptr<smoker> s,
    shared_ptr<int> prod1,
    shared_ptr<int> prod2,
    shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                lock_guard<mutex> lock(check);
                if((*prod1 == MATCH || *prod1 == TOBACO) 
                    && 
                    ((*prod2 == MATCH || *prod2 == TOBACO)))
                {
                    lock_guard<mutex> lock(smoke);
                    start_smoking(s);
                }
            }    
            *change = true;
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        
    }
}

void create_for_match(
    shared_ptr<smoker> s,
    shared_ptr<int> prod1,
    shared_ptr<int> prod2,
    shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                lock_guard<mutex> lock(check);
                if((*prod1 == PAPER || *prod1 == TOBACO) 
                    && 
                    ((*prod2 == PAPER || *prod2 == TOBACO)))
                {
                    lock_guard<mutex> lock(smoke);
                    start_smoking(s);
                }
            }
            *change = true;
            this_thread::sleep_for(chrono::milliseconds(50));
        }
       
    }
}

void stop_threads()
{
    cin.get();
    should_stop = true; 
}

void simulate(shared_ptr<smoker> s1,
              shared_ptr<smoker> s2,
              shared_ptr<smoker> s3)
{
    pair<int, int> p = generate_two_products();

    shared_ptr<int> prod1 = make_shared<int>();
    shared_ptr<int> prod2 = make_shared<int>();
    shared_ptr<bool> change = make_shared<bool>();

    *prod1 = get<0>(p);
    *prod2 = get<1>(p);

    thread t1(&create_for_tobacco, s1, prod1, prod2, change); 
    thread t2(&create_for_paper, s2, prod1, prod2, change);
    thread t3(&create_for_match, s3, prod1, prod2, change);

    thread stop(&stop_threads);

    while(!should_stop)
    {
        if(*change)
        {
            lock_guard<mutex> lock(check);
            p = generate_two_products();

            *prod1 = get<0>(p);
            *prod2 = get<1>(p);
        }
    }

    //Join the created threads
    t1.join();
    t2.join();
    t3.join();
    stop.join();
}

int main(int argc, char* argv[])
{
    /*
        create the 3 smokers
    */
    shared_ptr<smoker> smoker1 = make_shared<smoker>(1, 0, 0);
    shared_ptr<smoker> smoker2 = make_shared<smoker>(0, 1, 0);
    shared_ptr<smoker> smoker3 = make_shared<smoker>(0, 0, 1);

    simulate(smoker1, smoker2, smoker3);

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
/*
    OUTPUT
*/
//////////////////////
////////////////
/*
    smoker with match is now smoking..
    smoker with match is now smoking..
    smoker with match is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with paper is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
    smoker with tobaco is now smoking..
*/
