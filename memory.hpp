#include<iostream>

class main_memory{

    private:
    
    public:

    int *data;

    main_memory(int size){

        this->data = new int[size];
        for(int i=0;i<size;i++){
            this->data[i] = rand()%10 + 1;
        }

    }

};